-------------------------------------------------------------------------------
-- Project    : Series-7 Integrated Block for PCI Express
-- File       : pcie_app_7x.vhd
-- Version    : 1.2
-- Description:  PCI Express Endpoint Core sample application design.
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

library work;

entity pcie_app_7x is
port  (
  user_clk                       : in  std_logic;
  user_reset                     : in  std_logic;
  user_lnk_up                    : in  std_logic;

  -- Tx
  tx_buf_av                      : in  std_logic_vector(5 downto 0);
  tx_cfg_req                     : in  std_logic;
  tx_err_drop                    : in  std_logic;
  tx_cfg_gnt                     : out std_logic;

  s_axis_tx_tready               : in  std_logic;
  s_axis_tx_tdata                : out std_logic_vector(63 downto 0);
  s_axis_tx_tkeep                : out std_logic_vector(7 downto 0);
  s_axis_tx_tuser                : out std_logic_vector(3 downto 0);
  s_axis_tx_tlast                : out std_logic;
  s_axis_tx_tvalid               : out std_logic;

  -- RX
  rx_np_ok                       : out std_logic;
  rx_np_req                      : out std_logic;
  m_axis_rx_tdata                : in std_logic_vector(63 downto 0);
  m_axis_rx_tkeep                : in std_logic_vector(7 downto 0);
  m_axis_rx_tlast                : in  std_logic;
  m_axis_rx_tvalid               : in  std_logic;
  m_axis_rx_tready               : out std_logic;
  m_axis_rx_tuser                : in std_logic_vector(21 downto 0);

  -- Flow Control
  fc_cpld                        : in  std_logic_vector(11 downto 0);
  fc_cplh                        : in  std_logic_vector(7 downto 0);
  fc_npd                         : in  std_logic_vector(11 downto 0);
  fc_nph                         : in  std_logic_vector(7 downto 0);
  fc_pd                          : in  std_logic_vector(11 downto 0);
  fc_ph                          : in  std_logic_vector(7 downto 0);
  fc_sel                         : out std_logic_vector(2 downto 0);

 -- CFG

  cfg_err_cor                    : out std_logic;
  cfg_err_ur                     : out std_logic;
  cfg_err_ecrc                   : out std_logic;
  cfg_err_cpl_timeout            : out std_logic;
  cfg_err_cpl_unexpect           : out std_logic;
  cfg_err_cpl_abort              : out std_logic;
  cfg_err_atomic_egress_blocked  : out std_logic;
  cfg_err_internal_cor           : out std_logic;
  cfg_err_malformed              : out std_logic;
  cfg_err_mc_blocked             : out std_logic;
  cfg_err_poisoned               : out std_logic;
  cfg_err_norecovery             : out std_logic;
  cfg_err_acs                    : out std_logic;
  cfg_err_internal_uncor         : out std_logic;
  cfg_pm_halt_aspm_l0s           : out std_logic;
  cfg_pm_halt_aspm_l1            : out std_logic;
  cfg_pm_force_state_en          : out std_logic;
  cfg_pm_force_state             : out std_logic_vector( 1 downto 0);
  cfg_err_posted                 : out std_logic;
  cfg_err_locked                 : out std_logic;
  cfg_err_tlp_cpl_header         : out std_logic_vector(47 downto 0);
  cfg_err_cpl_rdy                : in  std_logic;
  cfg_interrupt                  : out std_logic;
  cfg_interrupt_rdy              : in  std_logic;
  cfg_interrupt_assert           : out std_logic;
  cfg_interrupt_di               : out std_logic_vector( 7 downto 0);
  cfg_interrupt_do               : in  std_logic_vector( 7 downto 0);
  cfg_interrupt_mmenable         : in  std_logic_vector( 2 downto 0);
  cfg_interrupt_msienable        : in  std_logic;
  cfg_interrupt_msixenable       : in  std_logic;
  cfg_interrupt_msixfm           : in  std_logic;
  cfg_turnoff_ok                 : out std_logic;
  cfg_to_turnoff                 : in  std_logic;
  cfg_trn_pending                : out std_logic;
  cfg_pm_wake                    : out std_logic;
  cfg_bus_number                 : in  std_logic_vector( 7 downto 0);
  cfg_device_number              : in  std_logic_vector( 4 downto 0);
  cfg_function_number            : in  std_logic_vector( 2 downto 0);
  cfg_status                     : in  std_logic_vector(15 downto 0);
  cfg_command                    : in  std_logic_vector(15 downto 0);
  cfg_dstatus                    : in  std_logic_vector(15 downto 0);
  cfg_dcommand                   : in  std_logic_vector(15 downto 0);
  cfg_lstatus                    : in  std_logic_vector(15 downto 0);
  cfg_lcommand                   : in  std_logic_vector(15 downto 0);
  cfg_dcommand2                  : in  std_logic_vector(15 downto 0);
  cfg_pcie_link_state            : in  std_logic_vector( 2 downto 0);

  cfg_interrupt_stat             : out std_logic;
  cfg_pciecap_interrupt_msgnum   : out std_logic_vector( 4 downto 0);

  pl_directed_link_change        : out std_logic_vector( 1 downto 0);
  pl_ltssm_state                 : in  std_logic_vector( 5 downto 0);
  pl_directed_link_width         : out std_logic_vector( 1 downto 0);
  pl_directed_link_speed         : out std_logic;
  pl_directed_link_auton         : out std_logic;
  pl_upstream_prefer_deemph      : out std_logic;
  pl_sel_lnk_width               : in  std_logic_vector( 1 downto 0);
  pl_sel_lnk_rate                : in  std_logic;
  pl_link_gen2_cap               : in  std_logic;
  pl_link_partner_gen2_supported : in  std_logic;
  pl_initial_link_width          : in  std_logic_vector( 2 downto 0);
  pl_link_upcfg_cap              : in  std_logic;
  pl_lane_reversal_mode          : in  std_logic_vector( 1 downto 0);
  pl_received_hot_rst            : in  std_logic;

  cfg_err_aer_headerlog          : out std_logic_vector(127 downto 0);
  cfg_aer_interrupt_msgnum       : out std_logic_vector(4 downto 0);
  cfg_err_aer_headerlog_set      : in  std_logic;
  cfg_aer_ecrc_check_en          : in  std_logic;
  cfg_aer_ecrc_gen_en            : in  std_logic;

  cfg_mgmt_di                    : out std_logic_vector(31 downto 0);
  cfg_mgmt_byte_en               : out std_logic_vector( 3 downto 0);
  cfg_mgmt_dwaddr                : out std_logic_vector( 9 downto 0);
  cfg_mgmt_wr_en                 : out std_logic;
  cfg_mgmt_rd_en                 : out std_logic;
  cfg_mgmt_wr_readonly           : out std_logic;

  cfg_dsn                        : out std_logic_vector(63 downto 0);

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
	
    InitDoneBtm     :in std_logic;
    InitDoneTop        :in std_logic;
    use_AXI            :out std_logic		

);
end pcie_app_7x;

architecture pcie_app of pcie_app_7x is

  constant PCI_EXP_EP_OUI      : std_logic_vector(23 downto 0) := x"000A35";
  constant PCI_EXP_EP_DSN_1    : std_logic_vector(31 downto 0) := x"01" & PCI_EXP_EP_OUI;
  constant PCI_EXP_EP_DSN_2    : std_logic_vector(31 downto 0) := x"00000001";

  signal cfg_completer_id       : std_logic_vector(15 downto 0);
  signal cfg_bus_mstr_enable    : std_logic;

component pcie_64b_master
port (
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

end component;

begin

  ------------------------------------------------------------------------------------------------------------------//
  -- PCIe Block EP Tieoffs - Example PIO doesn't support the following inputs                                       //
  ------------------------------------------------------------------------------------------------------------------//

  fc_sel                       <= "000";

  rx_np_ok                     <= '1';      -- Allow Reception of Non-posted Traffic
  rx_np_req                    <= '1';      -- Always request Non-posted Traffic if available
  s_axis_tx_tuser(0)           <= '0';      -- Unused for V6
  s_axis_tx_tuser(1)           <= '0';      -- Error forward packet
  s_axis_tx_tuser(2)           <= '0';      -- Stream packet

  tx_cfg_gnt                   <= '1';      -- Always allow transmission of Config traffic within block

  cfg_err_cor                  <= '0';      -- Never report Correctable Error
  cfg_err_ur                   <= '0';      -- Never report UR
  cfg_err_ecrc                 <= '0';      -- Never report ECRC Error
  cfg_err_cpl_timeout          <= '0';      -- Never report Completion Timeout
  cfg_err_cpl_abort            <= '0';      -- Never report Completion Abort
  cfg_err_cpl_unexpect         <= '0';      -- Never report unexpected completion
  cfg_err_posted               <= '0';      -- Never qualify cfg_err_* inputs
  cfg_err_locked               <= '0';      -- Never qualify cfg_err_ur or cfg_err_cpl_abort
  cfg_pm_wake                  <= '0';      -- Never direct the core to send a PM_PME Message
  cfg_trn_pending              <= '0';      -- Never set the transaction pending bit in the Device Status Register

  cfg_err_atomic_egress_blocked<= '0';      -- Never report Atomic TLP blocked
  cfg_err_internal_cor         <= '0';      -- Never report internal error occurred
  cfg_err_malformed            <= '0';      -- Never report malformed error
  cfg_err_mc_blocked           <= '0';      -- Never report multi-cast TLP blocked
  cfg_err_poisoned             <= '0';      -- Never report poisoned TLP received
  cfg_err_norecovery           <= '0';      -- Never qualify cfg_err_poisoned or cfg_err_cpl_timeout
  cfg_err_acs                  <= '0';      -- Never report an ACS violation
  cfg_err_internal_uncor       <= '0';      -- Never report internal uncorrectable error
  cfg_pm_halt_aspm_l0s         <= '0';      -- Allow entry into L0s
  cfg_pm_halt_aspm_l1          <= '0';      -- Allow entry into L1
  cfg_pm_force_state_en        <= '0';      -- Do not qualify cfg_pm_force_state
  cfg_pm_force_state           <= "00";     -- Do not move force core into specific PM state

  cfg_err_aer_headerlog        <= (others => '0');      -- Zero out the AER Header Log
  cfg_aer_interrupt_msgnum     <= "00000";  -- Zero out the AER Root Error Status Register

  cfg_interrupt_stat           <= '0';      -- Never set the Interrupt Status bit
  cfg_pciecap_interrupt_msgnum <= "00000";  -- Zero out Interrupt Message Number

--cfg_interrupt_assert         <= '0';      -- Always drive interrupt de-assert
--cfg_interrupt                <= '0';      -- Never drive interrupt by qualifying cfg_interrupt_assert

  pl_directed_link_change      <= "00";     -- Never initiate link change
  pl_directed_link_width       <= "00";     -- Zero out directed link width
  pl_directed_link_speed       <= '0';      -- Zero out directed link speed
  pl_directed_link_auton       <= '0';      -- Zero out link autonomous input
  pl_upstream_prefer_deemph    <= '1';      -- Zero out preferred de-emphasis of upstream port

  cfg_interrupt_di             <= x"00";    -- Do not set interrupt fields

  cfg_err_tlp_cpl_header <= x"000000000000";-- Zero out the header information

  cfg_mgmt_di            <= x"00000000";    -- Zero out CFG MGMT input data bus
  cfg_mgmt_byte_en       <= x"0";           -- Zero out CFG MGMT byte enables
  cfg_mgmt_dwaddr        <= "0000000000";   -- Zero out CFG MGMT 10-bit address port
  cfg_mgmt_wr_en         <= '0';            -- Do not write CFG space
  cfg_mgmt_rd_en         <= '0';            -- Do not read CFG space
  cfg_mgmt_wr_readonly   <= '0';            -- Never treat RO bit as RW

  cfg_dsn  <= PCI_EXP_EP_DSN_2 & PCI_EXP_EP_DSN_1;  -- Assign the input DSN

  ------------------------------------------------------------------------------------------------------------------//
  -- Programmable I/O Module                                                                                        //
  ------------------------------------------------------------------------------------------------------------------//

  cfg_completer_id     <= (cfg_bus_number & cfg_device_number & cfg_function_number);
  cfg_bus_mstr_enable  <= cfg_command(2);

-- Programmable I/O Module

master : pcie_64b_master
port map(
    user_clk            => user_clk,           -- I
    user_reset          => user_reset,         -- I
    user_lnk_up         => user_lnk_up,        -- I

    s_axis_tx_tready    => s_axis_tx_tready,   -- I
    s_axis_tx_tdata     => s_axis_tx_tdata,    -- O
    s_axis_tx_tkeep     => s_axis_tx_tkeep,    -- O
    s_axis_tx_tlast     => s_axis_tx_tlast,    -- O
    s_axis_tx_tvalid    => s_axis_tx_tvalid,   -- O
	tx_buf_av			=> tx_buf_av,          -- I
    tx_src_dsc          => s_axis_tx_tuser(3), -- O
    tx_err_drop         => tx_err_drop, 

    m_axis_rx_tdata     => m_axis_rx_tdata,    -- I
    m_axis_rx_tkeep     => m_axis_rx_tkeep,    -- I
    m_axis_rx_tlast     => m_axis_rx_tlast,    -- I
    m_axis_rx_tvalid    => m_axis_rx_tvalid,   -- I
    m_axis_rx_tready    => m_axis_rx_tready,   -- O
    m_axis_rx_tuser     => m_axis_rx_tuser,    -- I

    cfg_to_turnoff      => cfg_to_turnoff,     -- I
    cfg_turnoff_ok      => cfg_turnoff_ok,     -- O
    cfg_completer_id    => cfg_completer_id,   -- I (15:0)

    cfg_status  		=> cfg_status,
    cfg_command   		=> cfg_command,
	cfg_dcommand		=> cfg_dcommand, 

    cfg_interrupt 		 => cfg_interrupt,
    cfg_interrupt_rdy    => cfg_interrupt_rdy,
    cfg_interrupt_assert => cfg_interrupt_assert,

-------------------------------------------------------
-- HOST DMA Data 
	-- RX FIFO
	m_user_tvalid		=> m_user_tvalid,
	m_user_tready		=> m_user_tready,
	m_user_tdata		=> m_user_tdata,
	 
	-- TX FIFO
	s_user_tvalid		=> s_user_tvalid,
	s_user_tready		=> s_user_tready,
	s_user_tdata		=> s_user_tdata,

	m_axi_tvalid	=> m_axi_tvalid,
	m_axi_tready	=> m_axi_tready,
	m_axi_tdata		=> m_axi_tdata,
	 
	-- TX FIFO
	s_axi_tvalid	=> s_axi_tvalid,
	s_axi_tready	=> s_axi_tready,
	s_axi_tdata		=> s_axi_tdata,
	   
--------------------------------------------------- 
-- Stream AXI Bridge Interface Control to Datamover IP
	-- MM2S CMD interface
	StartPulse_mm2s         => StartPulse_mm2s,
	StartAddrB_mm2s         => StartAddrB_mm2s,
	BytesToTransfer_mm2s    => BytesToTransfer_mm2s,
	CmdTag_mm2s             => CmdTag_mm2s,
	CmdAck_mm2s             => CmdAck_mm2s,
					
					
	-- S2MM CMD interface
	StartPulse_s2mm         => StartPulse_s2mm,
	StartAddrB_s2mm         => StartAddrB_s2mm,
	BytesToTransfer_s2mm    => BytesToTransfer_s2mm,
	CmdTag_s2mm             => CmdTag_s2mm,
	CmdAck_s2mm             => CmdAck_s2mm,
					
	-- AXI Stream Master interface (Read Status from AXI)
	m_axis_s2mm_sts_tvalid  	=> m_axis_s2mm_sts_tvalid,
	m_axis_s2mm_sts_tready 		=> m_axis_s2mm_sts_tready,
	m_axis_s2mm_sts_tdata 	 	=> m_axis_s2mm_sts_tdata,
			
	m_axis_mm2s_sts_tvalid  => m_axis_mm2s_sts_tvalid,
	m_axis_mm2s_sts_tready  => m_axis_mm2s_sts_tready,
	m_axis_mm2s_sts_tdata   => m_axis_mm2s_sts_tdata,
           


---------------------------------------------------
-- User IP interface

	ip_clk		    => ip_clk,
	user_rst_n		=> user_rst_n,

	
    --  Registers interface
	Reg_RE		=> Reg_RE,
	Reg_WE		=> Reg_WE,	
	Reg_Addr	=> Reg_Addr,
	Reg_Dout	=> Reg_Dout,
	Reg_Din		=> Reg_Din,
	Reg_Valid	=> Reg_Valid,
	
	pcie_int_req	=> pcie_int_req,
	pcie_int_compl	=> pcie_int_compl,
	
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
	
	InitDoneBottom     =>	InitDoneBtm, 	
    InitDoneTop        =>   InitDoneTop,
    use_AXI            =>   use_AXI 	

  );

end; -- pcie_app
