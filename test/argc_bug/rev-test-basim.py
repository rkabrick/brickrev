#
# Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# See LICENSE in the top level directory for licensing details
#
# rev-test-basim.py
#

import os
import sst

# Environment settings
rev_exe = os.getenv("REV_EXE", "")
arch = os.getenv("ARCH","RV64G").upper()
simNodes = int(os.getenv('SIM_NODES', 1))
numHarts = os.getenv("NUM_HARTS",1)

if ( rev_exe == "" ):
  print("ERROR: REV_EXE is not set",file=sys.stderr);
  exit(1);

print(f"REV_EXE={rev_exe}")
print(f"ARCH={arch}")
print(f"SIM_NODES={simNodes}")
print(f"NUM_HARTS={numHarts}")

# test args from env
num_lanes = os.getenv("NUM_LANES", 1)
num_thr_per_lane = os.getenv("NUM_THR_PER_LANE", 1)
num_elem_per_thread = os.getenv("NUM_ELEM_PER_THREAD", 1)
chunk = os.getenv("CHUNK", 0)
test_args = f"{num_lanes} {num_thr_per_lane} {num_elem_per_thread} {chunk}"

print(f"NUM_LANES={num_lanes}");
print(f"NUM_THR_PER_LANE={num_thr_per_lane}");
print(f"NUM_ELEM_PER_THREAD={num_elem_per_thread}");
print(f"CHUNK={chunk}");
print(f"TEST_ARGS={test_args}");

# Define SST core options
sst.setProgramOption("timebase", "1ps")

# Tell SST what statistics handling we want
sst.setStatisticLoadLevel(4)

max_addr_gb = 1

# Define the simulation components
# Instantiate all the CPUs
for i in range(0, simNodes):
  print("Building "+ str(i))
  comp_cpu = sst.Component("cpu" + str(i), "revcpu.RevCPU")
  comp_cpu.addParams({
	"verbose" : 5,                                # Verbosity
        "numCores" : 1,                               # Number of cores
        "numHarts" : numHarts,                        # Number of harts
	      "clock" : "1.0GHz",                     # Clock
        "memSize" : 1024*1024*1024,                   # Memory size in bytes
        "machine" : f"[CORES:{arch}]",                # Core:Config; common arch for all
        "startAddr" : "[CORES:0x00000000]",           # Starting address for core 0
        # "startSymbol" : "[0:_start]",
        "memCost" : "[0:1:10]",                       # Memory loads required 1-10 cycles
        "program" : rev_exe,                          # Target executable
        "args"    : test_args,                        # Program arguments
        "trcStartCycle" : 1,                        # Trace everything after this cycle
        # "enableUpDown" : 1,
        "splash" : 0,                                 # Display the splash message
        })
#  comp_cpu.enableAllStatistics()

# udaccel = comp_cpu.setSubComponent("basim", "revcpu.SimpleUpDownCoProc")
# udaccel.addParams({
#   "clockFreq"  : "1.5Ghz",
#   "verbose" : 10
# })

# sst.setStatisticOutput("sst.statOutputCSV")
# sst.enableAllStatisticsForAllComponents()

# EOF
