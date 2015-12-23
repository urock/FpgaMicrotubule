
###############################################################################
# Timing Constraints
###############################################################################

#
# Timing requirements and related constraints.
#

create_clock -period 10.000 -name pcie_clk [get_pins base/refclk_ibuf/O]
create_clock -period 10.000 -name diff_clk [get_pins diffclk_ibuf/O]
create_clock -period 10.000 -name clk100 [get_ports clk100]


set_clock_groups -name asynch_clk_pcie_diff -asynchronous -group [get_clocks -include_generated_clocks pcie_clk] -group [get_clocks -include_generated_clocks diff_clk]

set_clock_groups -name asynch_clk_pcie_100 -asynchronous -group [get_clocks -include_generated_clocks pcie_clk] -group [get_clocks -include_generated_clocks clk100]

set_clock_groups -name asynch_clk_100_diff -asynchronous -group [get_clocks -include_generated_clocks clk100] -group [get_clocks -include_generated_clocks diff_clk]


create_generated_clock -name ip_clk -source [get_pins user/ip_clk_gen/CLK_IN1] -divide_by 5 -multiply_by 10 [get_pins user/ip_clk_gen/CLK_OUT1]






###############################################################################
# GTXE2
###############################################################################
set_property LOC GTXE2_COMMON_X0Y7 [get_cells {base/pcie_7x_0_i/inst/inst/gt_top_i/pipe_wrapper_i/pipe_lane[0].pipe_quad.gt_common_enabled.gt_common_int.gt_common_i/qpll_wrapper_i/gtx_common.gtxe2_common_i}]




###############################################################################
#
# SYS reset (input) signal.
#
set_property IOSTANDARD LVCMOS18 [get_ports sys_rst_n]
set_property PULLUP true [get_ports sys_rst_n]
set_property PACKAGE_PIN AL36 [get_ports sys_rst_n]
set_false_path -from [get_ports sys_rst_n]

###############################################################################
# Physical Constraints
###############################################################################
#
# SYS clock 100 MHz (input) signal.
#
set_property PACKAGE_PIN G9 [get_ports sys_clk_n]

set_property PACKAGE_PIN F4 [get_ports {pci_exp_txp[0]}]

set_property PACKAGE_PIN G2 [get_ports {pci_exp_txp[1]}]

set_property PACKAGE_PIN H4 [get_ports {pci_exp_txp[2]}]

set_property PACKAGE_PIN J2 [get_ports {pci_exp_txp[3]}]



# Diff Clock
set_property PACKAGE_PIN AB7 [get_ports aclk_n]

# Local Clock
set_property IOSTANDARD LVCMOS18 [get_ports clk100]
set_property PACKAGE_PIN AK34 [get_ports clk100]

# Command
#set_property IOSTANDARD LVCMOS18 [get_ports strtcmd]
#set_property PACKAGE_PIN AM34 [get_ports strtcmd]

## ID
#set_property IOSTANDARD LVCMOS18 [get_ports {id[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {id[1]}]
#set_property PACKAGE_PIN AH34 [get_ports {id[0]}]
#set_property PACKAGE_PIN AJ35 [get_ports {id[1]}]

## Command
#set_property IOSTANDARD LVCMOS18 [get_ports dval]
#set_property PACKAGE_PIN AM31 [get_ports dval]
#set_property PULLUP true [get_ports dval]
#set_property SLEW FAST [get_ports dval]

#set_property IOSTANDARD LVCMOS18 [get_ports {command[0]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[1]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[2]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[3]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[4]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[5]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[6]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {command[7]}]

#set_property PACKAGE_PIN AM36 [get_ports {command[0]}]
#set_property PACKAGE_PIN AN36 [get_ports {command[1]}]
#set_property PACKAGE_PIN AJ36 [get_ports {command[2]}]
#set_property PACKAGE_PIN AJ37 [get_ports {command[3]}]
#set_property PACKAGE_PIN AK37 [get_ports {command[4]}]
#set_property PACKAGE_PIN AL37 [get_ports {command[5]}]
#set_property PACKAGE_PIN AN35 [get_ports {command[6]}]
#set_property PACKAGE_PIN AP35 [get_ports {command[7]}]


###############################################################################
# Rosta Constraints
###############################################################################

set_property IO_BUFFER_TYPE none [get_ports sys_clk_p]
set_property IO_BUFFER_TYPE none [get_ports sys_clk_n]
set_property IO_BUFFER_TYPE none [get_ports {pci_exp_txp[*]}]
set_property IO_BUFFER_TYPE none [get_ports {pci_exp_txn[*]}]
set_property IO_BUFFER_TYPE none [get_ports {pci_exp_rxp[*]}]
set_property IO_BUFFER_TYPE none [get_ports {pci_exp_rxn[*]}]

set_property IO_BUFFER_TYPE none [get_ports clk100]



set_property IO_BUFFER_TYPE none [get_ports Top_ddr_reset_n]
set_property IO_BUFFER_TYPE none [get_ports {Top_ddr_dq[*]}]
set_property IO_BUFFER_TYPE none [get_ports {Top_ddr_dqs_p[*]}]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_we_n]
set_property IO_BUFFER_TYPE none [get_ports {Top_ddr_dqs_n[*]}]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_ras_n]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_odt]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_ck_n]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_cke]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_cs_n]
set_property IO_BUFFER_TYPE none [get_ports {Top_ddr_ba[*]}]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_cas_n]
set_property IO_BUFFER_TYPE none [get_ports Top_ddr_ck_p]
set_property IO_BUFFER_TYPE none [get_ports {Top_ddr_dm[*]}]
set_property IO_BUFFER_TYPE none [get_ports {Top_ddr_addr[*]}]





###############################################################################
# BITSTREAM
###############################################################################

set_property DCI_CASCADE {37 39} [get_iobanks 38]
set_property DCI_CASCADE {31 32} [get_iobanks 33]


set_property BITSTREAM.CONFIG.CCLKPIN Pullup [current_design]
set_property BITSTREAM.CONFIG.DONEPIN Pullup [current_design]
set_property BITSTREAM.STARTUP.STARTUPCLK Cclk [current_design]

set_property BITSTREAM.STARTUP.GTS_CYCLE 1 [current_design]
set_property BITSTREAM.STARTUP.GWE_CYCLE 2 [current_design]
set_property BITSTREAM.STARTUP.LCK_CYCLE NoWait [current_design]
set_property BITSTREAM.STARTUP.DONE_CYCLE 6 [current_design]
set_property BITSTREAM.STARTUP.DONEPIPE Yes [current_design]

set_property BITSTREAM.STARTUP.MATCH_CYCLE NoWait [current_design]

set_property BITSTREAM.GENERAL.COMPRESS true [current_design]


###############################################################################
# pBlock
###############################################################################



create_pblock pblock_base
add_cells_to_pblock [get_pblocks pblock_base] [get_cells -quiet [list base]]
add_cells_to_pblock [get_pblocks pblock_base] [get_cells -quiet [list user]]
#resize_pblock [get_pblocks pblock_base] -add {CLOCKREGION_X0Y6:CLOCKREGION_X1Y8}
resize_pblock [get_pblocks pblock_base] -add {SLICE_X0Y300:SLICE_X517Y449 DSP48_X0Y120:DSP48_X8Y179 RAMB18_X0Y120:RAMB18_X10Y179 RAMB36_X0Y60:RAMB36_X10Y89}

create_pblock pblock_hls
add_cells_to_pblock [get_pblocks pblock_hls] [get_cells -quiet [list hls]]
#resize_pblock [get_pblocks pblock_hls] -add {CLOCKREGION_X0Y0:CLOCKREGION_X1Y5}
resize_pblock [get_pblocks pblock_hls] -add {SLICE_X0Y0:SLICE_X517Y299 DSP48_X0Y0:DSP48_X8Y119 RAMB18_X0Y0:RAMB18_X10Y119 RAMB36_X0Y0:RAMB36_X10Y59}


###############################################################################
# FALSE PATH
###############################################################################

set_false_path -from [get_pins {base/app/master/UserControlReg_c_reg[24]/C}]
set_false_path -from [get_pins {base/axi_system/d_rst_cnt_reg[*]/C}]
set_false_path -from [get_pins {base/axi_system/axi_system_i/proc_sys_reset_0/U0/ACTIVE_LOW_PR_OUT_DFF[0].peripheral_aresetn_reg[0]/C}]
set_false_path -from [get_pins {base/axi_system/axi_system_i/proc_sys_reset_1/U0/ACTIVE_LOW_PR_OUT_DFF[0].peripheral_aresetn_reg[0]/C}]

set_false_path -from [get_pins base/pipe_clock_i/pclk_sel_reg/C]


set_false_path -from [get_pins {base/axi_system/den_in_counter_reg[*]/C}]
set_false_path -from [get_pins base/axi_system/axi_system_i/ddr_c_top/mig_7series_0/u_axi_system_mig_7series_0_0_mig/u_memc_ui_top_axi/mem_intfc0/ddr_phy_top0/u_ddr_calib_top/init_calib_complete_reg/C]
set_false_path -from [get_pins base/axi_system/axi_system_i/xadc_wiz_0/inst/DCLK] -to [get_pins {base/axi_system/axi_system_i/ddr_c_top/mig_7series_0/u_axi_system_mig_7series_0_0_mig/temp_mon_enabled.u_tempmon/device_temp_sync_r1_reg[*]/D}]



set_false_path -from [get_clocks clk100] -to [get_clocks ip_clk]
set_false_path -from [get_clocks ip_clk] -to [get_clocks clk100]

set_false_path -from [get_clocks userclk1] -to [get_clocks ip_clk]
set_false_path -from [get_clocks ip_clk] -to [get_clocks userclk1]

set_false_path -from [get_clocks clk_250mhz] -to [get_clocks clk_125mhz]
set_false_path -from [get_clocks clk_125mhz] -to [get_clocks clk_250mhz]

