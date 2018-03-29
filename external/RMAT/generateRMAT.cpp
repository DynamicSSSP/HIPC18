/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Mahantesh Halappanavar                           */
/*                        High Performance Computing                         */
/*                Pacific Northwest National Lab, Richland, WA               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Copyright (C) 2010 Mahantesh Halappanavar                                 */
/*                                                                           */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or (at your option) any later version.                    */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place-Suite 330,Boston,MA 02111-1307,USA.     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
//CODE: Adapted from original code by John Feo and Kamesh Madduri

#include "defs.h"
#include <iostream>
#include <omp.h>
#include "RngStream.h"
using namespace std;

//RMAT Generator
//SCALE == 2^SCALE Nodes; 16x Edges; Edge weights in the range SCALE-WT
//a..d  == The four probabilities (sum to one)
graph * generateRMAT(int SCALE, int SCALE_WT, int epv, double a, double b, double c, double d) {
  cout<<"Within R-MAT Generator: SCALE= "<<SCALE<<"; WT-SCALE= "<<SCALE_WT<<"; Probabilities: ("<<a;
  cout<<", "<<b<<", "<<c<<", "<<d<<")\n";
  int nT;
#pragma omp parallel
  {
    nT = omp_get_num_threads();
  }
  cout<<"Number of threads: "<<nT<<endl;
  double var;
  long *permV, tmpVal;
  
  assert(a+b+c+d == 1);
  
  long n = (long) pow(2, SCALE); //#Nodes = 2^SCALE
  long m = (long) n * epv;  //#Edges = (#Nodes X 16)
  cout<<"Num vertices= "<<n<<"  Num Edges= "<<m<<endl;
  
  long *src  = (long *) malloc (m * sizeof(long));
  assert(src != NULL);
  long *dest = (long *) malloc (m * sizeof(long));
  assert(dest != NULL);
  long *degrees = (long *) malloc ((n+1) * sizeof(long));
  assert(degrees != NULL);
  
#pragma omp parallel for
  for (long i=0; i<=n; i++)
    degrees[i] = 0; //Initialize to zero
  
  // STEP-1: Generate edges in parallel
  
  //Initialize parallel pseudo-random number generator
  unsigned long seed[6] = {1, 2, 3, 4, 5, 6};
  RngStream::SetPackageSeed(seed);
  RngStream RngArray[nT]; //array of RngStream Objects
  
  cout<<"About to generate "<<m<<" edges\n";
  long block = m / nT;
  cout<<"Each thread will add "<<block<<" edges\n";
  //Each thread will generate m/nT edges each
  double start = omp_get_wtime();
#pragma omp parallel
  {
    int myRank = omp_get_thread_num();
#pragma omp for schedule(static)
    for (long Ti=0; Ti<m; Ti++) {
      long u = 1;
      long v = 1;
      long step = n/2;
      
      double av = a;
      double bv = b;
      double cv = c;
      double dv = d;
      double S;
      
      double p =  RngArray[myRank].RandU01();
      if (p < av) {
	/* Do nothing */
      } else if ((p >= av) && (p < av+bv)) {
	v += step;
      } else if ((p >= av+bv) && (p < av+bv+cv)) {
	u += step;
      } else {
	u += step;
	v += step;
      }
      for (int Tj=1; Tj<SCALE; Tj++) {
	step = step/2;
	/* Vary a,b,c,d by up to 10% */
	var = 0.1;
	av *= 0.95 + var * RngArray[myRank].RandU01();
	bv *= 0.95 + var * RngArray[myRank].RandU01();
	cv *= 0.95 + var * RngArray[myRank].RandU01();
	dv *= 0.95 + var * RngArray[myRank].RandU01();
	
	S = av + bv + cv + dv;
	av = av/S;
	bv = bv/S;
	cv = cv/S;
	dv = dv/S;
	
	/* Choose partition */
	p = RngArray[myRank].RandU01();
	if (p < av) {
	  /* Do nothing */
	} else if ((p >= av) && (p < av+bv)) {
	  v += step;
	} else if ((p >= av+bv) && (p < av+bv+cv)) {
	  u += step;
	} else {
	  u += step;
	  v += step;
	}
      }
      if ((u>n)||(v>n)) {
	cout<<"WARNING:  u= "<<u<<" v= "<<v<<"("<<step<<")\n";
	exit(1);
      }
      src[Ti] = u-1;
      dest[Ti] = v-1;
    }//End of for loop
  }//End of parallel region
  double end = omp_get_wtime();
  cout<<"Done adding edges: Time= "<<end-start<<" seconds \n";
  
  // STEP-2: Permute vertices to distribute high degree nodes across the range
  permV = (long *) malloc (n * sizeof(long));
  assert(permV != NULL);
  
#pragma omp parallel for
  for (long i=0; i<n; i++) {
    permV[i] = i;
  }
  
  long j;
  for (long i=0; i<n; i++) {
    j = (long) ( n * RngArray[0].RandU01() );
    if(j >= n) {
      cout<<"j= "<<j<<endl;
      exit(1);
    }
    tmpVal = permV[i];
    permV[i] = permV[j];
    permV[j] = tmpVal;
  }
  
#pragma omp parallel for
  for (long i=0; i<m; i++) {
    src[i]  = permV[src[i]];
    dest[i] = permV[dest[i]];
  }
  
#pragma omp parallel for
  for (long i=0; i<m; i++) {
    __sync_fetch_and_add(&degrees[src[i]+1], 1); //Increment by one to make space for zero
    __sync_fetch_and_add(&degrees[dest[i]+1], 1);
  }
  
  cout<<"Done counting degrees \n";
  //STEP-3 Generate random edge weights:
  double max_weight = pow(2, SCALE_WT);
  cout<<"Max weight= "<<max_weight<<endl;
  double *tmp_weight = (double *) malloc (m * sizeof(double));
  assert(tmp_weight != NULL);
#pragma omp parallel
  {
    int myRank = omp_get_thread_num();
#pragma omp for schedule(static)
    for (long Ti=0; Ti<m; Ti++) {
      tmp_weight[Ti] = max_weight * RngArray[myRank].RandU01();
    }
  }//End of parallel region
  
  cout<<"Done building random weights (max wt= "<<max_weight<<")\n";
  
  //STEP-4 Build CSR from the edge list:
  long* counter = (long *) malloc (n * sizeof(long));
  assert(counter != NULL);
#pragma omp parallel for
  for (long i=0; i<n; i++)
    counter[i] = 0;
  
  //Allocate memory for Edge list:
  edge *eList = (edge *) malloc ((2*m) * sizeof (edge));
  assert(eList != NULL);
  //Build the Pointer Array:
  for (long i=1; i<=n; i++) {
    degrees[i] += degrees[i-1];
  }
  //Sanity check:
  if(degrees[n] != 2*m) {
    cout<<"Number of edges added is not correct\n";
    exit(1);
  }
  cout<<"Done building pointer array\n";
  
#pragma omp parallel for
  for (long i=0; i<m; i++) {
    long u = src[i];
    long v = dest[i];
    //Add edge u-->v
    long location = degrees[u] + __sync_fetch_and_add(&counter[u], 1);
    if (location >= 2*m) {
      cout<<"location is out of bound: "<<location<<"\n";
      exit(1);
    }
    eList[location].head   = u;
    eList[location].tail   = v;
    eList[location].weight = tmp_weight[i];
    
    //Add edge v-->u
    location = degrees[v] + __sync_fetch_and_add(&counter[v], 1);
    if (location >= 2*m) {
      cout<<"location is out of bound: "<<location<<"\n";
      exit(1);
    }
    eList[location].head   = v;
    eList[location].tail   = u;
    eList[location].weight = tmp_weight[i];
  }//End of for loop
  
  cout<<"Done building compressed format\n";
  //Clean up:
  free(src); free(dest); free(tmp_weight);
  free(permV); free(counter);
  
  //STEP-5: Build and return a graph data structure
  graph * G = (graph *) malloc (sizeof(graph));
  G->numVertices = n;
  G->sVertices   = n;
  G->numEdges    = m;
  G->edgeListPtrs = degrees;
  G->edgeList     = eList;
  
  return G;
}//End of RMAT-generator


