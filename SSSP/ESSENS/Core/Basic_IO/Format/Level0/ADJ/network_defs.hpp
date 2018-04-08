#ifndef ADJ_NETWORK_DEFS_HPP
#define ADJ_NETWORK_DEFS_HPP

#include"structure_defs.hpp"
#include<list>
using namespace std; 

//Structure in STATIC Adjacency List---For diagram go to () 
//Rows=global ID of the rows
//For edges connected with Rows
//NListW.first=Column number
//NListW.second=Value of edge
struct ADJ_Bundle
{
 	int Row;
	vector <int_double> ListW;
	
	//Constructor
	ADJ_Bundle() {ListW.resize(0);}
	
	//Destructor
	void clear()
	{while(!ListW.empty()) {ListW.pop_back();}}
	
	
};

typedef  vector<ADJ_Bundle> A_Network;


#endif
