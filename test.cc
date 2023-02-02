// Author: Prashant Pandey <prashant.pandey@utah.edu>
// For use in CS6968 & CS5968

#include <cstdint>
#include <iostream>
#include <set>
#include <chrono>
#include <openssl/rand.h>
#include "VanEmDeBoas.cpp"
#include <cstddef>
#include <cstdint>
#include<iostream>
#include <ostream>
#include <sys/ucontext.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std::chrono;

double elapsed(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2) {
	return (duration_cast<duration<double>>(t2 - t1)).count();
}

void safe_rand_bytes(unsigned char *v, uint32_t n) {
	while (n > 0) {
		size_t round_size = n >= INT_MAX ? INT_MAX - 1 : n;
		RAND_bytes(v, round_size);
		v += round_size;
		n -= round_size;
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Specify the number of items for the test.\n";
		exit(1);
	}
	uint32_t N = atoi(argv[1]); 	// number of items

	// Generate N numbers to insert
	uint32_t *in_numbers = (uint32_t *)malloc(N * sizeof(uint32_t));
	if(!in_numbers) {
		std::cerr << "Malloc in_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char *)in_numbers, sizeof(*in_numbers) * N);

	// Generate N numbers different from in_numbers for succ queries
	uint32_t *out_numbers = (uint32_t *)malloc(N * sizeof(uint32_t));
	if(!out_numbers) {
		std::cerr << "Malloc out_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char *)out_numbers, sizeof(*out_numbers) * N);

	// Create a bst using std::set
	std::set<uint32_t> bst;
	high_resolution_clock::time_point t1, t2;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		bst.insert(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";
    
	// VanEmDBTree INSERT
    VanEmDBTree* Vtree = new VanEmDBTree(UINT32_MAX);
    // Insert N items from in_numbers to VEB Tree
    t1 = high_resolution_clock::now();
	// cout<<"innumbers"<<endl;
    for (uint32_t i = 0; i < N; ++i) {
		
        insert(Vtree, in_numbers[i]);
		// cout<<unsigned(in_numbers[i])<<",";
	}
	// cout<<"innumbers end "<<endl;
    t2 = high_resolution_clock::now();
    std::cout << "VanEmDBTree: Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.find(in_numbers[i]);
		if (ret == bst.end()) {
			std::cerr << "Find in BST failed. Item: " + std::to_string(in_numbers[i]) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

    // VanEmDBTree QUERY
	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = query(Vtree, in_numbers[i]);
		if (!ret) {
			std::cerr << "Find in VanEmDBTree failed. Item: " + std::to_string(i) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "VanEmDBTree: Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// N Successor queries correctness from out_numbers
	// t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.lower_bound(out_numbers[i]);
		if ((ret != bst.end() && *ret < out_numbers[i])) {
			std::cerr << "successor query in BST failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) + "\n";
			exit(0);
		} 
		// Correctness check for Successor
		successor_ret s = getSuccessor(Vtree, out_numbers[i]);
		if (s.valid) {
			if ( s.successor != *ret) {
				std::cerr << "successor query in VanEmDBTree failed. Item: " + std::to_string(out_numbers[i]) + "\n";
				exit(0);
			}
		}
	}

	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.lower_bound(out_numbers[i]);
		if ((ret != bst.end() && *ret < out_numbers[i])) {
			std::cerr << "successor query in BST failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

    // VanEmDBTree SUCCESSOR
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		successor_ret s = getSuccessor(Vtree, out_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "VanEmDBTree: Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	return 0;
}