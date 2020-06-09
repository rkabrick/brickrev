//
// _RevMem_h_
//
// Copyright (C) 2017-2020 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#ifndef _SST_REVCPU_REVMEM_H_
#define _SST_REVCPU_REVMEM_H_

// -- C++ Headers
#include <ctime>
#include <vector>

// -- SST Headers
#include <sst/core/sst_config.h>
#include <sst/core/component.h>

// -- RevCPU Headers
#include "RevOpts.h"

#ifndef _REVMEM_BASE_
//#define _REVMEM_BASE_ 0x80000000
#define _REVMEM_BASE_ 0x00000000
#endif

namespace SST::RevCPU {
  class RevMem;
}

using namespace SST::RevCPU;

namespace SST {
  namespace RevCPU {
    class RevMem {
    public:
      /// RevMem: standard constructor
      RevMem( unsigned long MemSize, RevOpts *Opts, SST::Output *Output );

      /// RevMem: standard destructor
      ~RevMem();

      /// RevMem: get the stack_top address
      uint64_t GetStackTop() { return stacktop; }

      /// RevMem: set the stack_top address
      void SetStackTop(uint64_t Addr) { stacktop = Addr; }

      /// RevMem: write to the target memory location
      bool WriteMem( uint64_t Addr, size_t Len, void *Data );

      /// RevMem: read data from the target memory location
      bool ReadMem( uint64_t Addr, size_t Len, void *Data );

      /// RevMem: Read uint8 from the target memory location
      uint8_t ReadU8( uint64_t Addr );

      /// RevMem: Read uint16 from the target memory location
      uint16_t ReadU16( uint64_t Addr );

      /// RevMem: Read uint32 from the target memory location
      uint32_t ReadU32( uint64_t Addr );

      /// RevMem: Read uint64 from the target memory location
      uint64_t ReadU64( uint64_t Addr );

      /// RevMem: Read float from the target memory location
      float ReadFloat( uint64_t Addr );

      /// RevMem: Read double from the target memory location
      double ReadDouble( uint64_t Addr );

      /// RevMem: Write a uint8 to the target memory location
      void WriteU8( uint64_t Addr, uint8_t Value );

      /// RevMem: Write a uint16 to the target memory location
      void WriteU16( uint64_t Addr, uint16_t Value );

      /// RevMem: Write a uint32 to the target memory location
      void WriteU32( uint64_t Addr, uint32_t Value );

      /// RevMem: Write a uint64 to the target memory location
      void WriteU64( uint64_t Addr, uint64_t Value );

      /// RevMem: Write a float to the target memory location
      void WriteFloat( uint64_t Addr, float Value );

      /// RevMem: Write a double to the target memory location
      void WriteDouble( uint64_t Addr, double Value );

      /// RevMem: Randomly assign a memory cost
      unsigned RandCost( unsigned Min, unsigned Max );

      /// RevMem: Add a memory reservation for the target address
      bool LR(unsigned Hart, uint64_t Addr);

      /// RevMem: Clear a memory reservation for the target address
      bool SC(unsigned Hart, uint64_t Addr);

    private:
      unsigned long memSize;    ///< RevMem: size of the target memory
      RevOpts *opts;            ///< RevMem: options object
      SST::Output *output;      ///< RevMem: output handler

      char *mem;                ///< RevMem: memory container

      uint64_t stacktop;        ///< RevMem: top of the stack

      std::vector<std::pair<unsigned,uint64_t>> LRSC;   ///< RevMem: load reserve/store conditional vector

      /// RevMem: Virtual to physical address translation

    }; // class RevMem
  } // namespace RevCPU
} // namespace SST

#endif

// EOF
