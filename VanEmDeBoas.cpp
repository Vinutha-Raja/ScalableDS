#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

class successor_ret{
public:
   bool valid;
   uint32_t successor;
};

class VanEmDBTree{

public:
   uint32_t u_size;
   bool isEmpty;
   uint32_t max_val;
   uint32_t min_val;
   VanEmDBTree* summary;
   vector<VanEmDBTree*> clusters;
   uint8_t finalClusters[256];  
    
   uint32_t getClusterCount(uint32_t u) {
      // cout<<"getClusterCount"<<ceil(sqrt(u));
      return ceil(sqrt(u));
   }


   uint32_t getHighBits(uint32_t X, uint32_t u) {
      uint32_t shift_val = ceil(log2(u))/ 2;
      uint32_t X_h = X >> shift_val;
      return X_h;
   }

   uint32_t getLowBits(uint32_t X, uint32_t u) {
      uint32_t shift_val = ceil(log2(u))/ 2;
      uint32_t X_h = X >> shift_val;
      uint32_t X_l = X - (X_h << shift_val);
      return X_l;
   }

   uint32_t calculateValue(uint32_t X_h, uint32_t X_l, uint32_t u) {
      uint32_t clusterNo = getClusterCount(u);
      return X_h * clusterNo + X_l;
   }

   VanEmDBTree(uint32_t u){
       u_size = u;
       isEmpty = true;
       min_val = 0;
       max_val = 0;

       if (u_size <= 256) {
          summary = nullptr;
          finalClusters[0] = UINT8_MAX;
          for (u_int8_t i = 1; i < UINT8_MAX; i++){
            finalClusters[i] = 0;
          }
       } 

       else {
          uint32_t numClusters = getClusterCount(u);
          summary = new VanEmDBTree(numClusters);
          clusters = vector<VanEmDBTree *> (numClusters, nullptr);
          for (uint32_t i = 0; i < numClusters; i++) {
             clusters[i] = new VanEmDBTree(numClusters);
          }
       }
    }
};

void insert(VanEmDBTree* treeNode, uint32_t X){
   // If empty set X to min and max and insert X
   if ( treeNode->isEmpty ) {  
      treeNode->max_val = X;
      treeNode->min_val = X;
      treeNode->isEmpty = false;
   } 
   if (X > treeNode->max_val) {
      treeNode->max_val = X;
   } else if (X < treeNode->min_val) {
      treeNode->min_val = X;
   }

   if (treeNode->u_size <= 256) {
      treeNode->finalClusters[X] = X;
   }

   if (treeNode->u_size > 256) {
      uint32_t X_h = treeNode->getHighBits(X, treeNode->u_size);
      uint32_t X_l = treeNode->getLowBits(X, treeNode->u_size);
      if (treeNode->clusters[X_h]->isEmpty) {
         insert(treeNode->summary, X_h);
      } 
      insert(treeNode->clusters[X_h], X_l);      
   }
}

successor_ret successor(VanEmDBTree* treeNode, uint32_t X){
   if ( treeNode->u_size <= 256) {
      if (!treeNode->isEmpty) {
         for (uint8_t i = X ; i <= UINT8_MAX; i++) {
            if (treeNode->finalClusters[i] == i) {
               successor_ret *succ = new successor_ret();
               succ->successor = i;
               succ->valid = true;
               // cout<<"X: "<<X<<"returning...."<<unsigned(i)<<"min:"<<treeNode->min_val;
               return *succ;
            }
         }
         successor_ret *succ = new successor_ret();
         succ->valid = false;
         return *succ;
      } else {
         successor_ret *succ = new successor_ret();
         succ->valid = false;
         return *succ;
      }
   }
   if (!treeNode->isEmpty && X < treeNode->min_val) {
      successor_ret *succ = new successor_ret();
      succ->successor = treeNode->min_val;
      succ->valid = true;
      return *succ;
   } else if (!treeNode->isEmpty && X > treeNode->max_val) {
      successor_ret *succ = new successor_ret();
      succ->valid = false;
      return *succ;
   }
   else {
      uint32_t X_h = treeNode->getHighBits(X, treeNode->u_size);
      uint32_t X_l = treeNode->getLowBits(X, treeNode->u_size);
      // If X_h cluster is not empty then search for X_l in cluster[X_h]
      if ( !treeNode->clusters[X_h]->isEmpty && X_l <= treeNode->clusters[X_h]->max_val) {
         successor_ret low_bits = successor(treeNode->clusters[X_h], X_l);
         if (low_bits.valid) {
            uint32_t value = treeNode->calculateValue(X_h, low_bits.successor, treeNode->u_size);
            successor_ret *succ = new successor_ret();
            succ->successor = value;
            succ->valid = true;
            return *succ;
         }
      } else {
         // Searching for next non empty cluster in summary
         successor_ret next_cluster = successor(treeNode->summary, X_h+1);
         if (!next_cluster.valid) {
            successor_ret *succ = new successor_ret();
            succ->valid = false;
            return *succ;
         } else {
            // search for the minimum value in the next_Cluster
            uint32_t next_cluster_num = next_cluster.successor;
            uint32_t value = treeNode->calculateValue(next_cluster_num, treeNode->clusters[next_cluster_num]->min_val, treeNode->u_size);
            successor_ret *succ = new successor_ret();
            succ->successor = value;
            succ->valid = true;
            return *succ;
         }

      }

   }
   successor_ret *succ = new successor_ret();
   succ->valid = false;
   return *succ;
}


bool query(VanEmDBTree* treeNode, uint32_t X) {

   if (treeNode->isEmpty || X < treeNode->min_val || X > treeNode->max_val) {
      return false;
   }

   if (treeNode->min_val == X || treeNode->max_val == X){
      return true;
   } 

   if (treeNode->u_size <= 256) {
      if (treeNode->finalClusters[X] == X) {
         return true;
      } else {
         return false;
      }
   } 

   else {
      uint32_t X_h = treeNode->getHighBits(X, treeNode->u_size);
      uint32_t X_l = treeNode->getLowBits(X, treeNode->u_size);
      return query(treeNode->clusters[X_h], X_l);
   }

}

successor_ret getSuccessor(VanEmDBTree* treeNode, uint32_t X) {
   successor_ret s = successor(treeNode, X);
   return s;
}

void print(VanEmDBTree* treeNode) {
   cout<<"min: "<<treeNode->min_val<<" max: "<<treeNode->max_val<<endl;
   if (treeNode->u_size <= 256) {
      for (uint8_t i = 0; i < UINT8_MAX; i++ ){
         cout<<unsigned(treeNode->finalClusters[i])<<",";
      }
   } else {
      for (uint32_t i = 0; i < treeNode->clusters.size(); i++) {
         print(treeNode->clusters[i]);
      }
   }
   cout<<"-----------------"<<endl<<"\n";

}

// int main() {
//    cout << "Hello World. This is C++ program" << endl;
//    uint32_t u = uint32_t(pow(2, 32) - 1);
//    cout<<"u: "<<u;
//    VanEmDBTree* Vtree = new VanEmDBTree(UINT32_MAX);
//    // insert(Vtree, 1);
//    // cout<<"min: "<<Vtree->min_val<<endl;
//    // cout<<"max: "<<Vtree->max_val<<endl;
//    // for( uint32_t i = 512; i < 131072; i= i+ 5){
//    //    cout<<"inserting: "<<i<<endl;
      
//    //    insert(Vtree, i);
//    //    // print(Vtree);
//    //    // cout<<"______________"<<endl<<"\n";
//    // }
// // 3304136336,2597322298,2586798070,1708109324,1114020553,3093839019,4077717216,2731916208,3199594294,403243621
// // 2355525110,3995499145,1410364849,2415283070,2828064020,560803598,686011081,1321815037,597994729,2758805216 - 4125848878
//    insert(Vtree, 2355525110);
//    insert(Vtree, 3995499145);
//    insert(Vtree, 1410364849);
//    insert(Vtree, 2415283070);
//    insert(Vtree, 2828064020);
//    insert(Vtree, 560803598);
//    insert(Vtree, 686011081);
//    insert(Vtree, 1321815037);
//    insert(Vtree, 597994729);
//    // insert(Vtree, 2731916208);
//    insert(Vtree, 2758805216);

//    cout<<"min: "<<Vtree->min_val<<endl;

//    cout<<"max: "<<Vtree->max_val<<endl;
//    // print(Vtree);
//    for (uint32_t i = 4125848878; i < 4125848878 + 1; i++ ){
//       successor_ret s = getSuccessor(Vtree, i);
//       if (s.valid) {
//       cout<<"successor of "<<i<<" is "<<s.successor<<endl;
//       } else {
//          cout<<"successor of "<<i<<" is not valid"<<endl;
//       }

//    }
   


//    // cout<<"successor of 2: "<<successor(Vtree, 2);
//    // cout<<"successor of 3: "<<successor(Vtree, 3);
//    for( uint32_t i = 1; i < 100; i++){
//       cout<<"querying: "<<i<<endl;
//       cout<<query(Vtree, i)<<endl;
//       cout<<"______________"<<endl<<"\n";
//    }
//    // cout<< "query 1: "<<query(Vtree, 1);
//    // cout<< "query 3: "<<query(Vtree, 3);
//    // cout<< "query 2: "<<query(Vtree, 2);
//    // cout<< "query 4: "<<query(Vtree, 4);

//    return 0;

// }