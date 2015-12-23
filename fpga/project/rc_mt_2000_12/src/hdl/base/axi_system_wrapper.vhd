--Copyright 1986-2014 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2014.2 (win64) Build 932637 Wed Jun 11 13:33:10 MDT 2014
--Date        : Thu Sep 04 12:26:22 2014
--Host        : VM-XILINX running 64-bit Service Pack 1  (build 7601)
--Command     : generate_target axi_system_wrapper.bd
--Design      : axi_system_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity axi_system_wrapper is
  port (
  
  sys_rst_n 		: IN  std_logic;
--  sys_rst_out_n    : OUT std_logic;

      
  clk_in             : IN  std_logic;
  --clk_out         : OUT std_logic;
  
    -- S02_AXI_awid : in STD_LOGIC_VECTOR ( 0 downto 0 );
    -- S02_AXI_awuser : in STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_awaddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    S02_AXI_awlen : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S02_AXI_awsize : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_awburst : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_awlock : in STD_LOGIC_VECTOR ( 0 to 0 );
    S02_AXI_awcache : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_awregion : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_awqos : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_awvalid : in STD_LOGIC;
    S02_AXI_awready : out STD_LOGIC;
	
    -- S02_AXI_wid : in STD_LOGIC_VECTOR ( 0 downto 0 );
    -- S02_AXI_wuser : in STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_wdata : in STD_LOGIC_VECTOR ( 63 downto 0 );
    S02_AXI_wstrb : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S02_AXI_wlast : in STD_LOGIC;
    S02_AXI_wvalid : in STD_LOGIC;
    S02_AXI_wready : out STD_LOGIC;

    -- S02_AXI_buser : in STD_LOGIC_VECTOR ( 0 downto 0 );
    -- S02_AXI_bid : out STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_bvalid : out STD_LOGIC;
    S02_AXI_bready : in STD_LOGIC;
    
    -- S02_AXI_aruser : in STD_LOGIC_VECTOR ( 0 downto 0 );
	-- S02_AXI_arid : in STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_araddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    S02_AXI_arlen : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S02_AXI_arsize : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_arburst : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_arlock : in STD_LOGIC_VECTOR ( 0 to 0 );
    S02_AXI_arcache : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_arregion : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_arqos : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_arvalid : in STD_LOGIC;
    S02_AXI_arready : out STD_LOGIC;
    
    -- S02_AXI_ruser : in STD_LOGIC_VECTOR ( 0 downto 0 );	
	-- S02_AXI_rid : out STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_rdata : out STD_LOGIC_VECTOR ( 63 downto 0 );
    S02_AXI_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_rlast : out STD_LOGIC;
    S02_AXI_rvalid : out STD_LOGIC;
    S02_AXI_rready : in STD_LOGIC;

  ip_clk           : IN std_logic;
  

		
-- User interface

	-- MM2S CMD interface
	StartPulse_mm2s			: in  std_logic;
	StartAddrB_mm2s 		: in  std_logic_vector(31 downto 0);
	BytesToTransfer_mm2s 	: in  std_logic_vector(22 downto 0);
	CmdTag_mm2s				: in  std_logic_vector(3  downto 0);
	CmdAck_mm2s				: out std_logic;
		
		
	-- S2MM CMD interface
	StartPulse_s2mm			: in  std_logic;
	StartAddrB_s2mm 		: in  std_logic_vector(31 downto 0);
	BytesToTransfer_s2mm 	: in  std_logic_vector(22 downto 0);
	CmdTag_s2mm				: in  std_logic_vector(3  downto 0);
	CmdAck_s2mm				: out std_logic;
		
	-- AXI Stream Master interface (Read Status from AXI)
	-- To User IP
	m_axis_s2mm_sts_tvalid 	: out std_logic;
	m_axis_s2mm_sts_tready 	: in std_logic;
	m_axis_s2mm_sts_tdata 	: out std_logic_vector(7 downto 0);

	m_axis_mm2s_sts_tvalid 	: out std_logic;
	m_axis_mm2s_sts_tready 	: in std_logic;
	m_axis_mm2s_sts_tdata 	: out std_logic_vector(7 downto 0);

	-- AXI Stream Master Fifo interface (Write to fifo)
	m_axis_txd_tvalid : out std_logic;
	m_axis_txd_tready : in  std_logic;
	m_axis_txd_tdata  : out std_logic_vector(63 downto 0);
		
	-- AXI Stream Slave Fifo interface (Read from fifo)
	s_axis_rxd_tvalid : in  std_logic;
	s_axis_rxd_tready : out std_logic;
	s_axis_rxd_tdata  : in  std_logic_vector(63 downto 0);
		

	dm_mm2s_err : out std_logic;
	dm_s2mm_err : out std_logic;  
	
	
-- DDR PHY Interface	
  
    Top_ddr_dq 		: inout std_logic_vector(31 downto 0);
	Top_ddr_dqs_p 	: inout std_logic_vector(3 downto 0);
	Top_ddr_dqs_n 	: inout std_logic_vector(3 downto 0);
	Top_ddr_addr 	: out std_logic_vector(13 downto 0);
	Top_ddr_ba 		: out std_logic_vector(2 downto 0);
    Top_ddr_ras_n 	: out std_logic;
    Top_ddr_cas_n 	: out std_logic;                        
	Top_ddr_we_n 	: out std_logic;
    Top_ddr_reset_n : out std_logic;
	Top_ddr_ck_p 	: out std_logic;
	Top_ddr_ck_n 	: out std_logic;
	Top_ddr_cke 	: out std_logic;
	Top_ddr_cs_n 	: out std_logic;
	Top_ddr_dm 		: out std_logic_vector(3 downto 0);
	Top_ddr_odt 	: out std_logic;



    InitDoneTop 	: out std_logic;
	InitDoneBtm 	: out std_logic;
	clk_axi         : in  std_logic
    --clk_axi_ref     : in  std_logic
    
	);
end axi_system_wrapper;
	
		

		

architecture STRUCTURE of axi_system_wrapper is


  component axi_system is
  port (
    --clk_in : in STD_LOGIC;
    --clk_out : out STD_LOGIC;
    clk_axi         : in  std_logic;
    --clk_axi_ref     : in  std_logic;
    mig_rst_n   : in std_logic; 
    ip_clk : in std_logic;
    
    daddr_in : in STD_LOGIC_VECTOR ( 6 downto 0 );
--    InitDoneBtm : out STD_LOGIC;
    InitDoneTop : out STD_LOGIC;
    den_in : in STD_LOGIC;
    di_in : in STD_LOGIC_VECTOR ( 15 downto 0 );
    dwe_in : in STD_LOGIC;
--    sys_rst_out_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    sys_rst_n : in STD_LOGIC;
    drdy_out : out STD_LOGIC;
    Top_ddr_dq : inout STD_LOGIC_VECTOR ( 31 downto 0 );
    Top_ddr_dqs_p : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    Top_ddr_dqs_n : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    Top_ddr_addr : out STD_LOGIC_VECTOR ( 13 downto 0 );
    Top_ddr_ba : out STD_LOGIC_VECTOR ( 2 downto 0 );
    Top_ddr_ras_n : out STD_LOGIC;
    Top_ddr_cas_n : out STD_LOGIC;
    Top_ddr_we_n : out STD_LOGIC;
    Top_ddr_reset_n : out STD_LOGIC;
    Top_ddr_ck_p : out STD_LOGIC_VECTOR ( 0 to 0 );
    Top_ddr_ck_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    Top_ddr_cke : out STD_LOGIC_VECTOR ( 0 to 0 );
    Top_ddr_cs_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    Top_ddr_dm : out STD_LOGIC_VECTOR ( 3 downto 0 );
    Top_ddr_odt : out STD_LOGIC_VECTOR ( 0 to 0 );

	
    -- S02_AXI_awid : in STD_LOGIC_VECTOR ( 0 downto 0 );
    -- S02_AXI_awuser : in STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_awaddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    S02_AXI_awlen : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S02_AXI_awsize : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_awburst : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_awlock : in STD_LOGIC_VECTOR ( 0 to 0 );
    S02_AXI_awcache : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_awregion : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_awqos : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_awvalid : in STD_LOGIC;
    S02_AXI_awready : out STD_LOGIC;
	
--    S02_AXI_wid : in STD_LOGIC_VECTOR ( 0 downto 0 );
    -- S02_AXI_wuser : in STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_wdata : in STD_LOGIC_VECTOR ( 63 downto 0 );
    S02_AXI_wstrb : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S02_AXI_wlast : in STD_LOGIC;
    S02_AXI_wvalid : in STD_LOGIC;
    S02_AXI_wready : out STD_LOGIC;

    -- S02_AXI_buser : in STD_LOGIC_VECTOR ( 0 downto 0 );
    -- S02_AXI_bid : out STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_bvalid : out STD_LOGIC;
    S02_AXI_bready : in STD_LOGIC;
    
    -- S02_AXI_aruser : in STD_LOGIC_VECTOR ( 0 downto 0 );
	-- S02_AXI_arid : in STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_araddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    S02_AXI_arlen : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S02_AXI_arsize : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_arburst : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_arlock : in STD_LOGIC_VECTOR ( 0 to 0 );
    S02_AXI_arcache : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S02_AXI_arregion : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_arqos : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S02_AXI_arvalid : in STD_LOGIC;
    S02_AXI_arready : out STD_LOGIC;
    
    -- S02_AXI_ruser : in STD_LOGIC_VECTOR ( 0 downto 0 );	
	-- S02_AXI_rid : out STD_LOGIC_VECTOR ( 0 downto 0 );
    S02_AXI_rdata : out STD_LOGIC_VECTOR ( 63 downto 0 );
    S02_AXI_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S02_AXI_rlast : out STD_LOGIC;
    S02_AXI_rvalid : out STD_LOGIC;
    S02_AXI_rready : in STD_LOGIC;
	
	
	
	
    S01_AXI_awid : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S01_AXI_awaddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    S01_AXI_awlen : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S01_AXI_awsize : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S01_AXI_awburst : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S01_AXI_awlock : in STD_LOGIC_VECTOR ( 0 to 0 );
    S01_AXI_awcache : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S01_AXI_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S01_AXI_awregion : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S01_AXI_awqos : in STD_LOGIC_VECTOR ( 3 downto 0 );
    --S01_AXI_awuser : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S01_AXI_awvalid : in STD_LOGIC;
    S01_AXI_awready : out STD_LOGIC;
    S01_AXI_wdata : in STD_LOGIC_VECTOR ( 63 downto 0 );
    S01_AXI_wstrb : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S01_AXI_wlast : in STD_LOGIC;
    S01_AXI_wvalid : in STD_LOGIC;
    S01_AXI_wready : out STD_LOGIC;
    S01_AXI_bid : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S01_AXI_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S01_AXI_bvalid : out STD_LOGIC;
    S01_AXI_bready : in STD_LOGIC;
    S00_AXI_arid : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S00_AXI_araddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    S00_AXI_arlen : in STD_LOGIC_VECTOR ( 7 downto 0 );
    S00_AXI_arsize : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S00_AXI_arburst : in STD_LOGIC_VECTOR ( 1 downto 0 );
    S00_AXI_arlock : in STD_LOGIC_VECTOR ( 0 to 0 );
    S00_AXI_arcache : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S00_AXI_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    S00_AXI_arregion : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S00_AXI_arqos : in STD_LOGIC_VECTOR ( 3 downto 0 );
    --S00_AXI_aruser : in STD_LOGIC_VECTOR ( 3 downto 0 );
    S00_AXI_arvalid : in STD_LOGIC;
    S00_AXI_arready : out STD_LOGIC;
    S00_AXI_rid : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S00_AXI_rdata : out STD_LOGIC_VECTOR ( 63 downto 0 );
    S00_AXI_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    S00_AXI_rlast : out STD_LOGIC;
    S00_AXI_rvalid : out STD_LOGIC;
    S00_AXI_rready : in STD_LOGIC
  );
  end component axi_system;
  
	
signal clk_out_s		: std_logic;
--signal sys_rst_out_s_n	: std_logic;
  
signal S01_AXI_awid 	: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S01_AXI_awaddr 	: STD_LOGIC_VECTOR ( 31 downto 0 );
signal S01_AXI_awlen 	: STD_LOGIC_VECTOR ( 7 downto 0 );
signal S01_AXI_awsize 	: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S01_AXI_awburst 	: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S01_AXI_awlock 	: STD_LOGIC_VECTOR ( 0 to 0 );
signal S01_AXI_awcache 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S01_AXI_awprot 	: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S01_AXI_awregion : STD_LOGIC_VECTOR ( 3 downto 0 );
signal S01_AXI_awqos 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S01_AXI_awuser 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S01_AXI_awvalid 	: STD_LOGIC;
signal S01_AXI_awready 	: STD_LOGIC;
signal S01_AXI_wdata 	: STD_LOGIC_VECTOR ( 63 downto 0 );
signal S01_AXI_wstrb 	: STD_LOGIC_VECTOR ( 7 downto 0 );
signal S01_AXI_wlast 	: STD_LOGIC;
signal S01_AXI_wvalid 	: STD_LOGIC;
signal S01_AXI_wready 	: STD_LOGIC;
signal S01_AXI_bid 		: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S01_AXI_bresp 	: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S01_AXI_bvalid 	: STD_LOGIC;
signal S01_AXI_bready 	: STD_LOGIC;
signal S00_AXI_arid 	: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S00_AXI_araddr 	: STD_LOGIC_VECTOR ( 31 downto 0 );
signal S00_AXI_arlen 	: STD_LOGIC_VECTOR ( 7 downto 0 );
signal S00_AXI_arsize 	: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S00_AXI_arburst 	: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S00_AXI_arlock 	: STD_LOGIC_VECTOR ( 0 to 0 );
signal S00_AXI_arcache 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S00_AXI_arprot 	: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S00_AXI_arregion : STD_LOGIC_VECTOR ( 3 downto 0 );
signal S00_AXI_arqos 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S00_AXI_aruser 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S00_AXI_arvalid 	: STD_LOGIC;
signal S00_AXI_arready 	: STD_LOGIC;
signal S00_AXI_rid 		: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S00_AXI_rdata 	: STD_LOGIC_VECTOR ( 63 downto 0 );
signal S00_AXI_rresp 	: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S00_AXI_rlast 	: STD_LOGIC;
signal S00_AXI_rvalid 	: STD_LOGIC;
signal S00_AXI_rready 	: STD_LOGIC;


-- signal S02_AXI_awid 	: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_awaddr 	: STD_LOGIC_VECTOR ( 31 downto 0 );
-- signal S02_AXI_awlen 	: STD_LOGIC_VECTOR ( 7 downto 0 );
-- signal S02_AXI_awsize 	: STD_LOGIC_VECTOR ( 2 downto 0 );
-- signal S02_AXI_awburst 	: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_awlock 	: STD_LOGIC_VECTOR ( 1 to 0 );
-- signal S02_AXI_awcache 	: STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_awprot 	: STD_LOGIC_VECTOR ( 2 downto 0 );
-- signal S02_AXI_awregion : STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_awqos 	: STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_awuser 	: STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_awvalid 	: STD_LOGIC;
-- signal S02_AXI_awready 	: STD_LOGIC;
-- signal S02_AXI_wdata 	: STD_LOGIC_VECTOR ( 63 downto 0 );
-- signal S02_AXI_wstrb 	: STD_LOGIC_VECTOR ( 7 downto 0 );
-- signal S02_AXI_wlast 	: STD_LOGIC;
-- signal S02_AXI_wvalid 	: STD_LOGIC;
-- signal S02_AXI_wready 	: STD_LOGIC;
-- signal S02_AXI_bid 		: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_bresp 	: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_bvalid 	: STD_LOGIC;
-- signal S02_AXI_bready 	: STD_LOGIC;
-- signal S02_AXI_arid 	: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_araddr 	: STD_LOGIC_VECTOR ( 31 downto 0 );
-- signal S02_AXI_arlen 	: STD_LOGIC_VECTOR ( 7 downto 0 );
-- signal S02_AXI_arsize 	: STD_LOGIC_VECTOR ( 2 downto 0 );
-- signal S02_AXI_arburst 	: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_arlock 	: STD_LOGIC_VECTOR ( 1 to 0 );
-- signal S02_AXI_arcache 	: STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_arprot 	: STD_LOGIC_VECTOR ( 2 downto 0 );
-- signal S02_AXI_arregion : STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_arqos 	: STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_aruser 	: STD_LOGIC_VECTOR ( 3 downto 0 );
-- signal S02_AXI_arvalid 	: STD_LOGIC;
-- signal S02_AXI_arready 	: STD_LOGIC;
-- signal S02_AXI_rid 		: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_rdata 	: STD_LOGIC_VECTOR ( 63 downto 0 );
-- signal S02_AXI_rresp 	: STD_LOGIC_VECTOR ( 1 downto 0 );
-- signal S02_AXI_rlast 	: STD_LOGIC;
-- signal S02_AXI_rvalid 	: STD_LOGIC;
-- signal S02_AXI_rready 	: STD_LOGIC;


-- component ap_bus_axim_64_2
    -- PORT (
    -- bus_ptr_i_req_din    : IN STD_LOGIC;
    -- bus_ptr_i_req_full_n     : OUT STD_LOGIC;
    -- bus_ptr_i_req_write : IN STD_LOGIC;
    -- bus_ptr_i_rsp_empty_n  : OUT STD_LOGIC;
    -- bus_ptr_i_rsp_read  : IN STD_LOGIC;
    -- bus_ptr_i_address  : IN STD_LOGIC_VECTOR(31 downto 0);
    -- bus_ptr_i_datain  : OUT STD_LOGIC_VECTOR(63 downto 0);
    -- bus_ptr_i_dataout  : IN STD_LOGIC_VECTOR(63 downto 0);
    -- bus_ptr_i_size  : IN STD_LOGIC_VECTOR(31 downto 0);

     -- m_axi_bus_ptr_i_AWID       : OUT STD_LOGIC_VECTOR(0 downto 0);
     -- m_axi_bus_ptr_i_AWADDR     : OUT STD_LOGIC_VECTOR(31 downto 0);
     -- m_axi_bus_ptr_i_AWLEN      : OUT STD_LOGIC_VECTOR(7 downto 0);
     -- m_axi_bus_ptr_i_AWSIZE     : OUT STD_LOGIC_VECTOR(2 downto 0);
     -- m_axi_bus_ptr_i_AWBURST    : OUT STD_LOGIC_VECTOR(1 downto 0);
     -- m_axi_bus_ptr_i_AWLOCK     : OUT STD_LOGIC_VECTOR(1 downto 0);
     -- m_axi_bus_ptr_i_AWCACHE    : OUT STD_LOGIC_VECTOR(3 downto 0);
     -- m_axi_bus_ptr_i_AWPROT     : OUT STD_LOGIC_VECTOR(2 downto 0);
     -- m_axi_bus_ptr_i_AWQOS      : OUT STD_LOGIC_VECTOR(3 downto 0);
     -- m_axi_bus_ptr_i_AWVALID    : OUT STD_LOGIC;
     -- m_axi_bus_ptr_i_AWREADY    : IN STD_LOGIC;
     -- m_axi_bus_ptr_i_WDATA      : OUT STD_LOGIC_VECTOR(63 downto 0);
     -- m_axi_bus_ptr_i_WSTRB      : OUT STD_LOGIC_VECTOR(7 downto 0);
     -- m_axi_bus_ptr_i_WLAST      : OUT STD_LOGIC;
     -- m_axi_bus_ptr_i_WVALID     : OUT STD_LOGIC;
     -- m_axi_bus_ptr_i_WREADY     : IN STD_LOGIC;
     -- m_axi_bus_ptr_i_BID        : IN STD_LOGIC_VECTOR(0 downto 0);
     -- m_axi_bus_ptr_i_BRESP      : IN STD_LOGIC_VECTOR(1 downto 0);
     -- m_axi_bus_ptr_i_BVALID     : IN STD_LOGIC;
     -- m_axi_bus_ptr_i_BREADY     : OUT STD_LOGIC;
     -- m_axi_bus_ptr_i_ARID       : OUT STD_LOGIC_VECTOR(0 downto 0);
     -- m_axi_bus_ptr_i_ARADDR     : OUT STD_LOGIC_VECTOR(31 downto 0);
     -- m_axi_bus_ptr_i_ARLEN      : OUT STD_LOGIC_VECTOR(7 downto 0);
     -- m_axi_bus_ptr_i_ARSIZE     : OUT STD_LOGIC_VECTOR(2 downto 0);
     -- m_axi_bus_ptr_i_ARBURST    : OUT STD_LOGIC_VECTOR(1 downto 0);
     -- m_axi_bus_ptr_i_ARLOCK     : OUT STD_LOGIC_VECTOR(1 downto 0);
     -- m_axi_bus_ptr_i_ARCACHE    : OUT STD_LOGIC_VECTOR(3 downto 0);
     -- m_axi_bus_ptr_i_ARPROT     : OUT STD_LOGIC_VECTOR(2 downto 0);
     -- m_axi_bus_ptr_i_ARQOS      : OUT STD_LOGIC_VECTOR(3 downto 0);
     -- m_axi_bus_ptr_i_ARVALID    : OUT STD_LOGIC;
     -- m_axi_bus_ptr_i_ARREADY    : IN STD_LOGIC;
     -- m_axi_bus_ptr_i_RID        : IN STD_LOGIC_VECTOR(0 downto 0);
     -- m_axi_bus_ptr_i_RDATA      : IN STD_LOGIC_VECTOR(63 downto 0);
     -- m_axi_bus_ptr_i_RRESP      : IN STD_LOGIC_VECTOR(1 downto 0);
     -- m_axi_bus_ptr_i_RLAST      : IN STD_LOGIC;
     -- m_axi_bus_ptr_i_RVALID     : IN STD_LOGIC;
     -- m_axi_bus_ptr_i_RREADY     : OUT STD_LOGIC;

     -- aclk  : IN STD_LOGIC;
     -- aresetn  : IN STD_LOGIC 
    
    -- );
 -- END COMPONENT;   

COMPONENT axi_datamover_0
  PORT (
    m_axi_mm2s_aclk : IN STD_LOGIC;
    m_axi_mm2s_aresetn : IN STD_LOGIC;
    mm2s_err : OUT STD_LOGIC;
	
    m_axis_mm2s_cmdsts_aclk : IN STD_LOGIC;
    m_axis_mm2s_cmdsts_aresetn : IN STD_LOGIC;
    
	s_axis_mm2s_cmd_tvalid : IN STD_LOGIC;
    s_axis_mm2s_cmd_tready : OUT STD_LOGIC;
    s_axis_mm2s_cmd_tdata : IN STD_LOGIC_VECTOR(71 DOWNTO 0);
    
	m_axis_mm2s_sts_tvalid : OUT STD_LOGIC;
    m_axis_mm2s_sts_tready : IN STD_LOGIC;
    m_axis_mm2s_sts_tdata : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axis_mm2s_sts_tkeep : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
    m_axis_mm2s_sts_tlast : OUT STD_LOGIC;
	
    m_axi_mm2s_arid : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_mm2s_araddr : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axi_mm2s_arlen : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axi_mm2s_arsize : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
    m_axi_mm2s_arburst : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_mm2s_arprot : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
    m_axi_mm2s_arcache : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    m_axi_mm2s_aruser : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    m_axi_mm2s_arvalid : OUT STD_LOGIC;
    m_axi_mm2s_arready : IN STD_LOGIC;
    m_axi_mm2s_rdata : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
    m_axi_mm2s_rresp : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_mm2s_rlast : IN STD_LOGIC;
    m_axi_mm2s_rvalid : IN STD_LOGIC;
    m_axi_mm2s_rready : OUT STD_LOGIC;
	
	
    m_axis_mm2s_tdata : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
    m_axis_mm2s_tkeep : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axis_mm2s_tlast : OUT STD_LOGIC;
    m_axis_mm2s_tvalid : OUT STD_LOGIC;
    m_axis_mm2s_tready : IN STD_LOGIC;
	
    m_axi_s2mm_aclk : IN STD_LOGIC;
    m_axi_s2mm_aresetn : IN STD_LOGIC;
    s2mm_err : OUT STD_LOGIC;
	
    m_axis_s2mm_cmdsts_awclk : IN STD_LOGIC;
    m_axis_s2mm_cmdsts_aresetn : IN STD_LOGIC;
	
    s_axis_s2mm_cmd_tvalid : IN STD_LOGIC;
    s_axis_s2mm_cmd_tready : OUT STD_LOGIC;
    s_axis_s2mm_cmd_tdata : IN STD_LOGIC_VECTOR(71 DOWNTO 0);
	
    m_axis_s2mm_sts_tvalid : OUT STD_LOGIC;
    m_axis_s2mm_sts_tready : IN STD_LOGIC;
    m_axis_s2mm_sts_tdata : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axis_s2mm_sts_tkeep : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
    m_axis_s2mm_sts_tlast : OUT STD_LOGIC;
	
    m_axi_s2mm_awid : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_s2mm_awaddr : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axi_s2mm_awlen : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axi_s2mm_awsize : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
    m_axi_s2mm_awburst : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_s2mm_awprot : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
    m_axi_s2mm_awcache : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    m_axi_s2mm_awuser : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    m_axi_s2mm_awvalid : OUT STD_LOGIC;
    m_axi_s2mm_awready : IN STD_LOGIC;
    m_axi_s2mm_wdata : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
    m_axi_s2mm_wstrb : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axi_s2mm_wlast : OUT STD_LOGIC;
    m_axi_s2mm_wvalid : OUT STD_LOGIC;
    m_axi_s2mm_wready : IN STD_LOGIC;
    m_axi_s2mm_bresp : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_s2mm_bvalid : IN STD_LOGIC;
    m_axi_s2mm_bready : OUT STD_LOGIC;
	
    s_axis_s2mm_tdata : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
    s_axis_s2mm_tkeep : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    s_axis_s2mm_tlast : IN STD_LOGIC;
    s_axis_s2mm_tvalid : IN STD_LOGIC;
    s_axis_s2mm_tready : OUT STD_LOGIC
  );
END COMPONENT;


signal m_axis_mm2s_tdata : STD_LOGIC_VECTOR(63 DOWNTO 0);
signal m_axis_mm2s_tkeep : STD_LOGIC_VECTOR(7 DOWNTO 0);
signal m_axis_mm2s_tlast : STD_LOGIC;
signal m_axis_mm2s_tvalid : STD_LOGIC;
signal m_axis_mm2s_tready : STD_LOGIC;

signal s_axis_s2mm_tdata : STD_LOGIC_VECTOR(63 DOWNTO 0);
signal s_axis_s2mm_tkeep : STD_LOGIC_VECTOR(7 DOWNTO 0);
signal s_axis_s2mm_tlast : STD_LOGIC;
signal s_axis_s2mm_tvalid : STD_LOGIC;
signal s_axis_s2mm_tready : STD_LOGIC;
	
	

signal s_axis_s2mm_cmd_tvalid : STD_LOGIC;
signal s_axis_s2mm_cmd_tready : STD_LOGIC;
signal s_axis_s2mm_cmd_tdata : STD_LOGIC_VECTOR(71 DOWNTO 0);
	
signal s_axis_mm2s_cmd_tvalid : STD_LOGIC;
signal s_axis_mm2s_cmd_tready : STD_LOGIC;
signal s_axis_mm2s_cmd_tdata : STD_LOGIC_VECTOR(71 DOWNTO 0);




component stream_axi_bridge is
  generic(
		C_AXIS_DWIDTH_mm2s    : integer range 32 to 64    := 64;
		C_AXIS_DWIDTH_s2mm    : integer range 32 to 64    := 64
			); 
	port (
		-- clk and reset
		aclk 			: in std_logic;
		clk_axi         : in std_logic;
		aReset_n 	: in std_logic;

-- User interface

		-- MM2S CMD interface
		StartPulse_mm2s			: in  std_logic;
		StartAddrB_mm2s 		: in  std_logic_vector(31 downto 0);
		BytesToTransfer_mm2s 	: in  std_logic_vector(22 downto 0);
		CmdTag_mm2s				: in  std_logic_vector(3  downto 0);
		CmdAck_mm2s				: out std_logic;
		
		
		-- S2MM CMD interface
		StartPulse_s2mm			: in  std_logic;
		StartAddrB_s2mm 		: in  std_logic_vector(31 downto 0);
		BytesToTransfer_s2mm 	: in  std_logic_vector(22 downto 0);
		CmdTag_s2mm				: in  std_logic_vector(3  downto 0);
		CmdAck_s2mm				: out std_logic;
		
		-- AXI Stream Master interface (Read Status from AXI)
		-- To User IP
		m_axis_s2mm_sts_tvalid 	: out std_logic;
		m_axis_s2mm_sts_tready 	: in std_logic;
		m_axis_s2mm_sts_tdata 	: out std_logic_vector(7 downto 0);
		m_axis_s2mm_sts_tkeep 	: out std_logic_vector(0 to 0);
		m_axis_s2mm_sts_tlast 	: out std_logic;
		
		m_axis_mm2s_sts_tvalid 	: out std_logic;
		m_axis_mm2s_sts_tready 	: in std_logic;
		m_axis_mm2s_sts_tdata 	: out std_logic_vector(7 downto 0);
		m_axis_mm2s_sts_tkeep 	: out std_logic_vector(0 to 0);
		m_axis_mm2s_sts_tlast 	: out std_logic;		

		-- AXI Stream Master Fifo interface (Write to fifo)
		m_axis_txd_tvalid : out std_logic;
		m_axis_txd_tready : in  std_logic;
		m_axis_txd_tdata  : out std_logic_vector(C_AXIS_DWIDTH_mm2s-1 downto 0);
		
		-- AXI Stream Slave Fifo interface (Read from fifo)
		s_axis_rxd_tvalid : in  std_logic;
		s_axis_rxd_tready : out std_logic;
		s_axis_rxd_tdata  : in  std_logic_vector(C_AXIS_DWIDTH_s2mm-1 downto 0);
		
		
	
-- DataMover Interface

		-- AXI Stream Master interface (Write CMD to AXI Datamover)
		m_axis_cmd_s2mm_tvalid 	: out std_logic;
		m_axis_cmd_s2mm_tready 	: in  std_logic;
		m_axis_cmd_s2mm_tdata 	: out std_logic_vector(71 downto 0);
		
		m_axis_cmd_mm2s_tvalid 	: out std_logic;
		m_axis_cmd_mm2s_tready 	: in  std_logic;
		m_axis_cmd_mm2s_tdata 	: out std_logic_vector(71 downto 0);
		
		
		-- AXI Stream Master S2MM interface (Write DATA to AXI)
		m_axis_s2mm_tvalid 		: out std_logic;
		m_axis_s2mm_tready 		: in  std_logic;
		m_axis_s2mm_tdata 		: out std_logic_vector(C_AXIS_DWIDTH_s2mm-1 downto 0);
		m_axis_s2mm_tkeep 		: out std_logic_vector((C_AXIS_DWIDTH_s2mm/8)-1 downto 0);
		m_axis_s2mm_tlast 		: out std_logic;

		-- AXI Stream Slave S2MM interface (Read DATA from AXI)
		s_axis_mm2s_tvalid 		: in  std_logic;
		s_axis_mm2s_tready 		: out std_logic;
		s_axis_mm2s_tdata 		: in  std_logic_vector(C_AXIS_DWIDTH_mm2s-1 downto 0);
		s_axis_mm2s_tkeep 		: in  std_logic_vector((C_AXIS_DWIDTH_mm2s/8)-1 downto 0);
		s_axis_mm2s_tlast 		: in  std_logic;

		-- AXI Stream Master interface (Read Status from AXI)
		-- From Datamover
		m_axis_s2mm_sts_tvalid_i 	: in std_logic;
		m_axis_s2mm_sts_tready_i 	: out std_logic;
		m_axis_s2mm_sts_tdata_i 	: in std_logic_vector(7 downto 0);
		m_axis_s2mm_sts_tkeep_i 	: in std_logic_vector(0 to 0);
		m_axis_s2mm_sts_tlast_i 	: in std_logic;
		
		m_axis_mm2s_sts_tvalid_i 	: in std_logic;
		m_axis_mm2s_sts_tready_i 	: out std_logic;
		m_axis_mm2s_sts_tdata_i 	: in std_logic_vector(7 downto 0);
		m_axis_mm2s_sts_tkeep_i 	: in std_logic_vector(0 to 0);
		m_axis_mm2s_sts_tlast_i 	: in std_logic
	
	);
	
end component;

signal m_axis_s2mm_sts_tvalid_i 	: std_logic;
signal m_axis_s2mm_sts_tready_i 	: std_logic;
signal m_axis_s2mm_sts_tdata_i 	: std_logic_vector(7 downto 0);
signal m_axis_s2mm_sts_tkeep_i 	: std_logic_vector(0 to 0);
signal m_axis_s2mm_sts_tlast_i 	: std_logic;
		
signal m_axis_mm2s_sts_tvalid_i 	: std_logic;
signal m_axis_mm2s_sts_tready_i 	: std_logic;
signal m_axis_mm2s_sts_tdata_i 	: std_logic_vector(7 downto 0);
signal m_axis_mm2s_sts_tkeep_i 	: std_logic_vector(0 to 0);
signal m_axis_mm2s_sts_tlast_i 	: std_logic;



constant di_in				: std_logic_vector(15 downto 0) := X"0000";  
constant daddr_in			: std_logic_vector(6 downto 0) := "0000000";  
constant dwe_in				: std_logic := '0'; 

signal den_in			: std_logic;
signal den_in_counter 	: std_logic_vector(6 downto 0); 
signal drdy_out			: std_logic;

signal diff_clk       : std_logic;
signal aclk       : std_logic;
  
signal aclk_p       : std_logic;
signal aclk_n       : std_logic;

signal d_rst_n          : std_logic := '0';
signal d_rst_cnt        : std_logic_vector(15 downto 0) := X"0000";

  
begin

--clk_in_p_ibuf : IBUF
--	port map (
--		O => aclk_p,
--		I => clk_in_p);
      
--clk_in_n_ibuf : IBUF
--	port map (
--		O => aclk_n,
--		I => clk_in_n);
		
--diffclk_ibuf : IBUFDS_GTE2
--     port map(
--       O       => diff_clk,
--       ODIV2   => open,
--       I       => aclk_p,
--       IB      => aclk_n,
--       CEB     => '0');
       
--diff_bufg: BUFG
--       port map
--        (O => diff_clk,
--         I => aclk);
         

--clk_out 		<= clk_out_s;
--sys_rst_out_n	<= sys_rst_out_s_n; 

process(clk_axi)
begin
	if rising_edge(clk_axi) then
		if sys_rst_n = '0' then
			den_in_counter	<= (others => '0');
		else
			den_in_counter	<= den_in_counter + 1;
		end if; 
	end if;
end process;

den_in	<= '1' when (den_in_counter = "1111111") else '0';




process(clk_axi)
begin
	if rising_edge(clk_axi) then
		if d_rst_cnt < X"FFFF" then
			d_rst_cnt	<= d_rst_cnt + 1;
		end if; 
	end if;
end process;

d_rst_n <= '0' when d_rst_cnt < X"FFFF" else '1'; 


InitDoneBtm <='1';


axi_system_i: component axi_system
    port map (
	
      --clk_in 		=> clk_in,--diff_clk,
      --clk_out 		=> clk_out_s,
      clk_axi           =>clk_axi,
      --clk_axi_ref       =>clk_axi_ref,
     ip_clk => ip_clk,    

      sys_rst_n 		=> sys_rst_n,
--      sys_rst_out_n(0) 	=> sys_rst_out_s_n,	
      
      mig_rst_n         => d_rst_n, 
	  

      daddr_in(6 downto 0) 	=> daddr_in,
      den_in 				=> den_in,
      di_in(15 downto 0) 	=> di_in,
      drdy_out 				=> drdy_out,
      dwe_in 				=> dwe_in,

	
      Top_ddr_addr(13 downto 0) => Top_ddr_addr(13 downto 0),
      Top_ddr_ba(2 downto 0) => Top_ddr_ba(2 downto 0),
      Top_ddr_cas_n => Top_ddr_cas_n,
      Top_ddr_ck_n(0) => Top_ddr_ck_n,
      Top_ddr_ck_p(0) => Top_ddr_ck_p,
      Top_ddr_cke(0) => Top_ddr_cke,
      Top_ddr_cs_n(0) => Top_ddr_cs_n,
      Top_ddr_dm(3 downto 0) => Top_ddr_dm(3 downto 0),
      Top_ddr_dq(31 downto 0) => Top_ddr_dq(31 downto 0),
      Top_ddr_dqs_n(3 downto 0) => Top_ddr_dqs_n(3 downto 0),
      Top_ddr_dqs_p(3 downto 0) => Top_ddr_dqs_p(3 downto 0),
      Top_ddr_odt(0) => Top_ddr_odt,
      Top_ddr_ras_n => Top_ddr_ras_n,
      Top_ddr_reset_n => Top_ddr_reset_n,
      Top_ddr_we_n => Top_ddr_we_n,
	  
      InitDoneTop => InitDoneTop,
	  
      S00_AXI_araddr(31 downto 0) => S00_AXI_araddr(31 downto 0),
      S00_AXI_arburst(1 downto 0) => S00_AXI_arburst(1 downto 0),
      S00_AXI_arcache(3 downto 0) => S00_AXI_arcache(3 downto 0),
      S00_AXI_arid(1 downto 0) => S00_AXI_arid(1 downto 0),
      S00_AXI_arlen(7 downto 0) => S00_AXI_arlen(7 downto 0),
      S00_AXI_arlock(0) => S00_AXI_arlock(0),
      S00_AXI_arprot(2 downto 0) => S00_AXI_arprot(2 downto 0),
      S00_AXI_arqos(3 downto 0) => S00_AXI_arqos(3 downto 0),
      S00_AXI_arready => S00_AXI_arready,
      S00_AXI_arregion(3 downto 0) => S00_AXI_arregion(3 downto 0),
      S00_AXI_arsize(2 downto 0) => S00_AXI_arsize(2 downto 0),
      --S00_AXI_aruser(3 downto 0) => S00_AXI_aruser(3 downto 0),
      S00_AXI_arvalid => S00_AXI_arvalid,
      S00_AXI_rdata(63 downto 0) => S00_AXI_rdata(63 downto 0),
      S00_AXI_rid(1 downto 0) => S00_AXI_rid(1 downto 0),
      S00_AXI_rlast => S00_AXI_rlast,
      S00_AXI_rready => S00_AXI_rready,
      S00_AXI_rresp(1 downto 0) => S00_AXI_rresp(1 downto 0),
      S00_AXI_rvalid => S00_AXI_rvalid,
      S01_AXI_awaddr(31 downto 0) => S01_AXI_awaddr(31 downto 0),
      S01_AXI_awburst(1 downto 0) => S01_AXI_awburst(1 downto 0),
      S01_AXI_awcache(3 downto 0) => S01_AXI_awcache(3 downto 0),
      S01_AXI_awid(1 downto 0) => S01_AXI_awid(1 downto 0),
      S01_AXI_awlen(7 downto 0) => S01_AXI_awlen(7 downto 0),
      S01_AXI_awlock(0) => S01_AXI_awlock(0),
      S01_AXI_awprot(2 downto 0) => S01_AXI_awprot(2 downto 0),
      S01_AXI_awqos(3 downto 0) => S01_AXI_awqos(3 downto 0),
      S01_AXI_awready => S01_AXI_awready,
      S01_AXI_awregion(3 downto 0) => S01_AXI_awregion(3 downto 0),
      S01_AXI_awsize(2 downto 0) => S01_AXI_awsize(2 downto 0),
      --S01_AXI_awuser(3 downto 0) => S01_AXI_awuser(3 downto 0),
      S01_AXI_awvalid => S01_AXI_awvalid,
      S01_AXI_bid(1 downto 0) => S01_AXI_bid(1 downto 0),
      S01_AXI_bready => S01_AXI_bready,
      S01_AXI_bresp(1 downto 0) => S01_AXI_bresp(1 downto 0),
      S01_AXI_bvalid => S01_AXI_bvalid,
      S01_AXI_wdata(63 downto 0) => S01_AXI_wdata(63 downto 0),
      S01_AXI_wlast => S01_AXI_wlast,
      S01_AXI_wready => S01_AXI_wready,
      S01_AXI_wstrb(7 downto 0) => S01_AXI_wstrb(7 downto 0),
      S01_AXI_wvalid => S01_AXI_wvalid,

      -- S02_AXI_awid(0 downto 0)                          => S02_AXI_awid(0 downto 0),       
      -- S02_AXI_awuser(0 downto 0)                          => S02_AXI_awuser(0 downto 0),       
      S02_AXI_awaddr(31 downto 0)            => S02_AXI_awaddr(31 downto 0),
      S02_AXI_awlen(7 downto 0)                => S02_AXI_awlen(7 downto 0),
      S02_AXI_awsize(2 downto 0)            => S02_AXI_awsize(2 downto 0),
      S02_AXI_awburst(1 downto 0)            => S02_AXI_awburst(1 downto 0),
      S02_AXI_awlock(0)            => S02_AXI_awlock(0),
      S02_AXI_awcache(3 downto 0)            => S02_AXI_awcache(3 downto 0),
      S02_AXI_awprot(2 downto 0)            => S02_AXI_awprot(2 downto 0),
      S02_AXI_awqos(3 downto 0)                => S02_AXI_awqos(3 downto 0),
      S02_AXI_awvalid                        => S02_AXI_awvalid,
      S02_AXI_awready                        => S02_AXI_awready,
      S02_AXI_awregion(3 downto 0)          =>  S02_AXI_awregion(3 downto 0),
	  
 --     S02_AXI_wid(0 downto 0)                          => S02_AXI_wid(0 downto 0),       
      -- S02_AXI_wuser(0 downto 0)                          => S02_AXI_wuser(0 downto 0),       	  
      S02_AXI_wdata(63 downto 0)            => S02_AXI_wdata(63 downto 0),
      S02_AXI_wstrb(7 downto 0)                => S02_AXI_wstrb(7 downto 0),
      S02_AXI_wlast                            => S02_AXI_wlast,
      S02_AXI_wvalid                        => S02_AXI_wvalid,
      S02_AXI_wready                        => S02_AXI_wready,

	  
      -- S02_AXI_bid(0 downto 0)                          => S02_AXI_bid(0 downto 0),       
      -- S02_AXI_buser(0 downto 0)                          => S02_AXI_buser(0 downto 0),       
      S02_AXI_bresp(1 downto 0)                => S02_AXI_bresp(1 downto 0),
      S02_AXI_bvalid                        => S02_AXI_bvalid,
      S02_AXI_bready                        => S02_AXI_bready,

      -- S02_AXI_arid(0 downto 0)                          => S02_AXI_arid(0 downto 0),       
      -- S02_AXI_aruser(0 downto 0)                          => S02_AXI_aruser(0 downto 0),       
      S02_AXI_araddr(31 downto 0)             =>        S02_AXI_araddr(31 downto 0),
      S02_AXI_arlen(7 downto 0)             =>         S02_AXI_arlen(7 downto 0),
      S02_AXI_arsize(2 downto 0)             =>         S02_AXI_arsize(2 downto 0),
      S02_AXI_arburst(1 downto 0)            =>         S02_AXI_arburst(1 downto 0),
      S02_AXI_arlock(0)                      =>         S02_AXI_arlock(0),                          
      S02_AXI_arcache(3 downto 0)            =>         S02_AXI_arcache(3 downto 0),
      S02_AXI_arprot(2 downto 0)             =>         S02_AXI_arprot(2 downto 0),
      S02_AXI_arqos(3 downto 0)              =>         S02_AXI_arqos(3 downto 0),
      S02_AXI_arvalid                        =>         S02_AXI_arvalid,          
      S02_AXI_arready                        =>         S02_AXI_arready,
      S02_AXI_arregion(3 downto 0)           =>          S02_AXI_arregion(3 downto 0),

      -- S02_AXI_rid(0 downto 0)                         =>         S02_AXI_rid(0 downto 0),
      -- S02_AXI_ruser(0 downto 0)                         =>         S02_AXI_ruser(0 downto 0),
      S02_AXI_rdata(63 downto 0)             =>         S02_AXI_rdata(63 downto 0),
      S02_AXI_rresp(1 downto 0)              =>         S02_AXI_rresp(1 downto 0),
      S02_AXI_rlast                          =>         S02_AXI_rlast,
      S02_AXI_rvalid                         =>         S02_AXI_rvalid,
      S02_AXI_rready                         =>         S02_AXI_rready



    );


-- hls_bridge : ap_bus_axim_64_2
-- PORT MAP (
  -- m_axi_bus_ptr_i_AWID (0)             => S02_AXI_awid(0),
  -- m_axi_bus_ptr_i_AWADDR(31 downto 0)  => S02_AXI_awaddr(31 downto 0),
  -- m_axi_bus_ptr_i_AWLEN(7 downto 0)    => S02_AXI_awlen(7 downto 0),
  -- m_axi_bus_ptr_i_AWSIZE(2 downto 0)   => S02_AXI_awsize(2 downto 0),
  -- m_axi_bus_ptr_i_AWBURST(1 downto 0)  => S02_AXI_awburst(1 downto 0),
  -- m_axi_bus_ptr_i_AWLOCK(1 downto 0)   => S02_AXI_awlock(1 downto 0),
  -- m_axi_bus_ptr_i_AWCACHE(3 downto 0)  => S02_AXI_awcache(3 downto 0),
  -- m_axi_bus_ptr_i_AWPROT(2 downto 0)   => S02_AXI_awprot(2 downto 0),
  -- m_axi_bus_ptr_i_AWQOS (3 downto 0)   => S02_AXI_awqos(3 downto 0),
  -- m_axi_bus_ptr_i_AWVALID              => S02_AXI_awvalid,
  -- m_axi_bus_ptr_i_AWREADY              => S02_AXI_awready,
  -- m_axi_bus_ptr_i_WDATA(63 downto 0)   => S02_AXI_wdata(63 downto 0),
  -- m_axi_bus_ptr_i_WSTRB(7 downto 0)    => S02_AXI_wstrb(7 downto 0),
  -- m_axi_bus_ptr_i_WLAST                => S02_AXI_wlast,
  -- m_axi_bus_ptr_i_WVALID               => S02_AXI_wvalid,
  -- m_axi_bus_ptr_i_WREADY               => S02_AXI_wready,
  -- m_axi_bus_ptr_i_BID(0)               => S02_AXI_bid(0),
  -- m_axi_bus_ptr_i_BRESP(1 downto 0)    => S02_AXI_bresp(1 downto 0),
  -- m_axi_bus_ptr_i_BVALID               => S02_AXI_bvalid,
  -- m_axi_bus_ptr_i_BREADY               => S02_AXI_bready,
  -- m_axi_bus_ptr_i_ARID(0)              => S02_AXI_arid(0),      
  -- m_axi_bus_ptr_i_ARADDR(31 downto 0)  => S02_AXI_araddr(31 downto 0),
  -- m_axi_bus_ptr_i_ARLEN(7 downto 0)    => S02_AXI_arlen(7 downto 0),
  -- m_axi_bus_ptr_i_ARSIZE(2 downto 0)   => S02_AXI_arsize(2 downto 0),
  -- m_axi_bus_ptr_i_ARBURST(1 downto 0)  => S02_AXI_arburst(1 downto 0),
  -- m_axi_bus_ptr_i_ARLOCK(1 downto 0)            => S02_AXI_arlock(1 downto 0),
  -- m_axi_bus_ptr_i_ARCACHE(3 downto 0)  => S02_AXI_arcache(3 downto 0),
  -- m_axi_bus_ptr_i_ARPROT(2 downto 0)   => S02_AXI_arprot(2 downto 0),
  -- m_axi_bus_ptr_i_ARQOS(3 downto 0)    => S02_AXI_arqos(3 downto 0),
  -- m_axi_bus_ptr_i_ARVALID              => S02_AXI_arvalid,
  -- m_axi_bus_ptr_i_ARREADY              => S02_AXI_arready,
  -- m_axi_bus_ptr_i_RID(0)               => S02_AXI_rid(0),
  -- m_axi_bus_ptr_i_RDATA(63 downto 0)   => S02_AXI_rdata(63 downto 0),
  -- m_axi_bus_ptr_i_RRESP(1 downto 0)    => S02_AXI_rresp(1 downto 0),
  -- m_axi_bus_ptr_i_RLAST                => S02_AXI_rlast,
  -- m_axi_bus_ptr_i_RVALID               => S02_AXI_rvalid,
  -- m_axi_bus_ptr_i_RREADY               => S02_AXI_rready,
  -- bus_ptr_i_req_din			  =>   		    bus_ptr_i_req_din,
  -- bus_ptr_i_req_full_n        =>            bus_ptr_i_req_full_n,
  -- bus_ptr_i_req_write         =>             bus_ptr_i_req_write,
  -- bus_ptr_i_rsp_empty_n       =>             bus_ptr_i_rsp_empty_n,
  -- bus_ptr_i_rsp_read          =>             bus_ptr_i_rsp_read,
  -- bus_ptr_i_address           =>             bus_ptr_i_address,
  -- bus_ptr_i_datain            =>             bus_ptr_i_datain,
  -- bus_ptr_i_dataout           =>             bus_ptr_i_dataout,
  -- bus_ptr_i_size              =>             bus_ptr_i_size,

  -- aclk => ip_clk,
  -- aresetn => sys_rst_n 

-- );
		


	
datamover_i : axi_datamover_0
  PORT MAP (
    m_axi_mm2s_aclk => clk_axi,--clk_in,
    m_axi_mm2s_aresetn => sys_rst_n,
	
    mm2s_err => dm_mm2s_err,
	
    m_axis_mm2s_cmdsts_aclk => clk_axi,--clk_in,
    m_axis_mm2s_cmdsts_aresetn => sys_rst_n,
	
	
    s_axis_mm2s_cmd_tvalid => s_axis_mm2s_cmd_tvalid,
    s_axis_mm2s_cmd_tready => s_axis_mm2s_cmd_tready,
    s_axis_mm2s_cmd_tdata => s_axis_mm2s_cmd_tdata,
	
	
	
    m_axis_mm2s_sts_tvalid => m_axis_mm2s_sts_tvalid_i,
    m_axis_mm2s_sts_tready => m_axis_mm2s_sts_tready_i,
    m_axis_mm2s_sts_tdata => m_axis_mm2s_sts_tdata_i,
    m_axis_mm2s_sts_tkeep => m_axis_mm2s_sts_tkeep_i,
    m_axis_mm2s_sts_tlast => m_axis_mm2s_sts_tlast_i,
	
    m_axis_mm2s_tdata => m_axis_mm2s_tdata,
    m_axis_mm2s_tkeep => m_axis_mm2s_tkeep,
    m_axis_mm2s_tlast => m_axis_mm2s_tlast,
    m_axis_mm2s_tvalid => m_axis_mm2s_tvalid,
    m_axis_mm2s_tready => m_axis_mm2s_tready,	
	

	
	-- OK!
    m_axi_mm2s_arid => S00_AXI_arid,
    m_axi_mm2s_araddr => S00_AXI_araddr,
    m_axi_mm2s_arlen => S00_AXI_arlen,
    m_axi_mm2s_arsize => S00_AXI_arsize,
    m_axi_mm2s_arburst => S00_AXI_arburst,
    m_axi_mm2s_arprot => S00_AXI_arprot,
    m_axi_mm2s_arcache => S00_AXI_arcache,
    m_axi_mm2s_aruser => S00_AXI_aruser,
    m_axi_mm2s_arvalid => S00_AXI_arvalid,
    m_axi_mm2s_arready => S00_AXI_arready,
    m_axi_mm2s_rdata => S00_AXI_rdata,
    m_axi_mm2s_rresp => S00_AXI_rresp,
    m_axi_mm2s_rlast => S00_AXI_rlast,
    m_axi_mm2s_rvalid => S00_AXI_rvalid,
    m_axi_mm2s_rready => S00_AXI_rready,
	
	
    m_axi_s2mm_aclk => clk_axi,--clk_in,
    m_axi_s2mm_aresetn => sys_rst_n,
    s2mm_err => dm_s2mm_err,
	
    m_axis_s2mm_cmdsts_awclk => clk_axi,--clk_in,
    m_axis_s2mm_cmdsts_aresetn => sys_rst_n,
	
    s_axis_s2mm_cmd_tvalid => s_axis_s2mm_cmd_tvalid,
    s_axis_s2mm_cmd_tready => s_axis_s2mm_cmd_tready,
    s_axis_s2mm_cmd_tdata => s_axis_s2mm_cmd_tdata,
	
    m_axis_s2mm_sts_tvalid => m_axis_s2mm_sts_tvalid_i,
    m_axis_s2mm_sts_tready => m_axis_s2mm_sts_tready_i,
    m_axis_s2mm_sts_tdata => m_axis_s2mm_sts_tdata_i,
    m_axis_s2mm_sts_tkeep => m_axis_s2mm_sts_tkeep_i,
    m_axis_s2mm_sts_tlast => m_axis_s2mm_sts_tlast_i,
	


	
    s_axis_s2mm_tdata => s_axis_s2mm_tdata,
    s_axis_s2mm_tkeep => s_axis_s2mm_tkeep,
    s_axis_s2mm_tlast => s_axis_s2mm_tlast,
    s_axis_s2mm_tvalid => s_axis_s2mm_tvalid,
    s_axis_s2mm_tready => s_axis_s2mm_tready,	
	
	-- OK!
    m_axi_s2mm_awid => S01_AXI_awid,
    m_axi_s2mm_awaddr => S01_AXI_awaddr,
    m_axi_s2mm_awlen => S01_AXI_awlen,
    m_axi_s2mm_awsize => S01_AXI_awsize,
    m_axi_s2mm_awburst => S01_AXI_awburst,
    m_axi_s2mm_awprot => S01_AXI_awprot,
    m_axi_s2mm_awcache => S01_AXI_awcache,
    m_axi_s2mm_awuser => S01_AXI_awuser,
    m_axi_s2mm_awvalid => S01_AXI_awvalid,
    m_axi_s2mm_awready => S01_AXI_awready,
    m_axi_s2mm_wdata => S01_AXI_wdata,
    m_axi_s2mm_wstrb => S01_AXI_wstrb,
    m_axi_s2mm_wlast => S01_AXI_wlast,
    m_axi_s2mm_wvalid => S01_AXI_wvalid,
    m_axi_s2mm_wready => S01_AXI_wready,
    m_axi_s2mm_bresp => S01_AXI_bresp,
    m_axi_s2mm_bvalid => S01_AXI_bvalid,
    m_axi_s2mm_bready => S01_AXI_bready
	
  );	
  
  

  
---------------------------------------------
-- Stream to Memory Map = PCIe write to AXI
-- Memory Map to Stream = PCIe reads from AXI

bridge : stream_axi_bridge 
  generic map(
		C_AXIS_DWIDTH_mm2s    => 64,
		C_AXIS_DWIDTH_s2mm    => 64
			) 
	port map(
		-- clk and reset
		aclk 				=> clk_in,
		clk_axi             => clk_axi,
		aReset_n 		=> sys_rst_n ,

-- User interface

		-- User CMD interface
		StartPulse_mm2s			=> StartPulse_mm2s,
		StartAddrB_mm2s 		=> StartAddrB_mm2s,
		BytesToTransfer_mm2s  	=> BytesToTransfer_mm2s,
		CmdTag_mm2s				=> CmdTag_mm2s,
		CmdAck_mm2s				=> CmdAck_mm2s,
				
		
		-- User CMD interface
		StartPulse_s2mm			=> StartPulse_s2mm,
		StartAddrB_s2mm 		=> StartAddrB_s2mm,
		BytesToTransfer_s2mm 	=> BytesToTransfer_s2mm,
		CmdTag_s2mm				=> CmdTag_s2mm,
		CmdAck_s2mm				=> CmdAck_s2mm,
		

-- AXI Stream Master Fifo interface (Write to fifo)
		m_axis_txd_tvalid 	=> m_axis_txd_tvalid,
		m_axis_txd_tready 	=> m_axis_txd_tready,
		m_axis_txd_tdata 	=> m_axis_txd_tdata,
		
		-- AXI Stream Slave Fifo interface (Read from fifo)
		s_axis_rxd_tvalid 	=> s_axis_rxd_tvalid,
		s_axis_rxd_tready 	=> s_axis_rxd_tready,
		s_axis_rxd_tdata 	=> s_axis_rxd_tdata,	
		
-- AXI Stream Master interface (Read Status from AXI)
		-- To User IP
		m_axis_s2mm_sts_tvalid 	=> m_axis_s2mm_sts_tvalid,
		m_axis_s2mm_sts_tready 	=> m_axis_s2mm_sts_tready,
		m_axis_s2mm_sts_tdata 	=> m_axis_s2mm_sts_tdata,
		m_axis_s2mm_sts_tkeep 	=> open,
		m_axis_s2mm_sts_tlast 	=> open,
		
		m_axis_mm2s_sts_tvalid 	=> m_axis_mm2s_sts_tvalid,
		m_axis_mm2s_sts_tready 	=> m_axis_mm2s_sts_tready,
		m_axis_mm2s_sts_tdata 	=> m_axis_mm2s_sts_tdata,
		m_axis_mm2s_sts_tkeep 	=> open,
		m_axis_mm2s_sts_tlast 	=> open,		


-- AXI interface to DataMover block
		-- AXI Stream Master interface (Write CMD to AXI Datamover)
		m_axis_cmd_s2mm_tvalid 	=> s_axis_s2mm_cmd_tvalid,
		m_axis_cmd_s2mm_tready 	=> s_axis_s2mm_cmd_tready,
		m_axis_cmd_s2mm_tdata 	=> s_axis_s2mm_cmd_tdata,
		m_axis_cmd_mm2s_tvalid 	=> s_axis_mm2s_cmd_tvalid,
		m_axis_cmd_mm2s_tready 	=> s_axis_mm2s_cmd_tready,
		m_axis_cmd_mm2s_tdata 	=> s_axis_mm2s_cmd_tdata,
		
	

		-- AXI Stream Master S2MM interface (Write DATA to AXI)
		m_axis_s2mm_tvalid 	=> s_axis_s2mm_tvalid,
		m_axis_s2mm_tready 	=> s_axis_s2mm_tready,
		m_axis_s2mm_tdata 	=> s_axis_s2mm_tdata,
		m_axis_s2mm_tkeep 	=> s_axis_s2mm_tkeep,
		m_axis_s2mm_tlast 	=> s_axis_s2mm_tlast,

		-- AXI Stream Slave S2MM interface (Read DATA from AXI)
		s_axis_mm2s_tvalid 	=> m_axis_mm2s_tvalid,
		s_axis_mm2s_tready 	=> m_axis_mm2s_tready,
		s_axis_mm2s_tdata 	=> m_axis_mm2s_tdata,
		s_axis_mm2s_tkeep 	=> m_axis_mm2s_tkeep,
		s_axis_mm2s_tlast 	=> m_axis_mm2s_tlast,
		


		-- AXI Stream Master interface (Read Status from AXI)
		-- From Datamover		
		m_axis_s2mm_sts_tvalid_i  => m_axis_s2mm_sts_tvalid_i, 
		m_axis_s2mm_sts_tready_i  => m_axis_s2mm_sts_tready_i,
		m_axis_s2mm_sts_tdata_i   => m_axis_s2mm_sts_tdata_i,
		m_axis_s2mm_sts_tkeep_i   => m_axis_s2mm_sts_tkeep_i,
		m_axis_s2mm_sts_tlast_i   => m_axis_s2mm_sts_tlast_i,
		
		m_axis_mm2s_sts_tvalid_i  => m_axis_mm2s_sts_tvalid_i, 
		m_axis_mm2s_sts_tready_i  => m_axis_mm2s_sts_tready_i,
		m_axis_mm2s_sts_tdata_i   => m_axis_mm2s_sts_tdata_i,
		m_axis_mm2s_sts_tkeep_i   => m_axis_mm2s_sts_tkeep_i,
		m_axis_mm2s_sts_tlast_i   => m_axis_mm2s_sts_tlast_i
	
	);  
	

end STRUCTURE;
