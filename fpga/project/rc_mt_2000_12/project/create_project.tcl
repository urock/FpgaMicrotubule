

set part "xc7v2000tfhg1761-1"
set project_name "rc_mt_2000_12"
set top_entity	$project_name

set orgDir ".."
set srcDir	"$orgDir/src"
set hlsDir	"$orgDir/hls"
set hlsProject mt_hls

set randDir	"$orgDir/rand"
set randProject rand_hls
set rand_hls_ver 0.41

if {[file exists $srcDir]} {
   set srcDir [file normalize $srcDir]
   puts "Source Dir:\t $srcDir"
} else {
   error "ERROR: No valid location found for required Src Dir"
}


# create source files list
set src_files [list \
 "$srcDir/cores/axis_reg_slice_64/axis_reg_slice_64.xci"\
 "$srcDir/cores/fifo_to_user_ip/fifo_to_user_ip.xci"\
 "$srcDir/cores/fifo_from_user_ip/fifo_from_user_ip.xci"\
 "$srcDir/cores/tx_fifo/tx_fifo.xci"\
 "$srcDir/cores/ep_tx_fifo/ep_tx_fifo.xci"\
 "$srcDir/cores/rx_fifo/rx_fifo.xci"\
 "$srcDir/cores/ep_rx_fifo/ep_rx_fifo.xci"\
 "$srcDir/cores/fifo_1k_32/fifo_1k_32.xci"\
 "$srcDir/cores/cmd_fifo/cmd_fifo.xci"\
 "$srcDir/cores/axi_datamover_0/axi_datamover_0.xci"\
 "$srcDir/cores/axi_data_fifo/axi_data_fifo.xci"\
 "$srcDir/cores/axi_cmd_fifo/axi_cmd_fifo.xci"\
 "$srcDir/cores/axi_sts_fifo/axi_sts_fifo.xci"\
 "$srcDir/cores/axis_cc_buf/axis_cc_buf.xci"\
 "$srcDir/cores/clk_wiz/clk_wiz.xci"\
 "$srcDir/cores/pcie_7x_0/pcie_7x_0.xci"\
 "$srcDir/hdl/user/rand_wrapper.vhd"\
 "$srcDir/hdl/user/massive_rand_gen.vhd"\
 "$srcDir/hdl/user/user_ip.vhd"\
 "$srcDir/hdl/user/bd_hls_wrapper.vhd"\
 "$srcDir/hdl/base/base_ip.vhd"\
 "$srcDir/hdl/base/pcie_7x_0_pipe_clock.v"\
 "$srcDir/hdl/base/pcie_64b_master_ea.vhd"\
 "$srcDir/hdl/base/pcie_app_7x.vhd"\
 "$srcDir/hdl/base/rr_arbiter.vhd"\
 "$srcDir/hdl/base/reg_synch.vhd"\
 "$srcDir/hdl/base/axi_system_wrapper.vhd"\
 "$srcDir/hdl/base/stream_axi_bridge.vhd"\
 "$srcDir/hdl/rc_2000_top.vhd"\
]

# create xdc files list
set xdc_files [list \
	"$srcDir/xdc/rc_2000_top.xdc"\
]


#block design file
set bd_file "$srcDir/bd/axi_system/axi_system.bd"
#set bd_file "./$project_name.srcs/sources_1/bd/axi_system/axi_system.bd"


# Create project
create_project -force $project_name 

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [get_projects $project_name]
set_property "default_lib" "xil_defaultlib" $obj
set_property "part" $part $obj
set_property "simulator_language" "Mixed" $obj

# Add user repository
set_property ip_repo_paths  $hlsDir/$hlsProject/solution1/impl/ip [current_project]
update_ip_catalog

set_property ip_repo_paths  [list $hlsDir/$hlsProject/solution1/impl/ip $randDir/$randProject/solution1/impl/ip] [current_project]
update_ip_catalog
create_ip -name rand_top -vendor urock -library hls -version $rand_hls_ver -module_name rand_core

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}

# Set 'sources_1' fileset object
set file_set [get_filesets sources_1]
add_files -norecurse -fileset $file_set $src_files

foreach file $src_files {
	puts -nonewline "$file"
	if [regexp {.*\.vhd$} $file tmp ] {
	
		puts "\t\t is VHD file"
		set file_obj [get_files -of_objects $file_set [list "*$file"]]
		set_property "file_type" "VHDL" $file_obj
	
	} elseif [regexp {.*\.v$} $file tmp ] {
		puts "\t\t is Verilog file"
	} elseif [regexp {.*\.xci$} $file tmp ] {
	
		puts "\t\t is XCI file"
		set file_obj [get_files -of_objects $file_set [list "*$file"]]
		set_property "generate_synth_checkpoint" "0" $file_obj
	
	}
}
set_property "top" $top_entity $file_set


# Create 'constrs_1' fileset (if not found)
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset constrs_1
}


# Set 'constrs_1' fileset object
set file_set [get_filesets constrs_1]
add_files -norecurse -fileset $file_set $xdc_files

foreach file $xdc_files {
	puts "$file"
	set file_obj [get_files -of_objects $file_set [list "*$file"]]
	set_property "file_type" "XDC" $file_obj	
}


# Add Block Design Files to project

if {[file exists $bd_file]} {
	puts "Block Design exists. Adding $bd_file to design"
	add_files -norecurse $bd_file
} else {
	puts "Block Design does not exist. Create one"
	source create_bd_base.tcl
}

open_bd_design ./$project_name.srcs/sources_1/bd/axi_system/axi_system.bd
catch {validate_bd_design}
save_bd_design
close_bd_design [get_bd_designs axi_system]
open_bd_design ./$project_name.srcs/sources_1/bd/axi_system/axi_system.bd
catch {validate_bd_design}
save_bd_design
close_bd_design [get_bd_designs axi_system]


# Add HLS Block Design to project
puts "Create HLS Block Design"
source create_bd_hls.tcl


# Create 'synth_1' run (if not found)
if {[string equal [get_runs -quiet synth_1] ""]} {
  create_run -name synth_1 -part $part -flow {Vivado Synthesis 2014} -strategy "Vivado Synthesis Defaults" -constrset constrs_1
} else {
  set_property strategy "Vivado Synthesis Defaults" [get_runs synth_1]
  set_property flow "Vivado Synthesis 2014" [get_runs synth_1]
}
set obj [get_runs synth_1]
set_property "part" $part $obj

# set the current synth run
current_run -synthesis [get_runs synth_1]

# Create 'impl_1' run (if not found)
if {[string equal [get_runs -quiet impl_1] ""]} {
  create_run -name impl_1 -part $part -flow {Vivado Implementation 2014} -strategy "Vivado Implementation Defaults" -constrset constrs_1 -parent_run synth_1
} else {
  set_property strategy "Vivado Implementation Defaults" [get_runs impl_1]
  set_property flow "Vivado Implementation 2014" [get_runs impl_1]
}
set obj [get_runs impl_1]
set_property "part" $part $obj

# set the current impl run
current_run -implementation [get_runs impl_1]

puts "INFO: Project created: $project_name"
