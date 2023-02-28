// Author: Prashant Pandey <prashant.pandey@utah.edu>
// For use in CS6968 & CS5968

#include <iostream>
#include <cassert>
#include <chrono>
#include <openssl/rand.h>
#include <map>
#include <type_traits>
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
	uint64_t K = 500;
	int T = 20;
	cout<<" K: "<<K<< " T: "<<T<<endl;

	MisraGriesDS mg = MisraGriesDS(K);
	CountSketch cs = CountSketch(K, T, tau);
	CountMinSketch cms = CountMinSketch(K, T, tau);

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
    std::set <uint64_t> heavyHitters;
	std::cout << "Heavy hitter items: \n";
	for (auto it = topK.begin(); it != topK.end(); ++it) {
		// std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
		heavyHitters.insert(it->second);
	}


	// MisraGries Compute HeavyHitters
	std::multimap<uint64_t, uint64_t, std::greater<uint64_t> > HH = mg.heavyHitters(tau);
	std::cout << "MisraGries: Heavy hitter items: \n";
	std::cout << "MisraGries: Total number of phi-hitters: "<< HH.size()<<endl;
	int relevant_item_count = 0;

	for (auto it = HH.begin(); it != HH.end(); ++it) {
		// std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
		if (heavyHitters.find(it->second) != heavyHitters.end()) {
			relevant_item_count++;
		}
	}
	std::cout << "MisraGries: Precision of phi-hitters: "<< (double)relevant_item_count/HH.size()<<endl;
	std::cout << "MisraGries: Recall of phi-hitters: "<< (double)relevant_item_count/topK.size()<<endl;
	std::cout << "MisraGries: Sketch Size: "<< mg.size()<<endl;

	// CountSketch Compute HeavyHitters
	std::cout << "CountSketch: Heavy hitter items: \n";
	std::map<uint64_t, uint64_t, std::greater<uint64_t> > HHCS = cs.heavyHitters(tau);
	std::cout << "CountSketch: Total number of phi-hitters: "<< HHCS.size()<<endl;
	relevant_item_count = 0;
	for (auto it = HHCS.begin(); it != HHCS.end(); ++it) {
		// std::cout << "Item: " << it->first << " Count: " << it->second << "\n";
		if (heavyHitters.find(it->first) != heavyHitters.end()) {
			relevant_item_count++;
		}
	}
	std::cout << "CountSketch: Precision of phi-hitters: "<< std::setprecision(5)<<(double)relevant_item_count/HHCS.size()<<endl;
	std::cout << "CountSketch: Recall of phi-hitters: "<< std::setprecision(5)<<(double)relevant_item_count/topK.size()<<endl;
	std::cout << "CountSketch: Sketch Size: "<< cs.size()<<endl;


	// CountMinSketch Compute HeavyHitters
	std::map<uint64_t, uint64_t, std::greater<uint64_t> > HHCMS = cms.heavyHitters(tau);
	std::cout << "CountMinSketch: Heavy hitter items: \n";
	std::cout << "CountMinSketch: Total number of phi-hitters: "<< HHCMS.size()<<endl;
	relevant_item_count = 0;
	for (auto it = HHCMS.begin(); it != HHCMS.end(); ++it) {
		// std::cout << "Item: " << it->first << " Count: " << it->second << "\n";
		if (heavyHitters.find(it->first) != heavyHitters.end()) {
			relevant_item_count++;
		}
	}
	std::cout << "CountMinSketch: Precision of phi-hitters: "<< std::setprecision(5)<<(double)relevant_item_count/HHCMS.size()<<endl;
	std::cout << "CountMinSketch: Recall of phi-hitters: "<< std::setprecision(5)<<(double)relevant_item_count/topK.size()<<endl;
	std::cout << "CountMinSketch: Sketch Size: "<< cms.size()<<endl;

	return 0;
}

