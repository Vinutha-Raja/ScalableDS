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

class CountMinSketch{
public:
    typedef pair<uint64_t, uint64_t> p;
    uint64_t N;
    uint64_t K;
    int T;
    int* H;
    int** C;
    double phi;
    // std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyhitters;
    std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyhitters_final;
    priority_queue<p, vector<p>, greater<p> > pq;

    CountMinSketch(uint64_t k, int t, double phi_val){
        N = 0;
        K = k;
        T = t;
        phi = phi_val;
        H = (int *)malloc(T * sizeof(int));
        C = new int*[T];
        for (int i = 0; i < T; i++) {
            C[i] = new int[K];
        }

        for (int i = 0; i < T; ++i) {
            H[i] = std::experimental::randint(0, (int)N); 
        }   

        for (int i = 0; i < t; ++i) {
            for (uint64_t j = 0; j < k; j++) {
                C[i][j] = 0;
            }
        }      

    }

    uint64_t size() {
        uint64_t s = T * K * sizeof(uint64_t);
        return s;
    }

    void add(uint64_t num) {
        N += 1; 
        for (int i = 0; i < T; i++) {

            // int j = (H[i] * num) % K;
            uint64_t j = MurmurHash64A(&num, sizeof(num), H[i]) % K;

            // uint64_t j = MurmurHash64A(&num, 1, H[i]) % K;
            // cout<<num<<"j"<<(H[i] * num) % K<<endl;;
            // cout<<"C[i][j]: "<<C[i][j]<<endl;
            // for (uint64_t i = 0; i < T; ++i) {
            //     for (int j = 0; j < K; j++) {
            //         cout<<' '<<C[i][j];
            //     }
            //     cout<<endl;
            // } 
            C[i][j] = C[i][j] + 1;
        }
        // in order to get the heavy hitters, 
        // call the estimate for num and check if it exceeds the threshold then add to the heavy hitters list
        double freq = estimate(num);
        // cout<<freq<<endl;
        // if (freq >= phi) {
        //     heavyhitters.insert(std::make_pair(num, freq * N));
        //     // heavyhitters.insert(num);
        //     // cout<<num<<endl;
        // }
        if (freq >= (phi * N)) {
            // cout<<"num: "<<num<<"phi: "<<phi<<"freq: "<<freq<<"count: "<<(freq * N)<<endl;

            // check the map for the item, if not present add to map and min heap
            // If present already update the count in the map, dont add to the min heap
            // While removing from the min heap. Check if the item is in the map, 
            // check if it is phi hitter: 
            // if yes, add to min heap.
            // if not, remove from the min heap and the map

            // Update the hash map
            auto const result1 = heavyhitters_final.insert(std::make_pair(num, freq ));
            if (not result1.second) { result1.first->second = freq; } 
            else {
                pq.push(make_pair(freq , num));
            }
            
            if (!pq.empty()) {
                pair<uint64_t, uint64_t> top = pq.top();
                while (!pq.empty() && top.first < (phi * N) ) {
                    // cout<<"removing "<<top.second<<" count: "<<top.first<<" required: "<<(phi * N)<<endl;
                    pq.pop();
                    if (heavyhitters_final.find(top.second) != heavyhitters_final.end()) {
                        uint64_t val = heavyhitters_final[top.second];
                        if (val >= (phi * N)) {
                            // cout<<"pushing again to pq: "<<top.second<<" :"<<val;
                            pq.push(make_pair(val , top.second));
                        } else {
                            heavyhitters_final.erase(top.second);
                            // cout<<"removing from hash map"<<top.second<<endl;
                        }
                    }
                    // remove the element from heavy hitters finals map as well
                    top = pq.top();
                }
            }
        }
    }

    double estimate(uint64_t num) {
        double f[T];
        for (int i = 0; i < T; i++) {
            // int j = (H[i] * num) % K;
            uint64_t j = MurmurHash64A(&num, sizeof(num), H[i]) % K;
            f[i] = (double) (C[i][j]) ;
            // cout<<f[i]<<endl;
        }
        // double ans ;
        sort(f, f + T);
        return f[0];
    }

    std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyHitters(double phi) {
        return heavyhitters_final;
    }

};

// int main() {
//    cout << "Hello World. This is C++ program" << endl;
//    CountMinSketch cs = CountMinSketch(100, 10, 10, 0.1);
//    for (uint64_t i = 0; i < 10; ++i) {
//        cs.add( i);
//    }
//    cs.add( 10);
//    cs.add( 10);
//    cs.add( 10);
//    cs.add( 10);

//     for (uint64_t i = 0; i < cs.T; ++i) {
//         for (int j = 0; j < cs.K; j++) {
//             cout<<' '<<cs.C[i][j];
//         }
//         cout<<endl;
//     }  

//     for (uint64_t i = 0; i < cs.T; ++i) {
//         cout<<cs.H[i]<<endl;
//     }

//    cout<<"cs.estimate(10) :"<<cs.estimate(10);
//    std::set<uint64_t> hh = cs.heavyHitters(0.01);
//    for (auto it = hh.begin(); it !=hh.end(); ++it)
//         cout << ' ' << *it;
// }