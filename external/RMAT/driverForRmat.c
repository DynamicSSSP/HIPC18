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

int main(int argc, char** argv) {

  /* Step 1: Parse command line arguments */
  if (argc < 8) {
    printf("Usage: %s <SCALE> <SC-WT> <EPV> <a b c [d]> <Output filename>\n", argv[0]);
    printf("Details: SCALE = #Vertices = 2^SCALE  -- #Edges = EPV*#Vertices\n");
    printf("SC-WT: Random weights in the range zero to 2^SC-WT will be assigned\n");
    printf("\tIf you want unweighted, set SC-WT to a negative number.\n");
    printf("a,b,c,d: probabilities for R-MAT such that a+b+c+d=1.0.\n");
    printf("You can optionally not provide <d> and d=1-a-b-c.\n");
    printf("Output filename: File in which the graph will be stored\n");
    exit(-1);
  }
  
  int weighted = 1;
  int scale    = atoi(argv[1]);
  int scale_wt = atoi(argv[2]);
  int epv      = atoi(argv[3]);
  double a     = atof(argv[4]);
  double b     = atof(argv[5]);
  double c     = atof(argv[6]);
  double d     = 1.0 - (a + b + c);
  if (argc == 9)
    d = atof(argv[7]);

  if ((a+b+c+d) != 1.0) {
     printf("Probabilities do not sum to one\n");
  }

  /* Step 2: Parse the graph in Matrix Market format */
  if (scale_wt < 0) {
    scale_wt = 0.0;
    weighted = 0;
  }
  graph* G = generateRMAT(scale, scale_wt, epv, a, b, c, d);

  displayGraphCharacteristics(G);

  printf("Graph will be written in Pajek format in file: %s\n", argv[argc-1]);
  writeSimpleGraphFormatNewD(G, argv[argc-1], weighted);
  
  /* Step 5: Clean up */
  free(G->edgeListPtrs);
  free(G->edgeList);
  free(G);

  return 0;
}
