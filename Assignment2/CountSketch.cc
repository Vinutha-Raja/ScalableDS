#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <vector>
#include <bits/stdc++.h>
#include <experimental/random>

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
    std::set<uint64_t> heavyhitters;

    CountSketch(uint64_t n, uint64_t k, int t, double phi){
        N = n;
        K = k;
        T = t;
        phi = phi;
        H = (int *)malloc(T * sizeof(int));
        S = (int *)malloc(T * sizeof(int));
        C = new int*[T];
        for (int i = 0; i < T; i++) {
            C[i] = new int[K];
        }


        for (uint64_t i = 0; i < K; ++i) {
            H[i] = std::experimental::randint(0, (int)N); 
            S[i] = std::experimental::randint(0, (int)N);
        }   

        for (uint64_t i = 0; i < t; ++i) {
            for (int j = 0; j < k; j++) {
                C[i][j] = 0;
            }
        }      

    }

    void add(uint64_t num) {
        for (int i = 0; i < T; i++) {
            int j = (H[i] * num) % K;
            C[i][j] = C[i][j] + rand_array[(S[i] * num) % 2];
        }
        // in order to get the heavy hitters, 
        // call the estimate for num and check if it exceeds the threshold then add to the heavy hitters list
        double freq = estimate(num);
        // cout<<freq<<endl;
        if (freq >= phi) {
            heavyhitters.insert(num);
            // cout<<num<<endl;
        }
    }

    double estimate(uint64_t num) {
        double f[T];
        for (int i = 0; i < T; i++) {
            int j = (H[i] * num) % K;
            f[i] = (double) ((rand_array[(S[i] * num) % 2] ) * C[i][j] ) / N;
            // cout<<f[i]<<endl;
        }
        double ans ;
        sort(f, f + T);
        if (T % 2 == 0) {
            ans  = (f[T/2] + f[(T/2) - 1]) / 2;
            return ans;
        }
        return f[(int)T/2];
    }

    std::set<uint64_t> heavyHitters(double phi) {
        return heavyhitters;
    }

};

int main() {
   cout << "Hello World. This is C++ program" << endl;
   CountSketch cs = CountSketch(100, 10, 10, 0.1);
   for (uint64_t i = 0; i < 10; ++i) {
       cs.add( i);
   }
   cs.add( 10);
   cs.add( 10);
   cs.add( 10);
   cs.add( 10);


   cout<<"cs.estimate(10) :"<<cs.estimate(10);
   std::set<uint64_t> hh = cs.heavyHitters(0.01);
   for (auto it = hh.begin(); it !=hh.end(); ++it)
        cout << ' ' << *it;
}