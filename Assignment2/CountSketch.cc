#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <vector>
#include <bits/stdc++.h>
#include <experimental/random>
#include "hashutil.h"

using namespace std;

class CountSketch{
public:
    uint64_t N;
    uint64_t K;
    int rand_array[2] = {-1, 1};
    int T;
    int* H;
    int* S;
    int** C;
    double phi;
    // std::set<uint64_t> heavyhitters;
    std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyhitters;

    CountSketch(uint64_t n, uint64_t k, int t, double phi_val){
        N = n;
        K = k;
        T = t;
        phi = phi_val;
        cout<<"phi_val: "<<phi_val<< " "<<phi;
        H = (int *)malloc(T * sizeof(int));
        S = (int *)malloc(T * sizeof(int));
        C = new int*[T];
        for (int i = 0; i < T; i++) {
            C[i] = new int[K];
        }

        for (uint64_t i = 0; i < K; ++i) {
            H[i] = std::experimental::randint(0, INT8_MAX);  // seeds
            S[i] = std::experimental::randint(0, INT8_MAX);
        }   

        for (int i = 0; i < t; ++i) {
            for (uint64_t j = 0; j < k; j++) {
                C[i][j] = 0;
            }
        }      

    }

    void add(uint64_t num) {
        for (int i = 0; i < T; i++) {
            uint64_t j = MurmurHash64A(&num, 1, H[i]) % K;
            // int j = (H[i] * num) % K;
            C[i][j] = C[i][j] + rand_array[(S[i] * num) % 2];
        }
        // in order to get the heavy hitters, 
        // call the estimate for num and check if it exceeds the threshold then add to the heavy hitters list
        double freq = estimate(num);
        // cout<<freq<<endl;
        if (freq >= phi) {
            // cout<<"phi: "<<phi<<"freq: "<<freq<<endl<<"count: "<<(freq * N);
            heavyhitters.insert(std::make_pair(num, freq * N ));
            // heavyhitters.insert(num);
            // cout<<num<<endl;
        }
    }

    double estimate(uint64_t num) {
        double f[T];
        uint64_t *n = &num;
        for (int i = 0; i < T; i++) {

            uint64_t j = MurmurHash64A(n, 1, H[i]) % K;
            // int j = (H[i] * num) % K;
            f[i] = (double) ((rand_array[(S[i] * num) % 2] ) * C[i][j] ) / N;
        }
        double ans ;
        sort(f, f + T);
        if (T % 2 == 0) {
            ans  = (f[T/2] + f[(T/2) - 1]) / 2;
            return ans;
        }
        return f[(int)T/2];
    }

    std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyHitters(double phi) {
        return heavyhitters;
    }

};

int main() {
   cout << "Hello World. This is C++ program" << endl;
   CountSketch cs = CountSketch(100, 10, 10, 0.01);
   for (uint64_t i = 0; i < 10; ++i) {
       cs.add( i);
   }
   cs.add( 10);
   cs.add( 10);
   cs.add( 10);
   cs.add( 10);


   cout<<"cs.estimate(10) :"<<cs.estimate(10);

   std::map<uint64_t, uint64_t, std::greater<uint64_t> > HH = cs.heavyHitters(0.01);
   std::cout << "CountSketch Heavy hitter items: \n";
   for (auto it = HH.begin(); it != HH.end(); ++it) {
		std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
	}

//    std::set<uint64_t> hh = cs.heavyHitters(0.01);
//    for (auto it = hh.begin(); it !=hh.end(); ++it)
//         cout << ' ' << *it;
}