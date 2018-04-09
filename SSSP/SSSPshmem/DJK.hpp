#ifndef DJK_HPP
#define DJK_HPP

#include "SP_structure.hpp"
//#include "tree_info.hpp"
//#include "Insert_Delete.hpp"
//#include "rewire.hpp"
//#include <omp.h>
//#include<bits/stdc++.h>
//#include "SP_structure.hpp"
#include <queue>
#include <iostream>
#include <fstream>

using namespace std;

//typedef pair<int, double> iPair;



/*Sequential Dijkstra's Algorithm for SSSP with a given source */
void djk(int src, A_Network *X, A_Network *Y) {

    
    int nodes=X->size();
    
    
    
   //Set Edge Weights to high values
     double maxEdgeWeight = 100.00;
    
    //Initialize the distance for the nodes to INF
     vector<double> dist(nodes, maxEdgeWeight*nodes);
    //Initialize distance of src
    dist[src] = 0;
    
     //Initialize parents of nodes to -1
    vector<int> parent(nodes, -1);
    //Initialize parent of source to itself
    parent[src] = src;
    
    //Initialize weight of the edge connecting the node to the parent to -1
    vector<double> EdgeW(nodes, -1);
 
    
    //Create priotiy queue for traversing the graph
    priority_queue<int_double, vector<int_double>, greater<int_double> > prtQ;
    //Add source to priority Q
    prtQ.push(make_pair(0, src));
    
    //Mark whether node is in priorityQ--initialized to false
    vector<bool> inQ(nodes, false);
    //Set src to true
    inQ[src]=true;
    
    while(!prtQ.empty())
    {
        int u= prtQ.top().second;
        prtQ.pop();
        inQ[u]=false;
        
        
        //For all neighbors of thisn
        for(int i=0;i<X->at(u).ListW.size();i++)
        {
            int v = X->at(u).ListW[i].first;
            double weight = X->at(u).ListW[i].second;
            
            
            //If relaxed update weight and push to priorityQ
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                parent[v]=u;
                EdgeW[v]=weight;
                
                if(!inQ[v])
                {prtQ.push(make_pair(dist[v], v));
                 inQ[v]=true;}
                    
            }//end of if

        }//end of going through neighbors
        
        
    }//end of while
    
 
    
 //===Adding the Edges to Y
    
    //Create rows for Y
    ADJ_Bundle AList;
    for(int i=0;i<nodes;i++)
    {
        //Create Rows for Y;
        AList.Row=i;
        AList.ListW.clear();
        Y->push_back(AList);
    }//end of for
    
    //Add edges according to Parent Relation
    int_double myval;
    int j;
    for(int i=0;i<nodes;i++)
    {
        
        //If source continue
        if(i==src){continue;}
        
        //If edge in different component, then continue
        if(parent[i]==-1){continue;}
        
        j=parent[i];
        
     //   printf("%d :%d \n",i,j);
        
        
        myval.first=j;
        myval.second=EdgeW[i];
        Y->at(i).ListW.push_back(myval);
        
        myval.first=i;
        myval.second=EdgeW[i];
        Y->at(j).ListW.push_back(myval);
      
          
          
    }//end of for
    
        

    //Sort the edge lists in  Y
    for(int i=0;i<Y->size();i++)
    {
        sort(&Y->at(i).ListW);
        
    }//end of for

    
    return;
}

/**** END OF FUNCTION ***/

#endif

    
