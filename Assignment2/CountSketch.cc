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

    typedef pair<uint64_t, uint64_t> p;
    uint64_t N;
    uint64_t K;
    int rand_array[2] = {-1, 1};
    int T;
    int* H;
    int* S;
    int** C;
    double phi;
    priority_queue<p, vector<p>, greater<p> > pq;
    
    // std::set<uint64_t> heavyhitters;
    std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyhitters;
    std::map<uint64_t, uint64_t, std::greater<uint64_t> > heavyhitters_final;

    CountSketch(uint64_t k, int t, double phi_val){
        N = 0;
        K = k;
        T = t;
        phi = phi_val;
        H = (int *)malloc(T * sizeof(int));
        S = (int *)malloc(T * sizeof(int));
        C = new int*[T];
        for (int i = 0; i < T; i++) {
            C[i] = new int[K];
        }

        for (int i = 0; i < T; ++i) {
            H[i] = std::experimental::randint(0, INT8_MAX);  // seeds
            S[i] = std::experimental::randint(0, INT8_MAX);
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
            // uint64_t j = MurmurHash64A(&num, 1, H[i]) % K;
            // int j = (H[i] * num) % K;
            uint64_t j = MurmurHash64A(&num, sizeof(num), H[i]) % K;
            C[i][j] = C[i][j] + rand_array[(S[i] * num) % 2];
        }

        // in order to get the heavy hitters, 
        // call the estimate for num and check if it exceeds the threshold then add to the heavy hitters list
        double freq = estimate(num);
        // cout<<freq<<endl;
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
        // uint64_t *n = &num;
        for (int i = 0; i < T; i++) {

            uint64_t j = MurmurHash64A(&num, sizeof(num), H[i]) % K;
            // int j = (H[i] * num) % K;
            f[i] = (double) ((rand_array[(S[i] * num) % 2] ) * C[i][j] ) ;
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
        // std::cout << "Total number of phi-hitters: "<< heavyhitters_final.size()<<"pq size: "<<pq.size()<<endl;
        // while(!pq.empty()) {
        //     pair<uint64_t, uint64_t> top = pq.top();
        //     std::cout << "Item: " << top.second << " Count: " << top.first << "\n";
        //     pq.pop();
        // }
        // cout<<"-------------------------";
        // for (auto it = heavyhitters_final.begin(); it != heavyhitters_final.end(); ++it) {
	    // 	std::cout << "Item: " << it->first << " Count: " << it->second << "\n";
	    // }

        return heavyhitters_final;
    }

};

// int main() {
//    cout << "Hello World. This is C++ program" << endl;
//    CountSketch cs = CountSketch(100, 10, 10, 0.01);
//    for (uint64_t i = 0; i < 10; ++i) {
//        cs.add( i);
//    }
//    cs.add( 10);
//    cs.add( 10);
//    cs.add( 10);
//    cs.add( 10);


//    cout<<"cs.estimate(10) :"<<cs.estimate(10);

//    std::map<uint64_t, uint64_t, std::greater<uint64_t> > HH = cs.heavyHitters(0.01);
//    std::cout << "CountSketch Heavy hitter items: \n";
//    for (auto it = HH.begin(); it != HH.end(); ++it) {
// 		std::cout << "Item: " << it->second << " Count: " << it->first << "\n";
//    }


// //    std::set<uint64_t> hh = cs.heavyHitters(0.01);
// //    for (auto it = hh.begin(); it !=hh.end(); ++it)
// //         cout << ' ' << *it;
// }