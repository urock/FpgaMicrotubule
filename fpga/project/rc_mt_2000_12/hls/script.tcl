############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2014 Xilinx Inc. All rights reserved.
############################################################
open_project -reset mt_hls
set_top mt_top
add_files ./src/mt_fpga.cpp
add_files -tb ./src/mt_cpu.cpp
add_files -tb ./src/mt_test.cpp
open_solution "solution1"
set_part {xc7v2000tfhg1761-1}
create_clock -period 5 -name default
source "./src/directives.tcl"
#csim_design
csynth_design
export_design -format ip_catalog -vendor "urock" -version "0.23"

