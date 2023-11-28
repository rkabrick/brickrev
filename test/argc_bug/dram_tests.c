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

// uncomment to cause argc bug failure
//#define INDUCE_ARGC_BUG

// Standards includes
#include "stdlib.h"

// Rev includes
#include <cstdint>
#include "rev-macros.h"

//namespace udrev {

//
// basim::UDAccelerator init parameters
//
union ud_info_t
{
  uint64_t v = 0;
  struct
  {
    uint64_t id : 16;          // 15:0    
    uint64_t lanes: 8;         // 23:16
    uint64_t lmmode: 2;        // 25:24
    uint64_t s0:     6;        // 31:26 spare
    uint64_t prognum: 8;       // 39:32 program number
    uint64_t s1: 8;            // 47:40
    uint64_t max_sim_iter: 16; // 63:48                                            
  } f;
  #if 0
  ud_info_t() {};
  ud_info_t(uint64_t d) : v(d){};
  ud_info_t(uint64_t _id, uint64_t _lanes, uint64_t _lmmode, uint64_t _prognum, uint64_t _max_sim_iter) {
    f.id = _id & 0x0ffffULL;
    f.lanes = _lanes & 0x0ffULL;
    f.lmmode = _lmmode & 0x03ULL;
    f.prognum = _prognum & 0x0fULL;
    f.max_sim_iter = _max_sim_iter & 0x0ffffULL;
  }
  #endif
};

/// use for memory oriented sys_calls
struct ud_mem_desc_t {
  uint64_t  size;
  void* ptr = 0;
};

//
// Replaces updown_config.h
//
#define DEF_NUM_NODES 1           // 1 node system
#define DEF_NUM_LANES 64          // Number of lanes per CU
#define DEF_NUM_UDS 4             // Number of CUs
#define DEF_NUM_STACKS 8          // Number of Stacks per Node
#define DEF_SPMEM_BANK_SIZE 65536 // Scratchpad Memory size per lane
#define DEF_WORD_SIZE 8            // Wordsize
#define DEF_MAPPED_SIZE 1UL << 32  
#define DEF_GMAPPED_SIZE 1UL << 32  
// Base address for scratchpad memories
#define BASE_SPMEM_ADDR 0x400000000
// Base address for memory mapped control registers
#define BASE_CTRL_ADDR 0x600000000
// Base address for UpDown Program
#define BASE_PROG_ADDR 0x800000000
// Control signals address space capacity.
#define CONTROL_CAPACITY_PER_LANE 32
// Scratchpad address space maximum capacity = def currently
#define SPMEM_CAPACITY_PER_LANE 65536
// Number of lanes capacity - Max = Def currently
#define NUM_LANES_CAPACITY 64
// Number of UDs capacity - UD capacity
#define NUM_UDS_CAPACITY 4
// Number of nodes capacity
#define NUM_NODES_CAPACITY 16
// Number of stacks capacity
#define NUM_STACKS_CAPACITY 8
// Base address mapped memory - This is due to simulation?
#define BASE_SYNC_SPACE 0x7FFF0000
// Revisit this
#define BASE_MAPPED_ADDR 0x80000000
#define BASE_MAPPED_GLOBAL_ADDR 0x200000000
// CONTROL SIGNALS OFFSET IN WORDS
#define EVENT_QUEUE_OFFSET 0x0
#define OPERAND_QUEUE_OFFSET 0x1
#define START_EXEC_OFFSET 0x2
#define LOCK_OFFSET 0x3
// Thread spawning
static constexpr uint8_t CREATE_THREAD = 0xFF;

//
// Replaces ud_machine.h
//
struct ud_machine_t {
  // Offsets for addrs space
  uint64_t MapMemBase = BASE_MAPPED_ADDR;         // Base address for memory map
  uint64_t GMapMemBase = BASE_MAPPED_GLOBAL_ADDR; // Base address for memory map
  uint64_t UDbase = BASE_SPMEM_ADDR;              // Base address for upstream
  uint64_t SPMemBase = BASE_SPMEM_ADDR;           // ScratchPad Base address
  uint64_t ControlBase = BASE_CTRL_ADDR;          // Base for control operands
  uint64_t ProgBase = BASE_PROG_ADDR;          // Base for control operands
  uint64_t EventQueueOffset = EVENT_QUEUE_OFFSET; // Offset for Event Queues
  uint64_t OperandQueueOffset = OPERAND_QUEUE_OFFSET;  // Offset for Operands Queues
  uint64_t StartExecOffset = START_EXEC_OFFSET; // Offset for Start Exec signal
  uint64_t LockOffset = LOCK_OFFSET;            // Offset for Lock signal

  // Machine config and capacities
  uint64_t CapNumNodes = NUM_NODES_CAPACITY;          // Max number of UpDowns
  uint64_t CapNumStacks = NUM_STACKS_CAPACITY;        // Max number of UpDowns
  uint64_t CapNumUDs = NUM_UDS_CAPACITY;              // Max number of UpDowns
  uint64_t CapNumLanes = NUM_LANES_CAPACITY;          // Max number of UpDowns
  uint64_t CapSPmemPerLane = SPMEM_CAPACITY_PER_LANE; // Max bank size per lane
  uint64_t CapControlPerLane = CONTROL_CAPACITY_PER_LANE;       // Max Control Sigs and regs per lane
  uint64_t NumUDs = DEF_NUM_UDS;       // Number of UpDowns
  uint64_t NumLanes = DEF_NUM_LANES;   // Number of lanes
  uint_least64_t LocalMemAddrMode = 1;       // Local Memory Mode
  uint64_t NumStacks = DEF_NUM_STACKS; // Number of lanes
  uint64_t NumNodes = DEF_NUM_NODES;   // Number of lanes

  // Sizes for memories
  uint64_t MapMemSize = DEF_MAPPED_SIZE;   // Mapped Memory size
  uint64_t GMapMemSize = DEF_GMAPPED_SIZE; // Mapped Memory size
  uint64_t SPBankSize = DEF_SPMEM_BANK_SIZE; // Local Momory (scratchpad) Bank Size
  uint64_t SPBankSizeWords = DEF_SPMEM_BANK_SIZE * DEF_WORD_SIZE; // LocalMemorySize in Words
};

// ref updown.h
struct base_addr_t {
  #if 0
  volatile uint64_t *mmaddr;
  volatile uint64_t *spaddr;
  volatile uint64_t *ctrlAddr;
  volatile uint64_t *progAddr;
  #else
  uint64_t *mmaddr;
  uint64_t *spaddr;
  uint64_t *ctrlAddr;
  uint64_t *progAddr;
  #endif
};


//} // namespace udrev

// use this to allow tracing of the assertion but do not fail 
#define NO_TRACE_ASSERT(x) { TRACE_PUSH_ON; \
  if (!(x)) { asm volatile("add x0,x0,x0"); }; \
  TRACE_PUSH_OFF }

#undef assert
#if 0
#define assert TRACE_ASSERT
#else
#define assert NO_TRACE_ASSERT
#endif

//using udrev::ud_machine_t;
//using udrev::ud_info_t;
//using udrev::CREATE_THREAD;
//using udrev::ud_mem_desc_t;

// system calls for updown accelerator
int updown_init(ud_machine_t* p_machine, ud_info_t* p_info); // 0x3001
int updown_send(uint64_t *event);                            // 0x3002
int updown_start_exec(uint64_t nwid);                        // 0x3003
int updown_test_wait_addr(uint64_t nwid, uint32_t offset, uint64_t expected); // 0x3004
int updown_check_ok(uint64_t& ok);                   // 0x3005

// system calls for rev udrt services
int udrt_init();                                // 0x3040
int udrt_get_region_local(ud_mem_desc_t *desc); // 0x3041

// ud rev runtime 
void rt_init(ud_machine_t machine, uint64_t id, uint64_t prognum);
void rt_send_event(uint64_t* ev);
void rt_start_exec(uint64_t nwid);
void rt_test_wait_addr(uint64_t nwid, uint64_t offset, uint64_t expected);
void rt_wait();
void* rt_mm_malloc(uint64_t size);

int rt_status = 0;
ud_machine_t rt_machine_config;
ud_info_t rt_info;

// test functions
void test_read_write (uint64_t* data, uint64_t* dram_dst, uint64_t* dram_src,
		      int numlanes, int numthreadsperlane, int nelemsperthread);

// replace operand_t and event_t until rev handles c++ structs
uint64_t ops[8];         // Always 8 for now
uint64_t event_ops[4];   // ignore comments in updown code

int main(int argc, char **argv){

  assert(argc==4 || argc==5);

  int numlanes = atoi(argv[1]);
  int numthreadsperlane = atoi(argv[2]);
  int nelemsperthread = atoi(argv[3]);
  int chunk = 0;
  if(argc == 5) chunk = atoi(argv[4]);
  
  ud_machine_t machine;
  machine.NumLanes = numlanes;
  machine.NumUDs = 1;
  machine.NumStacks = 1;
  machine.NumNodes = 1;
  machine.LocalMemAddrMode = 0;

  for(int i = 1; i < 9; i++){
    if(chunk != 0 && chunk != i)
      continue;

    rt_init(machine,0,i);

    // originating source data
    unsigned n_words = numlanes * numthreadsperlane * nelemsperthread;
    unsigned total_size = n_words * sizeof(uint64_t);
    #if 0
    uint64_t data[n_words];
    uint64_t* dram_src = reinterpret_cast<uint64_t*>(rt_mm_malloc(total_size));
    uint64_t* dram_dst = reinterpret_cast<uint64_t*>(rt_mm_malloc(total_size));
    #else
    uint64_t data[n_words*3];
    uint64_t* dram_src = &(data[n_words]);
    uint64_t* dram_dst = &(data[n_words*2]);
    #endif
    if (dram_src==0 || dram_dst==0 || (dram_src==dram_dst)) {
      assert(0);
    }
    
    // give source some random data
    for(int j = 0; j < n_words; j++){
      data[j] = (j+1)<<8 + (i+1)<<4 + (chunk+1);
      dram_src[j] = data[j];
      dram_dst[j] = 0;
    }

    // if read tests, then the data will be written to spm, starting from the second word (offset 8)
    // write the same data to spm if write test and using sendm
    // just test read then write
    test_read_write(data, dram_dst, dram_src, numlanes, numthreadsperlane, nelemsperthread);
  }

  return 0;
}

void test_read_write (uint64_t* data, uint64_t* dram_dst, uint64_t* dram_src,
		      int numlanes, int numthreadsperlane, int nelemsperthread)
{
  uint64_t n_ops = 8;
  ops[0]= (uint64_t) dram_src;
  ops[1]= (uint64_t) dram_dst; // for dram_dest)= for read it is bogus
  ops[2]= (uint64_t) numlanes;
  ops[3]= (uint64_t) numthreadsperlane;
  ops[4]= (uint64_t) nelemsperthread;
  ops[5]= (uint64_t) numlanes * numthreadsperlane;
  ops[6]= (uint64_t) 0;
  ops[7]= (uint64_t) 0;

  /* similarly
  event_t event_ops = event_t(0, networkid_t(0), UpDown::CREATE_THREAD, &ops);
  */
  event_ops[0] = 0;  
  event_ops[1] = 0;
  event_ops[2] = (uint64_t) CREATE_THREAD;
  event_ops[3] = (uint64_t) ops;

  rt_send_event(event_ops);

  // rt->start_exec(networkid_t(0)); // create networkid_t in service instead
  rt_start_exec(0);

  // rt_test_wait_addr(networkid_t(0), 0, 1);
  rt_test_wait_addr(0, 0, 1);

  // make sure src is not changed
  for(int i = 0; i < numlanes * numthreadsperlane * nelemsperthread; i++){
    assert(dram_src[i] == data[i]);
    dram_dst[i] = dram_src[i];    
  }

  // check dst data
  for(int i = 0; i < numlanes * numthreadsperlane * nelemsperthread; i++){
    // cout << "dram_dst[" << i << "]: " << dram_dst[i] << ", expected: " << dram_src[i] << endl;
    assert(dram_dst[i] == dram_src[i]);
  }
  
}

// -------------------------------
// ud rev runtime
// -------------------------------

void rt_init(ud_machine_t machine, uint64_t id, uint64_t prognum) {

  rt_machine_config = machine;
  
  // memory management currently provided as a rev service.
  // calc_addrmap();
  // MappedMemoryManager = new ud_mapped_memory_t(this->MachineConfig);
  rt_info.f.id = id & 0x0ffULL;
  rt_info.f.lanes = rt_machine_config.NumLanes & 0x0ffULL;
  rt_info.f.lmmode =  rt_machine_config.LocalMemAddrMode & 0x03ULL;
  rt_info.f.prognum = prognum & 0x0ffULL; // selects dram_tests_CHUNK<prognum>.bin
  rt_info.f.max_sim_iter = 100;           // TODO env var: UPDOWN_SIM_ITERATIONS

  // create basim object and load test file. ref: initMachine(programFile, pgbase)
  rt_status = udrt_init(); // placeholder
  rt_status = updown_init(&rt_machine_config, &rt_info);
  rt_wait();
}

void rt_start_exec(uint64_t nwid) {
  rt_status = updown_start_exec(nwid);
  rt_wait();
}

void rt_send_event(uint64_t* ev) {
  rt_status = updown_send(ev);
  rt_wait();
}

void* __attribute__((optimize("O0"))) rt_mm_malloc(uint64_t size) {
  // Memory manager temporarily provided as a rev service.
  //return MappedMemoryManager->get_region(size, false);
  ud_mem_desc_t desc;
  desc.size = size;
  desc.ptr = 0;
  rt_status = udrt_get_region_local(&desc);
  rt_wait();
  return desc.ptr;
}

void rt_test_wait_addr(uint64_t nwid, uint64_t offset, uint64_t expected) {
  TRACE_PUSH_OFF;
  updown_test_wait_addr(nwid, offset, expected);
  rt_wait();
  TRACE_POP;
}

void __attribute__((optimize("O0"))) rt_wait() {
  #ifndef INDUCE_ARGC_BUG
  TRACE_PUSH_OFF;
  #endif
  uint64_t ok = true;
  do {
    updown_check_ok(ok);
  } while (!ok);
  #ifndef INDUCE_ARGC_BUG
  TRACE_POP;
  #endif
}

//-----------------------------
// ud accelerator system calls
//-----------------------------

int updown_init(ud_machine_t* p_machine, ud_info_t* p_info) {
  int rc;
  uint64_t info = p_info->v;
  asm volatile (
    "move a0, %1  \n\t"  // p_machine
    "move a1, %2  \n\t"  // info
    "li a7, 3001  \n\t"
    //"ecall        \n\t"
    "xor x0,x0,x0 \n\t"
    "mv %0, a0"  
    : "=r" (rc) 
    : "r" (p_machine),"r" (info) 
    : "a0", "a1", "a7"
    );
  return rc;
}

int updown_send(uint64_t *event) {
  int rc;
  asm volatile (
    "move a1, %1  \n\t"
    "li a7, 3002  \n\t"
    //    "ecall        \n\t"
    "xor x0, x0, x0 \n\t"    
    "mv %0, a0    \n\t"
    : "=r" (rc) 
    : "r" (event) 
    : "a0", "a1", "a7"
    );
  return rc;
}

int updown_start_exec(uint64_t nwid) {
  int rc;
  asm volatile (
    "move a1, %1  \n\t"
    "li a7, 3003  \n\t"
    //"ecall        \n\t"
    "xor x0, x0, x0 \n\t"
    "mv %0, a0    \n\t"  
    : "=r" (rc) 
    : "r" (nwid)
    :"a0", "a1", "a7"
  );
  return rc;
}

int updown_test_wait_addr(uint64_t nwid, uint32_t offset, uint64_t expected) {
  int rc;
  asm volatile (
    "move a0, %1  \n\t" // nwid
    "move a1, %2  \n\t" // offset
    "move a2, %3  \n\t" // expected
    "li a7, 3004  \n\t"
    //    "ecall        \n\t"
    "xor x0, x0, x0 \n\t"
    "mv %0, a0    \n\t"  
    : "=r" (rc) 
    : "r" (nwid), "r" (offset), "r" (expected)
    :"a0", "a1", "a2", "a7"
  );
  return rc;
}

int updown_check_ok(uint64_t& ok) {
  int rc;
  asm volatile (
    "move a1, %1  \n\t" // &ok
    "li a7, 3005  \n\t"
    #if 0
    "ecall        \n\t"
    #else
    "li a0,0      \n\t"
    #endif
     "mv %0, a0   \n\t" 
    : "=r" (rc)
    : "r" (&ok)
    : "a0", "a1", "a7"
		);
  return 0;
}
//-----------------------
// ud runtime services
//-----------------------
int udrt_init() {
  int rc;
  asm volatile (
    "li a7, 3040  \n\t"
    //"ecall        \n\t"
    "xor x0, x0, x0 \n\t"
    "mv %0, a0    \n\t"
    : "=r" (rc)
  );
  return rc;
}

int udrt_get_region_local(ud_mem_desc_t *p_desc) {
  int rc;
  uint64_t size = p_desc->size;
  void*  ptr = &(p_desc->ptr);
  asm volatile (
    "move a0, %1  \n\t"
    "move a1, %2  \n\t"
    "li a7, 3041  \n\t"
    //"ecall        \n\t"
    "xor x0,x0,x0 \n\t"
    "mv %0, a0    \n\t"
    : "=r" (rc)
    : "r" (size), "r" (ptr) 
    : "a0", "a1", "a7"
  );
  return rc;
}
