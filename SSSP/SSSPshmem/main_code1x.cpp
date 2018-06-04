
/**** THIS IS OPENMP ONLY CODE *****/


// HEADERS
#include "SP_structure.hpp"
#include "create_tree.hpp"
#include "readin_data.hpp"
#include "Ins_Del1.hpp"

//AUXILLIARY OPS
#include<queue>
#include<stack>
#include<list>
/*** All Headers Required From ESSENS **/

using namespace std;

int main(int argc, char *argv[]) {
    clock_t q, q1, q2, t;
    double startx, endx, starty, endy;

    /***** Preprocessing to Graph (GUI) ***********/
    t = clock();
    q = clock();    //Assume Processed Input
    //Form node node weight
    //Nodes are numbered contiguously
    //Starts from zero
    //Edges are undirected (both a-->b and b-->a present)
    //Check if valid input is given
    if ( argc < 7 ) { cout << "INPUT ERROR:: EIGHT inputs required. First: filename without tree. Second: Graph Certificate. Third: List of Edges to be Updated. Fourth: Upper Bound on Weight Fifth: number of nodes. Sixth: Size of batch. Seventh:number of threads\n"; return 0;}
    //Check to see if file opening succeeded
    ifstream the_file ( argv[1] ); if (!the_file.is_open() ) { cout<<"INPUT ERROR:: Could not open main //file\n";}
    ifstream the_file1 ( argv[2] ); if (!the_file1.is_open() ) { cout<<"INPUT ERROR:: Could not open certificate file\n";}
    ifstream the_file2 ( argv[3] ); if (!the_file2.is_open() ) { cout<<"INPUT ERROR:: Could not open update file\n";}
    

/* Set the number of Threads and Vertices**/
    int maxW = atoi(argv[4]);
    int nodes = atoi(argv[5]); //total number of vertices
    int batch=atoi(argv[6]); //the number of changes processed per point
    int p = atoi(argv[7]);  //total number of threads per core
/** Set the number of Threads and Vertices**/


    
  /**   Initializing Memory ***/ /** TBD: Change to Array and Malloc**/
// startx=omp_get_wtime();

    //List of Changes
    //There will be a list for inserts and a list for delete
    vector<xEdge> allChange;
    allChange.clear();
    /*** Initializing Memory ***/
    
    int src=atoi(argv[8]);; //the source from which the paths are computed
   
    
    /*** Read Remainder Edges as Graph ***/
    startx=omp_get_wtime();
    A_Network R;
   //readin_network(&R,  argv[1], -1);
   readin_graphU(&R, nodes,argv[1]);
    endx=omp_get_wtime();
    printf("Total Time for Reading Remainder Network %f \n", endx-startx);
    /*** Finished Reading CRT Tree **/
    
    
    /**** Read Remainder Edges as a list upto a Threshold ***
    startx=omp_get_wtime();
    //List of Remainder Edges
    vector<Edge> reBs;
    int highV=30; //highest weight to consider
    reBs.clear();
    printf("Highest Edge Value %d --\n", highV);
    readin_remainder(argv[1], highV, &reBs);
    endx=omp_get_wtime();
    printf("Total Time for Reading Remainder Edges %f \n", endx-startx);
    /*** Finished Reading Remainder Edges **/
    
    
    /*** Read the Certificate ***/
    startx=omp_get_wtime();
    A_Network X;
  readin_network(&X, argv[2],-1);
  // readin_graphU(&X, nodes,argv[1]);
    endx=omp_get_wtime();
    printf("Total Time for Reading Certificate Network %f \n", endx-startx);
    /*** Finished Reading CRT Tree **/
    
    /*** Read set of Changed Edges ***/
   startx=omp_get_wtime();
    readin_changes(argv[3], &allChange);
    endx=omp_get_wtime();
    printf("Total Time for Reading Update List %f \n", endx-startx);
    /*** Finished Reading Changed Edges **/

/** Add Buffer for Extra Nodes ***/
/*int buff=200;
//printf("%d::%d::%d \n", nodes,X.size(),R.size());

for(int b=1;b<=buff;b++)
{
nodes=nodes+1;
ADJ_Bundle Alist;
Alist.ListW.clear();
Alist.Row=nodes;
X.push_back(Alist);

//printf("%d:%d::%d\n", b,nodes,X.size());
}
printf("done..\n");
*/
 
    //Initializing  Rooted Tree
    RT_Vertex RTV;
    vector<RT_Vertex> SSSP;
    SSSP.clear();
    SSSP.resize(nodes, RTV);

//Initialize the Roots, Level and Distance of the nodes
   startx=omp_get_wtime();
#pragma omp parallel for num_threads(p)
    for(int i=0;i<nodes; i++)
    {

        if(i==src)
        { SSSP[i].Root=-1;}
        else
        { SSSP[i].Root=i;}
        SSSP[i].Level=0;
        SSSP[i].Dist =0.0;
        SSSP[i].degree=int(X[i].ListW.size());
    //printf("%d %d %d\n", i, nodes, SSSP.size());

    }    
    endx=omp_get_wtime();
    printf("Total Time for Computing degree and Updating SSSSP %f \n", endx-startx);
    printf("==Update Starts =======\n");

    //Create Rooted Tree
    startx=omp_get_wtime();
    create_treeS(&X, &R, &SSSP,src, p);
    endx=omp_get_wtime();
    printf("Total Time for Creating tree %f \n", endx-startx);
    
  /* for(int i=0;i<X.size();i++)
    {
        printf("Id:%d ",i);
    printRT(i,&SSSP[i], &R);
    }*/
 
    //One Batch
    double maxV=(double)maxW*X.size();
    
   starty=omp_get_wtime();
    //Update the inserted and delted edges in the tree
    startx=omp_get_wtime();
     int te=0;
    edge_update(&allChange, &X, &SSSP, &R, &maxV, &te,p);
    endx=omp_get_wtime();
    printf("Total Time for Initial Update %f,\n", endx-startx);
    
  /* printf("**** After Update ****\n");
    for(int i=0;i<X.size();i++)
    {
        printf("Id:%d ",i);
        printRT(i,&SSSP[i], &R);
   
    }*/
   //Update the other vertices based on new inserted edges
    startx=omp_get_wtime();
    rest_update(&X, &SSSP, &R,&maxV, &te,p);
    endx=omp_get_wtime();
    printf("Total Time for Complete Update %f\n", endx-startx);
    
    endy=omp_get_wtime();
    printf("Total Time for Initia+Complete Update %f %d\n", endy-starty, te);
 /*  printf("**** ALL Update ****\n");
    for(int i=0;i<X.size();i++)
{
        printf("Id:%d ",i);
        printRT(i,&SSSP[i], &R);
    }*/

    //Repair broken pieces
  /*   startx=omp_get_wtime();
    repair_tree(&reBs,&SSSP, &maxV, p);
    endx=omp_get_wtime();
    printf("Total Time for Initial Repair %f \n", endx-startx);*/
    
   /* printf("**** Repaired Step 1****\n");
    for(int i=0;i<X.size();i++)
    {
        printf("Id:%d ",i);
        printRT(i,&SSSP[i], &R);
    }*/
    //Update the other vertices based on new inserted edges
   /* startx=omp_get_wtime();
    rest_update(&X, &SSSP, &R, &maxV, p);
    endx=omp_get_wtime();
     printf("Total Time for Complete Repair %f \n", endx-startx);*/
    
   /* printf("**** ALL Repaired ****\n");
    for(int i=0;i<X.size();i++)
    {
        printf("Id:%d ",i);
        printRT(i,&SSSP[i], &R);
    }*/
    
    //Update X--this step is optional, use only if X is required in next batch
    
    
    
    return 0;
}//end of main

//==========================================//
	
	

