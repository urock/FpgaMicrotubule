-------------------------------------------------------------------------------
-- Project     : Rosta Base PCIe project Ver.10
-- File        : rc_2000_top.vhd
-- Version     : 10.0
-- Company     : Rosta
-- Engineer    : rkob & urock
-- Create Date : 14.03.2014 
-- Comments    : Top level 
-- Description : PCI Express Endpoint 
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

library unisim;
use unisim.vcomponents.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity rc_mt_2000_12 is
  port (
	pci_exp_txp	: out std_logic_vector(3 downto 0);
	pci_exp_txn	: out std_logic_vector(3 downto 0);
	pci_exp_rxp	: in std_logic_vector(3 downto 0);
	pci_exp_rxn	: in std_logic_vector(3 downto 0);

	sys_clk_p	: in std_logic;
	sys_clk_n	: in std_logic;
	sys_rst_n	: in std_logic;

	-- Identifier FPGA: "00" - LD, "01" - LU, "10" - RD, "11" - RU
    -- - - - - - - - - - C0 - - - - C1 - - - - C2 - - - - C3 - - -	                       
    id			: in std_logic_vector(1 downto 0);
	
	-- Command bus: need for transmit MAC-Address and ID
    strtcmd		: in std_logic;
    command		: inout std_logic_vector(7 downto 0);
    dval   		: inout std_logic;
    
--    Btm_ddr_cs_n : out std_logic;
    Top_ddr_reset_n : out std_logic;
    Top_ddr_dq : inout std_logic_vector(31 downto 0);
    Top_ddr_dqs_p : inout std_logic_vector(3 downto 0);
    Top_ddr_we_n : out std_logic;
    Top_ddr_dqs_n : inout std_logic_vector(3 downto 0);
    Top_ddr_ras_n : out std_logic;
    Top_ddr_odt : out std_logic;
    Top_ddr_ck_n : out std_logic;
    Top_ddr_cke : out std_logic;
    Top_ddr_cs_n : out std_logic;
    Top_ddr_ba : out std_logic_vector(2 downto 0);
    Top_ddr_cas_n : out std_logic;
    Top_ddr_ck_p : out std_logic;
    Top_ddr_dm : out std_logic_vector(3 downto 0);
--    Btm_ddr_cas_n : out std_logic;
--    Btm_ddr_ck_p : out std_logic;
--    Btm_ddr_ba : out std_logic_vector(2 downto 0);
--    Btm_ddr_dm : out std_logic_vector(3 downto 0);
--    Btm_ddr_dq : inout std_logic_vector(31 downto 0);
--    Btm_ddr_ras_n : out std_logic;
--    Btm_ddr_ck_n : out std_logic;
--    Btm_ddr_odt : out std_logic;
--    Btm_ddr_we_n : out std_logic;
--    Btm_ddr_dqs_n : inout std_logic_vector(3 downto 0);
--    Btm_ddr_reset_n : out std_logic;
--    Btm_ddr_cke : out std_logic;
--    Btm_ddr_dqs_p : inout std_logic_vector(3 downto 0);
--    Btm_ddr_addr : out std_logic_vector(13 downto 0);
    Top_ddr_addr : out std_logic_vector(13 downto 0);  
    


    clk100		: in std_logic; -- user clock from S6 - 100 mhz

    aclk_p      : in std_logic; -- user diff.clock - 100 mhz
    aclk_n      : in std_logic
);
end rc_mt_2000_12;

architecture base of rc_mt_2000_12 is


attribute buffer_type : string;
attribute buffer_type of pci_exp_txp : signal is "none";
attribute buffer_type of pci_exp_txn : signal is "none";
attribute buffer_type of pci_exp_rxp : signal is "none";
attribute buffer_type of pci_exp_rxn : signal is "none";

attribute buffer_type of sys_clk_p : signal is "none";
attribute buffer_type of sys_clk_n : signal is "none";
attribute buffer_type of sys_rst_n : signal is "none";





component base_ip is
port (
    pci_exp_txp     : out std_logic_vector(3 downto 0);
    pci_exp_txn     : out std_logic_vector(3 downto 0);
    pci_exp_rxp     : in std_logic_vector(3 downto 0);
    pci_exp_rxn     : in std_logic_vector(3 downto 0);
    sys_clk_p       : in std_logic;
    sys_clk_n       : in std_logic;
    sys_rst_n       : in std_logic;
    
---------------------------------------------------
-- User IP interface

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

	ip_clk		    : in  std_logic;
	user_rst_n		: out std_logic;
	
	clk_axi     : in std_logic;

-- HOST DMA Data 	
	-- RX FIFO
	m_user_tvalid	: out std_logic;
	m_user_tready	: in  std_logic;
	m_user_tdata		: out std_logic_vector(63 downto 0); 
	 
	-- TX FIFO
	s_user_tvalid	: in  std_logic;
	s_user_tready		: out std_logic;		
	s_user_tdata		: in  std_logic_vector(63 downto 0);		

--  Register interface
	Reg_RE		: out std_logic;
	Reg_WE		: out std_logic; 
	Reg_Addr	: out std_logic_vector( 7 downto 0); 
	Reg_Dout	: out std_logic_vector(31 downto 0); 
	Reg_Din		: in  std_logic_vector(31 downto 0);
	Reg_Valid	: in  std_logic; 	
	
	pcie_int_req	: in  std_logic;
	pcie_int_compl	: out std_logic;
    	
   	-- EP tx interface
  	-- EP TX CMD FIFO interface 
   	ep_tx_cmd_wr_en	: in  std_logic;
   	ep_tx_cmd_data	: in  std_logic_vector(63 downto 0);-- (addr_32_bits & length_24_bits & tag_8_bits)
   	ep_tx_cmd_full	: out std_logic;  
    	
   	ep_tx_error		: out std_logic;	
   	ep_tx_tag_error	: out std_logic_vector(7 downto 0); 
    		
   	ep_txd_s_tvalid	: in  std_logic;
   	ep_txd_s_tdata	: in  std_logic_vector(63 downto 0);
   	ep_txd_s_tready	: out std_logic;
    		
   	-- EP rx interface	    		
   	ep_rxd_m_tvalid	: out std_logic;
   	ep_rxd_m_tready : in  std_logic;
   	ep_rxd_m_tdata	: out std_logic_vector(63 downto 0);
   	

    
    
    Top_ddr_dq         : inout std_logic_vector(31 downto 0);
    Top_ddr_dqs_p     : inout std_logic_vector(3 downto 0);
    Top_ddr_dqs_n     : inout std_logic_vector(3 downto 0);
    Top_ddr_addr     : out std_logic_vector(13 downto 0);
    Top_ddr_ba         : out std_logic_vector(2 downto 0);
    Top_ddr_ras_n     : out std_logic;
    Top_ddr_cas_n     : out std_logic;                        
    Top_ddr_we_n     : out std_logic;
    Top_ddr_reset_n : out std_logic;
    Top_ddr_ck_p     : out std_logic;
    Top_ddr_ck_n     : out std_logic;
    Top_ddr_cke     : out std_logic;
    Top_ddr_cs_n     : out std_logic;
    Top_ddr_dm         : out std_logic_vector(3 downto 0);
    Top_ddr_odt     : out std_logic
      
	
    );
end component;

signal S02_AXI_awid 		: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_awuser 		: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_awaddr 		: STD_LOGIC_VECTOR ( 31 downto 0 );
signal S02_AXI_awlen 		: STD_LOGIC_VECTOR ( 7 downto 0 );
signal S02_AXI_awsize 		: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S02_AXI_awburst 		: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S02_AXI_awlock 		: STD_LOGIC_VECTOR ( 0 to 0 );
signal S02_AXI_awcache 		: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S02_AXI_awprot 		: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S02_AXI_awregion 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S02_AXI_awqos 		: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S02_AXI_awvalid 		: STD_LOGIC;
signal S02_AXI_awready 		: STD_LOGIC;
	
signal S02_AXI_wid 			: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_wuser 		: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_wdata 		: STD_LOGIC_VECTOR ( 63 downto 0 );
signal S02_AXI_wstrb 		: STD_LOGIC_VECTOR ( 7 downto 0 );
signal S02_AXI_wlast 		: STD_LOGIC;
signal S02_AXI_wvalid 		: STD_LOGIC;
signal S02_AXI_wready 		: STD_LOGIC;

signal S02_AXI_buser 		: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_bid 			: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_bresp 		: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S02_AXI_bvalid 		: STD_LOGIC;
signal S02_AXI_bready 		: STD_LOGIC;
    
signal S02_AXI_aruser 		: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_arid 		: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_araddr 		: STD_LOGIC_VECTOR ( 31 downto 0 );
signal S02_AXI_arlen 		: STD_LOGIC_VECTOR ( 7 downto 0 );
signal S02_AXI_arsize 		: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S02_AXI_arburst 		: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S02_AXI_arlock 		: STD_LOGIC_VECTOR ( 0 to 0 );
signal S02_AXI_arcache 		: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S02_AXI_arprot 		: STD_LOGIC_VECTOR ( 2 downto 0 );
signal S02_AXI_arregion 	: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S02_AXI_arqos 		: STD_LOGIC_VECTOR ( 3 downto 0 );
signal S02_AXI_arvalid 		: STD_LOGIC;
signal S02_AXI_arready 		: STD_LOGIC;
    
signal S02_AXI_ruser 		: STD_LOGIC_VECTOR ( 0 downto 0 );	
signal S02_AXI_rid 			: STD_LOGIC_VECTOR ( 0 downto 0 );
signal S02_AXI_rdata 		: STD_LOGIC_VECTOR ( 63 downto 0 );
signal S02_AXI_rresp 		: STD_LOGIC_VECTOR ( 1 downto 0 );
signal S02_AXI_rlast 		: STD_LOGIC;
signal S02_AXI_rvalid 		: STD_LOGIC;
signal S02_AXI_rready 		: STD_LOGIC;




component user_ip is 
port (
	user_rst_n		: in  std_logic;-- active low

	clk_in   		: in  std_logic;
	clk_out   		: out  std_logic;
	
--	axi_clk         : in std_logic;
		
	-- FIFO interface
	-- AXIs Slave interface (read from RX_FIFO)
	host_rxd_s_tvalid 	: in  std_logic;
	host_rxd_s_tready 	: out std_logic;
	host_rxd_s_tdata  	: in  std_logic_vector(63 downto 0);
		
	-- AXIs Master interface (write to TX_FIFO)
	host_txd_m_tvalid	: out std_logic; 
	host_txd_m_tready	: in  std_logic;
	host_txd_m_tdata	: out std_logic_vector(63 downto 0);
	
	--  Registers interface
	Reg_RE		: in  std_logic;
	Reg_WE		: in  std_logic;
	Reg_Addr	: in  std_logic_vector( 7 downto 0);
	Reg_Dout	: in  std_logic_vector(31 downto 0); 
	Reg_Din		: out std_logic_vector(31 downto 0);
	Reg_Valid	: out std_logic; 	
				
	user_int_req	: out std_logic;
	user_int_compl	: in  std_logic;
		
	-- EP tx interface
	-- EP TX CMD FIFO interface 
	ep_tx_cmd_wr_en		: out std_logic;
	ep_tx_cmd_data		: out std_logic_vector(63 downto 0);	-- (addr_32_bits & length_24_bits & tag_8_bits)
	ep_tx_cmd_full		: in  std_logic;  
   	
	ep_tx_error			: in std_logic;	
	ep_tx_tag_error		: in std_logic_vector(7 downto 0); 
   		
   	-- EP TXD AXIS Master (write to other EP)
	ep_txd_m_tvalid		: out std_logic;
	ep_txd_m_tdata		: out std_logic_vector(63 downto 0);
	ep_txd_m_tready		: in  std_logic;
   		
	-- EP RXD AXIS Slave (receive data from other EP) 
	ep_rxd_s_tvalid	: in  std_logic;
	ep_rxd_s_tready : out std_logic;
	ep_rxd_s_tdata	: in  std_logic_vector(63 downto 0);
	


		-- HLS signals 
    ap_rst_n 		: out STD_LOGIC;							-- +
    ap_ctrl_done 	: in  STD_LOGIC;							-- +
    ap_ctrl_idle 	: in  STD_LOGIC;
    ap_ctrl_ready 	: in  STD_LOGIC;
    ap_ctrl_start 	: out STD_LOGIC;							-- +
    ap_return 		: in  STD_LOGIC_VECTOR ( 31 downto 0 );		-- +  
	
    cp_size 	: out STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_i 	: out STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_o 	: out STD_LOGIC_VECTOR ( 31 downto 0 );
    p1 			: out STD_LOGIC_VECTOR ( 31 downto 0 );
    p2 			: out STD_LOGIC_VECTOR ( 31 downto 0 );
    InputLength : out STD_LOGIC_VECTOR ( 31 downto 0 );
		
		rand1_tdata 			: out std_logic_vector(63 downto 0);
		rand1_tvalid 			: out std_logic;
		rand1_tready 			: in  std_logic;
	
		rand2_tdata 			: out std_logic_vector(63 downto 0);
		rand2_tvalid 			: out std_logic;
		rand2_tready 			: in  std_logic;

		rand3_tdata 			: out std_logic_vector(63 downto 0);
		rand3_tvalid 			: out std_logic;
		rand3_tready 			: in  std_logic;

		rand4_tdata 			: out std_logic_vector(63 downto 0);
		rand4_tvalid 			: out std_logic;
		rand4_tready 			: in  std_logic;

		rand5_tdata 			: out std_logic_vector(63 downto 0);
		rand5_tvalid 			: out std_logic;
		rand5_tready 			: in  std_logic
    
	);
end component; 

signal rand1_tdata 			: std_logic_vector(63 downto 0);
signal rand1_tvalid 		: std_logic;
signal rand1_tready 		: std_logic;

signal rand2_tdata 			: std_logic_vector(63 downto 0);
signal rand2_tvalid 		: std_logic;
signal rand2_tready 		: std_logic;

signal rand3_tdata 			: std_logic_vector(63 downto 0);
signal rand3_tvalid 		: std_logic;
signal rand3_tready 		: std_logic;

signal rand4_tdata 			: std_logic_vector(63 downto 0);
signal rand4_tvalid 		: std_logic;
signal rand4_tready 		: std_logic;

signal rand5_tdata 			: std_logic_vector(63 downto 0);
signal rand5_tvalid 		: std_logic;
signal rand5_tready 		: std_logic;

		-- HLS signals 
signal ap_rst_n 		: STD_LOGIC;							-- +
signal ap_ctrl_done 	: STD_LOGIC;							-- +
signal ap_ctrl_idle 	: STD_LOGIC;
signal ap_ctrl_ready 	: STD_LOGIC;
signal ap_ctrl_start 	: STD_LOGIC;							-- +
signal ap_return 		: STD_LOGIC_VECTOR ( 31 downto 0 );		-- +  

signal cp_size 		: STD_LOGIC_VECTOR ( 31 downto 0 );
signal offset_i 	: STD_LOGIC_VECTOR ( 31 downto 0 );
signal offset_o 	: STD_LOGIC_VECTOR ( 31 downto 0 );
signal p1 			: STD_LOGIC_VECTOR ( 31 downto 0 );
signal p2 			: STD_LOGIC_VECTOR ( 31 downto 0 );
signal InputLength 	: STD_LOGIC_VECTOR ( 31 downto 0 );
		


signal const_logic_0	: std_logic_vector(0 downto 0);



component bd_hls_wrapper is
  port (
  
    ap_clk 			: in STD_LOGIC;
    ap_rst_n 		: in STD_LOGIC;
    ap_ctrl_done 	: out STD_LOGIC;
    ap_ctrl_idle 	: out STD_LOGIC;
    ap_ctrl_ready 	: out STD_LOGIC;
    ap_ctrl_start 	: in STD_LOGIC;
    ap_return 		: out STD_LOGIC_VECTOR ( 31 downto 0 );
	
    cp_size 	    : in STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_i 	    : in STD_LOGIC_VECTOR ( 31 downto 0 );
    offset_o 	    : in STD_LOGIC_VECTOR ( 31 downto 0 );
    p1 			    : in STD_LOGIC_VECTOR ( 31 downto 0 );
    p2 			    : in STD_LOGIC_VECTOR ( 31 downto 0 );
    InputLength     : in STD_LOGIC_VECTOR ( 31 downto 0 );

	
    M00_AXI_araddr 	 : out STD_LOGIC_VECTOR ( 31 downto 0 );
    M00_AXI_arburst  : out STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_arcache  : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arlen 	 : out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_arlock 	 : out STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXI_arprot 	 : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_arqos 	 : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arready  : in STD_LOGIC;
    M00_AXI_arregion : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_arsize 	 : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_arvalid  : out STD_LOGIC;
    M00_AXI_awaddr 	 : out STD_LOGIC_VECTOR ( 31 downto 0 );
    M00_AXI_awburst  : out STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_awcache  : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awlen 	 : out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_awlock 	 : out STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXI_awprot 	 : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_awqos 	 : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awready  : in STD_LOGIC;
    M00_AXI_awregion : out STD_LOGIC_VECTOR ( 3 downto 0 );
    M00_AXI_awsize 	 : out STD_LOGIC_VECTOR ( 2 downto 0 );
    M00_AXI_awvalid  : out STD_LOGIC;
    M00_AXI_bready 	 : out STD_LOGIC;
    M00_AXI_bresp 	 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_bvalid 	 : in STD_LOGIC;
    M00_AXI_rdata 	 : in STD_LOGIC_VECTOR ( 63 downto 0 );
    M00_AXI_rlast 	 : in STD_LOGIC;
    M00_AXI_rready 	 : out STD_LOGIC;
    M00_AXI_rresp 	 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    M00_AXI_rvalid 	 : in STD_LOGIC;
    M00_AXI_wdata 	 : out STD_LOGIC_VECTOR ( 63 downto 0 );
    M00_AXI_wlast 	 : out STD_LOGIC;
    M00_AXI_wready 	 : in STD_LOGIC;
    M00_AXI_wstrb 	 : out STD_LOGIC_VECTOR ( 7 downto 0 );
    M00_AXI_wvalid 	 : out STD_LOGIC;

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
	rand5_tready 			: out std_logic	
  );
end component;

signal strm1_tdata 			: std_logic_vector (63 downto 0);
signal strm1_tvalid 		: std_logic;
signal strm1_tready	 		: std_logic;

signal strm2_tdata 			: std_logic_vector (63 downto 0);
signal strm2_tvalid 		: std_logic;
signal strm2_tready	 		: std_logic;

signal strm3_tdata 			: std_logic_vector (63 downto 0);
signal strm3_tvalid 		: std_logic;
signal strm3_tready	 		: std_logic;

signal strm4_tdata 			: std_logic_vector (63 downto 0);
signal strm4_tvalid 		: std_logic;
signal strm4_tready	 		: std_logic;

signal strm5_tdata 			: std_logic_vector (63 downto 0);
signal strm5_tvalid 		: std_logic;
signal strm5_tready	 		: std_logic;


-- EP tx interface
-- EP TX CMD FIFO interface 
signal ep_tx_cmd_wr_en		: std_logic;
signal ep_tx_cmd_data		: std_logic_vector(63 downto 0);	-- (addr_32_bits & length_24_bits & tag_8_bits)
signal ep_tx_cmd_full		: std_logic;  
    	
signal ep_tx_error			: std_logic;	
signal ep_tx_tag_error		: std_logic_vector(7 downto 0); 
    		
signal ep_txd_s_tvalid		: std_logic;
signal ep_txd_s_tdata		: std_logic_vector(63 downto 0);
signal ep_txd_s_tready		: std_logic;
    		
-- EP rx interface	    		
signal ep_rxd_m_tvalid	: std_logic;
signal ep_rxd_m_tready 	: std_logic;
signal ep_rxd_m_tdata	: std_logic_vector(63 downto 0); 

-- FIFO interface
-- AXIs Slave interface (read from RX_FIFO)
signal host_rxd_s_tvalid 	: std_logic;
signal host_rxd_s_tready 	: std_logic;
signal host_rxd_s_tdata  	: std_logic_vector(63 downto 0);

-- AXIs Master interface (write to TX_FIFO)
signal host_txd_m_tvalid	: std_logic; 
signal host_txd_m_tready	: std_logic;
signal host_txd_m_tdata		: std_logic_vector(63 downto 0);

-- User Interface	
signal Reg_RE			: std_logic;
signal Reg_WE			: std_logic; 
signal Reg_Addr			: std_logic_vector( 7 downto 0); 
signal Reg_Dout			: std_logic_vector(31 downto 0); 
signal Reg_Din			: std_logic_vector(31 downto 0); 
signal Reg_Valid		: std_logic; 
	
signal user_rst_n		: std_logic;
signal user_rst : std_logic; 
signal user_int_req 	: std_logic; 
signal user_int_compl   : std_logic; 

signal aclk : std_logic; 
signal ip_clk : std_logic; 
signal axi_clk  : std_logic;

signal sys_reset_c : std_logic; 


signal clk100mhz : std_logic; 


begin

user_rst <= not user_rst_n;



clkin1_ibufg: IBUFG
port map
 (O => clk100mhz,
  I => clk100);
  

diffclk_ibuf : IBUFDS_GTE2
     port map(
       O       => aclk,
       ODIV2   => open,
       I       => aclk_p,
       IB      => aclk_n,
       CEB     => '0');

base : base_ip 
port map (
    pci_exp_txp  => pci_exp_txp,
    pci_exp_txn  => pci_exp_txn,
    pci_exp_rxp  => pci_exp_rxp,  
    pci_exp_rxn  => pci_exp_rxn,
    
    sys_clk_p    => sys_clk_p,     
    sys_clk_n    => sys_clk_n,
    sys_rst_n    => sys_rst_n,   

    user_rst_n	=> user_rst_n,
    ip_clk		=> ip_clk,  
    clk_axi => ip_clk, 
     	
    -- FIFO interface - AXI Stream
    -- RX FIFO
    m_user_tvalid => host_rxd_s_tvalid,
    m_user_tready => host_rxd_s_tready,
    m_user_tdata	 => host_rxd_s_tdata,
     
    -- TX FIFO
    s_user_tvalid => host_txd_m_tvalid,
    s_user_tready	 => host_txd_m_tready,		
    s_user_tdata	 => host_txd_m_tdata,	
    	
    --  Registers interface
    Reg_RE		=> Reg_RE,	
    Reg_WE		=> Reg_WE,	
    Reg_Addr	=> Reg_Addr,
    Reg_Dout	=> Reg_Dout,
    Reg_Din		=> Reg_Din,
    Reg_Valid	=> Reg_Valid,
    
    pcie_int_req	=> user_int_req,
    pcie_int_compl	=> user_int_compl,
	
    -- EP tx interface
    -- EP TX CMD FIFO interface 
    ep_tx_cmd_wr_en	=> ep_tx_cmd_wr_en,
    ep_tx_cmd_data	=> ep_tx_cmd_data,
    ep_tx_cmd_full	=> ep_tx_cmd_full,
    	
    ep_tx_error		=> ep_tx_error,
    ep_tx_tag_error	=> ep_tx_tag_error,
    	
    ep_txd_s_tvalid	=> ep_txd_s_tvalid,
    ep_txd_s_tdata	=> ep_txd_s_tdata,
    ep_txd_s_tready	=> ep_txd_s_tready,
    
    -- EP rx interface	
    ep_rxd_m_tvalid	=> ep_rxd_m_tvalid,
    ep_rxd_m_tready => ep_rxd_m_tready,
    ep_rxd_m_tdata	=> ep_rxd_m_tdata,


     Top_ddr_reset_n => Top_ddr_reset_n,
     Top_ddr_dq => Top_ddr_dq,
     Top_ddr_dqs_p => Top_ddr_dqs_p,
     Top_ddr_we_n => Top_ddr_we_n,
     Top_ddr_dqs_n => Top_ddr_dqs_n,
     Top_ddr_ras_n => Top_ddr_ras_n,
     Top_ddr_odt => Top_ddr_odt,
     Top_ddr_ck_n => Top_ddr_ck_n,
     Top_ddr_cke => Top_ddr_cke,
     Top_ddr_cs_n => Top_ddr_cs_n,
     Top_ddr_ba => Top_ddr_ba,
     Top_ddr_cas_n => Top_ddr_cas_n,
     Top_ddr_ck_p => Top_ddr_ck_p,
     Top_ddr_dm => Top_ddr_dm,
     Top_ddr_addr => Top_ddr_addr,
	 
      -- S02_AXI_awid          => const_logic_0,
      -- S02_AXI_awuser        => const_logic_0,
      S02_AXI_awaddr        => S02_AXI_awaddr,
      S02_AXI_awlen         => S02_AXI_awlen,
      S02_AXI_awsize        => S02_AXI_awsize,
      S02_AXI_awburst       => S02_AXI_awburst,
      S02_AXI_awlock  		=> S02_AXI_awlock,
      S02_AXI_awcache       => S02_AXI_awcache,
      S02_AXI_awprot        => S02_AXI_awprot,
      S02_AXI_awqos         => S02_AXI_awqos,
      S02_AXI_awvalid       => S02_AXI_awvalid,
      S02_AXI_awready       => S02_AXI_awready,
      S02_AXI_awregion      => S02_AXI_awregion,
	  
      -- S02_AXI_wid           => const_logic_0,
      -- S02_AXI_wuser         => const_logic_0,	  
      S02_AXI_wdata         => S02_AXI_wdata, 
      S02_AXI_wstrb         => S02_AXI_wstrb,
      S02_AXI_wlast         => S02_AXI_wlast,
      S02_AXI_wvalid        => S02_AXI_wvalid,
      S02_AXI_wready        => S02_AXI_wready,

	  
      -- S02_AXI_bid           => const_logic_0,
      -- S02_AXI_buser	        => const_logic_0,
      S02_AXI_bresp         => S02_AXI_bresp,
      S02_AXI_bvalid       	=> S02_AXI_bvalid,
      S02_AXI_bready       	=> S02_AXI_bready,

      -- S02_AXI_arid          => const_logic_0, 
      -- S02_AXI_aruser        => const_logic_0,  
      S02_AXI_araddr       	=> S02_AXI_araddr,
      S02_AXI_arlen      	=> S02_AXI_arlen,
      S02_AXI_arsize      	=> S02_AXI_arsize,
      S02_AXI_arburst      	=> S02_AXI_arburst,
      S02_AXI_arlock      	=> S02_AXI_arlock,            
      S02_AXI_arcache      	=> S02_AXI_arcache,
      S02_AXI_arprot      	=> S02_AXI_arprot,
      S02_AXI_arqos       	=> S02_AXI_arqos,
      S02_AXI_arvalid       => S02_AXI_arvalid,
      S02_AXI_arready       => S02_AXI_arready,
      S02_AXI_arregion      => S02_AXI_arregion,

      -- S02_AXI_rid           => const_logic_0,
      -- S02_AXI_ruser         => const_logic_0,
      S02_AXI_rdata  		=> S02_AXI_rdata,
      S02_AXI_rresp       	=> S02_AXI_rresp,
      S02_AXI_rlast         => S02_AXI_rlast,
      S02_AXI_rvalid        => S02_AXI_rvalid,
      S02_AXI_rready        => S02_AXI_rready 
	 
	 
    	
 );
  
const_logic_0(0) <= '0'; 

user :  user_ip 
	port map (
	user_rst_n		=> user_rst_n,		-- active low
	
	clk_in			=> aclk,
	clk_out			=> ip_clk, 	
	
--	axi_clk         => axi_clk, 	

	-- FIFO interface
	-- AXIs Slave interface (read from RX_FIFO) 
	host_rxd_s_tvalid	=> host_rxd_s_tvalid,
	host_rxd_s_tready	=> host_rxd_s_tready,
	host_rxd_s_tdata	=> host_rxd_s_tdata, 

	-- AXIs Master interface (write to TX_FIFO)	
	host_txd_m_tvalid	=> host_txd_m_tvalid, 
	host_txd_m_tready	=> host_txd_m_tready,
	host_txd_m_tdata	=> host_txd_m_tdata,

--  Registers interface
	Reg_RE		=> Reg_RE,	
	Reg_WE		=> Reg_WE,
	Reg_Addr	=> Reg_Addr,
	Reg_Dout	=> Reg_Dout,
	Reg_Din		=> Reg_Din,
	Reg_Valid	=> Reg_Valid,
	
	user_int_req   => user_int_req,
	user_int_compl => user_int_compl,
	
	-- EP tx interface
	-- EP TX CMD FIFO interface 
	ep_tx_cmd_wr_en		=> ep_tx_cmd_wr_en,
	ep_tx_cmd_data		=> ep_tx_cmd_data,
	ep_tx_cmd_full		=> ep_tx_cmd_full,
	
	ep_tx_error			=> ep_tx_error,
	ep_tx_tag_error		=> ep_tx_tag_error,
	
	-- EP TXD AXIS Master (write to other EP)
	ep_txd_m_tvalid		=> ep_txd_s_tvalid,
	ep_txd_m_tdata		=> ep_txd_s_tdata,
	ep_txd_m_tready		=> ep_txd_s_tready,
	
	-- EP rx interface	
	-- EP RXD AXIS Slave (receive data from other EP) 
	ep_rxd_s_tvalid	=> ep_rxd_m_tvalid,
	ep_rxd_s_tready => ep_rxd_m_tready,
	ep_rxd_s_tdata	=> ep_rxd_m_tdata,
	
		-- HLS signals 
    ap_rst_n 		=> ap_rst_n, 							-- +
    ap_ctrl_done 	=> ap_ctrl_done, 							-- +
    ap_ctrl_idle 	=> ap_ctrl_idle, 
    ap_ctrl_ready 	=> ap_ctrl_ready, 
    ap_ctrl_start 	=> ap_ctrl_start, 	-- +
    ap_return 		=> ap_return, 	-- +  
	
    cp_size 	=> cp_size, 
    offset_i 	=> offset_i, 
    offset_o 	=> offset_o, 
    p1 			=> p1, 
    p2 			=> p2, 
    InputLength => InputLength,
	
	rand1_tdata 			=> rand1_tdata, 
	rand1_tvalid	 		=> rand1_tvalid,
	rand1_tready	 		=> rand1_tready,
	
	rand2_tdata 			=> rand2_tdata, 
	rand2_tvalid	 		=> rand2_tvalid,
	rand2_tready	 		=> rand2_tready,
	
	rand3_tdata 			=> rand3_tdata, 
	rand3_tvalid	 		=> rand3_tvalid,
	rand3_tready	 		=> rand3_tready,
	
	rand4_tdata 			=> rand4_tdata, 
	rand4_tvalid	 		=> rand4_tvalid,
	rand4_tready	 		=> rand4_tready,
	
	rand5_tdata 			=> rand5_tdata, 
	rand5_tvalid	 		=> rand5_tvalid,
	rand5_tready	 		=> rand5_tready

	
    
  ); 
  

hls: bd_hls_wrapper
port map (
    ap_clk 			=> ip_clk,
    ap_rst_n 		=> ap_rst_n,
    ap_ctrl_done 	=> ap_ctrl_done,
    ap_ctrl_idle 	=> ap_ctrl_idle,
    ap_ctrl_ready 	=> ap_ctrl_ready,
    ap_ctrl_start 	=> ap_ctrl_start,
    ap_return 		=> ap_return,
	                
    cp_size 	    => cp_size,
    offset_i 	    => offset_i,
    offset_o 	    => offset_o,
    p1 			    => p1,
    p2 			    => p2,
    InputLength     => InputLength,


	rand1_tdata 			=> rand1_tdata, 
	rand1_tvalid	 		=> rand1_tvalid,
	rand1_tready	 		=> rand1_tready,
	
	rand2_tdata 			=> rand2_tdata, 
	rand2_tvalid	 		=> rand2_tvalid,
	rand2_tready	 		=> rand2_tready,
	
	rand3_tdata 			=> rand3_tdata, 
	rand3_tvalid	 		=> rand3_tvalid,
	rand3_tready	 		=> rand3_tready,
	
	rand4_tdata 			=> rand4_tdata, 
	rand4_tvalid	 		=> rand4_tvalid,
	rand4_tready	 		=> rand4_tready,
	
	rand5_tdata 			=> rand5_tdata, 
	rand5_tvalid	 		=> rand5_tvalid,
	rand5_tready	 		=> rand5_tready,
	
    M00_AXI_araddr 	 => S02_AXI_araddr 	 ,
    M00_AXI_arburst  => S02_AXI_arburst  ,
    M00_AXI_arcache  => S02_AXI_arcache  ,
    M00_AXI_arlen 	 => S02_AXI_arlen 	 ,
    M00_AXI_arlock 	 => S02_AXI_arlock 	 ,
    M00_AXI_arprot 	 => S02_AXI_arprot 	 ,
    M00_AXI_arqos 	 => S02_AXI_arqos 	 ,
    M00_AXI_arready  => S02_AXI_arready  ,
    M00_AXI_arregion => S02_AXI_arregion ,
    M00_AXI_arsize 	 => S02_AXI_arsize 	 ,
    M00_AXI_arvalid  => S02_AXI_arvalid  ,
	                    
    M00_AXI_awaddr 	 => S02_AXI_awaddr 	 ,
    M00_AXI_awburst  => S02_AXI_awburst  ,
    M00_AXI_awcache  => S02_AXI_awcache  ,
    M00_AXI_awlen 	 => S02_AXI_awlen 	 ,
    M00_AXI_awlock 	 => S02_AXI_awlock 	 ,
    M00_AXI_awprot 	 => S02_AXI_awprot 	 ,
    M00_AXI_awqos 	 => S02_AXI_awqos 	 ,
    M00_AXI_awready  => S02_AXI_awready  ,
    M00_AXI_awregion => S02_AXI_awregion ,
    M00_AXI_awsize 	 => S02_AXI_awsize 	 ,
    M00_AXI_awvalid  => S02_AXI_awvalid  ,
                        
	M00_AXI_bready 	 => S02_AXI_bready 	 ,
    M00_AXI_bresp 	 => S02_AXI_bresp 	 ,
    M00_AXI_bvalid 	 => S02_AXI_bvalid 	 ,
                        
	M00_AXI_rdata 	 => S02_AXI_rdata 	 ,
    M00_AXI_rlast 	 => S02_AXI_rlast 	 ,
    M00_AXI_rready 	 => S02_AXI_rready 	 ,
    M00_AXI_rresp 	 => S02_AXI_rresp 	 ,
    M00_AXI_rvalid 	 => S02_AXI_rvalid 	 ,
                        
	M00_AXI_wdata 	 => S02_AXI_wdata 	 ,
    M00_AXI_wlast 	 => S02_AXI_wlast 	 ,
    M00_AXI_wready 	 => S02_AXI_wready 	 ,
    M00_AXI_wstrb 	 => S02_AXI_wstrb 	 ,
    M00_AXI_wvalid 	 => S02_AXI_wvalid 	 

);  



end base;
