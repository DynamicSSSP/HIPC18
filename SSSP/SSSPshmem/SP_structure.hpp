#ifndef SP_STRUCTURE_HPP
#define SP_STRUCTURE_HPP


/*** All Headers Required From ESSENS **/
//INPUT HEADERS
#include "input_to_network.hpp"
#include<set>
#include<map>
#include "ADJ/create_networkP.hpp"

//OUTPUT HEADERS
#include "printout_network.hpp"
//#include "printout_others.hpp"

//SET_OPERATIONS
#include "unary_set_operations.hpp"
#include "binary_set_operations.hpp"

//NETWORK TRAVERSAL
#include "ADJ/traversal.hpp"

//ADD and DELETE EDGE
#include "ADJ/add_edge.hpp"
#include "ADJ/del_edge.hpp"

#include <omp.h>

//#include <concurrent_vector.h>
using namespace std;


//========================|
//Structure to indicate whether Edge is to be inserted/deleted
struct xEdge {
    Edge theEdge;
    int inst;
    bool insertedToDatastructure;

    xEdge()
    {
        insertedToDatastructure=false;
    }
    void clear()
    {}
};

//===================|
//Comparing endpoints of two edges
bool operator == (const Edge &mye1, const Edge &mye2 )
{
    if((mye1.node1==mye2.node1) && (mye1.node2==mye2.node2)) {  return true;}
    else
    {
     if((mye1.node1==mye2.node2) && (mye1.node2==mye2.node1)) {  return true;}
    else
    {
    return false;
    }
    }
};


//bool wt_sort(Edge e1, Edge e2) {return (e1.edge_wt<e2.edge_wt);}

//Creating dummy edge
void dummy_edge(Edge *mye)
{
    mye->node1=-1;
    mye->node2=-1;
    mye->edge_wt=-1;
    return;
}



struct SCC_vertex
{
    int node;
    int sccID;
    void clear()
    {}
};


// Data Structure for each vertex in the rooted tree
struct RT_Vertex
{
    int Root;    //root fo the tree
    int Parent; //mark the parent in the tree
    double EDGwt; //mark weight of the edge
    int Level; //Distance from the root
    int marked; //whether the vertex and the edge connecting its parent ..
                //..exists(-1); has been deleted(-2); is marked for replacement (+ve value index of changed edge)
    vector<int> PossN; //possible neighbor to check for
   
   double Dist;  //Distance from root
   bool Update;  //Whether the distance of this edge was updated
int degree;
    //Constructor
    RT_Vertex()
    {
        Root=-1;
        Parent=-1;
        EDGwt=0.0;
        Level=-1;
        marked=-1;
        
        PossN.clear();
        Dist =0.0;
        Update=false;
    }
    
    //Destructor
    void clear()
    {}

    
    
};
//The Rooted tree is a vector of structure RT_Vertex;


/*void printRT(int i, RT_Vertex *RT, A_Network *R)
{
printf("Root:%d; Parent:%d; EDGwt:%f; Level:%d; Dist%f PossN:%d \n", RT->Root, RT->Parent, RT->EDGwt, RT->Level, RT->Dist, RT->PossN.size());
    
    for(int j=0;j< RT->PossN.size();j++)
    {
        int myID= RT->PossN[j];
        
        int_double myval=R->at(i).ListW[myID];
        printf("%d:%f  \n", myval.first, myval.second);
        
    
    }
    printf("\n");
}*/


//Structure of Edge with level
struct lEdge {
    Edge theEdge;
    int Level;
};

//Data Structure for Remainder Edges
//Bucket of edge list and its Level in buckets ranging from 0-10; 10-20 20-30  ....90-100
struct RE_Bucket
{
    int st;
    int end;
    vector<Edge> REdges; //set of edges in this bucket
    vector<int_double> val_ind; //index from which a particular value starts
    
    //Constructor
    RE_Bucket()
    {
        st=-1;
        end=-1;
        REdges.clear();
        val_ind.clear();
    }
    
    //Destructor
    void clear()
    {}
    
};

//Define compare for edge
struct EdgeCompare
{
    bool operator() (const Edge& edge1, const Edge& edge2) const
    {
        return edge1.edge_wt< edge2.edge_wt;
    }
};

//Map of Edge to Position Index
typedef map<Edge,int, EdgeCompare> map_Edge_int;







#endif
