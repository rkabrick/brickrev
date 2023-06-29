// See LICENSE for license details.

//**************************************************************************
// Memcpy benchmark
//--------------------------------------------------------------------------
//
// This benchmark tests the memcpy implementation in syscalls.c.
// The input data (and reference data) should be generated using
// the memcpy_gendata.pl perl script and dumped to a file named
// dataset1.h.

#include <string.h>
#include "util.h"

//--------------------------------------------------------------------------
// Input/Reference Data

#include "dataset1.h"

//--------------------------------------------------------------------------
// Main

int main( int argc, char* argv[] )
{
  int results_data[DATA_SIZE];

#if PREALLOCATE
  // If needed we preallocate everything in the caches
  memcpy(results_data, input_data, sizeof(int) * DATA_SIZE);
#endif

  // Do the riscv-linux memcpy
  //setStats(1);
  memcpy(results_data, input_data, sizeof(int) * DATA_SIZE); //, DATA_SIZE * sizeof(int));
  //setStats(0);

  // Check the results
  int v =  verify( DATA_SIZE, results_data, input_data );
  return v;
}
