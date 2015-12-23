--Copyright 1986-2014 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2014.4 (win64) Build 1071353 Tue Nov 18 18:24:04 MST 2014
--Date        : Sat Jul 04 17:46:22 2015
--Host        : xserverPC running 64-bit major release  (build 7600)
--Command     : generate_target bd_hls_wrapper.bd
--Design      : bd_hls_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity bd_hls_wrapper is
  port (
  
    ap_clk 			: in STD_LOGIC;
    ap_rst_n 		: in STD_LOGIC;
    ap_ctrl_done 	: out STD_LOGIC;
    ap_ctrl_idle 	: out STD_LOGIC;
    ap_ctrl_ready 	: out STD_LOGIC;
    ap_ctrl_start 	: in STD_LOGIC;
    ap_return 		: out STD_LOGIC_VECTOR ( 31 downto 0 );
	
    cp_size 	: in STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_i 	: in STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_o 	: in STD_LOGIC_VECTOR ( 31 downto 0 );
    p1 			: in STD_LOGIC_VECTOR ( 31 downto 0 );
    p2 			: in STD_LOGIC_VECTOR ( 31 downto 0 );
    InputLength : in STD_LOGIC_VECTOR ( 31 downto 0 );
	
	rand1_tdata 			: in  std_logic_vector(63 downto 0);
	rand1_tvalid 			: in  std_logic;
	rand1_tready 			: out std_logic;
	
	rand2_tdata 			: in  std_logic_vector(63 downto 0);
	rand2_tvalid 			: in  std_logic;
	rand2_tready 			: out std_logic;

	rand3_tdata 			: in  std_logic_vector(63 downto 0);
	rand3_tvalid 			: in  std_logic;
	rand3_tready 			: out std_logic;

	rand4_tdata 			: in  std_logic_vector(63 downto 0);
	rand4_tvalid 			: in  std_logic;
	rand4_tready 			: out std_logic;

	rand5_tdata 			: in  std_logic_vector(63 downto 0);
	rand5_tvalid 			: in  std_logic;
	rand5_tready 			: out std_logic;

	
    M00_AXI_araddr 	: out STD_LOGIC_VECTOR ( 31 downto 0 );
    M00_AXI_arburst : out STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_arcache : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arlen 	: out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_arlock 	: out STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXI_arprot 	: out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_arqos 	: out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arready : in STD_LOGIC;
    M00_AXI_arregion : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arsize 	: out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_arvalid : out STD_LOGIC;
    M00_AXI_awaddr 	: out STD_LOGIC_VECTOR ( 31 downto 0 );
    M00_AXI_awburst : out STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_awcache : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awlen 	: out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_awlock 	: out STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXI_awprot 	: out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_awqos 	: out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awready : in STD_LOGIC;
    M00_AXI_awregion : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awsize 	: out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_awvalid : out STD_LOGIC;
    M00_AXI_bready 	: out STD_LOGIC;
    M00_AXI_bresp 	: in STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_bvalid 	: in STD_LOGIC;
    M00_AXI_rdata 	: in STD_LOGIC_VECTOR ( 63 downto 0 );
    M00_AXI_rlast 	: in STD_LOGIC;
    M00_AXI_rready 	: out STD_LOGIC;
    M00_AXI_rresp 	: in STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_rvalid 	: in STD_LOGIC;
    M00_AXI_wdata 	: out STD_LOGIC_VECTOR ( 63 downto 0 );
    M00_AXI_wlast 	: out STD_LOGIC;
    M00_AXI_wready 	: in STD_LOGIC;
    M00_AXI_wstrb 	: out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_wvalid 	: out STD_LOGIC


  );
end bd_hls_wrapper;

architecture STRUCTURE of bd_hls_wrapper is
  component bd_hls is
  port (
    ap_ctrl_start : in STD_LOGIC;
    ap_ctrl_done : out STD_LOGIC;
    ap_ctrl_idle : out STD_LOGIC;
    ap_ctrl_ready : out STD_LOGIC;
    ap_clk : in STD_LOGIC;
    ap_return : out STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_o : in STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_i : in STD_LOGIC_VECTOR ( 31 downto 0 );
    p2 : in STD_LOGIC_VECTOR ( 31 downto 0 );
    p1 : in STD_LOGIC_VECTOR ( 31 downto 0 );
    InputLength : in STD_LOGIC_VECTOR ( 31 downto 0 );
    cp_size : in STD_LOGIC_VECTOR ( 31 downto 0 );
    bus_ptr_i : in STD_LOGIC_VECTOR ( 31 downto 0 );
    ap_rst_n : in STD_LOGIC;
    M00_AXI_awaddr : out STD_LOGIC_VECTOR ( 31 downto 0 );
    M00_AXI_awlen : out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_awsize : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_awburst : out STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_awlock : out STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXI_awcache : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awprot : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_awregion : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awqos : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awvalid : out STD_LOGIC;
    M00_AXI_awready : in STD_LOGIC;
    M00_AXI_wdata : out STD_LOGIC_VECTOR ( 63 downto 0 );
    M00_AXI_wstrb : out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_wlast : out STD_LOGIC;
    M00_AXI_wvalid : out STD_LOGIC;
    M00_AXI_wready : in STD_LOGIC;
    M00_AXI_bresp : in STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_bvalid : in STD_LOGIC;
    M00_AXI_bready : out STD_LOGIC;
    M00_AXI_araddr : out STD_LOGIC_VECTOR ( 31 downto 0 );
    M00_AXI_arlen : out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_arsize : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_arburst : out STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_arlock : out STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXI_arcache : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arprot : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_arregion : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arqos : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arvalid : out STD_LOGIC;
    M00_AXI_arready : in STD_LOGIC;
    M00_AXI_rdata : in STD_LOGIC_VECTOR ( 63 downto 0 );
    M00_AXI_rresp : in STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_rlast : in STD_LOGIC;
    M00_AXI_rvalid : in STD_LOGIC;
    M00_AXI_rready : out STD_LOGIC;
	
	stream1				: in  std_logic_vector(63 downto 0);
	stream1_ap_ack		: out std_logic;
	stream2				: in  std_logic_vector(63 downto 0);
	stream2_ap_ack		: out std_logic;
	stream3				: in  std_logic_vector(63 downto 0);
	stream3_ap_ack		: out std_logic;
	stream4				: in  std_logic_vector(63 downto 0);
	stream4_ap_ack		: out std_logic;
	stream5				: in  std_logic_vector(63 downto 0);
	stream5_ap_ack		: out std_logic 		
  );
  end component bd_hls;
  
  signal AXI_TARGET_ADDRESS	: std_logic_vector(31 downto 0);


signal ap_rst_n_r 		: STD_LOGIC;							-- +
signal ap_ctrl_done_r 	: STD_LOGIC;							-- +
signal ap_ctrl_idle_r 	: STD_LOGIC;
signal ap_ctrl_ready_r 	: STD_LOGIC;
signal ap_ctrl_start_r 	: STD_LOGIC;							-- +
signal ap_return_r 		: STD_LOGIC_VECTOR ( 31 downto 0 );		-- +  

signal cp_size_r 		: STD_LOGIC_VECTOR ( 31 downto 0 );
signal offset_i_r 		: STD_LOGIC_VECTOR ( 31 downto 0 );
signal offset_o_r 		: STD_LOGIC_VECTOR ( 31 downto 0 );
signal p1_r 			: STD_LOGIC_VECTOR ( 31 downto 0 );
signal p2_r 			: STD_LOGIC_VECTOR ( 31 downto 0 );
signal InputLength_r 	: STD_LOGIC_VECTOR ( 31 downto 0 );

	
component axis_reg_slice_64
	port (
		s_aclk 			: in  std_logic;
		s_aresetn 		: in  std_logic;
		
		s_axis_tvalid 	: in  std_logic;
		s_axis_tready 	: out std_logic;
		s_axis_tdata 	: in  std_logic_vector(63 downto 0);
		
		m_axis_tvalid 	: out std_logic;
		m_axis_tready 	: in  std_logic;
		m_axis_tdata 	: out std_logic_vector(63 downto 0)
	);
end component;


signal stream1				: std_logic_vector(63 downto 0);
signal stream1_ap_ack		: std_logic;
signal stream2				: std_logic_vector(63 downto 0);
signal stream2_ap_ack		: std_logic;
signal stream3				: std_logic_vector(63 downto 0);
signal stream3_ap_ack		: std_logic;
signal stream4				: std_logic_vector(63 downto 0);
signal stream4_ap_ack		: std_logic;
signal stream5				: std_logic_vector(63 downto 0);
signal stream5_ap_ack		: std_logic;	

	
  
begin


process(ap_clk)
begin
	if rising_edge(ap_clk) then
	
	
		
		ap_ctrl_done 	<= ap_ctrl_done_r;
		ap_ctrl_idle 	<= ap_ctrl_idle_r;
		ap_ctrl_ready 	<= ap_ctrl_ready_r;
		ap_return 		<= ap_return_r;

		ap_rst_n_r 		<= ap_rst_n;
		ap_ctrl_start_r	<= ap_ctrl_start;
		                                 
		cp_size_r 		<= cp_size;
		offset_i_r 		<= offset_i;
		offset_o_r 		<= offset_o;
		p1_r 			<= p1;
		p2_r 			<= p2;
		InputLength_r 	<= InputLength;
	
	

	
	end if;
end process;


bd_hls_i: component bd_hls
    port map (
      InputLength(31 downto 0) => InputLength_r(31 downto 0),
      M00_AXI_araddr(31 downto 0) => M00_AXI_araddr(31 downto 0),
      M00_AXI_arburst(1 downto 0) => M00_AXI_arburst(1 downto 0),
      M00_AXI_arcache(3 downto 0) => M00_AXI_arcache(3 downto 0),
      M00_AXI_arlen(7 downto 0) => M00_AXI_arlen(7 downto 0),
      M00_AXI_arlock(0) => M00_AXI_arlock(0),
      M00_AXI_arprot(2 downto 0) => M00_AXI_arprot(2 downto 0),
      M00_AXI_arqos(3 downto 0) => M00_AXI_arqos(3 downto 0),
      M00_AXI_arready => M00_AXI_arready,
      M00_AXI_arregion(3 downto 0) => M00_AXI_arregion(3 downto 0),
      M00_AXI_arsize(2 downto 0) => M00_AXI_arsize(2 downto 0),
      M00_AXI_arvalid => M00_AXI_arvalid,
      M00_AXI_awaddr(31 downto 0) => M00_AXI_awaddr(31 downto 0),
      M00_AXI_awburst(1 downto 0) => M00_AXI_awburst(1 downto 0),
      M00_AXI_awcache(3 downto 0) => M00_AXI_awcache(3 downto 0),
      M00_AXI_awlen(7 downto 0) => M00_AXI_awlen(7 downto 0),
      M00_AXI_awlock(0) => M00_AXI_awlock(0),
      M00_AXI_awprot(2 downto 0) => M00_AXI_awprot(2 downto 0),
      M00_AXI_awqos(3 downto 0) => M00_AXI_awqos(3 downto 0),
      M00_AXI_awready => M00_AXI_awready,
      M00_AXI_awregion(3 downto 0) => M00_AXI_awregion(3 downto 0),
      M00_AXI_awsize(2 downto 0) => M00_AXI_awsize(2 downto 0),
      M00_AXI_awvalid => M00_AXI_awvalid,
      M00_AXI_bready => M00_AXI_bready,
      M00_AXI_bresp(1 downto 0) => M00_AXI_bresp(1 downto 0),
      M00_AXI_bvalid => M00_AXI_bvalid,
      M00_AXI_rdata => M00_AXI_rdata,
      M00_AXI_rlast => M00_AXI_rlast,
      M00_AXI_rready => M00_AXI_rready,
      M00_AXI_rresp(1 downto 0) => M00_AXI_rresp(1 downto 0),
      M00_AXI_rvalid => M00_AXI_rvalid,
      M00_AXI_wdata => M00_AXI_wdata,
      M00_AXI_wlast => M00_AXI_wlast,
      M00_AXI_wready => M00_AXI_wready,
      M00_AXI_wstrb => M00_AXI_wstrb,
      M00_AXI_wvalid => M00_AXI_wvalid,
      ap_clk => ap_clk,
      ap_ctrl_done => ap_ctrl_done_r,
      ap_ctrl_idle => ap_ctrl_idle_r,
      ap_ctrl_ready => ap_ctrl_ready_r,
      ap_ctrl_start => ap_ctrl_start_r,
      ap_return(31 downto 0) => ap_return_r(31 downto 0),
      ap_rst_n => ap_rst_n_r,
      bus_ptr_i(31 downto 0) => AXI_TARGET_ADDRESS,
      cp_size(31 downto 0) => cp_size_r(31 downto 0),
      offset_i(31 downto 0) => offset_i_r(31 downto 0),
      offset_o(31 downto 0) => offset_o_r(31 downto 0),
      p1(31 downto 0) => p1_r(31 downto 0),
      p2(31 downto 0) => p2_r(31 downto 0),
	  
	  stream1				=> stream1,	
      stream1_ap_ack        => stream1_ap_ack,
      stream2               => stream2,
      stream2_ap_ack        => stream2_ap_ack,
      stream3               => stream3,
      stream3_ap_ack        => stream3_ap_ack,
      stream4               => stream4,
      stream4_ap_ack        => stream4_ap_ack,
      stream5               => stream5,
      stream5_ap_ack        => stream5_ap_ack
      

		
    );
	
	AXI_TARGET_ADDRESS	<= X"20000000";
	
	
reg_slice1: axis_reg_slice_64
port map (
		s_aclk 			=> ap_clk,
		s_aresetn 		=> ap_rst_n_r,
		              
		s_axis_tvalid 	=> rand1_tvalid,
		s_axis_tready 	=> rand1_tready,
		s_axis_tdata 	=> rand1_tdata,
		               
		m_axis_tvalid 	=> open,
		m_axis_tready 	=> stream1_ap_ack,
		m_axis_tdata 	=> stream1
);
	
reg_slice2: axis_reg_slice_64
port map (
		s_aclk 			=> ap_clk,
		s_aresetn 		=> ap_rst_n_r,
		              
		s_axis_tvalid 	=> rand2_tvalid,
		s_axis_tready 	=> rand2_tready,
		s_axis_tdata 	=> rand2_tdata,
		               
		m_axis_tvalid 	=> open,
		m_axis_tready 	=> stream2_ap_ack,
		m_axis_tdata 	=> stream2
);
	
reg_slice3: axis_reg_slice_64
port map (
		s_aclk 			=> ap_clk,
		s_aresetn 		=> ap_rst_n_r,
		              
		s_axis_tvalid 	=> rand3_tvalid,
		s_axis_tready 	=> rand3_tready,
		s_axis_tdata 	=> rand3_tdata,
		               
		m_axis_tvalid 	=> open,
		m_axis_tready 	=> stream3_ap_ack,
		m_axis_tdata 	=> stream3
);
	
reg_slice4: axis_reg_slice_64
port map (
		s_aclk 			=> ap_clk,
		s_aresetn 		=> ap_rst_n_r,
		              
		s_axis_tvalid 	=> rand4_tvalid,
		s_axis_tready 	=> rand4_tready,
		s_axis_tdata 	=> rand4_tdata,
		               
		m_axis_tvalid 	=> open,
		m_axis_tready 	=> stream4_ap_ack,
		m_axis_tdata 	=> stream4
);
	
reg_slice5: axis_reg_slice_64
port map (
		s_aclk 			=> ap_clk,
		s_aresetn 		=> ap_rst_n_r,
		              
		s_axis_tvalid 	=> rand5_tvalid,
		s_axis_tready 	=> rand5_tready,
		s_axis_tdata 	=> rand5_tdata,
		               
		m_axis_tvalid 	=> open,
		m_axis_tready 	=> stream5_ap_ack,
		m_axis_tdata 	=> stream5
);
	

	
	
end STRUCTURE;
