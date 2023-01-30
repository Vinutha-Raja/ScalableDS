#include <cstddef>
#include <cstdint>
#include<iostream>
#include <ostream>
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
    
   uint32_t getClusterCount(uint32_t u) {
      return sqrt(u);
   }


   uint32_t getHighBits(uint32_t X, uint32_t u) {
      uint32_t shift_val = log2(u)/ 2;
      uint32_t X_h = X >> shift_val;
      return X_h;
   }

   uint32_t getLowBits(uint32_t X, uint32_t u) {
      uint32_t shift_val = log2(u)/ 2;
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

       if (u_size <= 2) {
          summary = nullptr;
          clusters = vector<VanEmDBTree*> (0, nullptr);
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
      cout<<"treeNode->max_val: "<<treeNode->max_val<<" treeNode->min_val: "<<treeNode->min_val<<endl;
   } 
   if (X > treeNode->max_val) {
      treeNode->max_val = X;
      cout<<"1treeNode->max_val: "<<treeNode->max_val<<" treeNode->min_val: "<<treeNode->min_val<<endl;
   } else if (X < treeNode->min_val) {
      treeNode->min_val = X;
      cout<<"2treeNode->max_val: "<<treeNode->max_val<<" treeNode->min_val: "<<treeNode->min_val<<endl;
   }

   if (treeNode->u_size > 2) {
      uint32_t X_h = treeNode->getHighBits(X, treeNode->u_size);
      uint32_t X_l = treeNode->getLowBits(X, treeNode->u_size);
      cout<<"X_h: "<<X_h<<"X_l: "<<X_l<<endl;
      if (treeNode->clusters[X_h]->isEmpty) {
         cout<<"inserting X_h: "<<X_h<<" in summary cluster: "<<endl;
         insert(treeNode->summary, X_h);
         cout<<endl;
      } 
      cout<<"inserting X_l: "<<X_l<<" in cluster : "<< X_h<<endl;
      insert(treeNode->clusters[X_h], X_l);
      cout<<endl;
      
   }
}

successor_ret successor(VanEmDBTree* treeNode, uint32_t X){
   if ( treeNode->u_size == 2) {
      if ( !treeNode->isEmpty && X == 0 && treeNode->max_val == 1) {
         successor_ret *succ = new successor_ret();
         succ->successor = treeNode->max_val;
         succ->valid = true;
         return *succ;
      } else {
         successor_ret *succ = new successor_ret();
         succ->valid = false;
         return *succ;
      }
   } 

   // if (!treeNode->isEmpty && X == treeNode->max_val){
   //    cout<<"check with max value;"<<X<<", "<<treeNode->max_val<<endl;
   //    successor_ret *succ = new successor_ret();
   //    succ->successor = treeNode->max_val;
   //    succ->valid = true;
   //    return *succ;
   // } else 
   if (!treeNode->isEmpty && X < treeNode->min_val) {
      cout<<"check with min value;"<<endl;
      successor_ret *succ = new successor_ret();
      succ->successor = treeNode->min_val;
      succ->valid = true;
      return *succ;
   } else {
      uint32_t X_h = treeNode->getHighBits(X, treeNode->u_size);
      uint32_t X_l = treeNode->getLowBits(X, treeNode->u_size);
      // If X_h cluster is not empty then search for X_l in cluster[X_h]
      if ( !treeNode->clusters[X_h]->isEmpty && X_l < treeNode->clusters[X_h]->max_val) {
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
         successor_ret next_cluster = successor(treeNode->summary, X_h);
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
      // cout<<treeNode->min_val<<", "<<X<<endl;
      // cout<<treeNode->max_val<<", "<<X<<endl;
      return true;
   } 
   else {
      if (treeNode->u_size == 2) {
         return false;
      } else {
         uint32_t X_h = treeNode->getHighBits(X, treeNode->u_size);
         uint32_t X_l = treeNode->getLowBits(X, treeNode->u_size);
         // cout<<"X_h: "<<X_h<<"X_l: "<<X_l<<", "<<treeNode->u_size<<endl;
         return query(treeNode->clusters[X_h], X_l);
      }
   }

}

successor_ret getSuccessor(VanEmDBTree* treeNode, uint32_t X) {
   if (query(treeNode, X)) {
      successor_ret *succ = new successor_ret();
      succ->successor = X;
      succ->valid = true;
      return *succ;
   }
   successor_ret s = successor(treeNode, X);
   return s;
}

void print(VanEmDBTree* treeNode) {
   cout<<"min: "<<treeNode->min_val<<" max: "<<treeNode->max_val<<endl;
   if (treeNode->u_size <= 2) {
      return;
   } else {
      for (uint32_t i = 0; i < treeNode->clusters.size(); i++) {
         print(treeNode->clusters[i]);
      }
   }
   cout<<"-----------------"<<endl<<"\n";

}

int main() {
   cout << "Hello World. This is C++ program" << endl;
   VanEmDBTree* Vtree = new VanEmDBTree(pow(2, 16));
   // insert(Vtree, 1);
   // cout<<"min: "<<Vtree->min_val<<endl;
   // cout<<"max: "<<Vtree->max_val<<endl;
   for( uint32_t i = 2; i < 16; i= i+ 2){
      cout<<"inserting: "<<i<<endl;
      
      insert(Vtree, i);
      // print(Vtree);
      // cout<<"______________"<<endl<<"\n";
   }

   cout<<"min: "<<Vtree->min_val<<endl;

   cout<<"max: "<<Vtree->max_val<<endl;
   // print(Vtree);
   for (uint32_t i = 0; i < 16; i++ ){
      successor_ret s = getSuccessor(Vtree, i);
      if (s.valid) {
      cout<<"successor of "<<i<<" is "<<s.successor<<endl;
      } else {
         cout<<"successor of "<<i<<" is not valid"<<endl;
      }

   }
   


   // cout<<"successor of 2: "<<successor(Vtree, 2);
   // cout<<"successor of 3: "<<successor(Vtree, 3);
   for( uint32_t i = 1; i < 16; i++){
      cout<<"querying: "<<i<<endl;
      cout<<query(Vtree, i)<<endl;
      cout<<"______________"<<endl<<"\n";
   }
   // cout<< "query 1: "<<query(Vtree, 1);
   // cout<< "query 3: "<<query(Vtree, 3);
   // cout<< "query 2: "<<query(Vtree, 2);
   // cout<< "query 4: "<<query(Vtree, 4);

   return 0;

}