-------------------------------------------------------------------------------
-- Project     : Rosta Base PCIe project Ver.9
-- File        : pcie_64b_master_ea.vhd
-- Version     : 7.0
-- Company     : Rosta
-- Engineer    : rkob & urock
-- Create Date : 20.11.2013 
-- Comments    :  
------------------------------------------------------------------------------
-- Revisions:


--		10. AXI Datamover control registers moved to Master Block, user_ip/axi fifo mux added

-- 				TX FIFO is now AXI Steam			

--		9. TX FIFO size extended = 2048 qwords

--      8. Urock: continue to fix ep bugs

--		7 Urock: ep-ep bugs fixing
--
--		6. Urock host speed test: added ReadTimerReg_c and WriteTimerReg_c registers


-- 		5. Bug fix Max_Read_Req_Size & CleanUpCode
--
-- 		4. Bug fix in User IP Interface
-- 
-- 		3. Bug in TX_MSTR_MEM_WR with MasterWrCnt_i - 2

	
--		User Ip reg interface updated - now with rising edge detectors on Reg_WE and Reg_RE signals				

 
--		2. TX MEM WRITE and EP TX MEM WRITE states updated for >=


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library UNISIM;
use UNISIM.VComponents.all;

library work;

entity pcie_64b_master is port (
  -- Common
   user_clk  		:in std_logic; 
   user_reset 		:in std_logic; 
   user_lnk_up	 	:in std_logic; 

  -- AXIS
-- RX
	m_axis_rx_tdata  : in std_logic_vector(63 downto 0);
	m_axis_rx_tkeep  : in std_logic_vector(7 downto 0);
	m_axis_rx_tlast  : in std_logic;
	m_axis_rx_tvalid : in std_logic;
	m_axis_rx_tready : out std_logic; 
	m_axis_rx_tuser  : in std_logic_vector(21 downto 0);
-- TX
	s_axis_tx_tready : in  std_logic;
	s_axis_tx_tdata  : out std_logic_vector(63 downto 0);
	s_axis_tx_tkeep  : out std_logic_vector(7 downto 0);
	s_axis_tx_tlast  : out std_logic;
	s_axis_tx_tvalid : out std_logic;
	tx_buf_av        : in  std_logic_vector(5 downto 0);
	tx_src_dsc  	 : out std_logic;
	tx_err_drop		 : in  std_logic; 
  
  -- Host (CFG) Interface
   cfg_status   	  : in std_logic_vector (15 downto 0); 
   cfg_command  	  : in std_logic_vector (15 downto 0); 
   cfg_dcommand     : in std_logic_vector(15 downto 0);  	
   cfg_completer_id : in std_logic_vector (15 downto 0);

  -- Interrupt
   cfg_interrupt       : out std_logic;
   cfg_interrupt_rdy   : in  std_logic;
   cfg_interrupt_assert: out std_logic;

  -- Turn-Off	 
   cfg_to_turnoff  	:in std_logic; 
   cfg_turnoff_ok 	:out std_logic;
 
-------------------------------------------------------
-- HOST DMA Data 
	-- RX FIFO
	m_user_tvalid	: out std_logic;
	m_user_tready	: in  std_logic;
	m_user_tdata		: out std_logic_vector(63 downto 0); 
	 
	-- TX FIFO
	s_user_tvalid	: in  std_logic;
	s_user_tready		: out std_logic;		
	s_user_tdata		: in  std_logic_vector(63 downto 0);	

	m_axi_tvalid	: out std_logic;
	m_axi_tready	: in  std_logic;
	m_axi_tdata		: out std_logic_vector(63 downto 0); 
	 
	-- TX FIFO
	s_axi_tvalid	: in  std_logic;
	s_axi_tready	: out std_logic;		
	s_axi_tdata		: in  std_logic_vector(63 downto 0);	
	   
--------------------------------------------------- 
-- Stream AXI Bridge Interface Control to Datamover IP
	-- MM2S CMD interface
	StartPulse_mm2s         : out std_logic;
	StartAddrB_mm2s         : out std_logic_vector(31 downto 0);
	BytesToTransfer_mm2s    : out std_logic_vector(22 downto 0);
	CmdTag_mm2s             : out std_logic_vector(3  downto 0);
	CmdAck_mm2s             : in  std_logic;
					
					
	-- S2MM CMD interface
	StartPulse_s2mm         : out std_logic;
	StartAddrB_s2mm         : out std_logic_vector(31 downto 0);
	BytesToTransfer_s2mm    : out std_logic_vector(22 downto 0);
	CmdTag_s2mm             : out std_logic_vector(3  downto 0);
	CmdAck_s2mm             : in  std_logic;
					
	-- AXI Stream Master interface (Read Status from AXI)
	m_axis_s2mm_sts_tvalid  : in  std_logic;
	m_axis_s2mm_sts_tready  : out std_logic;
	m_axis_s2mm_sts_tdata   : in std_logic_vector(7 downto 0);
			
	m_axis_mm2s_sts_tvalid  : in  std_logic;
	m_axis_mm2s_sts_tready  : out std_logic;
	m_axis_mm2s_sts_tdata   : in  std_logic_vector(7 downto 0);
           


---------------------------------------------------
-- User IP interface

	ip_clk		    : in  std_logic;
	user_rst_n		: out std_logic;	

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
	ep_tx_cmd_wr_en		: in  std_logic;
	ep_tx_cmd_data		: in  std_logic_vector(63 downto 0);	-- (addr_32_bits & length_24_bits & tag_8_bits)
	ep_tx_cmd_full		: out std_logic;  
	
	ep_tx_error			: out std_logic;	
	ep_tx_tag_error		: out std_logic_vector(7 downto 0); 
	
	ep_txd_s_tvalid		: in  std_logic;
	ep_txd_s_tdata		: in  std_logic_vector(63 downto 0);
	ep_txd_s_tready		: out std_logic;
	
	-- EP rx interface	
	ep_rxd_m_tvalid	: out std_logic;
	ep_rxd_m_tready : in  std_logic;
	ep_rxd_m_tdata	: out std_logic_vector(63 downto 0);
	
	InitDoneBottom     :in std_logic;
    InitDoneTop        :in std_logic;
    use_AXI            :out std_logic

  );
end entity pcie_64b_master;
	
architecture pci_exp_64b_master of pcie_64b_master is

-- Version 
constant	RostaPCIeEngineVersion	: std_logic_vector(7 downto 0) := X"09"; 

component reg_synch is
    Port ( 
			rst_n		: in  std_logic; 
			
			d1_clk 		: in  std_logic;
			d1_we 		: in  std_logic;
			d1_addr 	: in  std_logic_vector ( 7 downto 0);
			d1_dout 	: in  std_logic_vector (31 downto 0);
			d1_re 		: in  std_logic;
			d1_din 		: out std_logic_vector (31 downto 0);
			d1_valid 	: out std_logic;
			d1_ready	: in  std_logic;
			
			d2_clk 		: in  std_logic;
			d2_we 		: out std_logic;
			d2_addr 	: out std_logic_vector ( 7 downto 0);
			d2_dout 	: out std_logic_vector (31 downto 0);
			d2_re 		: out std_logic;
			d2_din 		: in  std_logic_vector (31 downto 0);
			d2_valid 	: in  std_logic
		);
end component;
    
component cmd_fifo
  PORT (
    rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC
  );
end component;

component fifo_1k_32  
port (
	din: in std_logic_vector(31 downto 0);
	clk: in std_logic;
	rd_en: in std_logic;
	rst: in std_logic;
	wr_en: in std_logic;
	dout: out std_logic_vector(31 downto 0);
	empty: out std_logic;
	full: out std_logic
	);
end component;

component rx_fifo
  port (
    s_aclk : in std_logic;
    s_aresetn : in std_logic;
    s_axis_tvalid : in std_logic;
    s_axis_tready : out std_logic;
    s_axis_tdata : in std_logic_vector(63 downto 0);
    m_axis_tvalid : out std_logic;
    m_axis_tready : in std_logic;
    m_axis_tdata : out std_logic_vector(63 downto 0);
    axis_prog_full : OUT STD_LOGIC  	
  );
end component;


component ep_rx_fifo
  port (
    s_aclk : in std_logic;
    s_aresetn : in std_logic;
    s_axis_tvalid : in std_logic;
    s_axis_tready : out std_logic;
    s_axis_tdata : in std_logic_vector(63 downto 0);
    m_aclk : in std_logic;
    m_axis_tvalid : out std_logic;
    m_axis_tready : in std_logic;
    m_axis_tdata : out std_logic_vector(63 downto 0);
    axis_prog_full : OUT STD_LOGIC  	
  );
end component;


--component tx_fifo
--	port(
--		 rst 				: in std_logic;
--		 wr_clk 			: in std_logic;
--		 rd_clk 			: in std_logic;
--		 din 				: in std_logic_vector(63 downto 0);
--		 wr_en 				: in std_logic;
--		 rd_en 				: in std_logic;

--		 dout 				: out std_logic_vector(63 downto 0);
--		 full 				: out std_logic;
--		 empty 				: out std_logic;
--		 rd_data_count 		: out std_logic_vector(11 downto 0);
--		 prog_full 			: out std_logic
--		);
--end component;

component tx_fifo
  PORT (
    s_aclk : IN STD_LOGIC;
    s_aresetn : IN STD_LOGIC;
    s_axis_tvalid : IN STD_LOGIC;
    s_axis_tready : OUT STD_LOGIC;
    s_axis_tdata : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
    m_axis_tvalid : OUT STD_LOGIC;
    m_axis_tready : IN STD_LOGIC;
    m_axis_tdata : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
    axis_data_count : OUT STD_LOGIC_VECTOR(11 DOWNTO 0)
  );
END component;


component ep_tx_fifo
  PORT (
    m_aclk : IN STD_LOGIC;
    s_aclk : IN STD_LOGIC;
    s_aresetn : IN STD_LOGIC;
    s_axis_tvalid : IN STD_LOGIC;
    s_axis_tready : OUT STD_LOGIC;
    s_axis_tdata : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
    m_axis_tvalid : OUT STD_LOGIC;
    m_axis_tready : IN STD_LOGIC;
    m_axis_tdata : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
    axis_wr_data_count : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
    axis_rd_data_count : OUT STD_LOGIC_VECTOR(11 DOWNTO 0)
  );
END component;



component arbiter is
port (
    clk,  rst  :in  std_logic;
    req0, req1 :in  std_logic;
    req2, req3 :in  std_logic;
    gnt0, gnt1 :out std_logic;
    gnt2, gnt3 :out std_logic
);
end component;

-- AXIS Clock Convertion Buffer (Distributed RAM 16 words depth)
component axis_cc_buf
  port (
    s_aresetn : in STD_LOGIC;
    s_aclk : in STD_LOGIC;
    s_axis_tvalid : in STD_LOGIC;
    s_axis_tready : out STD_LOGIC;
    s_axis_tdata : in STD_LOGIC_VECTOR ( 63 downto 0 );
    m_aclk : in STD_LOGIC;
    m_axis_tvalid : out STD_LOGIC;
    m_axis_tready : in STD_LOGIC;
    m_axis_tdata : out STD_LOGIC_VECTOR ( 63 downto 0 )
  );
end component; 

-----------------------------------------------------
-- Local signals 
-----------------------------------------------------
-- TX Arbiter
signal req_wr_host, req_rd_host, req_wr_ep, req_dummy	: std_logic;
signal gnt_wr_host, gnt_rd_host, gnt_wr_ep, gnt_dummy	: std_logic;

signal txd_data_ready   : std_logic; 

-- One clock cycle ticks, indicating acknowledge to gnt_i signals. Used to deassert current req_i signal, to allow next req_i take over 
signal start_mem_wr		: std_logic; 	
signal start_mem_rd		: std_logic; 	
signal start_ep_wr		: std_logic; 

-- BAR0 signals
signal bar0_reg_dout	: std_logic_vector(31 downto 0);
signal bar0_reg_addr	: std_logic_vector( 7 downto 0);
signal bar0_reg_we		: std_logic;
signal bar0_reg_re		: std_logic;
signal bar0_reg_din		: std_logic_vector(31 downto 0);
signal bar0_reg_valid	: std_logic;

signal frst		: std_logic;
signal clk, rst_n  : std_logic;
signal trn_pending : std_logic;

-----------------------------------------------------
-- Local signals -- PCI Express RX/TX
-----------------------------------------------------
-- RX TLP Packet
  signal req_fmt   : std_logic_vector(6 downto 0);  -- Fmt[1:0] and Type[4:0] Field Encodings
  signal req_tc    : std_logic_vector(2 downto 0);  -- TC
  signal req_td    : std_logic;                     -- TD 
  signal req_ep    : std_logic;                     -- EP 
  signal req_attr  : std_logic_vector(1 downto 0);  -- Attribute
  signal req_len_c : std_logic_vector(9 downto 0);  -- Length (1DW)
  signal req_rid   : std_logic_vector(15 downto 0); -- Requestor ID
  signal req_tag   : std_logic_vector(7 downto 0);  -- Tag
  signal req_be_c  : std_logic_vector(7 downto 0);  -- Request Byte Enables
  signal req_addr  : std_logic_vector(31 downto 0); -- Request Address (31..2)& "00"
  signal cmpl_id   : std_logic_vector(15 downto 0); -- Completer ID
  signal cmpl_sts  : std_logic_vector(2 downto 0);  -- Completion Status (000=Successfull)
  signal byte_cnt  : std_logic_vector(11 downto 0); -- Byte Count

  signal req_len_v : std_logic_vector(9 downto 0);  -- Length (1DW) for Temp
  signal req_len_tx: std_logic_vector(9 downto 0);  -- Length (1DW) for TX
  
  signal read_req_len_dw	: std_logic_vector(10 downto 0); -- Length for generated reads requests

-----------------------------------------------------
-- TLP Header
-----------------------------------------------------
signal  req_hdr1: std_logic_vector(31 downto 0);
signal  req_hdr2: std_logic_vector(31 downto 0);
signal  req_hdr3: std_logic_vector(31 downto 0);
signal  req_data: std_logic_vector(31 downto 0);

-----------------------------------------------------
-- TLP Format and Type Encoding Constant
-----------------------------------------------------
-- Fmt[1:0] and Type[4:0] Field Encodings
constant RX_MEM_RD32_FMT_TYPE : std_logic_vector(6 downto 0) := "0000000"; -- Memory Read Request 32
constant RX_MEM_WR32_FMT_TYPE : std_logic_vector(6 downto 0) := "1000000"; -- Memory Write Request 32
constant RX_MEM_RD64_FMT_TYPE : std_logic_vector(6 downto 0) := "0100000"; -- Memory Read Request 64
constant RX_MEM_WR64_FMT_TYPE : std_logic_vector(6 downto 0) := "1100000"; -- Memory Write Request 64
constant RX_IO_RD32_FMT_TYPE  : std_logic_vector(6 downto 0) := "0000010"; -- I/O Read Request
constant RX_IO_WR32_FMT_TYPE  : std_logic_vector(6 downto 0) := "1000010"; -- I/O Write Request
constant TX_CPLD_FMT_TYPE     : std_logic_vector(6 downto 0) := "1001010"; -- CplD - Completion with Data
constant TX_CPL_FMT_TYPE      : std_logic_vector(6 downto 0) := "0001010"; -- Cpl  - Completion without Data

signal MAX_READ_REQUEST	: integer range 0 to 4096;	-- bytes
constant MAX_PAYLOAD_SIZE	: integer := 128; 		-- bytes
signal MAX_PAYLOAD_SIZE_DW	: std_logic_vector (7 downto 0);  		

-----------------------------------------------------
-- RX State Machine
-----------------------------------------------------
type rx_state_type is (	RX_RST_STATE,
                    	RX_MEM_RD_DW1,
                    	RX_MEM_WR_DW1,
						RX_MEM_WR_DW2,
                    	RX_MEM_WR_FIFO,
						RX_CPLD,
						RX_WAIT_COMPL,
						RX_MEM_WR);

signal rx_state     : rx_state_type;

-----------------------------------------------------
-- TX State Machine
-----------------------------------------------------
type tx_state_type is (TX_RST_STATE,
						  TX_CPLD_DW1,
						  TX_INIT_EP_MEM_WR,
						  TX_EP_MEM_WR,
						  TX_INIT_MEM_WR,
						  TX_MSTR_MEM_WR,
						  TX_INIT_MEM_RD,
						  TX_MSTR_MEM_RD);

signal tx_state     : tx_state_type;

-----------------------------------------------------
signal bar_sel    : std_logic_vector(2 downto 0);
signal compl_done : std_logic;
signal req_compl  : std_logic;
signal cfg_interrupt_c : std_logic;
signal cfg_interrupt_assert_c : std_logic; 

signal byte_count  : std_logic_vector(11 downto 0);
signal lower_addr  : std_logic_vector(6 downto 0);
signal rd_be_int   : std_logic_vector(3 downto 0);
signal ReadReg     : std_logic_vector(31 downto 0);

-----------------------------------------------------
-- WR_FIFO
signal wr_addr_fifo_wr_en : std_logic;
signal wr_addr_fifo_rd_en : std_logic;
signal wr_addr_fifo_empty : std_logic;
signal wr_addr_fifo_full  : std_logic;
signal wr_addr : std_logic_vector(31 downto 0);
signal wr_cnt  : std_logic_vector(31 downto 0);

-- RD_FIFO
signal rd_addr_fifo_wr_en : std_logic;
signal rd_addr_fifo_rd_en : std_logic;
signal rd_addr_fifo_empty : std_logic;
signal rd_addr_fifo_full  : std_logic;
signal rd_addr : std_logic_vector(31 downto 0);
signal rd_cnt	: std_logic_vector(31 downto 0); 	

-- RD_CNT
signal rd_cnt_fifo_empty	: std_logic;
signal rd_cnt_fifo_full		: std_logic;
signal rd_cnt_fifo_rd_en	: std_logic;
signal rd_cnt_fifo_wr_en	: std_logic;

-- WR_CNT
signal wr_cnt_fifo_rd_en	: std_logic;
signal wr_cnt_fifo_wr_en	: std_logic;
signal wr_cnt_fifo_full		: std_logic;
signal wr_cnt_fifo_empty	: std_logic;

-- RXD_FIFO
signal dma_we   : std_logic;
signal dma_we_1 : std_logic;
signal dma_wd   : std_logic_vector(63 downto 0);
signal rxd_pfull: std_logic;
signal rxd_full : std_logic;
	
-- TXD_FIFO
signal dma_re   : std_logic;
signal dma_rd   : std_logic_vector(63 downto 0);
signal txd_data_count : std_logic_vector(11 downto 0); 

--signal txd_data_count_full : std_logic_vector(11 downto 0); 


-----------------------------------------------------
-- PCIe Master Reg's
-----------------------------------------------------
--- BAR0 region
constant MasterVersionReg : std_logic_vector(11 downto 0):=X"000"; -- RO
constant MasterControlReg : std_logic_vector(11 downto 0):=X"004"; 

constant MasterWrAddrReg : std_logic_vector(11 downto 0):=X"014";
constant MasterRdAddrReg : std_logic_vector(11 downto 0):=X"018";
constant MasterWrCntReg  : std_logic_vector(11 downto 0):=X"01C";
constant MasterRdCntReg  : std_logic_vector(11 downto 0):=X"020";
constant MasterModeReg   : std_logic_vector(11 downto 0):=X"024";
constant MasterTestReg   : std_logic_vector(11 downto 0):=X"028";
constant DebugReg        : std_logic_vector(11 downto 0):=X"030";

constant UserControlReg  : std_logic_vector(11 downto 0):=X"044";

constant WriteTimerReg 	 : std_logic_vector(11 downto 0):=X"048";
constant ReadTimerReg 	 : std_logic_vector(11 downto 0):=X"04C";

-- debug registers
constant MasterMemRdTotalCnt 	 : std_logic_vector(11 downto 0):=X"050";
constant MasterEpTxTotalCnt 	 : std_logic_vector(11 downto 0):=X"054";
constant MasterEpRxTotalCnt 	 : std_logic_vector(11 downto 0):=X"058";
constant MasterMemWrTotalCnt 	 : std_logic_vector(11 downto 0):=X"05C";

constant FifoMemRdTotalCnt 	 : std_logic_vector(11 downto 0):=X"060";
constant FifoEpTxTotalCnt 	 : std_logic_vector(11 downto 0):=X"064";
constant FifoEpRxTotalCnt 	 : std_logic_vector(11 downto 0):=X"068";
constant FifoMemWrTotalCnt 	 : std_logic_vector(11 downto 0):=X"06C";

constant MasterTxdDataCount 	 : std_logic_vector(11 downto 0):=X"070";

-- AXI Datamover control registers
-- S2MM
constant DmRxAddr		 	: std_logic_vector(7 downto 0) := X"74"; 
constant DmRxLength			: std_logic_vector(7 downto 0) := X"78"; 
constant DmRxStatus			: std_logic_vector(7 downto 0) := X"7C"; 
 
-- MM2S
constant DmTxAddr		 	: std_logic_vector(7 downto 0) := X"80"; 
constant DmTxLength	  		: std_logic_vector(7 downto 0) := X"84"; 
constant DmTxStatus	  		: std_logic_vector(7 downto 0) := X"88"; 



-- as measured by pcie state machine
signal MasterReadCnt_c  : std_logic_vector(31 downto 0); -- Master Host memory Read DWORDs Count Complete
signal MasterEpTxCnt_c  : std_logic_vector(31 downto 0); -- Master EP TX DWORDs Count Complete
signal MasterEpRxCnt_c  : std_logic_vector(31 downto 0); -- Master EP RX DWORDs Count Complete
signal MasterWriteCnt_c  : std_logic_vector(31 downto 0); -- Master Host memory Write DWORDs Count Complete 

-- as measured by fifo reads and writes
signal FifoMemRdTotalCnt_c	 : std_logic_vector(31 downto 0);
signal FifoEpTxTotalCnt_c	 : std_logic_vector(31 downto 0);
signal FifoEpRxTotalCnt_c	 : std_logic_vector(31 downto 0);
signal FifoMemWrTotalCnt_c	 : std_logic_vector(31 downto 0);

signal MasterTxdDataCount_c 	 : std_logic_vector(31 downto 0);


constant UserRegRegion	 : std_logic_vector( 3 downto 0):="0001"; 	-- correspods to (19 downto 15)

-- DWORD SWAP BYTE function : ( big-endian -> little-endian )
-- host_rd(31 downto 0):= trn_rd(7 downto 0)& trn_rd(15 downto 8)& trn_rd(23 downto 16)& trn_rd(31 downto 24); 
-----------
--trn_rd[63:32] = s_axis_rx_rdata[31:0]
--trn_rd[31:0]  = s_axis_rx_rdata[63:32]
-----------

signal MasterControlReg_c   : std_logic_vector(31 downto 0); 
-- MasterControlReg_c(0) -- max_read_request_size(2)
-- MasterControlReg_c(1) -- max_read_request_size(1)
-- MasterControlReg_c(2) -- max_read_request_size(0)
-- MasterControlReg_c(4) -- '1' - reset all fifo's 
-- MasterControlReg_c(5) -- axi bus access. '1' - DMA access from to internal AXI bus. '0' - to User IP
signal axi_access		: std_logic;


signal UserControlReg_c   : std_logic_vector(31 downto 0); -- User part control reg
---- 0 -- UserControlReg(24) -- reset user part while reconfiguration. active low
---- 1 -- UserControlReg(25) -- payload selector; 0 - 128, 1 - 256 bytes
---- 2 -- UserControlReg(26) -- 
---- 3 -- UserControlReg(27) -- 

signal MasterModeReg_c   : std_logic_vector(31 downto 0); -- Master Mode Reg
-- DWORD SWAP BYTE function : ( big-endian -> little-endian )
-- host_rd(31 downto 0):= trn_rd(7 downto 0)& trn_rd(15 downto 8)& trn_rd(23 downto 16)& trn_rd(31 downto 24); 
---- 0 -- MasterModeReg_c(24) -- Request  Master Write to DMA
---- 1 -- MasterModeReg_c(25) -- Complete Master Write
---- 2 -- MasterModeReg_c(26) -- Request Master Read from DMA
---- 3 -- MasterModeReg_c(27) -- Complete Master Read
---- 4 -- MasterModeReg_c(28) -- Master Memory Write Init Error
---- 5 -- MasterModeReg_c(29) -- Master Read Request Error
---- 6 -- MasterModeReg_c(30) -- Interrupt Error
---- 7 -- MasterModeReg_c(31) -- Timeout Error
--
---- 8 -- MasterModeReg_c(16) -- DUAL_DMA 
---- 9 -- MasterModeReg_c(17) -- Master Write Address FIFO Full 
---- 10-- MasterModeReg_c(18) -- Master Write Address FIFO Empty
---- 11 - MasterModeReg_c(19) -- Master Read Address FIFO Full 
---- 12 - MasterModeReg_c(20) -- Master Read Address FIFO Empty
---- 13 - MasterModeReg_c(21) -- nu (not used)
---- 14 - MasterModeReg_c(22) -- nu 
---- 15 - MasterModeReg_c(23) -- nu
--
---- 24 -- MasterModeReg_c(0) -- nu
---- 25 -- MasterModeReg_c(1) -- nu
---- 26 -- MasterModeReg_c(2) -- TX_INIT_EP_MEM_WR_ERROR
---- 27 -- MasterModeReg_c(3) -- TX_EP_MEM_WR_ERROR
---- 28 -- MasterModeReg_c(4) -- init_rd_error
---- 29 -- MasterModeReg_c(5) -- Master Memory Write Error
---- 30 -- MasterModeReg_c(6) -- WaitAnswer
---- 31 -- MasterModeReg_c(7) -- nu





signal WriteTimerReg_c   : std_logic_vector(31 downto 0); 
signal ReadTimerReg_c : std_logic_vector(31 downto 0);
signal write_timer_en		: std_logic;									   
signal read_timer_en		: std_logic;	

--
signal MasterWrAddrReg_c : std_logic_vector(31 downto 0); -- Master Write Address 
signal MasterRdAddrReg_c : std_logic_vector(31 downto 0); -- Master Read Address 

signal MasterWrCntReg_c  : std_logic_vector(31 downto 0); -- Master Write Count (Lenght Packet + Count Packet)
-- MasterWrCntReg_c[31:24] - packet length in DWORDS
-- MasterWrCntReg_c[22: 0] - number of packets for one address

signal MasterRdCntReg_c  : std_logic_vector(31 downto 0); -- Master Read Count  (transfer size in bytes for one address)
signal MasterTestReg_c   : std_logic_vector(31 downto 0); -- Test Reg 
signal MasterWrAddrReg_i : std_logic_vector(31 downto 0); -- Master Write Address 
signal MasterRdAddrReg_i : std_logic_vector(31 downto 0); -- Master Read Address 
signal MasterWrCntReg_i  : std_logic_vector(31 downto 0); -- Master Write Count (Lenght Packet + Count Packet)
signal MasterRdCntReg_i  : std_logic_vector(13 downto 0); -- Master Read Count  (transfer size in bytes for one address)

signal MasterWrCnt_i     : std_logic_vector(7 downto 0);  -- Master Write Lenght Packet (DWORDS)
signal DebugReg_c    	 : std_logic_vector(31 downto 0); -- Debug Register 

signal BAR0_WR_DATA		: std_logic_vector(31 downto 0); 
signal BAR0_WR_ADDR		: std_logic_vector(31 downto 0); 

signal mstr_req_tag  : std_logic_vector(7 downto 0); -- Master Reg Tag
signal mstr_req_tag_c: std_logic_vector(7 downto 0); -- Master Reg Tag Copy

signal temp32bit     : std_logic_vector(31 downto 0); -- temp 32-bit data
signal temp32bit_rx  : std_logic_vector(31 downto 0); -- temp 32-bit data



signal MasterReadCnt_inc	: std_logic; 
signal MasterReadCnt_cmp	: std_logic; 
--

signal pcie_int_req_tick	: std_logic; 
signal pcie_int_req_d		: std_logic; 
signal user_reg_rd		    : std_logic; 	-- read from  Registers indicator 

signal WaitAnswer       : std_logic; -- Master Wait Answer 
signal WaitAnswerOneReq : std_logic; 
signal wait_rxd_full_deassert	: std_logic;

signal AnswerCntForOneReq	: std_logic_vector(10 downto 0); -- 11 bits to hold 1024
signal dual_dma			: std_logic;
signal init_rd_error	: std_logic;

signal in_packet_q : std_logic;
signal sop : std_logic;

signal axis_rx_tready : std_logic;
signal s_axis_tready : std_logic; 

signal ep_tx_cmd_empty	: std_logic;
signal ep_tx_cmd_dout	: std_logic_vector(63 downto 0);	-- (addr_32_bits & length_24_bits & tag_8_bits)
signal ep_tx_cmd_rd_en	: std_logic;  
		

signal pcie_int_req_s   : std_logic;  -- internal signal 

-------------------------------------------------------------------------------------------------------------------
-- EP to EP
signal ep_write_ep		: std_logic; 		-- ep write ep transfer is going indicator
signal ep_tx_address	: std_logic_vector(31 downto 0);
signal ep_tx_length		: std_logic_vector(23 downto 0);
signal ep_tx_tag		: std_logic_vector(7 downto 0);

signal EpCurTxCntDw		: std_logic_vector(7 downto 0); 	-- EP writes EP: Dwords Counter in current packet
signal EpCurTxCntDw_d	: std_logic_vector(7 downto 0); 	-- EP writes EP: Dwords Counter in current packet
signal EpWrCntReg_i		: std_logic_vector(23 downto 0);	-- EP writes EP: Write counter for one address (dwords)
signal ep_temp32bit		: std_logic_vector(31 downto 0);
signal EpWrAddrReg_i	: std_logic_vector(31 downto 0);


signal ep_txd_re			: std_logic; 
signal ep_txd_dout			: std_logic_vector(63 downto 0);
signal ep_txd_data_count	: std_logic_vector(11 downto 0);

--signal ep_txd_data_count_full	: std_logic_vector( 11 downto 0);

signal ep_txd_m_tvalid			: std_logic; 


signal ep_rxd_we			: std_logic;  				
signal ep_rxd_we_1			: std_logic; 
signal ep_rxd_full			: std_logic; 
signal ep_s_axis_tready		: std_logic; 
signal ep_rxd_din			: std_logic_vector(63 downto 0);
signal wait_ep_rxd_full_deassert	: std_logic; 

signal EpWrTagReg_i			: std_logic_vector(7 downto 0);
signal ep_temp32bit_rx		: std_logic_vector(31 downto 0);
-------------------------------------------------------------------------------------------------------------------

signal frst_n	: std_logic;	
signal bar0_reg_din_rd_en	: std_logic; 
signal read_answer_error	: std_logic;

-- pipe -- rkob
signal tx_buf_av_d : std_logic_vector(5 downto 0);
signal user_lnk_up_d : std_logic; 


signal	TX_INIT_EP_MEM_WR_ERROR		: std_logic;
signal	TX_EP_MEM_WR_ERROR			: std_logic;



signal rx_state_dbg	: std_logic_vector(3 downto 0);
signal tx_state_dbg	: std_logic_vector(3 downto 0);


------------------------------------------------------------------
-- Datamover and Stream AXI Bridge Section

signal s2mm_start_p			: std_logic; 
signal mm2s_start_p			: std_logic; 
signal s2mm_cmd_tag, mm2s_cmd_tag	: std_logic_vector(3 downto 0); 

-- Master Block Registers
signal DmRxAddr_c				: std_logic_vector(31 downto 0); -- transfer address on AXI bus
signal DmRxLength_c 			: std_logic_vector(31 downto 0); -- transfer length in bytes
signal DmRxStatus_c 			: std_logic_vector(31 downto 0); -- transfer status						RO
-- RxStatus_c(8)	: status valid	

signal DmTxAddr_c				: std_logic_vector(31 downto 0); -- transfer address on AXI bus
signal DmTxLength_c 			: std_logic_vector(31 downto 0); -- transfer length in bytes
signal DmTxStatus_c 			: std_logic_vector(31 downto 0); -- transfer status						RO
-- TxStatus_c(8)	: status valid

signal DmRxStatus_clr		: std_logic;
signal DmTxStatus_clr		: std_logic;


-- RX FIFO
signal m_rxd_tvalid		: std_logic;
signal m_rxd_tready		: std_logic;
signal m_rxd_tdata		: std_logic_vector(63 downto 0); 
	 
-- TX FIFO
signal s_txd_tvalid		: std_logic;
signal s_txd_tready		: std_logic;		
signal s_txd_tdata		: std_logic_vector(63 downto 0);	

signal m_txd_tvalid		: std_logic;

signal m_user_tdata_i		: std_logic_vector(63 downto 0);
signal m_user_tvalid_i		: std_logic;
signal m_user_tready_i		: std_logic;

signal s_user_tdata_i		: std_logic_vector(63 downto 0);
signal s_user_tvalid_i		: std_logic;
signal s_user_tready_i		: std_logic;



attribute KEEP : string;

attribute KEEP of EpWrCntReg_i			: signal is "TRUE";
attribute KEEP of EpCurTxCntDw			: signal is "TRUE";
attribute KEEP of tx_state				: signal is "TRUE";
attribute KEEP of rx_state				: signal is "TRUE";
  
attribute KEEP of tx_buf_av_d			: signal is "TRUE";

attribute KEEP of s_axis_tx_tready		: signal is "TRUE";
attribute KEEP of s_axis_tx_tvalid		: signal is "TRUE";
attribute KEEP of s_axis_tx_tdata		: signal is "TRUE";



attribute KEEP of dma_we	: signal is "TRUE";
attribute KEEP of dma_re	: signal is "TRUE";

attribute KEEP of dma_wd	: signal is "TRUE";
attribute KEEP of dma_rd	: signal is "TRUE";
attribute KEEP of MasterModeReg_c	: signal is "TRUE";
attribute KEEP of txd_data_count	: signal is "TRUE";
attribute KEEP of MasterWrCnt_i		: signal is "TRUE";
attribute KEEP of MasterWrAddrReg_i	: signal is "TRUE";
attribute KEEP of MasterWrCntReg_i	: signal is "TRUE";



attribute KEEP of rx_state_dbg	: signal is "TRUE";
attribute KEEP of tx_state_dbg	: signal is "TRUE";

attribute KEEP of MasterReadCnt_c	: signal is "TRUE";
attribute KEEP of MasterEpTxCnt_c	: signal is "TRUE";
attribute KEEP of MasterEpRxCnt_c	: signal is "TRUE";
attribute KEEP of MasterWriteCnt_c	: signal is "TRUE";

attribute KEEP of FifoMemRdTotalCnt_c	: signal is "TRUE";
attribute KEEP of FifoEpTxTotalCnt_c	: signal is "TRUE";
attribute KEEP of FifoEpRxTotalCnt_c	: signal is "TRUE";
attribute KEEP of FifoMemWrTotalCnt_c	: signal is "TRUE";


--  attribute KEEP of MasterModeReg_c  : signal is "TRUE";
--  attribute KEEP of MasterRdAddrReg_i: signal is "TRUE";
--  attribute KEEP of MasterRdCntReg_i : signal is "TRUE";
--  attribute KEEP of WaitAnswer       : signal is "TRUE";
--  attribute KEEP of WaitAnswerOneReq       : signal is "TRUE";

--  attribute KEEP of rx_state 	: signal is "TRUE";
--  attribute KEEP of tx_state	 : signal is "TRUE";  
--  attribute KEEP of tx_buf_av	 : signal is "TRUE";  
--  attribute KEEP of rd_addr_fifo_rd_en : signal is "TRUE";
--  attribute KEEP of rd_addr_fifo_empty : signal is "TRUE";
--  attribute KEEP of dual_dma : signal is "TRUE";
--  attribute KEEP of read_req_len_dw : signal is "TRUE";  
--  attribute KEEP of AnswerCntForOneReq : signal is "TRUE";
--  attribute KEEP of MasterReadCnt_inc : signal is "TRUE";      
--  attribute KEEP of MasterReadCnt_cmp : signal is "TRUE";      
   
--  attribute KEEP of DebugReg_c : signal is "TRUE";      
  
  begin 


MAX_PAYLOAD_SIZE_DW <= 	"00100000" when UserControlReg_c(25) = '0' else 
						"01000000"; 		-- for EP2EP

-----------------------------------------------------------------------------------------------
	-- PIPE	--
-----------------------------------------------------------------------------------------------
  
-- Link-Up	--
  process(clk)
  begin
      if rising_edge(clk) then
        user_lnk_up_d <= user_lnk_up;
      end if;
  end process; 
  
  -- register input/output signals
  process(ip_clk)
  begin
      if rising_edge(ip_clk) then
          pcie_int_req_s    <= pcie_int_req;
      end if;
  end process; 
  
-----------------------------------------------------------------------------------------------
	-- Turn-Off controller	--
-----------------------------------------------------------------------------------------------
  -- Check if completion is pending
  --
  process begin
    wait until rising_edge(clk);
    if (rst_n = '0') then
      trn_pending    <= '0';
    else
      if ((trn_pending = '0') and (req_compl = '1')) then
        trn_pending  <= '1';
      elsif (compl_done = '1') then
        trn_pending  <= '0';
      end if;
    end if;
  end process;

  --  Turn-off OK if requested and no transaction is pending
  process begin
    wait until rising_edge(clk);
    if (rst_n = '0') then
      cfg_turnoff_ok  <= '0';
    else
      if ((cfg_to_turnoff = '1') and (trn_pending = '0')) then
        cfg_turnoff_ok <= '1';
      else
        cfg_turnoff_ok <= '0';
      end if;
    end if;
  end process;

-----------------------------------------------------------------------------------------------
	-- Interrupt controller	--
-----------------------------------------------------------------------------------------------
cfg_interrupt <= not cfg_interrupt_c;
cfg_interrupt_assert <= not cfg_interrupt_assert_c;

pcie_int_req_tick <= pcie_int_req_s and (not pcie_int_req_d); 

dual_dma	<= MasterModeReg_c(16); 
use_AXI     <=MasterModeReg_c(5);
-----------------------------------------------------------------------------------------------
	-- Reset & Clocking	--
-----------------------------------------------------------------------------------------------
clk <= user_clk;
rst_n  <= user_lnk_up_d and not user_reset;
user_rst_n <= UserControlReg_c(24); 

-----------------------------------------------------------------------------------------------
	-- PCI Express EP	--
-----------------------------------------------------------------------------------------------
-- Create logic to determine when a packet starts
sop <= not(in_packet_q) and m_axis_rx_tvalid;

process begin
	wait until rising_edge(clk);
		if (rst_n = '0') then
          in_packet_q    <= '0';
		elsif (m_axis_rx_tvalid = '1' and axis_rx_tready = '1' and m_axis_rx_tlast = '1') then
			in_packet_q    <= '0';
		elsif (sop = '1' and axis_rx_tready = '1') then
			in_packet_q    <= '1';
		end if;
end process;

-- Calculate byte count based on byte enable
rd_be_int  <=  req_be_c(3 downto 0);
tx_src_dsc <= '0';

process(rd_be_int)
begin
  case  rd_be_int(3 downto 0) is
    when X"9" => byte_count <= X"004";
    when X"B" => byte_count <= X"004";
    when X"D" => byte_count <= X"004";
    when X"F" => byte_count <= X"004";
    when X"5" => byte_count <= X"003";
    when X"7" => byte_count <= X"003";
    when X"A" => byte_count <= X"003";
    when X"E" => byte_count <= X"003";
    when X"3" => byte_count <= X"002";
    when X"6" => byte_count <= X"002";
    when X"C" => byte_count <= X"002";
    when X"1" => byte_count <= X"001";
    when X"2" => byte_count <= X"001";
    when X"4" => byte_count <= X"001";
    when X"8" => byte_count <= X"001";
    when X"0" => byte_count <= X"001";
    when others => byte_count <= X"001";
  end case;
end process;

-- Calculate lower address based on  byte enable
process(rd_be_int, req_addr)
begin
   if (rd_be_int(0) = '1') then          -- when "---1"
      lower_addr <= req_addr(6 downto 2) & "00";
   elsif (rd_be_int(1) = '1') then       -- when "--10"
      lower_addr <= req_addr(6 downto 2) & "01";
   elsif (rd_be_int(2) = '1') then       -- when "-100"
      lower_addr <= req_addr(6 downto 2) & "10";
   elsif (rd_be_int(3) = '1') then       -- when "1000"
      lower_addr <= req_addr(6 downto 2) & "11";
   else                                    -- when "0000"
      lower_addr <= req_addr(6 downto 2) & "00";
   end if;
end process;
-----------------------------------------------------------------------------------------------

m_axis_rx_tready <= axis_rx_tready; 


-----------------------------------------------------------------------------------------------
process (clk, rst_n) is





variable TimeOut_rx: integer range 0 to 16#8FFFF#:=0;
variable TimeOut_tx: integer range 0 to 16#FFFF#:=0;
variable TimeOut_tx_wr: integer range 0 to 16#FFFFF#:=0;
variable IntTimeOut_v: integer range 0 to 16#FFFF#:=0;
variable TimeOut_26	: integer range 0 to 4096; 
variable Mode_v: integer range 0 to 8;
variable Mode_rx_ep: integer range 0 to 1;

variable TimeOut_Init_TxEp: integer range 0 to 16#FFFF#:=0;







begin

  if (rst_n = '0' ) then


      req_tc     <= (others => '0');
      req_td     <= '0';
      req_ep     <= '0';
      req_attr   <= (others => '0');
      req_len_c  <= (others => '0');
      req_len_v  <= (others => '0');
      req_len_tx <= (others => '0');
      req_rid    <= (others => '0');
      req_tag    <= (others => '0');
      req_be_c   <= (others => '0');
      req_addr   <= (others => '0');
      bar_sel    <= (others => '0');
      req_fmt    <= (others => '1');
      req_compl  <= '0';


      wr_addr_fifo_wr_en <= '0';
      wr_addr_fifo_rd_en <= '0';
      rd_addr_fifo_wr_en <= '0';
      rd_addr_fifo_rd_en <= '0';

      
      wr_cnt_fifo_rd_en	 <= '0';
      wr_cnt_fifo_wr_en	 <= '0';
      rd_cnt_fifo_rd_en	 <= '0';
      rd_cnt_fifo_wr_en	 <= '0'; 

		
      dma_wd    <= (others => '0'); -- 64-bits
      dma_re    <= '0';


      axis_rx_tready   <= '0';
      s_axis_tx_tdata  <= (others => '0'); -- 64-bits
      s_axis_tx_tkeep  <= X"FF";
      compl_done       <= '0';
      user_reg_rd		<= '0';	-- read from  Registers indicator 


      cfg_interrupt_c <= '1';


      TimeOut_tx := 0;


      TimeOut_rx := 0;

      IntTimeOut_v := 0;
      Mode_v:= 0;
	  Mode_rx_ep := 0; 

	  
      req_hdr1  <= (others => '0');
      req_hdr2  <= (others => '0');
      req_hdr3  <= (others => '0');
      req_data  <= (others => '0');

      
      MasterWrAddrReg_c <= (others => '0');
      MasterRdAddrReg_c <= (others => '0');
      MasterWrAddrReg_i <= (others => '0');
      MasterRdAddrReg_i <= (others => '0');
      MasterWrCntReg_c  <= (others => '0');
      MasterRdCntReg_c  <= (others => '0');
      MasterWrCntReg_i  <= (others => '0');
      MasterRdCntReg_i  <= (others => '0');

      MasterModeReg_c   <= (others => '0');
      ReadReg           <= (others => '0');
      MasterTestReg_c   <= (others => '0');

      
      MasterReadCnt_c   <= (others => '0');
      MasterEpTxCnt_c   <= (others => '0');
      MasterEpRxCnt_c   <= (others => '0');
      MasterWriteCnt_c   <= (others => '0');


      FifoMemRdTotalCnt_c   <= (others => '0');
      FifoEpTxTotalCnt_c   <= (others => '0');
      FifoEpRxTotalCnt_c   <= (others => '0');
      FifoMemWrTotalCnt_c   <= (others => '0');

      		
      WaitAnswer       <= '0';
      WaitAnswerOneReq <= '0';
      mstr_req_tag   <= (others => '0');
      mstr_req_tag_c <= (others => '0');
      temp32bit      <= (others => '0');
      AnswerCntForOneReq <= (others => '0'); 


      rx_state    <= RX_RST_STATE;
      tx_state    <= TX_RST_STATE;

      
      pcie_int_compl		<= '0';

      
      BAR0_WR_DATA		<= (others => '0');
      BAR0_WR_ADDR		<= (others => '0');
      UserControlReg_c	<= (others => '0');

      
      MasterReadCnt_inc	<= '0';
      MasterReadCnt_cmp	<= '0';

      
    wait_rxd_full_deassert	<= '0';
    wait_ep_rxd_full_deassert <= '0';

		
	dma_we   <= '0';
	dma_we_1 <= '0';


	TimeOut_26	:= 0; 
	init_rd_error <= '0'; 

		
	bar0_reg_dout	<= (others => '0');
	bar0_reg_addr	<= (others => '0');
	bar0_reg_we		<= '0';
	bar0_reg_re		<= '0'; 		


		
		
	EpCurTxCntDw	<= (others => '0'); 
	EpWrCntReg_i	<= (others => '0'); 
	EpWrTagReg_i	<= (others => '0');

		
		ep_rxd_we	<= '0'; 
		ep_rxd_we_1	<= '0';		

		
		ep_write_ep	<= '0';

		
		start_mem_wr	<= '0';
		start_mem_rd	<= '0';
		start_ep_wr		<= '0';			

		
		bar0_reg_din_rd_en	<= '0'; 

		
		DebugReg_c	<= (others => '0'); 

		
		read_answer_error	<= '0';

		
		MasterControlReg_c(2 downto 0)	<= "101"; -- 4k 
		MasterControlReg_c(4)	<= '0'; 

		
		WriteTimerReg_c	<= (others => '0');
		ReadTimerReg_c	<= (others => '0');

		
		write_timer_en	<= '0';
		read_timer_en	<= '0';				

		
		TX_INIT_EP_MEM_WR_ERROR	<= '0';
		TX_EP_MEM_WR_ERROR	<= '0';
		
		txd_data_ready    <= '0';

		s2mm_cmd_tag		<= (others => '0');
		mm2s_cmd_tag		<= (others => '0');

		s2mm_start_p	<= '0';
		mm2s_start_p	<= '0'; 
		
		DmTxStatus_clr	<= '0';
		DmRxStatus_clr	<= '0';
		
		
		DmRxAddr_c 	<= (others => '0'); 
		DmRxLength_c 	<= (others => '0'); 
		DmRxStatus_c	<= (others => '0'); 
		
		DmTxAddr_c 	<= (others => '0'); 
		DmTxLength_c 	<= (others => '0'); 
		DmTxStatus_c	<= (others => '0'); 		
		

  else

    if (clk'event and clk = '1') then

    
    	case MasterControlReg_c(2 downto 0) is
    	
    		when "000" => MAX_READ_REQUEST	<= 128;
    		when "001" => MAX_READ_REQUEST	<= 256;
    		when "010" => MAX_READ_REQUEST	<= 512;
    		when "011" => MAX_READ_REQUEST	<= 1024;
    		when "100" => MAX_READ_REQUEST	<= 2048;
    		when "101" => MAX_READ_REQUEST	<= 4096;
    		 
    		when others => 	MAX_READ_REQUEST <= 128; 
    	
    	end case; 



	 
	    -- rkob 
	    tx_buf_av_d <= tx_buf_av;

		-- For control Address FIFO State
		MasterModeReg_c(17) <= wr_addr_fifo_full;  -- Master Write Address FIFO Full 
		MasterModeReg_c(18) <= wr_addr_fifo_empty; -- Master Write Address FIFO Empty
		MasterModeReg_c(19) <= rd_addr_fifo_full;  -- Master Read Address FIFO Full 
		MasterModeReg_c(20) <= rd_addr_fifo_empty; -- Master Read Address FIFO Empty
		MasterModeReg_c(22) <= InitDoneBottom;
		MasterModeReg_c(23) <= InitDoneTop;

		-- cfq change urock
		pcie_int_req_d	<= pcie_int_req_s; 
		
		if pcie_int_req_tick = '1' then 
		
			MasterModeReg_c(8) 		<= '1'; 
			cfg_interrupt_c 		<= '0';
			cfg_interrupt_assert_c 	<= '0';
			pcie_int_compl			<= '0';			-- reset interrupt complete status
			
		end if;

		if (MasterModeReg_c(25)='1' or MasterModeReg_c(27)='1' -- DMA mode Complete
					or  MasterModeReg_c(8)='1') 	-- int_USER_IP
						and (cfg_interrupt_c='0') and (cfg_interrupt_assert_c='0')  then 

			if cfg_interrupt_rdy = '1' then -- ready for send interrupt 
				cfg_interrupt_c <= '1'; -- Interrupt Disable 
				IntTimeOut_v := 0;
			else
				if IntTimeOut_v < 16#3FFFFFF# then 
					IntTimeOut_v := IntTimeOut_v + 1;
				else -- timeout!!!
					IntTimeOut_v := 0;
					cfg_interrupt_c <= '1'; 
					MasterModeReg_c(30) <= '1'; -- Interrupt Error
				end if;
			end if;

		elsif (MasterModeReg_c(25)='0') and (MasterModeReg_c(27)='0') and (MasterModeReg_c(8)='0')
				 and (cfg_interrupt_c='1') and (cfg_interrupt_assert_c='0') then
			cfg_interrupt_c <= '0';
			cfg_interrupt_assert_c <= '1';

		elsif (cfg_interrupt_c='0') and (cfg_interrupt_assert_c='1') then
			
			if cfg_interrupt_rdy = '1' then
				cfg_interrupt_c <= '1'; -- Interrupt Disable 
				pcie_int_compl		<= '1';	-- interrupt is serviced
				IntTimeOut_v := 0;
			else
				if IntTimeOut_v < 16#3FFFFFF# then 
					IntTimeOut_v := IntTimeOut_v + 1;
				else -- timeout!!!
					IntTimeOut_v := 0;
					cfg_interrupt_c <= '1'; 
					MasterModeReg_c(30) <= '1'; -- Interrupt Error
				end if;
			end if;				
		end if; 
		
-----------------------------------------------------
-- RX State Machine
-----------------------------------------------------


		dma_we	<= dma_we_1; 
		ep_rxd_we	<= ep_rxd_we_1; 
			
      case (rx_state) is

        when RX_RST_STATE =>
		  
			dma_we	<= '0'; 
			dma_we_1	<= '0'; 
			
			ep_rxd_we	<= '0'; 
			ep_rxd_we_1	<= '0';
          
         	axis_rx_tready <= '1';
         	req_compl      <= '0';

			wr_addr_fifo_wr_en <= '0';
			rd_addr_fifo_wr_en <= '0';
			wr_cnt_fifo_wr_en  <= '0';
			rd_cnt_fifo_wr_en  <= '0';
			 
			TimeOut_rx := 0;


			
			bar0_reg_we		<= '0';
			

			if (axis_rx_tready = '1') then
			
				if ((m_axis_rx_tvalid = '1') and (sop = '1')) then -- RSOF

					req_fmt <= m_axis_rx_tdata(30 downto 24);

                case (m_axis_rx_tdata(30 downto 24)) is

						when RX_MEM_RD32_FMT_TYPE =>
       
							axis_rx_tready <= '0';
							req_tc    <= m_axis_rx_tdata(22 downto 20);
							req_td    <= m_axis_rx_tdata(15);
							req_ep    <= m_axis_rx_tdata(14);
							req_attr  <= m_axis_rx_tdata(13 downto 12);
							req_rid   <= m_axis_rx_tdata(63 downto 48);
							req_tag   <= m_axis_rx_tdata(47 downto 40);
							req_be_c  <= m_axis_rx_tdata(39 downto 32);
							bar_sel   <= m_axis_rx_tuser(4 downto 2);

							if (m_axis_rx_tdata(9 downto 0) = "0000000001") then
								req_len_c <= m_axis_rx_tdata(9 downto 0);
								rx_state  <= RX_MEM_RD_DW1; -- need to read DW
							end if;						

						when RX_MEM_WR32_FMT_TYPE =>

							req_len_c <= m_axis_rx_tdata(9 downto 0);
							bar_sel   <= m_axis_rx_tuser(4 downto 2);

							if (m_axis_rx_tdata(9 downto 0) = "0000000001") then
								axis_rx_tready <= '0';
								rx_state     	<= RX_MEM_WR_DW1;
							else
							
								MasterEpRxCnt_c	<= MasterEpRxCnt_c +  m_axis_rx_tdata(9 downto 0); 
								
								if ep_rxd_full = '0' then
									axis_rx_tready 	<= '1';
									rx_state  		<= RX_MEM_WR;
								else
									axis_rx_tready 				<= '0';
									wait_ep_rxd_full_deassert 	<= '1'; 
								end if; 
							
							end if;						
  
						when TX_CPLD_FMT_TYPE =>
       
							req_tc    <= m_axis_rx_tdata(22 downto 20);
							req_td    <= m_axis_rx_tdata(15);
							req_ep    <= m_axis_rx_tdata(14);
							req_attr  <= m_axis_rx_tdata(13 downto 12);
							req_len_c <= m_axis_rx_tdata(9 downto 0);
							cmpl_id   <= m_axis_rx_tdata(63 downto 48);
							cmpl_sts  <= m_axis_rx_tdata(47 downto 45);
							byte_cnt  <= m_axis_rx_tdata(43 downto 32);
							bar_sel   <= m_axis_rx_tuser(4 downto 2);
							
							if rxd_full = '0' then
								axis_rx_tready <= '1';
								rx_state  		<= RX_CPLD;
							else
								axis_rx_tready 			<= '0';
								wait_rxd_full_deassert 	<= '1'; 
							end if; 

						when others => rx_state <= RX_RST_STATE;

					end case;
			
				end if;

			 end if; -- (axis_rx_tready = '1')
			 
			if wait_rxd_full_deassert = '1' then
				axis_rx_tready	<= '0';
				if rxd_full = '0' then
					axis_rx_tready <= '1';
					rx_state  		<= RX_CPLD;
					wait_rxd_full_deassert	<= '0';
				end if;		
				
			elsif wait_ep_rxd_full_deassert = '1' then
				axis_rx_tready	<= '0';
				if ep_rxd_full = '0' then
					axis_rx_tready			<= '1';
					rx_state  				<= RX_MEM_WR;
					wait_ep_rxd_full_deassert	<= '0';
				end if;				
			end if;						

        when RX_MEM_RD_DW1 =>

          if (m_axis_rx_tvalid = '1') then

            axis_rx_tready <= '0';
            req_compl    	<= '1';
				rx_state <= RX_WAIT_COMPL; -- wait compl_dw_done = '1'

				if (bar_sel(0)= '1') then -- BAR0 - Read Master Mode reg's

					case m_axis_rx_tdata(11 downto 0) is

						when MasterVersionReg =>
							ReadReg  <= RostaPCIeEngineVersion & X"000000";

						when MasterControlReg =>
							ReadReg  <= MasterControlReg_c(7 downto 0)& MasterControlReg_c(15 downto 8)&
										MasterControlReg_c(23 downto 16)& MasterControlReg_c(31 downto 24); 

						when MasterRdAddrReg =>
							ReadReg  <= MasterRdAddrReg_c(7 downto 0)& MasterRdAddrReg_c(15 downto 8)&
												MasterRdAddrReg_c(23 downto 16)& MasterRdAddrReg_c(31 downto 24); 

						when MasterWrAddrReg => 
							ReadReg  <= MasterWrAddrReg_c(7 downto 0)& MasterWrAddrReg_c(15 downto 8)&
												MasterWrAddrReg_c(23 downto 16)& MasterWrAddrReg_c(31 downto 24); 							

						when MasterRdCntReg => 
							ReadReg  <= MasterRdCntReg_c(7 downto 0)& MasterRdCntReg_c(15 downto 8)&
												MasterRdCntReg_c(23 downto 16)& MasterRdCntReg_c(31 downto 24); 

						when MasterWrCntReg => 
							ReadReg  <= MasterWrCntReg_c(7 downto 0)& MasterWrCntReg_c(15 downto 8)&
												MasterWrCntReg_c(23 downto 16)& MasterWrCntReg_c(31 downto 24); 

						when MasterModeReg => -- not need swap bytes
							ReadReg  <= MasterModeReg_c;
							
						when MasterTestReg =>
							ReadReg  <=  MasterTestReg_c(7 downto 0)& MasterTestReg_c(15 downto 8)&
												MasterTestReg_c(23 downto 16)& MasterTestReg_c(31 downto 24); 

						when UserControlReg => ReadReg 	<= UserControlReg_c; 	
						
						when WriteTimerReg =>
							ReadReg  <=  WriteTimerReg_c(7 downto 0)& WriteTimerReg_c(15 downto 8)&
    									WriteTimerReg_c(23 downto 16)& WriteTimerReg_c(31 downto 24);

						when ReadTimerReg =>
    						ReadReg  <=  ReadTimerReg_c(7 downto 0)& ReadTimerReg_c(15 downto 8)&
    									ReadTimerReg_c(23 downto 16)& ReadTimerReg_c(31 downto 24);							
						
						when DebugReg =>
							ReadReg 	<= DebugReg_c(7 downto 0)& DebugReg_c(15 downto 8)&
												DebugReg_c(23 downto 16)& DebugReg_c(31 downto 24); 	
												
						when MasterMemRdTotalCnt =>
							ReadReg 	<= 	MasterReadCnt_c(7 downto 0)& MasterReadCnt_c(15 downto 8)&
											MasterReadCnt_c(23 downto 16)& MasterReadCnt_c(31 downto 24); 		

						when MasterEpTxTotalCnt =>
							ReadReg 	<= 	MasterEpTxCnt_c(7 downto 0)& MasterEpTxCnt_c(15 downto 8)&
											MasterEpTxCnt_c(23 downto 16)& MasterEpTxCnt_c(31 downto 24); 												

						when MasterEpRxTotalCnt =>
							ReadReg 	<= 	MasterEpRxCnt_c(7 downto 0)& MasterEpRxCnt_c(15 downto 8)&
											MasterEpRxCnt_c(23 downto 16)& MasterEpRxCnt_c(31 downto 24); 	
											
						when MasterMemWrTotalCnt =>
							ReadReg 	<= 	MasterWriteCnt_c(7 downto 0)& MasterWriteCnt_c(15 downto 8)&
											MasterWriteCnt_c(23 downto 16)& MasterWriteCnt_c(31 downto 24); 	
					

						when FifoMemRdTotalCnt =>
							ReadReg 	<= 	FifoMemRdTotalCnt_c(7 downto 0)& FifoMemRdTotalCnt_c(15 downto 8)&
											FifoMemRdTotalCnt_c(23 downto 16)& FifoMemRdTotalCnt_c(31 downto 24); 	
											
						when FifoEpTxTotalCnt =>
							ReadReg 	<= 	FifoEpTxTotalCnt_c(7 downto 0)& FifoEpTxTotalCnt_c(15 downto 8)&
											FifoEpTxTotalCnt_c(23 downto 16)& FifoEpTxTotalCnt_c(31 downto 24); 	
											

						when FifoEpRxTotalCnt =>
							ReadReg 	<= 	FifoEpRxTotalCnt_c(7 downto 0)& FifoEpRxTotalCnt_c(15 downto 8)&
											FifoEpRxTotalCnt_c(23 downto 16)& FifoEpRxTotalCnt_c(31 downto 24); 	
											

						when FifoMemWrTotalCnt =>
							ReadReg 	<= 	FifoMemWrTotalCnt_c(7 downto 0)& FifoMemWrTotalCnt_c(15 downto 8)&
											FifoMemWrTotalCnt_c(23 downto 16)& FifoMemWrTotalCnt_c(31 downto 24); 	

	
                        when MasterTxdDataCount   =>
                            ReadReg     <= MasterTxdDataCount_c(7 downto 0) & MasterTxdDataCount_c(15 downto 8) &
                                            MasterTxdDataCount_c(23 downto 16) & MasterTxdDataCount_c(31 downto 24);
						
						when DmRxAddr   =>
                            ReadReg     <= DmRxAddr_c(7 downto 0) & DmRxAddr_c(15 downto 8) &
                                            DmRxAddr_c(23 downto 16) & DmRxAddr_c(31 downto 24);
						
                        when DmRxLength   =>
                            ReadReg     <= DmRxLength_c(7 downto 0) & DmRxLength_c(15 downto 8) &
                                            DmRxLength_c(23 downto 16) & DmRxLength_c(31 downto 24);
											
                        when DmRxStatus   =>
                            ReadReg     <= DmRxStatus_c(7 downto 0) & DmRxStatus_c(15 downto 8) &
                                            DmRxStatus_c(23 downto 16) & DmRxStatus_c(31 downto 24);
							if DmRxStatus_c(8) = '1' then
								DmRxStatus_clr	<= '1';
							end if; 	

											
                        when DmTxAddr   =>
                            ReadReg     <= DmTxAddr_c(7 downto 0) & DmTxAddr_c(15 downto 8) &
                                            DmTxAddr_c(23 downto 16) & DmTxAddr_c(31 downto 24);

                        when DmTxLength   =>
                            ReadReg     <= DmTxLength_c(7 downto 0) & DmTxLength_c(15 downto 8) &
                                            DmTxLength_c(23 downto 16) & DmTxLength_c(31 downto 24);
											
                        when DmTxStatus   =>
                            ReadReg     <= DmTxStatus_c(7 downto 0) & DmTxStatus_c(15 downto 8) &
                                            DmTxStatus_c(23 downto 16) & DmTxStatus_c(31 downto 24);											
							if DmTxStatus_c(8) = '1' then
								DmTxStatus_clr	<= '1';
							end if; 
							

						when others => -- Read DW1 to tx_fifo
							ReadReg  <=  X"BEBEBEBE";
					end case;
					
					if m_axis_rx_tdata(11 downto 8) = UserRegRegion then -- accessing  Regs region 

						bar0_reg_addr	<= m_axis_rx_tdata(7 downto 0); -- 8 bits
						bar0_reg_re		<= '1';
						user_reg_rd		<= '1';  
						
					end if; 
					
				end if;

          else

            rx_state    <= RX_MEM_RD_DW1;

          end if;


			when RX_MEM_WR_DW1 =>

				if (m_axis_rx_tvalid = '1') then

					axis_rx_tready <= '1';
				
					rx_state     <= RX_RST_STATE;

					if (bar_sel(0)= '1') then -- BAR0 - Write Master Mode reg's
					
						BAR0_WR_DATA	<= m_axis_rx_tdata(63 downto 32);
						BAR0_WR_ADDR	<= m_axis_rx_tdata(31 downto 0);
						rx_state	<= RX_MEM_WR_DW2; 

					end if;
				end if;
			
			when RX_MEM_WR_DW2 => -- pipeline state fow writting to BAR0 region
			
				rx_state     <= RX_RST_STATE;
			
				case BAR0_WR_ADDR(11 downto 0) is 
				
					when MasterControlReg =>
						MasterControlReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
										 		BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 
												
					when MasterRdAddrReg =>
						MasterRdAddrReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
													BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 
						rd_addr_fifo_wr_en <= '1';
						
					when MasterWrAddrReg => 
						MasterWrAddrReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
													BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 
						wr_addr_fifo_wr_en <= '1';

					when MasterRdCntReg => 
						MasterRdCntReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
													BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 
						rd_cnt_fifo_wr_en <= '1';

					when MasterWrCntReg => 
						MasterWrCntReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
													BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 
						wr_cnt_fifo_wr_en <= '1';
							
					when MasterTestReg =>
						MasterTestReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
													BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 

					when MasterModeReg => MasterModeReg_c 	<= BAR0_WR_DATA(31 downto 0);
							
					when UserControlReg =>	UserControlReg_c	<= BAR0_WR_DATA(31 downto 0);
					
					when WriteTimerReg =>
						WriteTimerReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
    									   BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 							   

					when ReadTimerReg =>
						ReadTimerReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
										  BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 					 
						
					when DebugReg =>
							DebugReg_c <= BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 			
											 
					when MasterMemRdTotalCnt =>
							MasterReadCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when MasterEpTxTotalCnt =>
							MasterEpTxCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when MasterEpRxTotalCnt =>
							MasterEpRxCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when MasterMemWrTotalCnt =>
							MasterWriteCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when FifoMemRdTotalCnt =>
							FifoMemRdTotalCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when FifoEpTxTotalCnt =>
							FifoEpTxTotalCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when FifoEpRxTotalCnt =>
							FifoEpRxTotalCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24); 												 			

					when FifoMemWrTotalCnt =>
							FifoMemWrTotalCnt_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24);

					when DmRxAddr =>
							DmRxAddr_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24);												
					when DmRxLength =>
							DmRxLength_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24);
												
							s2mm_start_p	<= '1'; 					
							s2mm_cmd_tag		<= s2mm_cmd_tag + 1; 							


					when DmTxAddr =>
							DmTxAddr_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24);

					when DmTxLength =>
							DmTxLength_c <= 	BAR0_WR_DATA(7 downto 0)& BAR0_WR_DATA(15 downto 8)&
											 	BAR0_WR_DATA(23 downto 16)& BAR0_WR_DATA(31 downto 24);
												
							mm2s_start_p	<= '1';								 
							mm2s_cmd_tag		<= mm2s_cmd_tag + 1; 												

										
					when others => null;

				end case;
					
				if BAR0_WR_ADDR(11 downto 8) = UserRegRegion then -- accessing  Regs region 
				
					bar0_reg_addr	<= BAR0_WR_ADDR(7 downto 0); -- 8 bits
					bar0_reg_we	 	<= '1'; 
					bar0_reg_dout	<= BAR0_WR_DATA( 7 downto  0)	& BAR0_WR_DATA(15 downto  8)&
											BAR0_WR_DATA(23 downto 16)	& BAR0_WR_DATA(31 downto 24); 
											
				end if; 				


			when RX_CPLD => -- Come Answer to Master Memory Read Request -- CplD

				if cmpl_sts = "000" and -- Succesfull Completion
					(m_axis_rx_tdata(31 downto 16) = cfg_completer_id) then -- RequstID

					if (m_axis_rx_tvalid = '1') then

						axis_rx_tready    <= '1';
						MasterReadCnt_inc	<= '1'; 
						dma_we_1			 <= '1'; 
						rx_state        <= RX_MEM_WR_FIFO;
					else
					
						axis_rx_tready <= '0'; -- Stop reading data from reciever
					
						if TimeOut_rx < 16#FFFF# then --8192 then
							TimeOut_rx := TimeOut_rx + 1;
						else -- timeout!!!
							MasterModeReg_c(31) <= '1'; -- Timeout Error
							rx_state        <= RX_RST_STATE;
						end if;
					end if;
				else
							
					axis_rx_tready <= '1';
					rx_state     	<= RX_RST_STATE;
							
					MasterModeReg_c(26)	<= '0';
					read_answer_error	<= '1'; 
				end if;

        when RX_MEM_WR_FIFO => -- Come Answer to Master Memory Read Request / write to fifo
				
			dma_we_1 <= axis_rx_tready; 

           	if m_axis_rx_tvalid = '1' then 
					
				if ((m_axis_rx_tlast = '1') and (axis_rx_tready = '1')) then  -- REOF -- last dword
					rx_state <= RX_RST_STATE;					
				else					
					rx_state <= RX_MEM_WR_FIFO;
				end if;			

				axis_rx_tready <= not rxd_full; 
				
			else
					
				axis_rx_tready	<= '0'; 	-- stop reading data from pcie
				
				if TimeOut_rx < 16#8FFFF# then --8192*16 then
					TimeOut_rx := TimeOut_rx + 1;
				else -- timeout!!!
					MasterModeReg_c(31) <= '1'; -- Timeout Error
	                rx_state   <= RX_RST_STATE;
				end if;
          
			end if;


		when RX_MEM_WR => 
		
			if Mode_rx_ep = 0 then
			
				if (m_axis_rx_tvalid = '1') then
	
					axis_rx_tready  <= '1';
					ep_rxd_we_1		<= '1'; 
					Mode_rx_ep      := 1; 
	
				else
		
					axis_rx_tready <= '0'; 
					-- do we need timeout here?
			  
				end if;			
				
			elsif Mode_rx_ep = 1 then
				
				ep_rxd_we_1 <= axis_rx_tready; 
	
				if m_axis_rx_tvalid = '1' then 
						
					if ((m_axis_rx_tlast = '1') and (axis_rx_tready = '1')) then  -- REOF -- last dword
						rx_state <= RX_RST_STATE;					
					end if;			
	
					axis_rx_tready <= not ep_rxd_full; 
					
				else
						
					axis_rx_tready	<= '0'; 	-- stop reading data from pcie
					
					if TimeOut_rx < 16#8FFFF# then --8192*16 then
						TimeOut_rx := TimeOut_rx + 1;
					else -- timeout!!!
						MasterModeReg_c(31) <= '1'; -- Timeout Error
						rx_state   <= RX_RST_STATE;
					end if;
			  
				end if;				
			
			
			end if;

        when RX_WAIT_COMPL =>
		  
			 bar0_reg_re <= '0'; 		  
		  			
            if (compl_done = '1') then
				req_compl 		<= '0';
                rx_state  <= RX_RST_STATE;
            else
                rx_state  <= RX_WAIT_COMPL;
			end if;

        when others => null;

      end  case;


-----------------------------------------------------
-- TX State Machine
-----------------------------------------------------
	case (tx_state) is

        when TX_RST_STATE =>

          	compl_done   <= '0';
          	dma_re       <= '0';

          	s_axis_tx_tvalid <= '0';
			s_axis_tx_tlast  <= '0';
			s_axis_tx_tdata  <= (others => '0');
			s_axis_tx_tkeep  <= X"FF";

			
			req_hdr1  <= (others => '0');
			req_hdr2  <= (others => '0');
			req_hdr3  <= (others => '0');
			req_data  <= (others => '0');
			TimeOut_tx := 0;
			
			Mode_v:= 0;
			

			if (req_compl = '1') then -- need transmit 1 dw
				
				if user_reg_rd = '1' then
					if bar0_reg_valid = '1' then
					
						bar0_reg_din_rd_en	<= '1'; 
						
						ReadReg 		<= bar0_reg_din(7 downto 0)&bar0_reg_din(15 downto 8)&
											bar0_reg_din(23 downto 16)&bar0_reg_din(31 downto 24);
						user_reg_rd	<= '0';
						tx_state		<= TX_CPLD_DW1;
						
					else
						tx_state <= TX_RST_STATE;
					end if;
				


				else
					tx_state <= TX_CPLD_DW1;

				end if; 

			elsif (gnt_rd_host = '1') and (req_rd_host = '1')then 		--Init_Master_Memory_Read
				start_mem_rd	<= '1';
				tx_state 		<= TX_INIT_MEM_RD;

				read_timer_en 	<= '1'; 				
				
			elsif (gnt_wr_host = '1') and (req_wr_host = '1')then 		-- Init_Master_Memory_Write
				start_mem_wr	<= '1';
				tx_state <= TX_INIT_MEM_WR;
				
				write_timer_en 	<= '1';				
				
			elsif (gnt_wr_ep = '1') and (req_wr_ep = '1') then 		-- Init_EP_Memory_Write_to_EP
				start_ep_wr	<= '1';
				tx_state 	<= TX_INIT_EP_MEM_WR;
				ep_write_ep	<= '1'; 
			else
				tx_state <= TX_RST_STATE;
			end if;	

			
        when TX_CPLD_DW1 =>  -- Completion with Data  --  CPLD  --
        	
        	bar0_reg_din_rd_en	<= '0'; 

          if ((s_axis_tx_tready = '1') and (tx_buf_av_d > "000011") ) then -- CplD

				case Mode_v is
					
					when 0 => -- req_hdr1 & req_hdr2

						s_axis_tx_tlast  <= '0';
						s_axis_tx_tvalid <= '1';

						s_axis_tx_tdata <= cfg_completer_id &
                                    "000" &
                                    '0' &
                                    byte_count &
						                  '0' &
                                    TX_CPLD_FMT_TYPE &
                                    '0' &
                                    req_tc &
                                    "0000" &
                                    req_td &
                                    req_ep &
                                    req_attr &
                                    "00" &
                                    req_len_c;

						s_axis_tx_tkeep <= X"FF";
                  		compl_done      <= '1'; -- DONE
						Mode_v:= 1;

					when 1 => -- first_data & req_hdr3

                  		s_axis_tx_tlast  <= '1'; -- end of packet
                  		s_axis_tx_tvalid <= '1';
						
                  		s_axis_tx_tdata <= ReadReg &
						                   req_rid &
										   req_tag &
										   '0' &
										   lower_addr;
																			
                 	 	s_axis_tx_tkeep <= X"FF";
						tx_state        <= TX_RST_STATE;

					when others => null;

				end case;

          else

            	tx_state         <= TX_CPLD_DW1;
				s_axis_tx_tvalid <= '0'; -- source not ready
				s_axis_tx_tlast  <= '0';
				
				if TimeOut_tx < 16#FFFF# then --8192 then
					TimeOut_tx := TimeOut_tx + 1;
				else -- timeout!!!
					tx_state   <= TX_RST_STATE;
				end if;

			end if;

        when TX_INIT_EP_MEM_WR =>		-- point to point memory write
        
        	start_ep_wr	<= '0';
        	s_axis_tx_tlast  <= '0';
        	s_axis_tx_tvalid <= '0';         	

			if EpWrCntReg_i = X"000000" then -- begin
				
				ep_tx_cmd_rd_en	<= '1'; 
				
				EpWrAddrReg_i  		<= ep_tx_address;  -- load address
				EpWrCntReg_i 		<= ep_tx_length; 	-- reload counter's
				EpWrTagReg_i		<= ep_tx_tag; 
								
				-- calc packet length 
				if (ep_tx_length(23 downto 8) /= X"0000")  then
					EpCurTxCntDw	<= MAX_PAYLOAD_SIZE_DW;
				else
					if (EpWrCntReg_i(7 downto 0) >= MAX_PAYLOAD_SIZE_DW) then
						EpCurTxCntDw	<= MAX_PAYLOAD_SIZE_DW;
					else
						EpCurTxCntDw 	<= ep_tx_length(7 downto 0);
					end if; 
				end if;					
								
				tx_state           <= TX_INIT_EP_MEM_WR;

				
			else 	
			
				ep_tx_cmd_rd_en	<= '0'; 

				if (s_axis_tx_tready = '1') and (tx_buf_av_d > "000011")	and
				 ( (ep_txd_data_count) >= ("00000"&EpCurTxCntDw(7 downto 1) ))  then -- txd_data_count counts QW (8 bytes) 		(10 bits)			


					MasterTestReg_c(7 downto 0)	<= EpCurTxCntDw;
					MasterTestReg_c(17 downto 8)	<=  ep_txd_data_count(9 downto 0); 

					s_axis_tx_tvalid <= '1';
                    s_axis_tx_tdata <= 	cfg_completer_id &
						                "00000000" &
					                    "11111111" &
					                    '0' &
					                    RX_MEM_WR32_FMT_TYPE &
					                    '0' &
					                    "000" &
					                    "0000" &
					                    '0' &
					                    '0' &
					                    "00" &
					                    "00" &
					                    "00" & EpCurTxCntDw;
					                    
					s_axis_tx_tkeep <= X"FF";
					ep_txd_re		<= '1';
					Mode_v:= 1;
					
					EpWrCntReg_i 	<= EpWrCntReg_i - EpCurTxCntDw;	-- update Write counter for one address 
					
					MasterEpTxCnt_c	<= MasterEpTxCnt_c + EpCurTxCntDw; 
							
					EpCurTxCntDw 		<= EpCurTxCntDw - 2;-- data length for one packet (DWORDS)
					TimeOut_tx := 0;
					Mode_v:= 1;
					
					tx_state    <= TX_EP_MEM_WR;					

				else	-- loop
				
										
					tx_state <= TX_RST_STATE;

					if TimeOut_Init_TxEp < 16#FFFF# then
						TimeOut_Init_TxEp	:= TimeOut_Init_TxEp + 1; 
					else
						-- TX_INIT_EP_MEM_WR_ERROR	<= '1'; 
						
						--ep_tx_error		<= '1'; 
						--EpWrCntReg_i	<= (others => '0');		
						
						--ep_write_ep		<= '0';
						
						TimeOut_Init_TxEp	:= 0;
									
					end if;

				
				end if;				
					


			end if;
										

        when TX_EP_MEM_WR =>		-- point to point memory write

			if (s_axis_tx_tready = '1') then
			
				ep_txd_re			<= '1';

				case Mode_v is
					
					when 1 => -- first_data & req_hdr3

						EpCurTxCntDw  <= EpCurTxCntDw - 2;
						s_axis_tx_tdata <= ep_txd_dout(7 downto 0)& ep_txd_dout(15 downto 8)& 
													ep_txd_dout(23 downto 16)& ep_txd_dout(31 downto 24) & EpWrAddrReg_i;
						ep_temp32bit      <= ep_txd_dout(39 downto 32)&ep_txd_dout(47 downto 40)&
													ep_txd_dout(55 downto 48)&ep_txd_dout(63 downto 56); 

						s_axis_tx_tvalid <= '1';
						Mode_v := 2; 


					when 2 =>-- even data

						s_axis_tx_tdata <= ep_txd_dout(7 downto 0)&ep_txd_dout(15 downto 8)&
												  ep_txd_dout(23 downto 16)&ep_txd_dout(31 downto 24) & ep_temp32bit;
						ep_temp32bit      <= ep_txd_dout(39 downto 32)&ep_txd_dout(47 downto 40)&
												 ep_txd_dout(55 downto 48)&ep_txd_dout(63 downto 56); 


						if EpWrCntReg_i = X"000000" then 
							ep_write_ep	 <= '0';	-- end transfer flag
						end if;

						if EpCurTxCntDw = X"00" then
							ep_txd_re 	<= '0';
							Mode_v 		:= 3;
							
						else
							EpCurTxCntDw   <= EpCurTxCntDw - 2;				
						end if;
				
					when 3 => -- last data, finish packet and select new tx state

						s_axis_tx_tlast <= '1';
                  		s_axis_tx_tkeep <= X"0F";
						s_axis_tx_tdata(63 downto 32) <= X"FEED_4C0D";
						s_axis_tx_tdata(31 downto 0)  <= ep_temp32bit;
						Mode_v := 0;
						ep_txd_re			<= '0';
						-- calc new EpCurTxCntDw					
						if (EpWrCntReg_i(23 downto 8) /= X"0000")  then
							EpCurTxCntDw	<= MAX_PAYLOAD_SIZE_DW;
						else
							if (EpWrCntReg_i(7 downto 0) >= MAX_PAYLOAD_SIZE_DW) then
								EpCurTxCntDw	<= MAX_PAYLOAD_SIZE_DW;
							else
								EpCurTxCntDw 	<= EpWrCntReg_i(7 downto 0);
							end if; 
						end if;													
						
						
						-- select new state for TX
						if (req_compl = '1') then -- need transmit 1 dw
							
							if user_reg_rd = '1' then
								if bar0_reg_valid = '1' then
								
									bar0_reg_din_rd_en	<= '1'; 
									
									ReadReg 		<= bar0_reg_din(7 downto 0)&bar0_reg_din(15 downto 8)&
														bar0_reg_din(23 downto 16)&bar0_reg_din(31 downto 24);
									user_reg_rd	<= '0';
									tx_state		<= TX_CPLD_DW1;
									
								else
									tx_state <= TX_RST_STATE;
								end if;
									
							else
								tx_state <= TX_CPLD_DW1;
							end if; 
			
						elsif (gnt_rd_host = '1') and (req_rd_host = '1')then 		--Init_Master_Memory_Read
							start_mem_rd	<= '1';
							tx_state 		<= TX_INIT_MEM_RD;
							
							read_timer_en 	<= '1'; 							
							
						elsif (gnt_wr_host = '1') and (req_wr_host = '1')then 		-- Init_Master_Memory_Write
							start_mem_wr	<= '1';
							tx_state <= TX_INIT_MEM_WR;
							
							write_timer_en 	<= '1'; 							
							
						elsif (gnt_wr_ep = '1') and (req_wr_ep = '1') then 		-- Init_EP_Memory_Write_to_EP
							start_ep_wr	<= '1';
							tx_state 	<= TX_INIT_EP_MEM_WR;
							ep_write_ep	<= '1'; 
						else
							tx_state <= TX_RST_STATE;
						end if;	
			
					when others => null;

				end case;

			else
			
				--s_axis_tx_tvalid <= '0'; -- source not ready
				
				-- ep_txd_re			<= '0';
				
				if TimeOut_tx < 16#FFFF# then --8192 then
					TimeOut_tx := TimeOut_tx + 1;
				else -- timeout!!!
					tx_state <= TX_RST_STATE;
					
					TX_EP_MEM_WR_ERROR	<= '1'; 
					
					ep_tx_error		<= '1'; 
					EpWrCntReg_i	<= (others => '0');
										
				end if;

			end if;


        when TX_INIT_MEM_WR =>
        
        	start_mem_wr	<= '0';
        	s_axis_tx_tlast  <= '0';
        	s_axis_tx_tvalid <= '0';
        	
					if MasterWrCntReg_i(22 downto 0) = X"000000" then -- 1 address complete or begin

						if wr_addr_fifo_empty = '0' then -- not all write address complete
							wr_addr_fifo_rd_en <= '1';
							wr_cnt_fifo_rd_en  <= '1';
							MasterWrAddrReg_i  <= wr_addr;         -- load new address
							MasterWrCntReg_i   <= wr_cnt; 			-- reload counter's
							tx_state           <= TX_INIT_MEM_WR;
						else -- write address fifo empty
							MasterModeReg_c(24)<='0'; -- Request  Master Write to DMA
							MasterModeReg_c(25)<='1'; -- Complete Master Write
							cfg_interrupt_c 		<= '0';
							cfg_interrupt_assert_c <= '0';										
							tx_state           <= TX_RST_STATE;
							write_timer_en 		<= '0';							
						end if;

					else			

						wr_addr_fifo_rd_en <= '0';
						wr_cnt_fifo_rd_en  <= '0';

						req_hdr1    <= '0' &
                              RX_MEM_WR32_FMT_TYPE &
                              '0' &
                              "000" &
                              "0000" &
                              '0' &
                              '0' &
                              "00" &
                              "00" &
                               "00" & MasterWrCntReg_i(31 downto 25)& '0';

						req_hdr2    <= cfg_completer_id &
                              "00000000" &
                              "11111111";

						req_hdr3    <= MasterWrAddrReg_i(31 downto  2) & "00";

						tx_state    <= TX_MSTR_MEM_WR;

						MasterWrAddrReg_i(31 downto 2) <= MasterWrAddrReg_i(31 downto 2) + (MasterWrCntReg_i(31 downto 25) & '0');
						MasterWrCntReg_i(22 downto 0) <= MasterWrCntReg_i(22 downto 0) - '1';-- number of packets for this address

						MasterWrCnt_i <= MasterWrCntReg_i(31 downto 24);-- data length for one packet (DWORDS)
						TimeOut_tx_wr := 0;
						TimeOut_tx    := 0;
						Mode_v:= 0;
						
						MasterWriteCnt_c	<= MasterWriteCnt_c + (MasterWrCntReg_i(31 downto 25)& '0'); 

					end if;

        when TX_MSTR_MEM_WR =>

			if (s_axis_tx_tready = '1') then

				case Mode_v is
					
					when 0 => -- req_hdr1 & req_hdr2

						if (tx_buf_av_d > "000011") and 		-- core has buffer space
						( (txd_data_count) >= ("00000"&MasterWrCnt_i(7 downto 1)) )  then -- txd_data_count counts QW (8 bytes) 					
							MasterWrCnt_i	<= MasterWrCnt_i - 2; 
							s_axis_tx_tlast <= '0';
							s_axis_tx_tvalid <= '1';
                     		s_axis_tx_tdata <= req_hdr2 & req_hdr1;
							s_axis_tx_tkeep <= X"FF";
							dma_re          <= '1';
							Mode_v:= 1;
				
						else	-- loop
						
							if TimeOut_tx_wr < 16#FFFFF# then 
								TimeOut_tx_wr := TimeOut_tx_wr + 1;
							else -- timeout!!!
								MasterModeReg_c(28)<= '1'; -- Master Memory Write Init Error
								
								tx_state <= TX_RST_STATE;
							end if;
						
						end if;

					when 1 => -- req_hdr3 & first_data

							MasterWrCnt_i  <= MasterWrCnt_i - 2;
							s_axis_tx_tdata <= dma_rd(7 downto 0)& dma_rd(15 downto 8)& 
														dma_rd(23 downto 16)& dma_rd(31 downto 24) & req_hdr3;
							temp32bit      <= dma_rd(39 downto 32)&dma_rd(47 downto 40)&
														dma_rd(55 downto 48)&dma_rd(63 downto 56); 
				
							
							Mode_v := 2;

					when 2 =>-- even data

						s_axis_tx_tdata <= dma_rd(7 downto 0)&dma_rd(15 downto 8)&
												  dma_rd(23 downto 16)&dma_rd(31 downto 24) & temp32bit;
						temp32bit      <= dma_rd(39 downto 32)&dma_rd(47 downto 40)&
												 dma_rd(55 downto 48)&dma_rd(63 downto 56); 

						if MasterWrCnt_i = X"00" then
							dma_re <= '0';
							Mode_v := 3;
						else
							MasterWrCnt_i   <= MasterWrCnt_i - 2;
							s_axis_tx_tkeep <= X"FF";					
						end if;
						
					when 3 => -- last data
						
						s_axis_tx_tlast <= '1';
                  		s_axis_tx_tkeep <= X"0F";
						s_axis_tx_tdata(63 downto 32) <= X"FEED_4C0D";
						s_axis_tx_tdata(31 downto 0)  <= temp32bit;
						
						Mode_v := 0;
						tx_state <= TX_RST_STATE;

--					when 4 => -- finish packtet!!!
						
--						s_axis_tx_tlast  <= '0';
--						s_axis_tx_tvalid <= '0';
--						Mode_v := 0;
--						tx_state <= TX_RST_STATE;
						
					when others => null;

				end case;

			else
			
--				dma_re         <= '0';
--				s_axis_tx_tvalid <= '0'; -- source not ready

				if TimeOut_tx < 16#FFFF# then --8192 then
					TimeOut_tx := TimeOut_tx + 1;
				else -- timeout!!!
					tx_state <= TX_RST_STATE;
					
					-- error 
					MasterModeReg_c(5) <= '1'; -- Master Memory Write Error
				end if;

			end if;


        when TX_INIT_MEM_RD =>
        
        	start_mem_rd	<= '0';
        	
        	s_axis_tx_tlast  <= '0';
        	s_axis_tx_tvalid <= '0';        	
		  
					if tx_buf_av_d >= "000011" then -- Core buffer almost full, need wait
					
						if MasterRdCntReg_i  = conv_std_logic_vector(0,14) then -- 1 address complete or begin -- 8k - max range for 1 address

							if rd_addr_fifo_empty = '0' then -- not all read address complete
								rd_addr_fifo_rd_en <= '1';
								rd_cnt_fifo_rd_en  <= '1';
								MasterRdAddrReg_i  <= rd_addr;      			-- load new address
								MasterRdCntReg_i   <= rd_cnt(13 downto 0); 	-- load length in bytes for this transfer

								tx_state           <= TX_INIT_MEM_RD;
																																	
								if rd_cnt(13 downto 0) >= conv_std_logic_vector(MAX_READ_REQUEST,14) then -- more than 4096
									read_req_len_dw <= conv_std_logic_vector(MAX_READ_REQUEST/4,11); 
								else
									read_req_len_dw <= rd_cnt(12 downto 2); 
								end if; 
								
							else
								if WaitAnswer = '0' then  -- All requested packets recieved
									MasterModeReg_c(26)<='0'; -- Request  Master Write to DMA
									init_rd_error	    <= '0'; 
									tx_state           <= TX_RST_STATE;
									
									read_timer_en	<= '0';									

									if dual_dma = '0' then
										MasterModeReg_c(27)<='1'; -- Complete Master Read
										cfg_interrupt_c 	 <= '0'; -- requesting interrupt assert
										cfg_interrupt_assert_c <= '0';									
									end if; 

								end if;
							end if;

						else			
				
							rd_addr_fifo_rd_en <= '0';
							rd_cnt_fifo_rd_en  <= '0';

							req_hdr1    <= '0' &
											RX_MEM_RD32_FMT_TYPE &
											'0' &
											"000" &
											"0000" &
											'0' &
											'0' &
											"00" &
											"00" &
											read_req_len_dw(9 downto 0); -- packet length in DW

							req_hdr2    <= cfg_completer_id &
											mstr_req_tag &
											"11111111";

							req_hdr3      <= MasterRdAddrReg_i(31 downto  2) & "00";

							WaitAnswer    	<= '1';
							WaitAnswerOneReq 	<= '1'; 
							AnswerCntForOneReq <= (others => '0'); -- reset 
							
							mstr_req_tag(4 downto 0)       <= mstr_req_tag(4 downto 0) + 1;
							mstr_req_tag_c(4 downto 0)     <= mstr_req_tag(4 downto 0);
							
							MasterRdAddrReg_i(31 downto 2) <= MasterRdAddrReg_i(31 downto 2) + read_req_len_dw;
							MasterRdCntReg_i(13 downto 2)	 <= MasterRdCntReg_i(13 downto 2) - read_req_len_dw; 

							
							TimeOut_tx := 0;
							Mode_v:= 0;
							TimeOut_26	:= 0; 

							tx_state       <= TX_MSTR_MEM_RD;
							
							read_answer_error	<= '0'; 

						end if;

					else

						tx_state <= TX_RST_STATE;
						
						if TimeOut_26 < 2048 then --8192 then
							TimeOut_26 := TimeOut_26 + 1;
						else -- timeout!!!
							TimeOut_26	:= 0; 
							init_rd_error	<= '1'; 
						end if;						

					end if;

        when TX_MSTR_MEM_RD =>

			if (s_axis_tx_tready = '1') then

				case Mode_v is
					
					when 0 => -- req_hdr1 & req_hdr2

						if (tx_buf_av_d > "000011") then -- loop

							s_axis_tx_tlast <= '0';
							s_axis_tx_tvalid <= '1';
							s_axis_tx_tdata <= req_hdr2 & req_hdr1;
							s_axis_tx_tkeep    <= X"FF";
							Mode_v:= 1;

						else	-- loop
						
							if TimeOut_tx < 16#FFFF# then --8192 then
								TimeOut_tx := TimeOut_tx + 1;
							else -- timeout!!!
								MasterModeReg_c(29)<= '1'; -- Master Read Request Error
								MasterModeReg_c(31)<= '1'; -- Timeout Error
								tx_state <= TX_RST_STATE;
							end if;
						
						end if;
						
					when 1 => -- req_hdr3 & xxxxxxxxxx

				 		s_axis_tx_tdata <= X"FEED_4C0D" & req_hdr3;
						s_axis_tx_tvalid <= '1';
						s_axis_tx_tlast <= '1';
                  s_axis_tx_tkeep <= X"0F";
						Mode_v := 2; 
					
					when 2 =>
						
						s_axis_tx_tlast <= '0';
						s_axis_tx_tvalid <= '0';
						Mode_v := 0;
						tx_state <= TX_RST_STATE; -- wait answer
						
					when others => null;

				end case;

			else
			
				s_axis_tx_tvalid <= '0'; -- source not ready

				if TimeOut_tx < 16#FFFF# then --8192 then
					TimeOut_tx := TimeOut_tx + 1;
				else -- timeout!!!
					tx_state <= TX_RST_STATE;
				end if;

			end if;

        when others => null;

      end  case;
      
      
      
      if (dma_we = '1') and (s_axis_tready = '1') then
      	FifoMemRdTotalCnt_c <= FifoMemRdTotalCnt_c + 2;
      end if; 
      
      if (ep_txd_re = '1') and (ep_txd_m_tvalid = '1') and (s_axis_tx_tready = '1') then
      	FifoEpTxTotalCnt_c <= FifoEpTxTotalCnt_c + 2;
      end if; 
      
      if (ep_rxd_we = '1') and (ep_s_axis_tready = '1') then
      	FifoEpRxTotalCnt_c <= FifoEpRxTotalCnt_c + 2;
      end if; 
      
      if (dma_re = '1') and (m_txd_tvalid = '1') and (s_axis_tx_tready = '1')then
      	FifoMemWrTotalCnt_c <= FifoMemWrTotalCnt_c + 2;
      end if;                   
            
      
      
      if UserControlReg_c(24) = '0' then
      	WriteTimerReg_c <= (others => '0');
   		   	write_timer_en  <= '0';  
    	  elsif write_timer_en = '1' then
			WriteTimerReg_c	<= WriteTimerReg_c + 1;
		  end if;  

		if UserControlReg_c(24) = '0' then
			ReadTimerReg_c <= (others => '0');
			read_timer_en  <= '0';  
		elsif read_timer_en = '1' then
			ReadTimerReg_c	<= ReadTimerReg_c + 1;
		end if;       

		ep_rxd_din    	<=  m_axis_rx_tdata(7 downto 0) &     m_axis_rx_tdata(15 downto 8) &
							m_axis_rx_tdata(23 downto 16)	& m_axis_rx_tdata(31 downto 24) & ep_temp32bit_rx;
							
		ep_temp32bit_rx	<=  m_axis_rx_tdata(39 downto 32) &   m_axis_rx_tdata(47 downto 40)&
							m_axis_rx_tdata(55 downto 48)	& m_axis_rx_tdata(63 downto 56);
      					    


		dma_wd    		<=  m_axis_rx_tdata(7 downto 0) & m_axis_rx_tdata(15 downto 8) &
							m_axis_rx_tdata(23 downto 16)	& m_axis_rx_tdata(31 downto 24) & temp32bit_rx;
							
		temp32bit_rx	 <= m_axis_rx_tdata(39 downto 32) & m_axis_rx_tdata(47 downto 40)&
							m_axis_rx_tdata(55 downto 48)	& m_axis_rx_tdata(63 downto 56);
							


		MasterModeReg_c(4) <= init_rd_error; 
		
		MasterModeReg_c(6) <= WaitAnswer;
		
		MasterModeReg_c(2) <= TX_INIT_EP_MEM_WR_ERROR; 
		MasterModeReg_c(3) <= TX_EP_MEM_WR_ERROR; 
		 
		
													
		-- rx_state	RX_CPLD	RX_MEM_WR_FIFO
	
		if MasterReadCnt_inc = '1' then 			-- RX_CPLD
													--  RX_MEM_WR_FIFO
			-- Read Complete DWORDs counter								
			MasterReadCnt_c 		<= MasterReadCnt_c + req_len_c;
			-- Answer Count for one read request 
			AnswerCntForOneReq 	<= AnswerCntForOneReq + req_len_c;		
			
			MasterReadCnt_inc		<= '0'; 
			MasterReadCnt_cmp		<= '1'; 
		end if; 
		
		if MasterReadCnt_cmp	= '1' then	-- RX_MEM_WR_FIFO

			MasterReadCnt_cmp		<= '0'; 
															
			-- got all answers for one read request
			if AnswerCntForOneReq = read_req_len_dw then 
				WaitAnswerOneReq <= '0';
					
				-- Calc read request size for next transfer 
				if MasterRdCntReg_i >= conv_std_logic_vector(MAX_READ_REQUEST,14) then -- more than 4096
					read_req_len_dw <= conv_std_logic_vector(MAX_READ_REQUEST/4,11); 
				else
					read_req_len_dw <= MasterRdCntReg_i(12 downto 2); 
				end if; 	
			end if; 
			
			if (MasterRdCntReg_i = "00000000000000" ) then 
				WaitAnswer <= '0';
			end if;
					
		end if; 
		
		MasterTxdDataCount_c(9 downto 0)  <= txd_data_count(9 downto 0);
		
		if MasterModeReg_c(25) = '1' then
		  txd_data_ready  <= '0';
		elsif (MasterModeReg_c(24) = '1') and (m_txd_tvalid = '1') then
		  txd_data_ready  <= '1';
		end if;  
		
		
		
		-- Datamover control  section
		
		if s2mm_start_p = '1' then
			s2mm_start_p	<= '0';
		end if;
		
		if mm2s_start_p = '1' then
			mm2s_start_p	<= '0';
		end if; 
		
		if m_axis_s2mm_sts_tvalid = '1' then
			DmRxStatus_c(8 downto 0)	<= '1' & m_axis_s2mm_sts_tdata;
		elsif DmRxStatus_clr = '1' then
			DmRxStatus_c		<= (others => '0');
			DmRxStatus_clr	<= '0';
		end if;				
		
		if m_axis_mm2s_sts_tvalid = '1' then
			DmTxStatus_c(8 downto 0)	<= '1' & m_axis_mm2s_sts_tdata;
		elsif DmTxStatus_clr = '1' then
			DmTxStatus_c		<= (others => '0');
			DmTxStatus_clr	<= '0';
		end if;

		
    end if;	 
	end if;
end process;

-----------------------------------------------------------
---- Address FIFO's 4MB/4kB = 1024 (depth)
---- Cnt, addr Fifo -- FWFT fifo 32x1k based on Built-in FIFO
-----------------------------------------------------------

frst	<= (not rst_n) or (not UserControlReg_c(24)) or MasterControlReg_c(4); 
frst_n	<= rst_n and UserControlReg_c(24) and (not MasterControlReg_c(4));

WR_ADDR_FIFO: fifo_1k_32 -- write address fifo
	port map(
	clk 	 => clk,
	wr_en  => wr_addr_fifo_wr_en,
	din    => MasterWrAddrReg_c,
	rd_en  => wr_addr_fifo_rd_en,
	dout   => wr_addr,
	empty  => wr_addr_fifo_empty,
	full   => wr_addr_fifo_full,
	rst    => frst
	);

RD_ADDR_FIFO: fifo_1k_32 -- read address fifo
	port map(
	clk	 => clk,
	wr_en  => rd_addr_fifo_wr_en,
	din    => MasterRdAddrReg_c,
	rd_en  => rd_addr_fifo_rd_en,
	dout   => rd_addr,
	empty  => rd_addr_fifo_empty,
	full   => rd_addr_fifo_full,
	rst    => frst
	);


RdCntFIFO: fifo_1k_32
	port map (
		clk		=> clk,
		rst		=> frst,
		din		=> MasterRdCntReg_c,
		wr_en		=> rd_cnt_fifo_wr_en,
		rd_en		=> rd_cnt_fifo_rd_en,
		dout		=> rd_cnt,		--  rd_cnt(13 downto 0),
		full		=> rd_cnt_fifo_full,
		empty		=> rd_cnt_fifo_empty
	);

WrCntFIFO: fifo_1k_32
	port map (
		clk		=> clk,
		rst		=> frst,
		din		=> MasterWrCntReg_c,
		wr_en		=> wr_cnt_fifo_wr_en,
		rd_en		=> wr_cnt_fifo_rd_en,
		dout		=> wr_cnt,
		full		=> wr_cnt_fifo_full,
		empty		=> wr_cnt_fifo_empty
	);


-- AXIS FIFO, 64x512, BRAM, full prog 508, valid = not empty

rxd_fifo : rx_fifo
  port MAP (
    s_aresetn 		=> frst_n,
    s_aclk 			=> clk,
    
	 -- write to fifo side
    s_axis_tvalid => dma_we,
    s_axis_tready => s_axis_tready,
    s_axis_tdata 	=> dma_wd,
	 
	 --read from fifo side
    m_axis_tvalid => m_rxd_tvalid,
    m_axis_tready => m_rxd_tready,
    m_axis_tdata 	=> m_rxd_tdata,
	
	axis_prog_full	=> rxd_full    	
	 
  );
 
-- AXIS fifo 64x512
-- slave - write port
-- master - read port 
txd_fifo: tx_fifo
  port map (     
    s_aclk   		=> clk, 
    s_aresetn   	=> frst_n, 
    
    s_axis_tvalid   => s_txd_tvalid, 
    s_axis_tready   => s_txd_tready, 
    s_axis_tdata   	=> s_txd_tdata, 
    
    m_axis_tvalid   => m_txd_tvalid, 
    m_axis_tready   => (dma_re and s_axis_tx_tready), 
    m_axis_tdata   	=> dma_rd, 

    axis_data_count   => txd_data_count
  );
	





user_reg_synch: reg_synch 
    port map( 
			rst_n		=> rst_n,
			
			d1_clk 		=> clk,
			d1_we 		=> bar0_reg_we,
			d1_addr 	=> bar0_reg_addr,
			d1_dout 	=> bar0_reg_dout,
			d1_re 		=> bar0_reg_re,
			d1_din 		=> bar0_reg_din,
			d1_valid 	=> bar0_reg_valid,
			d1_ready	=> bar0_reg_din_rd_en,
			
			d2_clk 		=> ip_clk,
			d2_we 		=> Reg_WE,
			d2_addr 	=> Reg_Addr,
			d2_dout 	=> Reg_Dout,
			d2_re 		=> Reg_RE,
			d2_din 		=> Reg_Din,
			d2_valid 	=> Reg_Valid
		);


-------------------------------------------------------------------------------------------
-- EP to EP

ep_tx_cmd_fifo: cmd_fifo	-- First Word Fall Through fifo, 16x62 bits
port map (
	rst 	=> frst,
	wr_clk 	=> ip_clk,
	rd_clk 	=> clk,
	din 	=> ep_tx_cmd_data,
	wr_en 	=> ep_tx_cmd_wr_en,
	
	rd_en 	=> ep_tx_cmd_rd_en,
	dout 	=> ep_tx_cmd_dout,
	
	full 	=> ep_tx_cmd_full,
	empty 	=> ep_tx_cmd_empty
);	

ep_tx_address	<= ep_tx_cmd_dout(63 downto 32); 
ep_tx_length	<= ep_tx_cmd_dout(31 downto  8);
ep_tx_tag		<= ep_tx_cmd_dout( 7 downto  0); 

-- AXIS fifo 64x512
-- slave - write port
-- master - read port 
ep_txd_fifo: ep_tx_fifo 
  port map (
    m_aclk   		=> clk,        
    s_aclk   		=> ip_clk, 
    s_aresetn   	=> frst_n, 
    
    s_axis_tvalid   => ep_txd_s_tvalid, 
    s_axis_tready   => ep_txd_s_tready, 
    s_axis_tdata   	=> ep_txd_s_tdata, 
    
    m_axis_tvalid   => ep_txd_m_tvalid, 
    m_axis_tready   => (ep_txd_re and s_axis_tx_tready), 
    m_axis_tdata   	=> ep_txd_dout, 
    
    axis_wr_data_count   => open, 
    axis_rd_data_count   => ep_txd_data_count
  );
	



-- AXIS FIFO, 64x1024, BRAM, full prog 1020

ep_rxd_fifo : ep_rx_fifo
  port MAP (
    s_aresetn 		=> frst_n,

	 -- write to fifo side
    s_aclk 			=> clk,
    s_axis_tvalid 	=> ep_rxd_we,
    s_axis_tready 	=> ep_s_axis_tready,
    s_axis_tdata 	=> ep_rxd_din,
	 
	 --read from fifo side
    m_aclk 			=> ip_clk,
    m_axis_tvalid => ep_rxd_m_tvalid,
    m_axis_tready => ep_rxd_m_tready,
    m_axis_tdata 	=> ep_rxd_m_tdata,
	
	axis_prog_full	=> ep_rxd_full    	
  );
	

	
-------------------------------------------------------
-- TX Arbiter
req_wr_host	<= '1' when (MasterModeReg_c(24) = '1' and txd_data_ready = '1' and start_mem_wr = '0') else '0';
req_rd_host	<= '1' when (MasterModeReg_c(26) = '1' and WaitAnswerOneReq = '0' and start_mem_rd = '0') else '0';
req_wr_ep	<= '1' when (((ep_tx_cmd_empty = '0') and (ep_txd_m_tvalid = '1')) or (ep_write_ep = '1')) and start_ep_wr = '0' else '0';
req_dummy	<= '0'; 

robin : arbiter
port map(
    clk  => clk,
    rst  => frst,
    req0 => req_wr_host,
    req1 => req_rd_host,
    req2 => req_wr_ep,
    req3 => req_dummy,
    gnt0 => gnt_wr_host,
    gnt1 => gnt_rd_host,
    gnt2 => gnt_wr_ep,
    gnt3 => gnt_dummy
);


-----------------------------------------------------
-- Internal AXI Bus section

-- AXI Datamover clock = clk = Master 250 MHz

-- ip_clk - input clock from user_ip

axi_access	<= MasterControlReg_c(5); 


	
-- slave - write port
-- master - read port 
m_user_tc: axis_cc_buf
	port map (
    s_aresetn 		=> frst_n, 
	
    s_aclk 			=> clk, 
    s_axis_tvalid 	=> m_user_tvalid_i, 
    s_axis_tready 	=> m_user_tready_i, 
    s_axis_tdata 	=> m_user_tdata_i, 
	
    m_aclk 			=> ip_clk, 
    m_axis_tvalid 	=> m_user_tvalid, 
    m_axis_tready 	=> m_user_tready, 
    m_axis_tdata 	=> m_user_tdata
	
	);
	
-- AXIS Clock Convertion Buffer (Distributed RAM 16 words depth)
-- slave - write port
-- master - read port 
s_user_tc: axis_cc_buf
	port map (
    s_aresetn 		=> frst_n, 
		
    s_aclk 			=> ip_clk, 
    s_axis_tvalid 	=> s_user_tvalid, 
    s_axis_tready 	=> s_user_tready, 
    s_axis_tdata 	=> s_user_tdata, 
	
    m_aclk 			=> clk, 
    m_axis_tvalid 	=> s_user_tvalid_i, 
    m_axis_tready 	=> s_user_tready_i, 
    m_axis_tdata 	=> s_user_tdata_i
	
	);


	
	m_user_tdata_i	<= m_rxd_tdata;
	m_axi_tdata		<= m_rxd_tdata;	

process(axi_access, m_rxd_tvalid, 	s_txd_tready,
					m_user_tready_i, m_axi_tready, 
					s_user_tvalid_i, s_user_tdata_i, s_axi_tvalid, s_axi_tdata)
begin
	
	m_user_tvalid_i	<= '0';
	m_axi_tvalid	<= '0';

	s_user_tready_i	<= '0'; 
	s_axi_tready	<= '0'; 

if axi_access = '0' then


	m_user_tvalid_i	<= m_rxd_tvalid;
	m_rxd_tready	<= m_user_tready_i; 
	
	s_txd_tdata		<= s_user_tdata_i; 
	s_txd_tvalid	<= s_user_tvalid_i; 
	s_user_tready_i	<= s_txd_tready; 

else 


	m_axi_tvalid	<= m_rxd_tvalid;	
	m_rxd_tready	<= m_axi_tready; 
	
	s_txd_tdata		<= s_axi_tdata; 
	s_txd_tvalid	<= s_axi_tvalid; 
	s_axi_tready	<= s_txd_tready; 

end if;

end process;								
	
StartPulse_s2mm			<= s2mm_start_p;
StartAddrB_s2mm 		<= DmRxAddr_c;
BytesToTransfer_s2mm 	<= DmRxLength_c(22 downto 0);
CmdTag_s2mm				<= s2mm_cmd_tag;


StartPulse_mm2s			<= mm2s_start_p;
StartAddrB_mm2s 		<= DmTxAddr_c;
BytesToTransfer_mm2s  	<= DmTxLength_c(22 downto 0);
CmdTag_mm2s				<= mm2s_cmd_tag;




m_axis_mm2s_sts_tready	<= '1'; 
m_axis_s2mm_sts_tready	<= '1'; 




-----------------------------
-- DEBUG Section

process(rx_state)
begin

case rx_state is

    when RX_RST_STATE   => rx_state_dbg   <= "0000";
    when RX_MEM_RD_DW1  => rx_state_dbg   <= "0001";
    when RX_MEM_WR_DW1  => rx_state_dbg   <= "0010";
    when RX_MEM_WR_DW2  => rx_state_dbg   <= "0011";
    when RX_MEM_WR_FIFO => rx_state_dbg   <= "0100";
    when RX_CPLD        => rx_state_dbg   <= "0101";
    when RX_WAIT_COMPL  => rx_state_dbg   <= "0110";
    when RX_MEM_WR      => rx_state_dbg   <= "0111";
    
    when others => rx_state_dbg   <= "1111";
 end case;   
                            
end process;


process(tx_state)
begin

case tx_state is

    when TX_RST_STATE       => tx_state_dbg   <= "0000";
    when TX_CPLD_DW1        => tx_state_dbg   <= "0001";
    when TX_INIT_EP_MEM_WR  => tx_state_dbg   <= "0010";
    when TX_EP_MEM_WR   	 => tx_state_dbg   <= "0011";
    when TX_INIT_MEM_WR     => tx_state_dbg   <= "0100";
    when TX_MSTR_MEM_WR     => tx_state_dbg   <= "0101";
    when TX_INIT_MEM_RD     => tx_state_dbg   <= "0110";
    when TX_MSTR_MEM_RD     => tx_state_dbg   <= "0111";
    
    when others 				 => tx_state_dbg   <= "1111";
end case;    
                            
end process;



end architecture;
