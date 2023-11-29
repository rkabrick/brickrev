/*
 * dram_tests.c
 *
 * RISC-V ISA: RV64G
 *
 * Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
 * All Rights Reserved
 * contact@tactcomplabs.com
 *
 * See LICENSE in the top level directory for licensing details
 *
 * bug test
 */

//
// uncomment to cause argc bug failure
//
#define INDUCE_ARGC_BUG

#include "stdlib.h"
#include "rev-macros.h"

#define assert TRACE_ASSERT

int main(int argc, char **argv){

  assert(argc==4 || argc==5);

  int numlanes = atoi(argv[1]);
  int numthreadsperlane = atoi(argv[2]);
  int nelemsperthread = atoi(argv[3]);
  int chunk = 0;
  if(argc == 5) chunk = atoi(argv[4]);

#ifndef INDUCE_ARGC_BUG
  assert(numlanes==64);
  assert(numthreadsperlane==4);
  assert(nelemsperthread==2);
#endif
  
  return 0;
}
