#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

class MisraGriesDS{
public:
    uint64_t N;
    uint64_t K;
    uint64_t *Y;
    uint64_t *C;

    MisraGriesDS(uint64_t n, uint64_t k){
        N = n;
        K = k;
        Y = (uint64_t *)malloc(K * sizeof(uint64_t));
        C = (uint64_t *)malloc(K * sizeof(uint64_t));
        for (uint64_t i = 0; i < K; ++i) {
            Y[i] = 0;
            C[i] = 0;
        }
    }

    void add(uint64_t num) {
        bool found = false;
        for (uint64_t i = 0; i < K; ++i) {
            if (Y[i] == num) {
                C[i] = C[i] + 1;
                found = true;
                break;
            }
        }
        found = false;
        if (!found) {
            for (uint64_t i = 0; i < K; ++i) {
                if (C[i] == 0) {
                    Y[i] = num;
                    C[i] = 1;
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            for (uint64_t i = 0; i < K; ++i) {
                C[i] = C[i] - 1;
            }
        }
    }

    uint64_t estimate(uint64_t num) {
        for (uint64_t i = 0; i < K; ++i) {
            if (Y[i] == num) {
                return C[i];
            }
        }
        return 0;
    }

    std::set<uint64_t> heavyHitters(uint64_t phi) {
        std::set<uint64_t> heavyhitters;
        for (uint64_t i = 0; i < K; ++i) {
            if (C[i] >= N * phi) {
                heavyhitters.insert(Y[i]);
            }
        }
        return heavyhitters;
    }

};