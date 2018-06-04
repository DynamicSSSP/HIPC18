#ifndef INSDEL_HPP
#define INSDEL_HPP

#include "SP_structure.hpp"
//#include "tree_info.hpp"
//#include "rewire.hpp"
using namespace std;

//Process all Changes
//At the end of executing this function
//All Edges that can be inserted to the SSSP tree will be inserted, and their Parents and Edge weights updated
//All Edges to be inserted into the Remainder List will be added
//All Edges to be deleted from Key edges will be deleted, and their Parents and Edge weights updated
//All Edges to be deleted from Remainder Edges will be marked as deleted
//The rest of the nodes may not be updated.
void edge_update(vector<xEdge> *allChange, A_Network *X,vector<RT_Vertex> *SSSP, A_Network *R, double *maxW, int *te, int p)
{
    
    vector<int> Edgedone;
    vector<double> UpdatedDist;

    int iter=0;
   
        //Mark how the edge is processed
        Edgedone.clear();
        Edgedone.resize(allChange->size(),-1);
        
        //Store the updated distance value
        UpdatedDist.clear();
        UpdatedDist.resize(X->size(),0.0);

     printf("%d %d:%d \n", UpdatedDist.size(), SSSP->size(),X->size());        
        //Initialize with current distance for each node
       #pragma omp parallel for num_threads(p)
        for(int i=0; i<X->size(); i++)
        { UpdatedDist[i]=SSSP->at(i).Dist;}
        
    
    bool change=true;
    
        printf("Insertions %d %d %d  \n", iter, allChange->size(), Edgedone.size());
    

   int numS=allChange->size();
    //numS=10000000; 
  #pragma omp parallel for num_threads(p) 
    for(int xe=0; xe<numS; xe++)
    {
        //get the edge
        xEdge myxE=allChange->at(xe);
        Edge mye=myxE.theEdge;
        
        /***Conditions for ignoring edges to insert ***/
        
        //check if edge not same tree--then do not insert
        if(mye.node1>X->size()||mye.node2>X->size())
        { Edgedone[xe]=0; //mark to not add
            continue;}
  
 if(SSSP->at(mye.node1).Root != SSSP->at(mye.node2).Root)
        { Edgedone[xe]=0; //mark to not add
            continue;}      
        /*** End of conditions to ignore ***/
       
        


        //if insert
        if(myxE.inst==1)
        {
        //We check the distances based on updateddist, to cull some insertion edges
        //In case of conflicts, actual distance remains correct
            
            //Default is remainder edge
            Edgedone[xe]=2;
    
           //Check if edge exists--then dont insert 
                for(int k=0;k<X->at(mye.node1).ListW.size();k++)
                {
                    //TEPS:
                    *te=*te+1;
                    int myn=X->at(mye.node1).ListW[k].first;
                    if(myn==mye.node2)
                    {
                        break;
                    }
                   continue; 
                }//end of for


            //Check twice once for  n1->n2 and once for n2->n1
            for(int yy=0;yy<2;yy++)
            {
                int node1, node2;
                if(yy==0)
                {node1=mye.node1;
                node2=mye.node2;}
                else
                {node1=mye.node2;
                    node2=mye.node1;}
                
              //  printf("%d:%f:::%d::%f:::%f \n", node1, UpdatedDist[node1],node2, UpdatedDist[node2], mye.edge_wt);
        //Check whether node1 is relaxed
        if(UpdatedDist[node2]> UpdatedDist[node1]+mye.edge_wt)
        {
            //Update Parent and EdgeWt
            SSSP->at(node2).Parent=node1;
            SSSP->at(node2).EDGwt=mye.edge_wt;
            UpdatedDist[node2]=UpdatedDist[SSSP->at(node2).Parent]+SSSP->at(node2).EDGwt;
            SSSP->at(mye.node2).Update=true;
            
          //Mark Edge to be added--node1 updated
            Edgedone[xe]=1;
            break;
        }
            
            }//end of for
        
        }//end of if insert
        
        else  //if deleted
        {
            Edgedone[xe]=3;
            //Check if edge exists in the tree
                //this will happen if node1 is parentof node or vice-versa
            bool iskeyedge=false;
            
           // printf("XXX:%d:%d \n",mye.node1, mye.node2 );
            
                    //Mark edge as deleted
                 if(SSSP->at(mye.node1).Parent==mye.node2)
                 {
                     //printf("YYY:%d:%d \n",mye.node1, mye.node2 );
                     SSSP->at(mye.node1).EDGwt=*maxW;
                     SSSP->at(mye.node1).Update=true;
                     iskeyedge=true;
                 }
                 else {
                 //Mark edge as deleted
                    if(SSSP->at(mye.node2).Parent==mye.node1)
                    {
                       // printf("ZZZ:%d:%d \n",mye.node1, mye.node2 );
                        SSSP->at(mye.node2).EDGwt=*maxW;
                        SSSP->at(mye.node2).Update=true;
                        iskeyedge=true;
                    }
                 }
            
            
             //If  Key Edge Delete from key edges
            //Set weights to -1;
            if(iskeyedge)
            {
            
                for(int k=0;k<X->at(mye.node1).ListW.size();k++)
                {
                    //TEPS:
                    *te=*te+1;
                    int myn=X->at(mye.node1).ListW[k].first;
                    if(myn==mye.node2)
                    {
                        X->at(mye.node1).ListW[k].second=-1;
                        break;
                    }
                    
                }//end of for
                
                for(int k=0;k<X->at(mye.node2).ListW.size();k++)
                {
                    //TEPS:
                    *te=*te+1;
                    int myn=X->at(mye.node2).ListW[k].first;
                    if(myn==mye.node1)
                    {
                        X->at(mye.node2).ListW[k].second=-1;
                        break;
                    }
            
                }
            }//end of if
                //If not Key Edge Delete from remainder edges
                //Set weights to -1;
            else
            {
                //==From remainder edges
                //TBD: Only delete for mye.node1< mye.node2
                
                for(int k=0;k<R->at(mye.node1).ListW.size();k++)
                {
                    //TEPS:
                    *te=*te+1;
                    int myn=R->at(mye.node1).ListW[k].first;
                     if(myn==mye.node2)
                     {
                         R->at(mye.node1).ListW[k].second=-1;
                         break;
                     }
                
                }//end of for
                
                for(int k=0;k<R->at(mye.node2).ListW.size();k++)
                {
                    //TEPS:
                    *te=*te+1;
                    int myn=R->at(mye.node2).ListW[k].first;
                    if(myn==mye.node1)
                    {
                        R->at(mye.node2).ListW[k].second=-1;
                        break;
                    }
                    
                }//end of for
                
            }//end of if
        
        }//end of else if deleted
        
    }//end of for--going through the inputs
    

    
   // printf("donee \n");
    
        //Add Edge to Remainder Edges
        //This is done sequentially due to vector addition not being thread safe
    /*for(int xe=0; xe<numS; xe++)
    {
        if(Edgedone[xe]==2)
        {
            xEdge myxE=allChange->at(xe);
            Edge mye=myxE.theEdge;
            
            int_double myval;
            myval.second=mye.edge_wt;
            
            myval.first=mye.node2;
            R->at(mye.node1).ListW.push_back(myval);
            
            myval.first=mye.node1;
            R->at(mye.node2).ListW.push_back(myval);
            
        }
    }
    //End of Adding edges
    */
    
        //Go over the inserted edges to see if they need to be changed
    change=true;
     while(change)
   {
       change=false;
#pragma omp parallel for num_threads(p)
        for(int xe=0; xe<numS; xe++)
        {
            
            if(Edgedone[xe]==1)
            {
                
                //get the edge
                xEdge myxE=allChange->at(xe);
                Edge mye=myxE.theEdge;
                
                //reset it to 0
                Edgedone[xe]=0;
                
                
                  int node1, node2;
                    if(UpdatedDist[mye.node1]> UpdatedDist[mye.node2])
                    {node1=mye.node1;
                        node2=mye.node2;}
                    else
                    {node1=mye.node2;
                    node2=mye.node1;}
                    
                    //printf("TTT:%d:%f %d:%f %f \n", node1, UpdatedDist[node1], node2, UpdatedDist[node2], mye.edge_wt);
             //Check if some other edge was added--mark edge to be added
                if(UpdatedDist[node1]> UpdatedDist[node2]+mye.edge_wt)
                {Edgedone[xe]=1;}
               
                //Check if correct edge wt was written--mark edge to be added
                if( (SSSP->at(node1).Parent==node2) && (SSSP->at(node1).EDGwt>mye.edge_wt))
                   {Edgedone[xe]=1;}
                
                   
                   if(Edgedone[xe]==1)
                   {
                   //Update Parent and EdgeWt
                   SSSP->at(node1).Parent=node2;
                   SSSP->at(node1).EDGwt=mye.edge_wt;
                   UpdatedDist[node1]=UpdatedDist[SSSP->at(node1).Parent]+SSSP->at(node1).EDGwt;
                   SSSP->at(node2).Update=true;
                 change =true;
                   }
             
                
            }//end of if
            
       
        }//end of for
   }//end of while change
   
    //Update the distances
    //Initialize with current distance for each node
#pragma omp parallel for num_threads(p)
    for(int i=0; i<X->size(); i++)
    {
        //do not update source node
        int px=SSSP->at(i).Parent;
        if(SSSP->at(i).Parent==-1){continue;}
        

         if(i==SSSP->at(px).Parent)
 {printf("DP: %d:%d %d:%d \n", i, SSSP->at(i).Parent, px, SSSP->at(px).Parent);}

        if(SSSP->at(i).EDGwt==*maxW)
        {SSSP->at(i).Dist=*maxW; SSSP->at(i).Update=true; continue;}
        
        if(UpdatedDist[p]==*maxW)
        {SSSP->at(i).Dist=*maxW; SSSP->at(i).Update=true; continue;}
        
        /*if(SSSP->at(i).Dist > UpdatedDist[SSSP->at(i).Parent]+SSSP->at(i).EDGwt)
        { SSSP->at(i).Dist=UpdatedDist[SSSP->at(i).Parent]+SSSP->at(i).EDGwt;
            SSSP->at(i).Update=true;}*/
        
        if(SSSP->at(i).Dist > UpdatedDist[i])
        { SSSP->at(i).Dist=UpdatedDist[i];
            SSSP->at(i).Update=true;}
    }
    
   printf("%d--\n", *te); 
    return;
}
/**** End of Function*****/





//At the end of this function  the distance of other vertices that were affected by the insertion and deletion will be updated.
//The disconnected parts will remain disconnected

void rest_update(A_Network *X,vector<RT_Vertex> *SSSP, A_Network *R, double *maxW, int *te, int p)
{

        int nodes=X->size();
        int its=0; //number of iterations
        bool change=true; //marking whether the connections changed in the iteration

   
     vector<double> UpdatedDist;
    //Store the updated distance value
    UpdatedDist.clear();
    UpdatedDist.resize(X->size(),0.0);
    
   vector<double> OldUpdate;
    //Store the old updated distance value
    OldUpdate.clear();
    OldUpdate.resize(X->size(),0.0);

    //Store Change for each node
    vector<bool> mychange;
    //Store the updated distance value
    mychange.clear();
    mychange.resize(X->size(),false);

    //Initialize with current distance for each node
#pragma omp parallel for num_threads(p)
    for(int i=0; i<X->size(); i++)
    { UpdatedDist[i]=SSSP->at(i).Dist;
    OldUpdate[i]=UpdatedDist[i];} 
    
     int iter=0;
      while(change && its < 70)
      {
          printf("Iteration:%d \n", its);
       
          change=false;
#pragma omp parallel for num_threads(p)
        for(int i=0;i<nodes; i++)
        {
            
            //If i is updated--update its neighbors as required
            if(SSSP->at(i).Update)
            {
                mychange[i]=false;
                int px=SSSP->at(i).Parent;
            
                //For its nieghbors in X
                for(int j=0;j<X->at(i).ListW.size();j++)
                {
                    //TEPS:
                    *te=*te+1;
                    int myn=X->at(i).ListW[j].first;
                    double mywt=X->at(i).ListW[j].second;
                    
                    if(mywt<0){continue;}
                    
                    
                   // printf("RR %d %d %f %f %f \n", i, myn,UpdatedDist[i],UpdatedDist[myn], mywt);

                    //Check that the Edge weight matches the parent
                    //NOTE:using atomic structures can reduce this step
                    if(myn==px)
                    {
                       // printf("hhh \n");
                        if(SSSP->at(i).EDGwt==*maxW) {continue;}
                        
                        double mydist=UpdatedDist[SSSP->at(i).Parent]+SSSP->at(i).EDGwt;
                        
//if(its>30)
 //{ printf("\n, VVVV%d %d:%d %f %f:%f \n", i, myn, SSSP->at(i).Parent, UpdatedDist[SSSP->at(i).Parent],mywt, SSSP->at(i).EDGwt);}
                        if( (SSSP->at(i).EDGwt !=mywt) || (UpdatedDist[i]>mydist ))
                        {
                            SSSP->at(i).EDGwt=mywt;
                            if(UpdatedDist[i]>=*maxW )
                            {UpdatedDist[i]=*maxW;}
                            else
                            {UpdatedDist[i]=mydist;}
                           
//if(UpdatedDist[i] >OldUpdate[i])
 //{ printf("\n, %d %d %f %f %f \n", i, myn, mywt, UpdatedDist[i], OldUpdate[i]);} 
//{printf("\n QQQ%d-%d..%f:%d\n",i,SSSP->at(i).Parent, UpdatedDist[i], its);}
                            
                            //SSSP->at(myn).Update=true;
                            mychange[i]=true;
                            change=true;
                        } //end of if
                        
                    //    double mycontinue;
                        
 continue;
                    }//end of if

                    
                    //Update for Neigbors
                   // if(UpdatedDist[myn]>=*maxW) {continue;}
                    
                    //If distance INF set all neghbors distance to INF at the first iteration
                    if(UpdatedDist[i]>=*maxW && its==0)
                    {
                        UpdatedDist[myn]=*maxW;
                        SSSP->at(myn).Update=true;
                        //mychange[i]=true;
                        change=true;
                        
                    }
                    else {
                      //If Distance of myn is larger--make i its parent
                      if(UpdatedDist[myn]>UpdatedDist[i]+mywt)
                      {
                          //printf("came heruu e\n");

                          SSSP->at(myn).Parent=i;
                          SSSP->at(myn).EDGwt=mywt;
                          UpdatedDist[myn]=UpdatedDist[SSSP->at(myn).Parent]+SSSP->at(myn).EDGwt;
                         
                          SSSP->at(myn).Update=true;
                          //mychange[i]=true;
                          change=true;
                      }
                    }//end of else
                    
                }//end of for
             
                 //++++++++++++++++++++//
    
                //Check Possible Neighbors in R
               //for(int j=0;j<SSSP->at(i).PossN.size();j++)
                for(int j=0;j<R->at(i).ListW.size();j++)
                {
                    //TEPS:
                    *te=*te+1;
                   // int k=SSSP->at(i).PossN[j];
                    int k=j;
                    
                    int myn=R->at(i).ListW[k].first;
                    double mywt=R->at(i).ListW[k].second;
                    
                   //printf("SS %d %d %f\n", i, myn, mywt);
                    
                    //check if edge is deleted
                    if(mywt<0){continue;}
                    
                    //Check that the Edge weight && Distance matches the parent
                    //NOTE:using atomic structur es can reduce this step
                    if(myn==px)
                    {
                        //printf("hhhXX %d %f\n", myn, mywt);
                         if(SSSP->at(i).EDGwt==*maxW) {continue;}
                        
                        double mydist=UpdatedDist[SSSP->at(i).Parent]+SSSP->at(i).EDGwt;
                        
//if(its>30)
 //{ printf("\n, NNNN%d %d:%d %f %f:%f \n", i, myn, SSSP->at(i).Parent, UpdatedDist[SSSP->at(i).Parent],mywt, SSSP->at(i).EDGwt);}
                        if( (SSSP->at(i).EDGwt !=mywt) || (UpdatedDist[i]>mydist ))                        {
                            SSSP->at(i).EDGwt=mywt;
                            if(UpdatedDist[i]>=*maxW )
                            {UpdatedDist[i]=*maxW;}
                            else
                            {UpdatedDist[i]=mydist;}
                            
                            
//if(UpdatedDist[i] >OldUpdate[i])
 //{ printf("\n, %d %d %f %f %f \n", i, myn, mywt, UpdatedDist[i], OldUpdate[i]);} 
//{printf("\n KKKK%d-%d..%f:%d\n",i,SSSP->at(i).Parent, UpdatedDist[i], its);}
                            //SSSP->at(myn).Update=true;
                            mychange[i]=true;
                            change=true;
                        } //end of if
                    continue;    
                    }//end of if
                    
                    
                    //Update for Possible Neighbors
                    if(UpdatedDist[myn]>=*maxW) {continue;}
                    
                    
                    
                    
                   // printf("TTT %d %f \n", i, UpdatedDist[i]);
                    //Connect disconnected vertices--after first iter
                        if((UpdatedDist[i]>=*maxW) &&(UpdatedDist[myn]<*maxW) )
                        {
                            
                            
                                
                                //printf("came her e %d::%d %f %f %f\n", i,myn,UpdatedDist[myn],UpdatedDist[i],mywt);
                            
                            if(its>0 && (SSSP->at(myn).Parent!=i))
                            {
                                SSSP->at(i).Parent=myn;
                                SSSP->at(i).EDGwt=mywt;
                                UpdatedDist[i]=UpdatedDist[SSSP->at(i).Parent]+SSSP->at(i).EDGwt;
                            }
                                //SSSP->at(myn).Update=true;
                                mychange[i]=true;
                                change=true;
                            
                           
                            
                            //printf("bbb %d %f %d  %f\n", i, UpdatedDist[i], SSSP->at(i).Parent, SSSP->at(i).EDGwt);
                            
                         //   continue;
                        }
                    
                        //If Distance of myn is larger--make i its parent
                        if(UpdatedDist[myn]>UpdatedDist[i]+mywt)
                        {
                         // printf("came herXXe %d::%d %f %f %f\n", i,myn,UpdatedDist[myn],UpdatedDist[i],mywt);
                            SSSP->at(myn).Parent=i;
                            SSSP->at(myn).EDGwt=mywt;
                            
                            if(UpdatedDist[myn]>=*maxW )
                            {UpdatedDist[myn]=*maxW;}
                            else
                            {UpdatedDist[myn]=UpdatedDist[SSSP->at(myn).Parent]+SSSP->at(myn).EDGwt;}
                            
                            SSSP->at(myn).Update=true;
                            //mychange[i]=true;
                            change=true;
                        }
                    
                    
                  
                    
                }//end of for
                
                //if no change occured then update is done
                if(!mychange[i])
                 {SSSP->at(i).Update=false;}
                else
                {SSSP->at(i).Update=true;}
  
                
            }//end of if Updated
          
        
            
            
        }//end of for all nodes
          
    //Check if distance was updated
#pragma omp parallel for num_threads(p)
          for(int i=0; i<X->size(); i++)
          { if(OldUpdate[i]!=UpdatedDist[i])
          { OldUpdate[i]=UpdatedDist[i];
            SSSP->at(i).Update=true;
          }
        else { SSSP->at(i).Update=false;}
/*int px=SSSP->at(i).Parent;
if(px >-1)
{
//printf("XX %d :%d \n", i, px);  
if(i==SSSP->at(px).Parent)
{printf("DP %d %d \n", i, px);}
}*/
 //{printf("DP: %d:%d %d:%d --%d\n", i, SSSP->at(i).Parent, px, SSSP->at(px).Parent,its);}
}       
          its++;
      }//end of while
    printf("Total Iterations to Converge %d \n", its);
    
    //Update the distances
    //Initialize with current distance for each node
#pragma omp parallel for num_threads(p)
    for(int i=0; i<X->size(); i++)
    {
        //do not update parent
        if(SSSP->at(i).Parent==-1){continue;}
 

int px=SSSP->at(i).Parent;
if(px >-1)
{
//printf("XX %d :%d \n", i, px);  
if(i==SSSP->at(px).Parent)
{printf("BBP %d %d \n", i, px);}
}       
        if(UpdatedDist[i]>=*maxW)
        {SSSP->at(i).Dist=*maxW;}
        else
        {SSSP->at(i).Dist=UpdatedDist[SSSP->at(i).Parent]+SSSP->at(i).EDGwt;}
    }
    
    return;
}


#endif

    
