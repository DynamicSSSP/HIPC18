

#ifndef CREATETREE_HPP
#define CREATETREE_HPP

#include "SP_structure.hpp"
using namespace std;

/***Printing the Tree via Parent Relations ***/





/**** Creating the Rooted Tree **/

/**** Rooting the Tree Sequential Code ***/
void create_treeS( A_Network *X, A_Network *R, vector<RT_Vertex> *SSSP, int src, int p)
{
     int nodes=SSSP->size();
    
/*    //Initialize the Roots, Level and Distance of the nodes
#pragma omp parallel for num_threads(p)
    for(int i=0;i<nodes; i++)
    {
        
        if(i==src)
        { SSSP->at(i).Root=-1;}
        else
        { SSSP->at(i).Root=i;}
        SSSP->at(i).Level=0;
        SSSP->at(i).Dist =0.0;
        SSSP->at(i).degree=int(X->at(i).ListW.size());
    }
    
  */  
#pragma omp parallel for num_threads(p) 
    for(int i=0;i<X->at(src).ListW.size();i++)
    {
    
        int y=X->at(src).ListW[i].first;
        double mywt=X->at(src).ListW[i].second;
        
        if(mywt==-1){continue;}
        
        //mark the parent
        SSSP->at(y).Parent=src; //mark the parent
        SSSP->at(y).EDGwt=mywt; //mark the edgewt
        SSSP->at(y).Level=1; //mark the Level
        SSSP->at(y).Root=-1;
        SSSP->at(y).Dist=mywt;
        
    list<int> NodesQ;
    NodesQ.clear();
    //Add v to NodesQ
    NodesQ.push_back(y);
    
   
    Edge mye;
    
    while(!NodesQ.empty())
    {
        int thisn=NodesQ.front();
        NodesQ.pop_front();
        
        for(int i=0;i<X->at(thisn).ListW.size();i++)
        {
            int myn=X->at(thisn).ListW[i].first;
            
            //printf("%d ...%d %d\n", thisn, myn, CRT->at(myn).degree);
            double mywt=X->at(thisn).ListW[i].second;
            if(mywt==-1){continue;} //invalid edge
            if(SSSP->at(myn).Root==-1){continue;} //if Parent is src  then continue
             if(SSSP->at(i).Parent==myn){continue;} //if parent then continue
            
            
            //mark the parent
            SSSP->at(myn).Parent=thisn; //mark the parent
            SSSP->at(myn).EDGwt=mywt; //mark the edgewt
            SSSP->at(myn).Level=SSSP->at(thisn).Level+1; //mark the Level
            SSSP->at(myn).Root=SSSP->at(thisn).Root;
            SSSP->at(myn).Dist=SSSP->at(thisn).Dist+mywt;
            
                        //Put in Q
            if(SSSP->at(myn).degree>1)
            {NodesQ.push_front(myn);}
            
            
        } //end of for i--going thru Neighbors
        
        
    }//end of while for BFS
    }//end of for
    
    
    //Find the possible neighors for each node.
    //Possible neighbors are those with whom the edge wt is less than the distance
/*#pragma omp parallel for num_threads(p)
    for(int i=0;i<nodes;i++)
    {
        
        //Go thru neighbors of i in the remainder edges
        for(int j=0;j<R->at(i).ListW.size();j++)
        {
            
            int_double myval=R->at(i).ListW[j];
            int myn=myval.first;
            double mywt=myval.second;
            
            
            //Add as possible neighbor only if distance of neighbor is more than the edge weight
            if( (SSSP->at(myn).Dist > mywt))
            {SSSP->at(i).PossN.push_back(j);}
            
        }//end of for j
        
    }//end of for i
  */
  
    return;
}

/**** END OF FUNCTION ***/


/**Parallel Code ***/
void create_tree(A_Network *X, A_Network *R, vector<RT_Vertex> *SSSP, int src, int p)
{
    
    int nodes=X->size();
    int its=0; //number of iterations
    bool change=true; //marking whether the connections changed in the iteration
    
    //Initialize the Roots, Level and Distance of the nodes
    #pragma omp parallel for num_threads(p)
    for(int i=0;i<nodes; i++)
    {
        
        if(i==src)
        { SSSP->at(i).Root=-1;}
        else
        { SSSP->at(i).Root=i;}
        SSSP->at(i).Level=0;
        SSSP->at(i).Dist =0.0;
    }
    
    //Continue until no change in parameters of SSSP
    while(change)
    {
       //printf("Iteration:%d \n", its);
        change=false;
        
        /*for(int i=0;i<X->size();i++)
        { printf("%d::", i);
            printRT(&SSSP->at(i));}*/
        
    #pragma omp parallel for num_threads(p) 
    for(int i=0;i<nodes; i++)
    {
        
        //printf("%d--:%d \n", i,SSSP->at(i).Root );
        //Initialize the minimum root and the id of vertex having the min root
        int min_root=SSSP->at(i).Root;
        int min_n=i;
        double min_wt=0.0;
       
        
        //If root already minimum, then update neighbors
        if(min_root==-1)
        {continue;}
        
        //Go thru neighbors of i
        for(int j=0;j<X->at(i).ListW.size();j++)
        {
        
            int myn=X->at(i).ListW[j].first;
            double mywt=X->at(i).ListW[j].second;
            
              if(myn<j){continue;}
            
            //printf("%d: %f %d \n",i ,mywt, myn);
            
            //Find the neighbor with the smallest root
               if(SSSP->at(myn).Root < min_root)
               {min_root=SSSP->at(myn).Root;
                min_n=myn;
                min_wt=mywt;}
            
            //If minimum value found, break;
            if(min_root==-1) {break;}
        
        } //end of for j
        
        //If lower root found, then min_n is parent of i
        if(min_root < SSSP->at(i).Root)
        {
        SSSP->at(i).Parent=min_n;
        SSSP->at(i).Root=min_root;
        SSSP->at(i).Level=SSSP->at(min_n).Level+1;
        SSSP->at(i).Dist=SSSP->at(min_n).Dist+min_wt;
        SSSP->at(i).EDGwt=min_wt;
        change=true;
            
        }
        
       
        
    }//end of for i
        
        its++;
        
    }//end of while
    
    
    
    //Find the possible neighors for each node.
    //Possible neighbors are those with whom the edge wt is less than the distance
/*    #pragma omp parallel for num_threads(p)
    for(int i=0;i<nodes;i++)
    {
        
               //Go thru neighbors of i in the remainder edges
        for(int j=0;j<R->at(i).ListW.size();j++)
        {
            
            int_double myval=R->at(i).ListW[j];
            int myn=myval.first;
            double mywt=myval.second;
            
           
            //Add as possible neighbor only if distance of neighbor is more than the edge weight
            if( (SSSP->at(myn).Dist > mywt))
            {SSSP->at(i).PossN.push_back(j);}
            
        }//end of for j
        
    }//end of for i
  */  
    
    
}
/**** END OF FUNCTION ***/


#endif
