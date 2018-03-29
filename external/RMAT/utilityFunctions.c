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

#include "defs.h"

void displayGraphCharacteristics(graph *G) {
  printf("Within displayGraphCharacteristics()\n");        
  long    sum = 0, sum_sq = 0;
  double  average, avg_sq, variance, std_dev;
  long    maxDegree = 0;
  long    isolated  = 0;
  long    NS        = G->sVertices;    
  long    NV        = G->numVertices;
  long    NT        = NV - NS;
  long    NE        = G->numEdges;
  long    *vtxPtr   = G->edgeListPtrs;
  long    tNV       = NV; //Number of vertices    
     
  if ( (NS == 0)||(NS == NV) ) {  //Nonbiparite graph	
    for (long i = 0; i < NV; i++) {
      long degree = vtxPtr[i+1] - vtxPtr[i];
      sum_sq += degree*degree;
      sum    += degree;
      if (degree > maxDegree)
	maxDegree = degree;
      if ( degree == 0 )
	isolated++;
    }	
    average  = (double) sum / tNV;
    avg_sq   = (double) sum_sq / tNV;
    variance = avg_sq - (average*average);
    std_dev  = sqrt(variance);
    
    printf("*******************************************\n");
    printf("General Graph: Characteristics :\n");
    printf("*******************************************\n");
    printf("Number of vertices   :  %ld\n", NV);
    printf("Number of edges      :  %ld\n", NE);
    printf("Maximum out-degree is:  %ld\n", maxDegree);
    printf("Average out-degree is:  %lf\n",average);
    printf("Expected value of X^2:  %lf\n",avg_sq);
    printf("Variance is          :  %lf\n",variance);
    printf("Standard deviation   :  %lf\n",std_dev);
    printf("Isolated vertices    :  %ld (%3.2lf%%)\n", isolated, ((double)isolated/tNV)*100);
    printf("Density              :  %lf%%\n",((double)NE/(NV*NV))*100);
    printf("*******************************************\n");
    
  }//End of nonbipartite graph
  else { //Bipartite graph
    
    //Compute characterisitcs from S side:
    for (long i = 0; i < NS; i++) {
      long degree = vtxPtr[i+1] - vtxPtr[i];
      sum_sq += degree*degree;
      sum    += degree;
      if (degree > maxDegree)
	maxDegree = degree;
      if ( degree == 0 )
	isolated++;
    }    
    average  = (double) sum / NS;
    avg_sq   = (double) sum_sq / NS;
    variance = avg_sq - (average*average);
    std_dev  = sqrt(variance);
    
    printf("*******************************************\n");
    printf("Bipartite Graph: Characteristics of S:\n");
    printf("*******************************************\n");
    printf("Number of S vertices :  %ld\n", NS);
    printf("Number of T vertices :  %ld\n", NT);
    printf("Number of edges      :  %ld\n", NE);
    printf("Maximum out-degree is:  %ld\n", maxDegree);
    printf("Average out-degree is:  %lf\n",average);
    printf("Expected value of X^2:  %lf\n",avg_sq);
    printf("Variance is          :  %lf\n",variance);
    printf("Standard deviation   :  %lf\n",std_dev);
    printf("Isolated (S)vertices :  %ld (%3.2lf%%)\n", isolated, ((double)isolated/NS)*100);
    printf("Density              :  %lf%%\n",((double)NE/(NS*NS))*100);
    printf("*******************************************\n");
    
    sum = 0; 
    sum_sq = 0;
    maxDegree = 0;
    isolated  = 0;
    //Compute characterisitcs from T side:
    for (long i = NS; i < NV; i++) {
      long degree = vtxPtr[i+1] - vtxPtr[i];
      sum_sq += degree*degree;
      sum    += degree;
      if (degree > maxDegree)
	maxDegree = degree;
      if ( degree == 0 )
	isolated++;
    }
    
    average  = (double) sum / NT;
    avg_sq   = (double) sum_sq / NT;
    variance = avg_sq - (average*average);
    std_dev  = sqrt(variance);
    
    printf("Bipartite Graph: Characteristics of T:\n");	
    printf("*******************************************\n");
    printf("Number of T vertices :  %ld\n", NT);
    printf("Number of S vertices :  %ld\n", NS);
    printf("Number of edges      :  %ld\n", NE);
    printf("Maximum out-degree is:  %ld\n", maxDegree);
    printf("Average out-degree is:  %lf\n",average);
    printf("Expected value of X^2:  %lf\n",avg_sq);
    printf("Variance is          :  %lf\n",variance);
    printf("Standard deviation   :  %lf\n",std_dev);
    printf("Isolated (T)vertices :  %ld (%3.2lf%%)\n", isolated, ((double)isolated/NT)*100);
    printf("Density              :  %lf%%\n",((double)NE/(NT*NT))*100);
    printf("*******************************************\n");    
  }//End of bipartite graph    
}


