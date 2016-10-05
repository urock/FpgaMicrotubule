
################################################################
# This is a generated script based on design: bd_hls
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2014.4
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   puts "ERROR: This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source bd_hls_script.tcl

# If you do not already have a project created,
# you can create a project using the following command:
#    create_project project_1 myproj -part xc7v2000tfhg1761-1

set mt_hls_ver 0.22


# CHANGE DESIGN NAME HERE
set design_name bd_hls

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# CHECKING IF PROJECT EXISTS
if { [get_projects -quiet] eq "" } {
   puts "ERROR: Please open or create a project!"
   return 1
}


# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "ERROR: Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      puts "INFO: Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   puts "INFO: Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "ERROR: Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "ERROR: Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   puts "INFO: Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   puts "INFO: Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

puts "INFO: Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   puts $errMsg
   return $nRet
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  global mt_hls_ver

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     puts "ERROR: Unable to find parent cell <$parentCell>!"
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     puts "ERROR: Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set M00_AXI [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M00_AXI ]
  set_property -dict [ list CONFIG.ADDR_WIDTH {32} CONFIG.DATA_WIDTH {64} CONFIG.FREQ_HZ {200000000} CONFIG.NUM_READ_OUTSTANDING {2} CONFIG.NUM_WRITE_OUTSTANDING {2} CONFIG.PROTOCOL {AXI4}  ] $M00_AXI
  set ap_ctrl [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:acc_handshake_rtl:1.0 ap_ctrl ]

  # Create ports
  set InputLength [ create_bd_port -dir I -from 31 -to 0 -type data InputLength ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}}}}  ] $InputLength
  set ap_clk [ create_bd_port -dir I -type clk ap_clk ]
  set_property -dict [ list CONFIG.FREQ_HZ {200000000}  ] $ap_clk
  set ap_return [ create_bd_port -dir O -from 31 -to 0 -type data ap_return ]
  set ap_rst_n [ create_bd_port -dir I -type rst ap_rst_n ]
  set_property -dict [ list CONFIG.POLARITY {ACTIVE_LOW}  ] $ap_rst_n
  set bus_ptr_i [ create_bd_port -dir I -from 31 -to 0 -type data bus_ptr_i ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0}}}}}  ] $bus_ptr_i
  set cp_size [ create_bd_port -dir I -from 31 -to 0 -type data cp_size ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}}}}  ] $cp_size
  set offset_i [ create_bd_port -dir I -from 31 -to 0 -type data offset_i ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}}}}  ] $offset_i
  set offset_o [ create_bd_port -dir I -from 31 -to 0 -type data offset_o ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}}}}  ] $offset_o
  set p1 [ create_bd_port -dir I -from 31 -to 0 -type data p1 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}}}}  ] $p1
  set p2 [ create_bd_port -dir I -from 31 -to 0 -type data p2 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}}}}  ] $p2
  set stream1 [ create_bd_port -dir I -from 63 -to 0 -type data stream1 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type automatic dependency {} format long minimum {} maximum {}} value 0} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} struct {field_d0 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d0} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}} field_d1 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d1} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}}}}}}}  ] $stream1
  set stream1_ap_ack [ create_bd_port -dir O stream1_ap_ack ]
  set stream2 [ create_bd_port -dir I -from 63 -to 0 -type data stream2 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type automatic dependency {} format long minimum {} maximum {}} value 0} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} struct {field_d0 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d0} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}} field_d1 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d1} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}}}}}}}  ] $stream2
  set stream2_ap_ack [ create_bd_port -dir O stream2_ap_ack ]
  set stream3 [ create_bd_port -dir I -from 63 -to 0 -type data stream3 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type automatic dependency {} format long minimum {} maximum {}} value 0} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} struct {field_d0 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d0} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}} field_d1 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d1} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}}}}}}}  ] $stream3
  set stream3_ap_ack [ create_bd_port -dir O stream3_ap_ack ]
  set stream4 [ create_bd_port -dir I -from 63 -to 0 -type data stream4 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type automatic dependency {} format long minimum {} maximum {}} value 0} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} struct {field_d0 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d0} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}} field_d1 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d1} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}}}}}}}  ] $stream4
  set stream4_ap_ack [ create_bd_port -dir O stream4_ap_ack ]
  set stream5 [ create_bd_port -dir I -from 63 -to 0 -type data stream5 ]
  set_property -dict [ list CONFIG.LAYERED_METADATA {xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type automatic dependency {} format long minimum {} maximum {}} value 0} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} struct {field_d0 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d0} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}} field_d1 {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value d1} enabled {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value true} datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 32} real {float {sigwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 24}}}}}}}}}}  ] $stream5
  set stream5_ap_ack [ create_bd_port -dir O stream5_ap_ack ]

  # Create instance: axi_interconn_hls, and set properties
  set axi_interconn_hls [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconn_hls ]
  set_property -dict [ list CONFIG.ENABLE_ADVANCED_OPTIONS {1} CONFIG.M00_HAS_REGSLICE {1} CONFIG.NUM_MI {1} CONFIG.S00_HAS_DATA_FIFO {0} CONFIG.S00_HAS_REGSLICE {1}  ] $axi_interconn_hls

  # Create instance: mt_top_0, and set properties
  set mt_top_0 [ create_bd_cell -type ip -vlnv urock:hls:mt_top:$mt_hls_ver mt_top_0 ]

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]
  set_property -dict [ list CONFIG.C_EXT_RST_WIDTH {4}  ] $proc_sys_reset_0

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create interface connections
  connect_bd_intf_net -intf_net ap_ctrl_1 [get_bd_intf_ports ap_ctrl] [get_bd_intf_pins mt_top_0/ap_ctrl]
  connect_bd_intf_net -intf_net axi_interconn_hls_M00_AXI [get_bd_intf_ports M00_AXI] [get_bd_intf_pins axi_interconn_hls/M00_AXI]
  connect_bd_intf_net -intf_net mt_top_0_m_axi_gmem64 [get_bd_intf_pins axi_interconn_hls/S00_AXI] [get_bd_intf_pins mt_top_0/m_axi_gmem64]

  # Create port connections
  connect_bd_net -net InputLength_1 [get_bd_ports InputLength] [get_bd_pins mt_top_0/InputLength]
  connect_bd_net -net ap_clk_1 [get_bd_ports ap_clk] [get_bd_pins axi_interconn_hls/ACLK] [get_bd_pins axi_interconn_hls/M00_ACLK] [get_bd_pins axi_interconn_hls/S00_ACLK] [get_bd_pins mt_top_0/ap_clk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk]
  connect_bd_net -net ap_rst_n_1 [get_bd_ports ap_rst_n] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net bus_ptr_i_1 [get_bd_ports bus_ptr_i] [get_bd_pins mt_top_0/bus_ptr_i]
  connect_bd_net -net cp_size_1 [get_bd_ports cp_size] [get_bd_pins mt_top_0/cp_size]
  connect_bd_net -net mt_top_0_ap_return [get_bd_ports ap_return] [get_bd_pins mt_top_0/ap_return]
  connect_bd_net -net mt_top_0_stream1_ap_ack [get_bd_ports stream1_ap_ack] [get_bd_pins mt_top_0/stream1_ap_ack]
  connect_bd_net -net mt_top_0_stream2_ap_ack [get_bd_ports stream2_ap_ack] [get_bd_pins mt_top_0/stream2_ap_ack]
  connect_bd_net -net mt_top_0_stream3_ap_ack [get_bd_ports stream3_ap_ack] [get_bd_pins mt_top_0/stream3_ap_ack]
  connect_bd_net -net mt_top_0_stream4_ap_ack [get_bd_ports stream4_ap_ack] [get_bd_pins mt_top_0/stream4_ap_ack]
  connect_bd_net -net mt_top_0_stream5_ap_ack [get_bd_ports stream5_ap_ack] [get_bd_pins mt_top_0/stream5_ap_ack]
  connect_bd_net -net offset_i_1 [get_bd_ports offset_i] [get_bd_pins mt_top_0/offset_i]
  connect_bd_net -net offset_o_1 [get_bd_ports offset_o] [get_bd_pins mt_top_0/offset_o]
  connect_bd_net -net p1_1 [get_bd_ports p1] [get_bd_pins mt_top_0/p1]
  connect_bd_net -net p2_1 [get_bd_ports p2] [get_bd_pins mt_top_0/p2]
  connect_bd_net -net proc_sys_reset_0_interconnect_aresetn [get_bd_pins axi_interconn_hls/ARESETN] [get_bd_pins axi_interconn_hls/M00_ARESETN] [get_bd_pins axi_interconn_hls/S00_ARESETN] [get_bd_pins mt_top_0/ap_rst_n] [get_bd_pins proc_sys_reset_0/interconnect_aresetn]
  connect_bd_net -net stream1_1 [get_bd_ports stream1] [get_bd_pins mt_top_0/stream1]
  connect_bd_net -net stream2_1 [get_bd_ports stream2] [get_bd_pins mt_top_0/stream2]
  connect_bd_net -net stream3_1 [get_bd_ports stream3] [get_bd_pins mt_top_0/stream3]
  connect_bd_net -net stream4_1 [get_bd_ports stream4] [get_bd_pins mt_top_0/stream4]
  connect_bd_net -net stream5_1 [get_bd_ports stream5] [get_bd_pins mt_top_0/stream5]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins proc_sys_reset_0/dcm_locked] [get_bd_pins xlconstant_0/dout]

  # Create address segments
  create_bd_addr_seg -range 0x100000000 -offset 0x0 [get_bd_addr_spaces mt_top_0/Data_m_axi_gmem64] [get_bd_addr_segs M00_AXI/Reg] SEG_M00_AXI_Reg
  

  # Restore current instance
  current_bd_instance $oldCurInst

  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


