// Author: Prashant Pandey <prashant.pandey@utah.edu>
// For use in CS6968 & CS5968

#include <iostream>
#include <cassert>
#include <chrono>
#include <openssl/rand.h>
#include <map>
#include <unordered_map>
#include "MisraGries.cc"
#include "CountSketch.cc"
#include "CountMinSketch.cc"

#include "zipf.h"

using namespace std::chrono;

#define UNIVERSE 1ULL << 30
#define EXP 1.5 

double elapsed(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2) {
	return (duration_cast<duration<double> >(t2 - t1)).count();
}

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cerr << "Specify the number of items N and phi.\n";
		exit(1);
	}
	uint64_t N = atoi(argv[1]);
	double tau = atof(argv[2]);
	uint64_t *numbers = (uint64_t *)malloc(N * sizeof(uint64_t));

	MisraGriesDS mg = MisraGriesDS(N, 1/tau);
	CountSketch cs = CountSketch(N, 1/tau, 1/tau, tau);
	CountMinSketch cms = CountMinSketch(N, 1/tau, 1/tau, tau);

	if(!numbers) {
		std::cerr << "Malloc numbers failed.\n";
		exit(0);
	}
	high_resolution_clock::time_point t1, t2;
	t1 = high_resolution_clock::now();
	generate_random_keys(numbers, UNIVERSE, N, EXP);
	t2 = high_resolution_clock::now();
	std::cout << "Time to generate " << N << " items: " << elapsed(t1, t2) << " secs\n";

	std::unordered_map<uint64_t, uint64_t> map(N);

	t1 = high_resolution_clock::now();
	for (uint64_t i = 0; i < N; ++i) {
		map[numbers[i]]++;
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to count " << N << " items: " << elapsed(t1, t2) << " secs\n";

    // Adding to MG
	t1 = high_resolution_clock::now();
	for (uint64_t i = 0; i < N; ++i) {
		mg.add(numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "MisraGries: Time to count " << N << " items: " << elapsed(t1, t2) << " secs\n";

   	// Adding to CountSketch
	t1 = high_resolution_clock::now();
	for (uint64_t i = 0; i < N; ++i) {
		cs.add(numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "CountSketch: Time to count " << N << " items: " << elapsed(t1, t2) << " secs\n";

  	// Adding to CountMinSketch
	t1 = high_resolution_clock::now();
	for (uint64_t i = 0; i < N; ++i) {
		cms.add(numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "CountMinSketch: Time to count " << N << " items: " << elapsed(t1, t2) << " secs\n";

	free(numbers); // free stream

	// Compute heavy hitters
	uint64_t total = 0;
	double threshold = tau * N;
	std::multimap<uint64_t, uint64_t, std::greater<uint64_t> > topK;
	t1 = high_resolution_clock::now();
	for (auto it = map.begin(); it != map.end(); ++it) {
		if (it->second >= threshold) {
			topK.insert(std::make_pair(it->second, it->first));
		}
		/*
		if (topK.size() < K) {
			topK.insert(std::make_pair(it->second, it->first));
		} else {
			if (std::prev(topK.end())->first < it->second) {
				// the last item has lower freq than the new item
				topK.erase(std::prev(topK.end())); // delete the last item
				topK.insert(std::make_pair(it->second, it->first)); 
			}
		}
		*/
		total += it->second;
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to compute phi-heavy hitter items: " << elapsed(t1, t2) << " secs\n";
	std::cout << "Total number of phi-hitters: "<<topK.size()<<endl;
	assert(total == N);

	std::cout << "Heavy hitter items: \n";
	for (auto it = topK.begin(); it != topK.end(); ++it) {
		std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
	}


	// MisraGries Compute HeavyHitters
	std::multimap<uint64_t, uint64_t, std::greater<uint64_t> > HH = mg.heavyHitters(tau);
	std::cout << "MisraGries Heavy hitter items: \n";
	std::cout << "Total number of phi-hitters: "<< HH.size()<<endl;
	for (auto it = HH.begin(); it != HH.end(); ++it) {
		std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
	}

	// CountSketch Compute HeavyHitters
	std::map<uint64_t, uint64_t, std::greater<uint64_t> > HHCS = cs.heavyHitters(tau);
	std::cout << "CountSketch Heavy hitter items: \n";
	std::cout << "Total number of phi-hitters: "<< HHCS.size()<<endl;
	for (auto it = HHCS.begin(); it != HHCS.end(); ++it) {
		std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
	}

	// CountMinSketch Compute HeavyHitters
	std::map<uint64_t, uint64_t, std::greater<uint64_t> > HHCMS = cms.heavyHitters(tau);
	std::cout << "CountMinSketch Heavy hitter items: \n";
	std::cout << "Total number of phi-hitters: "<< HHCMS.size()<<endl;
	for (auto it = HHCMS.begin(); it != HHCMS.end(); ++it) {
		std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
	}

	return 0;
}
