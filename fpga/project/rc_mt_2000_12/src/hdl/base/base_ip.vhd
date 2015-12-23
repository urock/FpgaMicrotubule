-------------------------------------------------------------------------------
-- Project     : Rosta Base PCIe project Ver.10
-- File        : rosta_pcie_2000.vhd
-- Version     : 10.0
-- Company     : Rosta
-- Engineer    : rkob & urock
-- Create Date : 25.11.2014
-- Comments    : Rosta Module 
------------------------------------------------------------------------------
-- Series-7 Integrated Block for PCI Express - Version 3.0
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

library unisim;
use unisim.vcomponents.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity base_ip is
  generic (
   C_DATA_WIDTH         : integer range 64 to 128 := 64
  );
  port (
		pci_exp_txp		: out std_logic_vector(3 downto 0);
		pci_exp_txn		: out std_logic_vector(3 downto 0);
		pci_exp_rxp		: in std_logic_vector(3 downto 0);
		pci_exp_rxn		: in std_logic_vector(3 downto 0);

		sys_clk_p		: in std_logic;
		sys_clk_n		: in std_logic;
		sys_rst_n		: in std_logic;
  
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
	
	
-----------------------------------------------------------------		
-- Debug interface
--        debug_clk       : out std_logic;
--		debug_rst       : out std_logic;		-- active high			
--        debug_trig      : out std_logic; 
--        debug_data      : out std_logic_vector(31 downto 0);

-----------------------------------------------------------------				
-- CBUS section

--	cbus_clk  	: in  std_logic;

--	id      : in std_logic_vector(1 downto 0);
--	strtcmd : in std_logic;
--	command	: inout std_logic_vector(7 downto 0);
--	dval    : inout std_logic;

--    -- Register Interface
--    -- to USER IP
--	hardid	: out std_logic_vector( 1 downto 0); 
--	mac		: out std_logic_vector(31 downto 0);  
--	userid	: out std_logic_vector(31 downto 0);

--	OutReg0	: out std_logic_vector(31 downto 0);
--	OutReg1	: out std_logic_vector(31 downto 0);
--	OutReg2	: out std_logic_vector(31 downto 0);
--	OutReg3	: out std_logic_vector(31 downto 0);
--	OutReg4	: out std_logic_vector(31 downto 0);
--	OutReg5	: out std_logic_vector(31 downto 0);
--	OutReg6	: out std_logic_vector(31 downto 0);

--    -- from USER IP
--	InReg0	: in std_logic_vector(31 downto 0);
--	InReg1	: in std_logic_vector(31 downto 0);
--	InReg2	: in std_logic_vector(31 downto 0);
--	InReg3	: in std_logic_vector(31 downto 0);
--	InReg4	: in std_logic_vector(31 downto 0);
--	InReg5	: in std_logic_vector(31 downto 0);
--	InReg6	: in std_logic_vector(31 downto 0);
	
--    -- S6 FIFO Interface
--    fifo_rst : in std_logic;

--    -- RX FIFO
--    s6_v7_empty : out std_logic;
--    s6_v7_dout  : out std_logic_vector(31 downto 0);
--    s6_v7_rd_en : in std_logic;
--    s6_v7_data_count : out std_logic_vector(10 downto 0);

--    -- TX FIFO
--    v7_s6_din   : in std_logic_vector(31 downto 0);
--    v7_s6_wr_en : in std_logic;
--    v7_s6_full  : out std_logic;
    
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
      
      
--    Btm_ddr_dq         : inout std_logic_vector(31 downto 0);
--    Btm_ddr_dqs_p     : inout std_logic_vector(3 downto 0);
--    Btm_ddr_dqs_n     : inout std_logic_vector(3 downto 0);
--    Btm_ddr_addr     : out std_logic_vector(13 downto 0);
--    Btm_ddr_ba         : out std_logic_vector(2 downto 0);
--    Btm_ddr_ras_n     : out std_logic;
--    Btm_ddr_cas_n     : out std_logic;
--    Btm_ddr_we_n     : out std_logic;
--    Btm_ddr_reset_n : out std_logic;
--    Btm_ddr_ck_p     : out std_logic;
--    Btm_ddr_ck_n     : out std_logic;
--    Btm_ddr_cke     : out std_logic;
--    Btm_ddr_cs_n     : out std_logic;        
--    Btm_ddr_dm        : out std_logic_vector(3 downto 0);
--    Btm_ddr_odt     : out std_logic		
);
end base_ip;

architecture pcie_7x of base_ip is
  attribute DowngradeIPIdentifiedWarnings: string;
   attribute DowngradeIPIdentifiedWarnings of pcie_7x : architecture is "yes";
   
 --component clk_wiz_0

-- port(
--     clk_in : in std_logic;
 --    clk_out : out std_logic
     
-- );
 --end component;  
  

  component pcie_7x_0_pipe_clock
    generic (
          PCIE_ASYNC_EN                : string  :=   "FALSE";     -- PCIe async enable
          PCIE_TXBUF_EN                : string  :=   "FALSE";     -- PCIe TX buffer enable for Gen1/Gen2 only
          PCIE_CLK_SHARING_EN          : string  :=   "FALSE";     -- Enable Clock Sharing
          PCIE_LANE                    : integer :=   4;           -- PCIe number of lanes
          PCIE_LINK_SPEED              : integer :=   2;           -- PCIe link speed
          PCIE_REFCLK_FREQ             : integer :=   0;           -- PCIe reference clock frequency
          PCIE_USERCLK1_FREQ           : integer :=   4;           -- PCIe user clock 1 frequency
          PCIE_USERCLK2_FREQ           : integer :=   4;           -- PCIe user clock 2 frequency
          PCIE_OOBCLK_MODE             : integer :=   1;           -- PCIe oob clock mode
          PCIE_DEBUG_MODE              : integer :=   0            -- PCIe Debug Mode
    );
    port  (

          ------------ Input -------------------------------------
          CLK_CLK                        : in std_logic;
          CLK_TXOUTCLK                   : in std_logic;
          CLK_RXOUTCLK_IN                : in std_logic_vector(3 downto 0);
          CLK_RST_N                      : in std_logic;
          CLK_PCLK_SEL                   : in std_logic_vector(3 downto 0);
          CLK_PCLK_SEL_SLAVE             : in std_logic_vector(3 downto 0);
          CLK_GEN3                       : in std_logic;

          ------------ Output ------------------------------------
          CLK_PCLK                       : out std_logic;
          CLK_PCLK_SLAVE                 : out std_logic;
          CLK_RXUSRCLK                   : out std_logic;
          CLK_RXOUTCLK_OUT               : out std_logic_vector(3 downto 0);
          CLK_DCLK                       : out std_logic;
          CLK_USERCLK1                   : out std_logic;
          CLK_USERCLK2                   : out std_logic;
          CLK_OOBCLK                     : out std_logic;
          CLK_AXI                        : out std_logic;
          CLK_MMCM_LOCK                  : out std_logic );
          --CLK_AXI                        : out std_logic;
          --CLK_AXI_REF                    : out std_logic);
  end component;

  component pcie_7x_0
    port (
     -------------------------------------------------------------------------------------------------------------------
     -- 1. PCI Express (pci_exp) Interface                                                                            --
     -------------------------------------------------------------------------------------------------------------------
      pci_exp_txp                                : out std_logic_vector(3 downto 0);
      pci_exp_txn                                : out std_logic_vector(3 downto 0);
      pci_exp_rxp                                : in std_logic_vector(3 downto 0);
      pci_exp_rxn                                : in std_logic_vector(3 downto 0);

     -------------------------------------------------------------------------------------------------------------------
     -- 2. Clocking Interface                                                                                         --
     -------------------------------------------------------------------------------------------------------------------
      pipe_pclk_in                               : in std_logic;
      pipe_rxusrclk_in                           : in std_logic;
      pipe_rxoutclk_in                           : in std_logic_vector(3 downto 0);
      pipe_mmcm_rst_n                            : in std_logic;   --     // Async      | Async
      pipe_dclk_in                               : in std_logic;
      pipe_userclk1_in                           : in std_logic;
      pipe_userclk2_in                           : in std_logic;
      pipe_oobclk_in                             : in std_logic;
      pipe_mmcm_lock_in                          : in std_logic;
      pipe_txoutclk_out                          : out std_logic;
      pipe_rxoutclk_out                          : out std_logic_vector(3 downto 0);
      pipe_pclk_sel_out                          : out std_logic_vector(3 downto 0);
      pipe_gen3_out                              : out std_logic;

     -------------------------------------------------------------------------------------------------------------------
     -- 3. AXI-S Interface                                                                                            --
     -------------------------------------------------------------------------------------------------------------------
      -- Common
      user_clk_out                               : out std_logic;
      user_reset_out                             : out std_logic;
      user_lnk_up                                : out std_logic;
      user_app_rdy                               : out std_logic;

      -- TX
      tx_buf_av                                  : out std_logic_vector(5 downto 0);
      tx_cfg_req                                 : out std_logic;
      tx_err_drop                                : out std_logic;
      s_axis_tx_tready                           : out std_logic;
      s_axis_tx_tdata                            : in std_logic_vector((C_DATA_WIDTH - 1) downto 0);
      s_axis_tx_tkeep                            : in std_logic_vector((C_DATA_WIDTH / 8 - 1) downto 0);
      s_axis_tx_tlast                            : in std_logic;
      s_axis_tx_tvalid                           : in std_logic;
      s_axis_tx_tuser                            : in std_logic_vector(3 downto 0);
      tx_cfg_gnt                                 : in std_logic;

      -- RX
      m_axis_rx_tdata                            : out std_logic_vector((C_DATA_WIDTH - 1) downto 0);
      m_axis_rx_tkeep                            : out std_logic_vector((C_DATA_WIDTH / 8 - 1) downto 0);
      m_axis_rx_tlast                            : out std_logic;
      m_axis_rx_tvalid                           : out std_logic;
      m_axis_rx_tready                           : in std_logic;
      m_axis_rx_tuser                            : out std_logic_vector(21 downto 0);
      rx_np_ok                                   : in std_logic;
      rx_np_req                                  : in std_logic;

      -- Flow Control
      fc_cpld                                    : out std_logic_vector(11 downto 0);
      fc_cplh                                    : out std_logic_vector(7 downto 0);
      fc_npd                                     : out std_logic_vector(11 downto 0);
      fc_nph                                     : out std_logic_vector(7 downto 0);
      fc_pd                                      : out std_logic_vector(11 downto 0);
      fc_ph                                      : out std_logic_vector(7 downto 0);
      fc_sel                                     : in std_logic_vector(2 downto 0);

     -------------------------------------------------------------------------------------------------------------------
     -- 4. Configuration (CFG) Interface                                                                              --
     -------------------------------------------------------------------------------------------------------------------
     ---------------------------------------------------------------------
      -- EP and RP                                                      --
     ---------------------------------------------------------------------
      cfg_mgmt_do                                : out std_logic_vector (31 downto 0);
      cfg_mgmt_rd_wr_done                        : out std_logic;

      cfg_status                                 : out std_logic_vector(15 downto 0);
      cfg_command                                : out std_logic_vector(15 downto 0);
      cfg_dstatus                                : out std_logic_vector(15 downto 0);
      cfg_dcommand                               : out std_logic_vector(15 downto 0);
      cfg_lstatus                                : out std_logic_vector(15 downto 0);
      cfg_lcommand                               : out std_logic_vector(15 downto 0);
      cfg_dcommand2                              : out std_logic_vector(15 downto 0);
      cfg_pcie_link_state                        : out std_logic_vector(2 downto 0);

      cfg_pmcsr_pme_en                           : out std_logic;
      cfg_pmcsr_powerstate                       : out std_logic_vector(1 downto 0);
      cfg_pmcsr_pme_status                       : out std_logic;
      cfg_received_func_lvl_rst                  : out std_logic;

      -- Management Interface
      cfg_mgmt_di                                : in std_logic_vector (31 downto 0);
      cfg_mgmt_byte_en                           : in std_logic_vector (3 downto 0);
      cfg_mgmt_dwaddr                            : in std_logic_vector (9 downto 0);
      cfg_mgmt_wr_en                             : in std_logic;
      cfg_mgmt_rd_en                             : in std_logic;
      cfg_mgmt_wr_readonly                       : in std_logic;

      -- Error Reporting Interface
      cfg_err_ecrc                               : in std_logic;
      cfg_err_ur                                 : in std_logic;
      cfg_err_cpl_timeout                        : in std_logic;
      cfg_err_cpl_unexpect                       : in std_logic;
      cfg_err_cpl_abort                          : in std_logic;
      cfg_err_posted                             : in std_logic;
      cfg_err_cor                                : in std_logic;
      cfg_err_atomic_egress_blocked              : in std_logic;
      cfg_err_internal_cor                       : in std_logic;
      cfg_err_malformed                          : in std_logic;
      cfg_err_mc_blocked                         : in std_logic;
      cfg_err_poisoned                           : in std_logic;
      cfg_err_norecovery                         : in std_logic;
      cfg_err_tlp_cpl_header                     : in std_logic_vector(47 downto 0);
      cfg_err_cpl_rdy                            : out std_logic;
      cfg_err_locked                             : in std_logic;
      cfg_err_acs                                : in std_logic;
      cfg_err_internal_uncor                     : in std_logic;
      cfg_trn_pending                            : in std_logic;
      cfg_pm_halt_aspm_l0s                       : in std_logic;
      cfg_pm_halt_aspm_l1                        : in std_logic;
      cfg_pm_force_state_en                      : in std_logic;
      cfg_pm_force_state                         : std_logic_vector(1 downto 0);
      cfg_dsn                                    : std_logic_vector(63 downto 0);

     ---------------------------------------------------------------------
      -- EP Only                                                        --
     ---------------------------------------------------------------------
      cfg_interrupt                              : in std_logic;
      cfg_interrupt_rdy                          : out std_logic;
      cfg_interrupt_assert                       : in std_logic;
      cfg_interrupt_di                           : in std_logic_vector(7 downto 0);
      cfg_interrupt_do                           : out std_logic_vector(7 downto 0);
      cfg_interrupt_mmenable                     : out std_logic_vector(2 downto 0);
      cfg_interrupt_msienable                    : out std_logic;
      cfg_interrupt_msixenable                   : out std_logic;
      cfg_interrupt_msixfm                       : out std_logic;
      cfg_interrupt_stat                         : in std_logic;
      cfg_pciecap_interrupt_msgnum               : in std_logic_vector(4 downto 0);
      cfg_to_turnoff                             : out std_logic;
      cfg_turnoff_ok                             : in std_logic;
      cfg_bus_number                             : out std_logic_vector(7 downto 0);
      cfg_device_number                          : out std_logic_vector(4 downto 0);
      cfg_function_number                        : out std_logic_vector(2 downto 0);
      cfg_pm_wake                                : in std_logic;

     ---------------------------------------------------------------------
      -- RP Only                                                        --
     ---------------------------------------------------------------------
      cfg_pm_send_pme_to                         : in std_logic;
      cfg_ds_bus_number                          : in std_logic_vector(7 downto 0);
      cfg_ds_device_number                       : in std_logic_vector(4 downto 0);
      cfg_ds_function_number                     : in std_logic_vector(2 downto 0);

      cfg_mgmt_wr_rw1c_as_rw                     : in std_logic;
      cfg_msg_received                           : out std_logic;
      cfg_msg_data                               : out std_logic_vector(15 downto 0);

      cfg_bridge_serr_en                         : out std_logic;
      cfg_slot_control_electromech_il_ctl_pulse  : out std_logic;
      cfg_root_control_syserr_corr_err_en        : out std_logic;
      cfg_root_control_syserr_non_fatal_err_en   : out std_logic;
      cfg_root_control_syserr_fatal_err_en       : out std_logic;
      cfg_root_control_pme_int_en                : out std_logic;
      cfg_aer_rooterr_corr_err_reporting_en      : out std_logic;
      cfg_aer_rooterr_non_fatal_err_reporting_en : out std_logic;
      cfg_aer_rooterr_fatal_err_reporting_en     : out std_logic;
      cfg_aer_rooterr_corr_err_received          : out std_logic;
      cfg_aer_rooterr_non_fatal_err_received     : out std_logic;
      cfg_aer_rooterr_fatal_err_received         : out std_logic;

      cfg_msg_received_err_cor                   : out std_logic;
      cfg_msg_received_err_non_fatal             : out std_logic;
      cfg_msg_received_err_fatal                 : out std_logic;
      cfg_msg_received_pm_as_nak                 : out std_logic;
      cfg_msg_received_pm_pme                    : out std_logic;
      cfg_msg_received_pme_to_ack                : out std_logic;
      cfg_msg_received_assert_int_a              : out std_logic;
      cfg_msg_received_assert_int_b              : out std_logic;
      cfg_msg_received_assert_int_c              : out std_logic;
      cfg_msg_received_assert_int_d              : out std_logic;
      cfg_msg_received_deassert_int_a            : out std_logic;
      cfg_msg_received_deassert_int_b            : out std_logic;
      cfg_msg_received_deassert_int_c            : out std_logic;
      cfg_msg_received_deassert_int_d            : out std_logic;
      cfg_msg_received_setslotpowerlimit         : out std_logic;

     -------------------------------------------------------------------------------------------------------------------
     -- 5. Physical Layer Control and Status (PL) Interface                                                           --
     -------------------------------------------------------------------------------------------------------------------
      pl_directed_link_change                    : in std_logic_vector(1 downto 0);
      pl_directed_link_width                     : in std_logic_vector(1 downto 0);
      pl_directed_link_speed                     : in std_logic;
      pl_directed_link_auton                     : in std_logic;
      pl_upstream_prefer_deemph                  : in std_logic;

      pl_sel_lnk_rate                            : out std_logic;
      pl_sel_lnk_width                           : out std_logic_vector(1 downto 0);
      pl_ltssm_state                             : out std_logic_vector(5 downto 0);
      pl_lane_reversal_mode                      : out std_logic_vector(1 downto 0);

      pl_phy_lnk_up                              : out std_logic;
      pl_tx_pm_state                             : out std_logic_vector(2 downto 0);
      pl_rx_pm_state                             : out std_logic_vector(1 downto 0);

      pl_link_upcfg_cap                          : out std_logic;
      pl_link_gen2_cap                           : out std_logic;
      pl_link_partner_gen2_supported             : out std_logic;
      pl_initial_link_width                      : out std_logic_vector(2 downto 0);

      pl_directed_change_done                    : out std_logic;

     ---------------------------------------------------------------------
      -- EP Only                                                        --
     ---------------------------------------------------------------------
      pl_received_hot_rst                        : out std_logic;

     ---------------------------------------------------------------------
      -- RP Only                                                        --
     ---------------------------------------------------------------------
      pl_transmit_hot_rst                        : in std_logic;
      pl_downstream_deemph_source                : in std_logic;

     -------------------------------------------------------------------------------------------------------------------
     -- 6. AER interface                                                                                              --
     -------------------------------------------------------------------------------------------------------------------
     cfg_err_aer_headerlog                      : in std_logic_vector(127 downto 0);
     cfg_aer_interrupt_msgnum                   : in std_logic_vector(4 downto 0);
     cfg_err_aer_headerlog_set                  : out std_logic;
     cfg_aer_ecrc_check_en                      : out std_logic;
     cfg_aer_ecrc_gen_en                        : out std_logic;
 
     -------------------------------------------------------------------------------------------------------------------
     -- 7. VC interface                                                                                               --
     -------------------------------------------------------------------------------------------------------------------
     cfg_vc_tcvc_map                            : out std_logic_vector(6 downto 0);

     -------------------------------------------------------------------------------------------------------------------
     -- 8. System(SYS) Interface                                                                                      --
     -------------------------------------------------------------------------------------------------------------------
      sys_clk                       : in std_logic;
      sys_rst_n                     : in std_logic);

  end component;

  component pcie_app_7x
    port  (
      user_clk                       : in std_logic;
      user_reset                     : in std_logic;
      user_lnk_up                    : in std_logic;
      s_axis_tx_tdata                : out std_logic_vector(63 downto 0);
      s_axis_tx_tkeep                : out std_logic_vector(7 downto 0);
      s_axis_tx_tlast                : out std_logic;
      s_axis_tx_tvalid               : out std_logic;
      s_axis_tx_tready               : in std_logic;
      s_axis_tx_tuser                : out std_logic_vector(3 downto 0);
      tx_cfg_req                     : in std_logic;
      tx_cfg_gnt                     : out std_logic;
      tx_err_drop                    : in std_logic;
      tx_buf_av                      : in std_logic_vector(5 downto 0);
      m_axis_rx_tdata                : in std_logic_vector(63 downto 0);
      m_axis_rx_tkeep                : in std_logic_vector(7 downto 0);
      m_axis_rx_tlast                : in std_logic;
      m_axis_rx_tvalid               : in std_logic;
      m_axis_rx_tready               : out std_logic;
      m_axis_rx_tuser                : in std_logic_vector(21 downto 0);
      rx_np_ok                       : out std_logic;
      rx_np_req                      : out std_logic;
      fc_nph                         : in std_logic_vector(7 downto 0);
      fc_npd                         : in std_logic_vector(11 downto 0);
      fc_ph                          : in std_logic_vector(7 downto 0);
      fc_pd                          : in std_logic_vector(11 downto 0);
      fc_cplh                        : in std_logic_vector(7 downto 0);
      fc_cpld                        : in std_logic_vector(11 downto 0);
      fc_sel                         : out std_logic_vector(2 downto 0);
      cfg_err_cor                    : out std_logic;
      cfg_err_ur                     : out std_logic;
      cfg_err_cpl_rdy                : in std_logic;
      cfg_err_ecrc                   : out std_logic;
      cfg_err_cpl_timeout            : out std_logic;
      cfg_err_cpl_abort              : out std_logic;
      cfg_err_cpl_unexpect           : out std_logic;
      cfg_err_posted                 : out std_logic;
      cfg_err_locked                 : out std_logic;
      cfg_err_atomic_egress_blocked  : out std_logic;
      cfg_err_internal_cor           : out std_logic;
      cfg_err_malformed              : out std_logic;
      cfg_err_mc_blocked             : out std_logic;
      cfg_err_poisoned               : out std_logic;
      cfg_err_norecovery             : out std_logic;
      cfg_err_acs                    : out std_logic;
      cfg_err_internal_uncor         : out std_logic;
      cfg_err_aer_headerlog          : out std_logic_vector(127 downto 0);
      cfg_aer_interrupt_msgnum       : out std_logic_vector(4 downto 0);
      cfg_err_aer_headerlog_set      : in  std_logic;
      cfg_aer_ecrc_check_en          : in  std_logic;
      cfg_aer_ecrc_gen_en            : in  std_logic;
      cfg_interrupt                  : out std_logic;
      cfg_interrupt_rdy              : in std_logic;
      cfg_interrupt_assert           : out std_logic;
      cfg_interrupt_di               : out std_logic_vector(7 downto 0);
      cfg_interrupt_do               : in  std_logic_vector(7 downto 0);
      cfg_interrupt_mmenable         : in  std_logic_vector(2 downto 0);
      cfg_interrupt_msienable        : in  std_logic;
      cfg_interrupt_msixenable       : in  std_logic;
      cfg_interrupt_msixfm           : in  std_logic;
      cfg_interrupt_stat             : out std_logic;
      cfg_pciecap_interrupt_msgnum   : out std_logic_vector( 4 downto 0);
      cfg_turnoff_ok                 : out std_logic;
      cfg_to_turnoff                 : in std_logic;
      cfg_pm_wake                    : out std_logic;
      cfg_pm_force_state_en          : out std_logic;
      cfg_pm_force_state             : out std_logic_vector( 1 downto 0);
      cfg_pm_halt_aspm_l0s           : out std_logic;
      cfg_pm_halt_aspm_l1            : out std_logic;
      cfg_pcie_link_state            : in std_logic_vector(2 downto 0);
      cfg_trn_pending                : out std_logic;
      cfg_err_tlp_cpl_header         : out std_logic_vector(47 downto 0);
      cfg_bus_number                 : in std_logic_vector(7 downto 0);
      cfg_device_number              : in std_logic_vector(4 downto 0);
      cfg_function_number            : in std_logic_vector(2 downto 0);
      cfg_status                     : in std_logic_vector(15 downto 0);
      cfg_command                    : in std_logic_vector(15 downto 0);
      cfg_dstatus                    : in std_logic_vector(15 downto 0);
      cfg_dcommand                   : in std_logic_vector(15 downto 0);
      cfg_lstatus                    : in std_logic_vector(15 downto 0);
      cfg_lcommand                   : in std_logic_vector(15 downto 0);
      cfg_dcommand2                  : in std_logic_vector(15 downto 0);
      cfg_mgmt_di                    : out std_logic_vector(31 downto 0);
      cfg_mgmt_byte_en               : out std_logic_vector( 3 downto 0);
      cfg_mgmt_dwaddr                : out std_logic_vector( 9 downto 0);
      cfg_mgmt_wr_en                 : out std_logic;
      cfg_mgmt_rd_en                 : out std_logic;
      cfg_mgmt_wr_readonly           : out std_logic;
      pl_directed_link_change        : out std_logic_vector(1 downto 0);
      pl_ltssm_state                 : in std_logic_vector(5 downto 0);
      pl_directed_link_width         : out std_logic_vector(1 downto 0);
      pl_directed_link_speed         : out std_logic;
      pl_directed_link_auton         : out std_logic;
      pl_upstream_prefer_deemph      : out std_logic;
      pl_sel_lnk_width               : in std_logic_vector(1 downto 0);
      pl_sel_lnk_rate                : in std_logic;
      pl_link_gen2_cap               : in std_logic;
      pl_link_partner_gen2_supported : in std_logic;
      pl_initial_link_width          : in std_logic_vector(2 downto 0);
      pl_link_upcfg_cap              : in std_logic;
      pl_lane_reversal_mode          : in std_logic_vector(1 downto 0);
      pl_received_hot_rst            : in std_logic;
      cfg_dsn                        : out std_logic_vector(63 downto 0);


	   
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
           
-- HOST DMA Data 
	-- RX FIFO
	m_axi_tvalid	: out std_logic;
	m_axi_tready	: in  std_logic;
	m_axi_tdata		: out std_logic_vector(63 downto 0); 
	 
	-- TX FIFO
	s_axi_tvalid	: in  std_logic;
	s_axi_tready	: out std_logic;		
	s_axi_tdata		: in  std_logic_vector(63 downto 0);	
		   


---------------------------------------------------
-- User IP interface

	ip_clk		    : in  std_logic;
	user_rst_n		: out std_logic;

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
	
	InitDoneTop     : in std_logic;
    InitDoneBtm     : in std_logic;
    
    use_AXI         : out std_logic		
      
  );
  end component;
  
  
 component axi_system_wrapper is
    port (
    
    sys_rst_n         : IN  std_logic;
--    sys_rst_out_n    : OUT std_logic; 
    clk_in             : IN  std_logic;
    --clk_out         : OUT std_logic;  
    ip_clk           :  IN std_logic;
	
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

          
  -- User interface
  
      -- MM2S CMD interface
      StartPulse_mm2s            : in  std_logic;
      StartAddrB_mm2s         : in  std_logic_vector(31 downto 0);
      BytesToTransfer_mm2s     : in  std_logic_vector(22 downto 0);
      CmdTag_mm2s                : in  std_logic_vector(3  downto 0);
      CmdAck_mm2s                : out std_logic;
          
          
      -- S2MM CMD interface
      StartPulse_s2mm            : in  std_logic;
      StartAddrB_s2mm         : in  std_logic_vector(31 downto 0);
      BytesToTransfer_s2mm     : in  std_logic_vector(22 downto 0);
      CmdTag_s2mm                : in  std_logic_vector(3  downto 0);
      CmdAck_s2mm                : out std_logic;
          
      -- AXI Stream Master interface (Read Status from AXI)
      -- To User IP
      m_axis_s2mm_sts_tvalid     : out std_logic;
      m_axis_s2mm_sts_tready     : in std_logic;
      m_axis_s2mm_sts_tdata     : out std_logic_vector(7 downto 0);
  
      m_axis_mm2s_sts_tvalid     : out std_logic;
      m_axis_mm2s_sts_tready     : in std_logic;
      m_axis_mm2s_sts_tdata     : out std_logic_vector(7 downto 0);
  
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
      Top_ddr_odt     : out std_logic;
  
  
  
      InitDoneTop     : out std_logic;
      InitDoneBtm     : out std_logic;
      clk_axi         : in  std_logic
      --clk_axi_ref     : in  std_logic
      
      );
  end component;
  
  
--------------------------------------------------- 
-- Stream AXI Bridge Interface Control to Datamover IP
	-- MM2S CMD interface
signal 	StartPulse_mm2s         : std_logic;
signal 	StartAddrB_mm2s         : std_logic_vector(31 downto 0);
signal 	BytesToTransfer_mm2s    : std_logic_vector(22 downto 0);
signal 	CmdTag_mm2s             : std_logic_vector(3  downto 0);
signal 	CmdAck_mm2s             : std_logic;
					
					
	-- S2MM CMD interface
signal 	StartPulse_s2mm         : std_logic;
signal 	StartAddrB_s2mm         : std_logic_vector(31 downto 0);
signal 	BytesToTransfer_s2mm    : std_logic_vector(22 downto 0);
signal 	CmdTag_s2mm             : std_logic_vector(3  downto 0);
signal 	CmdAck_s2mm             : std_logic;
					
	-- AXI Stream Master interface (Read Status from AXI)
signal 	m_axis_s2mm_sts_tvalid  : std_logic;
signal 	m_axis_s2mm_sts_tready  : std_logic;
signal 	m_axis_s2mm_sts_tdata   : std_logic_vector(7 downto 0);
			
signal 	m_axis_mm2s_sts_tvalid  : std_logic;
signal 	m_axis_mm2s_sts_tready  : std_logic;
signal 	m_axis_mm2s_sts_tdata   : std_logic_vector(7 downto 0);
           
-- HOST DMA Data 
	-- RX FIFO
signal 	m_axi_tvalid	: std_logic;
signal 	m_axi_tready	: std_logic;
signal 	m_axi_tdata		: std_logic_vector(63 downto 0); 
	 
	-- TX FIFO
signal 	s_axi_tvalid	: std_logic;
signal 	s_axi_tready	: std_logic;		
signal 	s_axi_tdata		: std_logic_vector(63 downto 0);	  
  
signal InitDoneTop_s   		: std_logic;       
signal InitDoneBottom_s       : std_logic;

signal dm_mm2s_err 			: std_logic;
signal dm_s2mm_err 			: std_logic;

signal user_rst_n_sig  : std_logic;  


--component cbus is 
--	port (
--	cbus_clk  	: in  std_logic;

--	id      : in std_logic_vector(1 downto 0);
--	strtcmd : in std_logic;
--	command	: inout std_logic_vector(7 downto 0);
--	dval    : inout std_logic;

--    -- Register Interface
--    -- to USER IP
--	hardid	: out std_logic_vector( 1 downto 0); 
--	mac		: out std_logic_vector(31 downto 0);  
--	userid	: out std_logic_vector(31 downto 0);

--	OutReg0	: out std_logic_vector(31 downto 0);
--	OutReg1	: out std_logic_vector(31 downto 0);
--	OutReg2	: out std_logic_vector(31 downto 0);
--	OutReg3	: out std_logic_vector(31 downto 0);
--	OutReg4	: out std_logic_vector(31 downto 0);
--	OutReg5	: out std_logic_vector(31 downto 0);
--	OutReg6	: out std_logic_vector(31 downto 0);

--    -- from USER IP
--	InReg0	: in std_logic_vector(31 downto 0);
--	InReg1	: in std_logic_vector(31 downto 0);
--	InReg2	: in std_logic_vector(31 downto 0);
--	InReg3	: in std_logic_vector(31 downto 0);
--	InReg4	: in std_logic_vector(31 downto 0);
--	InReg5	: in std_logic_vector(31 downto 0);
--	InReg6	: in std_logic_vector(31 downto 0);
	
--    -- S6 FIFO Interface
--    fifo_clk : in std_logic;
--    fifo_rst : in std_logic;

--    -- RX FIFO
--    s6_v7_empty : out std_logic;
--    s6_v7_dout  : out std_logic_vector(31 downto 0);
--    s6_v7_rd_en : in std_logic;
--    s6_v7_data_count : out std_logic_vector(10 downto 0);

--    -- TX FIFO
--    v7_s6_din   : in std_logic_vector(31 downto 0);
--    v7_s6_wr_en : in std_logic;
--    v7_s6_full  : out std_logic
--	);
--end component; 
  

--signal	InReg0	: std_logic_vector(31 downto 0);
--signal	InReg1	: std_logic_vector(31 downto 0);
--signal	InReg2	: std_logic_vector(31 downto 0);
--signal	InReg3	: std_logic_vector(31 downto 0);
--signal	InReg4	: std_logic_vector(31 downto 0);
--signal	InReg5	: std_logic_vector(31 downto 0);
--signal	InReg6	: std_logic_vector(31 downto 0);
	

  constant TCQ                  : time           := 1 ps;

  signal sys_rst_c              : std_logic;

  -- Common
  signal user_lnk_up            : std_logic;
  signal user_lnk_up_q          : std_logic;
  signal user_clk               : std_logic;
  signal user_reset             : std_logic;
  signal user_reset_i           : std_logic;
  signal user_reset_q           : std_logic;

  -- Tx
  signal tx_buf_av              : std_logic_vector(5 downto 0);
  signal tx_cfg_req             : std_logic;
  signal tx_err_drop            : std_logic;
  signal tx_cfg_gnt             : std_logic;
  signal s_axis_tx_tready       : std_logic;
  signal s_axis_tx_tuser        : std_logic_vector (3 downto 0);
  signal s_axis_tx_tdata        : std_logic_vector((C_DATA_WIDTH - 1) downto 0);
  signal s_axis_tx_tkeep        : std_logic_vector((C_DATA_WIDTH/8 - 1) downto 0);
  signal s_axis_tx_tlast        : std_logic;
  signal s_axis_tx_tvalid       : std_logic;

  -- Rx
  signal m_axis_rx_tdata        : std_logic_vector((C_DATA_WIDTH - 1) downto 0);
  signal m_axis_rx_tkeep        : std_logic_vector((C_DATA_WIDTH/8- 1) downto 0);
  signal m_axis_rx_tlast        : std_logic;
  signal m_axis_rx_tvalid       : std_logic;
  signal m_axis_rx_tready       : std_logic;
  signal m_axis_rx_tuser        : std_logic_vector (21 downto 0);
  signal rx_np_ok               : std_logic;
  signal rx_np_req              : std_logic;

  -- Flow Control
  signal fc_cpld                : std_logic_vector(11 downto 0);
  signal fc_cplh                : std_logic_vector(7 downto 0);
  signal fc_npd                 : std_logic_vector(11 downto 0);
  signal fc_nph                 : std_logic_vector(7 downto 0);
  signal fc_pd                  : std_logic_vector(11 downto 0);
  signal fc_ph                  : std_logic_vector(7 downto 0);
  signal fc_sel                 : std_logic_vector(2 downto 0);

  ---------------------------------------------------------
  -- 3. Configuration (CFG) Interface
  ---------------------------------------------------------
  signal cfg_err_cor                   : std_logic;
  signal cfg_err_ur                    : std_logic;
  signal cfg_err_ecrc                  : std_logic;
  signal cfg_err_cpl_timeout           : std_logic;
  signal cfg_err_cpl_abort             : std_logic;
  signal cfg_err_cpl_unexpect          : std_logic;
  signal cfg_err_posted                : std_logic;
  signal cfg_err_locked                : std_logic;
  signal cfg_err_tlp_cpl_header        : std_logic_vector(47 downto 0);
  signal cfg_err_cpl_rdy               : std_logic;
  signal cfg_err_atomic_egress_blocked : std_logic;
  signal cfg_err_internal_cor          : std_logic;
  signal cfg_err_malformed             : std_logic;
  signal cfg_err_mc_blocked            : std_logic;
  signal cfg_err_poisoned              : std_logic;
  signal cfg_err_norecovery            : std_logic;
  signal cfg_err_acs                   : std_logic;
  signal cfg_err_internal_uncor        : std_logic;
  signal cfg_interrupt                 : std_logic;
  signal cfg_interrupt_rdy             : std_logic;
  signal cfg_interrupt_assert          : std_logic;
  signal cfg_interrupt_di              : std_logic_vector(7 downto 0);
  signal cfg_interrupt_do              : std_logic_vector(7 downto 0);
  signal cfg_interrupt_mmenable        : std_logic_vector(2 downto 0);
  signal cfg_interrupt_msienable       : std_logic;
  signal cfg_interrupt_msixenable      : std_logic;
  signal cfg_interrupt_msixfm          : std_logic;
  signal cfg_interrupt_stat            : std_logic;
  signal cfg_pciecap_interrupt_msgnum  : std_logic_vector(4 downto 0);
  signal cfg_turnoff_ok                : std_logic;
  signal cfg_to_turnoff                : std_logic;
  signal cfg_trn_pending               : std_logic;
  signal cfg_pm_halt_aspm_l0s          : std_logic;
  signal cfg_pm_halt_aspm_l1           : std_logic;
  signal cfg_pm_force_state_en         : std_logic;
  signal cfg_pm_force_state            : std_logic_vector(1 downto 0);
  signal cfg_pm_wake                   : std_logic;
  signal cfg_bus_number                : std_logic_vector(7 downto 0);
  signal cfg_device_number             : std_logic_vector(4 downto 0);
  signal cfg_function_number           : std_logic_vector(2 downto 0);
  signal cfg_status                    : std_logic_vector(15 downto 0);
  signal cfg_command                   : std_logic_vector(15 downto 0);
  signal cfg_dstatus                   : std_logic_vector(15 downto 0);
  signal cfg_dcommand                  : std_logic_vector(15 downto 0);
  signal cfg_lstatus                   : std_logic_vector(15 downto 0);
  signal cfg_lcommand                  : std_logic_vector(15 downto 0);
  signal cfg_dcommand2                 : std_logic_vector(15 downto 0);
  signal cfg_pcie_link_state           : std_logic_vector(2 downto 0);
  signal cfg_dsn                       : std_logic_vector(63 downto 0);
  signal cfg_err_aer_headerlog         : std_logic_vector(127 downto 0);
  signal cfg_aer_interrupt_msgnum      : std_logic_vector(4 downto 0);
  signal cfg_err_aer_headerlog_set     : std_logic;
  signal cfg_aer_ecrc_check_en         : std_logic;
  signal cfg_aer_ecrc_gen_en           : std_logic;

  signal cfg_mgmt_di                   : std_logic_vector(31 downto 0);
  signal cfg_mgmt_byte_en              : std_logic_vector(3 downto 0);
  signal cfg_mgmt_dwaddr               : std_logic_vector(9 downto 0);
  signal cfg_mgmt_wr_en                : std_logic;
  signal cfg_mgmt_rd_en                : std_logic;
  signal cfg_mgmt_wr_readonly          : std_logic;

  ---------------------------------------------------------
  -- 4. Physical Layer Control and Status (PL) Interface
  ---------------------------------------------------------

  signal pl_initial_link_width          : std_logic_vector(2 downto 0);
  signal pl_lane_reversal_mode          : std_logic_vector(1 downto 0);
  signal pl_link_gen2_cap               : std_logic;
  signal pl_link_partner_gen2_supported : std_logic;
  signal pl_link_upcfg_cap              : std_logic;
  signal pl_ltssm_state                 : std_logic_vector(5 downto 0);
  signal pl_received_hot_rst            : std_logic;
  signal pl_sel_lnk_rate                : std_logic;
  signal pl_sel_lnk_width               : std_logic_vector(1 downto 0);
  signal pl_directed_link_auton         : std_logic;
  signal pl_directed_link_change        : std_logic_vector(1 downto 0);
  signal pl_directed_link_speed         : std_logic;
  signal pl_directed_link_width         : std_logic_vector(1 downto 0);
  signal pl_upstream_prefer_deemph      : std_logic;

  signal sys_clk                        : std_logic;
  signal sys_rst_n_c                    : std_logic;
  signal sys_rst                        : std_logic;
  
  --signal clk_axi                        : std_logic;
  signal clk_axi_ref                    : std_logic;

  -- Wires used for external clocking connectivity
  signal pipe_pclk_in                   : std_logic;
  signal pipe_rxusrclk_in               : std_logic;
  signal pipe_rxoutclk_in               : std_logic_vector(3 downto 0);
  signal pipe_dclk_in                   : std_logic;
  signal pipe_userclk1_in               : std_logic;
  signal pipe_userclk2_in               : std_logic;
  signal pipe_oobclk_in                 : std_logic;
  signal pipe_mmcm_lock_in              : std_logic;

  signal pipe_txoutclk_out              : std_logic;
  signal pipe_rxoutclk_out              : std_logic_vector(3 downto 0);
  signal pipe_pclk_sel_out              : std_logic_vector(3 downto 0);
  signal pipe_gen3_out                  : std_logic := '0';

  signal pipe_mmcm_rst_n                : std_logic := '1';
  
  
signal pci_exp_txp_s                   : std_logic_vector(3 downto 0);
signal pci_exp_txn_s                   : std_logic_vector(3 downto 0);
signal pci_exp_rxp_s                   : std_logic_vector(3 downto 0);
signal pci_exp_rxn_s                   : std_logic_vector(3 downto 0);  
  
signal sys_clk_p_s    : std_logic;
signal sys_clk_n_s    : std_logic;

signal sys_rst_n_c_0    : std_logic;
signal sys_rst_n_c_1    : std_logic;
signal sys_rst_n_c_2    : std_logic;
   
   
signal cnt1     : std_logic_vector(11 downto 0)		:= X"000";

signal debug_rst_s	: std_logic; 
signal debug_data_s : std_logic_vector(15 downto 0); 

signal m_tvalid : std_logic;
signal m_tready : std_logic;
signal m_tdata : std_logic_vector(63 downto 0);

signal s_tvalid : std_logic;
signal s_tready : std_logic;
signal s_tdata : std_logic_vector(63 downto 0);

signal use_AXI : std_logic;

-------------------------------------------------------

begin


-- Debug interface
-- debug_rst_s	<= not pipe_mmcm_lock_in; 

-- debug_rst	<= debug_rst_s;
-- debug_clk   <= user_clk;

-- debug_data  <= X"0000" & debug_data_s;
    
-- process(debug_rst_s, user_clk)
-- begin
	-- if debug_rst_s = '1' then
	
		-- debug_trig    <= '0'; 
		-- debug_data_s  <= (others => '0'); 
		
    -- elsif rising_edge(user_clk) then
    
        -- debug_trig  <= '0'; 
    
        -- if cnt1 = X"FFF" then
            -- cnt1 <= (others => '0');
            
            -- if debug_data_s < X"FFFF" then
                -- debug_trig  <= '1';
                -- debug_data_s        <= debug_data_s + 1;
            -- end if;                  
            
        -- else
            -- cnt1 <= cnt1 + 1;
        -- end if;
        
    
    -- end if; 
    
-- end process;
--m_axis_txd_tvalid <= m_axi_tvalid;
--m_axi_tready <= m_axis_txd_tready;
--m_axis_txd_tdata <= m_axi_tdata;


--s_axi_tvalid <= s_axis_rxd_tvalid;
--s_axis_rxd_tready <= s_axi_tready;
--s_axi_tdata <= s_axis_rxd_tdata;


	

--process(use_AXI, 
--       m_tvalid, m_user_tready, m_tdata, 
--       s_user_tvalid, s_tready, s_user_tdata,
--       s_axis_rxd_tvalid, m_axis_txd_tready, s_axis_rxd_tdata )
--begin
-- CASE use_AXI IS
 
-- when '0' =>
--m_user_tvalid<= m_tvalid;
--m_tready <= m_user_tready;
--m_user_tdata <= m_tdata;

--s_tvalid  <= s_user_tvalid;
--s_user_tready <= s_tready;
--s_tdata   <= s_user_tdata;

-- m_axis_txd_tvalid <= s_axis_rxd_tvalid;
-- s_axis_rxd_tready <= m_axis_txd_tready;
-- m_axis_txd_tdata <= s_axis_rxd_tdata;

--when '1' =>

-- m_axis_txd_tvalid <= m_tvalid;
-- m_tready <= m_axis_txd_tready;
-- m_axis_txd_tdata <= m_tdata;

-- s_tvalid  <= s_axis_rxd_tvalid;
-- s_axis_rxd_tready <= s_tready;
-- s_tdata   <= s_axis_rxd_tdata;
 
-- m_user_tvalid<= s_user_tvalid;
-- s_user_tready <= m_user_tready;
-- m_user_tdata <= s_user_tdata;
 
-- end case;

--end process;


              
user_rst_n <= user_rst_n_sig;    
    

 sys_clk_p_ibuf: IBUF
	port map ( I => sys_clk_p, O => sys_clk_p_s);  
 
 sys_clk_n_ibuf: IBUF
	port map ( I => sys_clk_n, O => sys_clk_n_s);  
         
  f_loop1: for i in 0 to 3 generate
 begin
     rxp_ibuf: IBUF
         port map ( I => pci_exp_rxp(i), O => pci_exp_rxp_s(i));              
     rxn_ibuf: IBUF
          port map ( I => pci_exp_rxn(i), O => pci_exp_rxn_s(i));
          
     txp_obuf: OBUF
          port map ( I => pci_exp_txp_s(i), O => pci_exp_txp(i));              
                        
     txn_obuf: OBUF
          port map ( I => pci_exp_txn_s(i), O => pci_exp_txn(i));
                  
 end generate f_loop1;         
       
  process(user_clk)
  begin
    if (user_clk'event and user_clk='1') then
     if (user_reset = '1') then
       user_reset_q  <= '1' after TCQ;
       user_lnk_up_q <= '0' after TCQ;
     else
       user_reset_q  <= user_reset after TCQ;
       user_lnk_up_q <= user_lnk_up after TCQ;
     end if;
    end if;
  end process;

  refclk_ibuf : IBUFDS_GTE2
     port map(
       O       => sys_clk,
       ODIV2   => open,
       I       => sys_clk_p_s,
       IB      => sys_clk_n_s,
       CEB     => '0');

  sys_reset_n_ibuf : IBUF
     port map(
       O       => sys_rst_n_c_0,
       I       => sys_rst_n);
       
       
process(pipe_userclk2_in)
begin
    if rising_edge(pipe_userclk2_in) then
    
        sys_rst_n_c_1   <= sys_rst_n_c_0;
        sys_rst_n_c_2   <= sys_rst_n_c_1;
        
        sys_rst_n_c     <=  sys_rst_n_c_1 and sys_rst_n_c_2;   
    
    end if;
    
end process;

--axi_clk : clk_wiz_0

-- port map(
--     clk_in => pipe_userclk1_in,
--     clk_out => clk_axi
     
-- );

---------- PIPE Clock Shared Mode ------------------------------
pipe_clock_i : pcie_7x_0_pipe_clock
      generic map
      (
          PCIE_ASYNC_EN                  =>  "FALSE",-- PCIe async enable
          PCIE_TXBUF_EN                  =>  "FALSE",-- PCIe TX buffer enable for Gen1/Gen2 only
          PCIE_CLK_SHARING_EN            =>  "FALSE",-- Enable Clock Sharing
          PCIE_LANE                      =>  4,      -- PCIe number of lanes
          PCIE_LINK_SPEED                =>  2,
          PCIE_REFCLK_FREQ               =>  0,      -- PCIe reference clock frequency
          PCIE_USERCLK1_FREQ             =>  4,      -- PCIe user clock 1 frequency
          PCIE_USERCLK2_FREQ             =>  4,      -- PCIe user clock 2 frequency
          PCIE_OOBCLK_MODE               =>  1,      -- PCIe oob clock mode
          PCIE_DEBUG_MODE                =>  0
      )
      port map
      (
          ------------ Input -------------------------------------
          CLK_CLK                        => sys_clk,
          CLK_TXOUTCLK                   => pipe_txoutclk_out,-- Reference clock from lane 0
          CLK_RXOUTCLK_IN                => pipe_rxoutclk_out,
          CLK_RST_N                      => pipe_mmcm_rst_n,  -- Allow system reset for error_recovery             
          CLK_PCLK_SEL                   => pipe_pclk_sel_out,
          CLK_PCLK_SEL_SLAVE             => ( others => '0'),
          CLK_GEN3                       => pipe_gen3_out,

          ------------ Output ------------------------------------
          CLK_PCLK                       => pipe_pclk_in,
          CLK_PCLK_SLAVE                =>  open,
          CLK_RXUSRCLK                   => pipe_rxusrclk_in,
          CLK_RXOUTCLK_OUT               => pipe_rxoutclk_in,
          CLK_DCLK                       => pipe_dclk_in,
          CLK_USERCLK1                   => pipe_userclk1_in,
          CLK_USERCLK2                   => pipe_userclk2_in,
          CLK_OOBCLK                     => pipe_oobclk_in,
          CLK_MMCM_LOCK                  => pipe_mmcm_lock_in, 
          CLK_AXI                        => open);
          --CLK_AXI_REF                    => clk_axi_ref);


  pcie_7x_0_i : pcie_7x_0
  port map(

  -------------------------------------------------------------------------------------------------------------------
  -- 1. PCI Express (pci_exp) Interface                                                                            --
  -------------------------------------------------------------------------------------------------------------------
  -- TX
  pci_exp_txp                               => pci_exp_txp_s,
  pci_exp_txn                               => pci_exp_txn_s,
  -- RX
  pci_exp_rxp                               => pci_exp_rxp_s,
  pci_exp_rxn                               => pci_exp_rxn_s,

  -------------------------------------------------------------------------------------------------------------------
  -- 2. Clocking Interface - For Partial Reconfig Support                                                          --
  -------------------------------------------------------------------------------------------------------------------
  pipe_pclk_in                               => pipe_pclk_in,
  pipe_rxusrclk_in                           => pipe_rxusrclk_in,
  pipe_rxoutclk_in                           => pipe_rxoutclk_in,
  pipe_mmcm_rst_n                            => pipe_mmcm_rst_n,        -- Async      | Async
  pipe_dclk_in                               => pipe_dclk_in,
  pipe_userclk1_in                           => pipe_userclk1_in,
  pipe_userclk2_in                           => pipe_userclk2_in,
  pipe_oobclk_in                             => pipe_oobclk_in,
  pipe_mmcm_lock_in                          => pipe_mmcm_lock_in,
  pipe_txoutclk_out                          => pipe_txoutclk_out,
  pipe_rxoutclk_out                          => pipe_rxoutclk_out,
  pipe_pclk_sel_out                          => pipe_pclk_sel_out,
  pipe_gen3_out                              => pipe_gen3_out,

  -------------------------------------------------------------------------------------------------------------------
  -- 3. AXI-S Interface                                                                                            --
  -------------------------------------------------------------------------------------------------------------------
  -- Common
  user_clk_out                               => user_clk ,
  user_reset_out                             => user_reset,
  user_lnk_up                                => user_lnk_up,
  user_app_rdy                               => open,

  -- TX
  tx_buf_av                                  => tx_buf_av ,
  tx_cfg_req                                 => tx_cfg_req ,
  tx_err_drop                                => tx_err_drop ,
  s_axis_tx_tready                           => s_axis_tx_tready ,
  s_axis_tx_tdata                            => s_axis_tx_tdata ,
  s_axis_tx_tkeep                            => s_axis_tx_tkeep ,
  s_axis_tx_tlast                            => s_axis_tx_tlast ,
  s_axis_tx_tvalid                           => s_axis_tx_tvalid ,
  s_axis_tx_tuser                            => s_axis_tx_tuser,
  tx_cfg_gnt                                 => tx_cfg_gnt ,

  -- RX
  m_axis_rx_tdata                            => m_axis_rx_tdata ,
  m_axis_rx_tkeep                            => m_axis_rx_tkeep ,
  m_axis_rx_tlast                            => m_axis_rx_tlast ,
  m_axis_rx_tvalid                           => m_axis_rx_tvalid ,
  m_axis_rx_tready                           => m_axis_rx_tready ,
  m_axis_rx_tuser                            => m_axis_rx_tuser,
  rx_np_ok                                   => rx_np_ok ,
  rx_np_req                                  => rx_np_req ,

  -- Flow Control
  fc_cpld                                    => fc_cpld ,
  fc_cplh                                    => fc_cplh ,
  fc_npd                                     => fc_npd ,
  fc_nph                                     => fc_nph ,
  fc_pd                                      => fc_pd ,
  fc_ph                                      => fc_ph ,
  fc_sel                                     => fc_sel ,

  -------------------------------------------------------------------------------------------------------------------
  -- 4. Configuration (CFG) Interface                                                                              --
  -------------------------------------------------------------------------------------------------------------------
  ---------------------------------------------------------------------
   -- EP and RP                                                      --
  ---------------------------------------------------------------------

  cfg_mgmt_do                                => open ,
  cfg_mgmt_rd_wr_done                        => open ,

  cfg_status                                 => cfg_status ,
  cfg_command                                => cfg_command ,
  cfg_dstatus                                => cfg_dstatus ,
  cfg_dcommand                               => cfg_dcommand ,
  cfg_lstatus                                => cfg_lstatus ,
  cfg_lcommand                               => cfg_lcommand ,
  cfg_dcommand2                              => cfg_dcommand2 ,
  cfg_pcie_link_state                        => cfg_pcie_link_state ,

  cfg_pmcsr_pme_en                           => open ,
  cfg_pmcsr_pme_status                       => open ,
  cfg_pmcsr_powerstate                       => open ,
  cfg_received_func_lvl_rst                  => open ,

  cfg_mgmt_di                                => cfg_mgmt_di ,
  cfg_mgmt_byte_en                           => cfg_mgmt_byte_en ,
  cfg_mgmt_dwaddr                            => cfg_mgmt_dwaddr ,
  cfg_mgmt_wr_en                             => cfg_mgmt_wr_en ,
  cfg_mgmt_rd_en                             => cfg_mgmt_rd_en ,
  cfg_mgmt_wr_readonly                       => cfg_mgmt_wr_readonly ,

  cfg_err_ecrc                               => cfg_err_ecrc ,
  cfg_err_ur                                 => cfg_err_ur ,
  cfg_err_cpl_timeout                        => cfg_err_cpl_timeout ,
  cfg_err_cpl_unexpect                       => cfg_err_cpl_unexpect ,
  cfg_err_cpl_abort                          => cfg_err_cpl_abort ,
  cfg_err_posted                             => cfg_err_posted ,
  cfg_err_cor                                => cfg_err_cor ,
  cfg_err_atomic_egress_blocked              => cfg_err_atomic_egress_blocked ,
  cfg_err_internal_cor                       => cfg_err_internal_cor ,
  cfg_err_malformed                          => cfg_err_malformed ,
  cfg_err_mc_blocked                         => cfg_err_mc_blocked ,
  cfg_err_poisoned                           => cfg_err_poisoned ,
  cfg_err_norecovery                         => cfg_err_norecovery ,
  cfg_err_tlp_cpl_header                     => cfg_err_tlp_cpl_header,
  cfg_err_cpl_rdy                            => cfg_err_cpl_rdy ,
  cfg_err_locked                             => cfg_err_locked ,
  cfg_err_acs                                => cfg_err_acs ,
  cfg_err_internal_uncor                     => cfg_err_internal_uncor ,

  cfg_trn_pending                            => cfg_trn_pending ,
  cfg_pm_halt_aspm_l0s                       => cfg_pm_halt_aspm_l0s ,
  cfg_pm_halt_aspm_l1                        => cfg_pm_halt_aspm_l1 ,
  cfg_pm_force_state_en                      => cfg_pm_force_state_en ,
  cfg_pm_force_state                         => cfg_pm_force_state ,

  ---------------------------------------------------------------------
   -- EP Only                                                        --
  ---------------------------------------------------------------------

  cfg_interrupt                              => cfg_interrupt ,
  cfg_interrupt_rdy                          => cfg_interrupt_rdy ,
  cfg_interrupt_assert                       => cfg_interrupt_assert ,
  cfg_interrupt_di                           => cfg_interrupt_di ,
  cfg_interrupt_do                           => cfg_interrupt_do ,
  cfg_interrupt_mmenable                     => cfg_interrupt_mmenable ,
  cfg_interrupt_msienable                    => cfg_interrupt_msienable ,
  cfg_interrupt_msixenable                   => cfg_interrupt_msixenable ,
  cfg_interrupt_msixfm                       => cfg_interrupt_msixfm ,
  cfg_interrupt_stat                         => cfg_interrupt_stat ,
  cfg_pciecap_interrupt_msgnum               => cfg_pciecap_interrupt_msgnum ,
  cfg_to_turnoff                             => cfg_to_turnoff ,
  cfg_turnoff_ok                             => cfg_turnoff_ok ,
  cfg_bus_number                             => cfg_bus_number ,
  cfg_device_number                          => cfg_device_number ,
  cfg_function_number                        => cfg_function_number ,
  cfg_pm_wake                                => cfg_pm_wake ,

  ---------------------------------------------------------------------
   -- RP Only                                                        --
  ---------------------------------------------------------------------
  cfg_pm_send_pme_to                         => '0' ,
  cfg_ds_bus_number                          => x"00" ,
  cfg_ds_device_number                       => "00000" ,
  cfg_ds_function_number                     => "000" ,
  cfg_mgmt_wr_rw1c_as_rw                     => '0' ,
  cfg_msg_received                           => open ,
  cfg_msg_data                               => open ,

  cfg_bridge_serr_en                         => open ,
  cfg_slot_control_electromech_il_ctl_pulse  => open ,
  cfg_root_control_syserr_corr_err_en        => open ,
  cfg_root_control_syserr_non_fatal_err_en   => open ,
  cfg_root_control_syserr_fatal_err_en       => open ,
  cfg_root_control_pme_int_en                => open ,
  cfg_aer_rooterr_corr_err_reporting_en      => open ,
  cfg_aer_rooterr_non_fatal_err_reporting_en => open ,
  cfg_aer_rooterr_fatal_err_reporting_en     => open ,
  cfg_aer_rooterr_corr_err_received          => open ,
  cfg_aer_rooterr_non_fatal_err_received     => open ,
  cfg_aer_rooterr_fatal_err_received         => open ,

  cfg_msg_received_err_cor                   => open ,
  cfg_msg_received_err_non_fatal             => open ,
  cfg_msg_received_err_fatal                 => open ,
  cfg_msg_received_pm_as_nak                 => open ,
  cfg_msg_received_pm_pme                    => open ,
  cfg_msg_received_pme_to_ack                => open ,
  cfg_msg_received_assert_int_a              => open ,
  cfg_msg_received_assert_int_b              => open ,
  cfg_msg_received_assert_int_c              => open ,
  cfg_msg_received_assert_int_d              => open ,
  cfg_msg_received_deassert_int_a            => open ,
  cfg_msg_received_deassert_int_b            => open ,
  cfg_msg_received_deassert_int_c            => open ,
  cfg_msg_received_deassert_int_d            => open ,

  -------------------------------------------------------------------------------------------------------------------
  -- 5. Physical Layer Control and Status (PL) Interface                                                           --
  -------------------------------------------------------------------------------------------------------------------
  pl_directed_link_auton                     => pl_directed_link_auton ,
  pl_directed_link_change                    => pl_directed_link_change ,
  pl_directed_link_speed                     => pl_directed_link_speed ,
  pl_directed_link_width                     => pl_directed_link_width ,
  pl_upstream_prefer_deemph                  => pl_upstream_prefer_deemph ,

  pl_sel_lnk_rate                            => pl_sel_lnk_rate ,
  pl_sel_lnk_width                           => pl_sel_lnk_width ,
  pl_ltssm_state                             => pl_ltssm_state ,
  pl_lane_reversal_mode                      => pl_lane_reversal_mode ,

  pl_phy_lnk_up                              => open ,
  pl_tx_pm_state                             => open ,
  pl_rx_pm_state                             => open ,

  cfg_dsn                                    => cfg_dsn ,

  pl_link_upcfg_cap                          => pl_link_upcfg_cap ,
  pl_link_gen2_cap                           => pl_link_gen2_cap ,
  pl_link_partner_gen2_supported             => pl_link_partner_gen2_supported ,
  pl_initial_link_width                      => pl_initial_link_width ,

  pl_directed_change_done                    => open ,

  ---------------------------------------------------------------------
   -- EP Only                                                        --
  ---------------------------------------------------------------------
  pl_received_hot_rst                        => pl_received_hot_rst ,

  ---------------------------------------------------------------------
   -- RP Only                                                        --
  ---------------------------------------------------------------------
  pl_transmit_hot_rst                        => '0' ,
  pl_downstream_deemph_source                => '0' ,

  -------------------------------------------------------------------------------------------------------------------
  -- 6. AER interface                                                                                              --
  -------------------------------------------------------------------------------------------------------------------
  cfg_err_aer_headerlog                      => cfg_err_aer_headerlog ,
  cfg_aer_interrupt_msgnum                   => cfg_aer_interrupt_msgnum ,
  cfg_err_aer_headerlog_set                  => cfg_err_aer_headerlog_set ,
  cfg_aer_ecrc_check_en                      => cfg_aer_ecrc_check_en ,
  cfg_aer_ecrc_gen_en                        => cfg_aer_ecrc_gen_en ,

  -------------------------------------------------------------------------------------------------------------------
  -- 7. VC interface                                                                                               --
  -------------------------------------------------------------------------------------------------------------------
  cfg_vc_tcvc_map                            => open ,

  -------------------------------------------------------------------------------------------------------------------
  -- 8. System(SYS) Interface                                                                                      --
  -------------------------------------------------------------------------------------------------------------------
  sys_clk                                    => sys_clk ,
  sys_rst_n                                  => sys_rst_n_c

);

app : pcie_app_7x
  port map(

  -------------------------------------------------------------------------------------------------------------------
  -- 1. AXI-S Interface                                                                                            --
  -------------------------------------------------------------------------------------------------------------------
  -- Common
  user_clk                       => user_clk ,
  user_reset                     => user_reset_q ,
  user_lnk_up                    => user_lnk_up_q ,

  -- TX
  tx_buf_av                      => tx_buf_av ,
  tx_cfg_req                     => tx_cfg_req ,
  tx_err_drop                    => tx_err_drop ,
  s_axis_tx_tready               => s_axis_tx_tready ,
  s_axis_tx_tdata                => s_axis_tx_tdata ,
  s_axis_tx_tkeep                => s_axis_tx_tkeep ,
  s_axis_tx_tlast                => s_axis_tx_tlast ,
  s_axis_tx_tvalid               => s_axis_tx_tvalid ,
  s_axis_tx_tuser                => s_axis_tx_tuser,
  tx_cfg_gnt                     => tx_cfg_gnt ,

  -- RX
  m_axis_rx_tdata                => m_axis_rx_tdata ,
  m_axis_rx_tkeep                => m_axis_rx_tkeep ,
  m_axis_rx_tlast                => m_axis_rx_tlast ,
  m_axis_rx_tvalid               => m_axis_rx_tvalid ,
  m_axis_rx_tuser                => m_axis_rx_tuser,
  m_axis_rx_tready               => m_axis_rx_tready ,
  rx_np_ok                       => rx_np_ok ,
  rx_np_req                       => rx_np_req ,

  -- Flow Control
  fc_cpld                        => fc_cpld ,
  fc_cplh                        => fc_cplh ,
  fc_npd                         => fc_npd ,
  fc_nph                         => fc_nph ,
  fc_pd                          => fc_pd ,
  fc_ph                          => fc_ph ,
  fc_sel                         => fc_sel ,

  --------------------------------------------------------------------------------------------------------------------
  -- 2. Configuration (CFG) Interface                                                                               --
  --------------------------------------------------------------------------------------------------------------------
  cfg_err_cor                    => cfg_err_cor ,
  cfg_err_atomic_egress_blocked  => cfg_err_atomic_egress_blocked ,
  cfg_err_internal_cor           => cfg_err_internal_cor ,
  cfg_err_malformed              => cfg_err_malformed ,
  cfg_err_mc_blocked             => cfg_err_mc_blocked ,
  cfg_err_poisoned               => cfg_err_poisoned ,
  cfg_err_norecovery             => cfg_err_norecovery ,
  cfg_err_ur                     => cfg_err_ur ,
  cfg_err_ecrc                   => cfg_err_ecrc ,
  cfg_err_cpl_timeout            => cfg_err_cpl_timeout ,
  cfg_err_cpl_abort              => cfg_err_cpl_abort ,
  cfg_err_cpl_unexpect           => cfg_err_cpl_unexpect ,
  cfg_err_posted                 => cfg_err_posted ,
  cfg_err_locked                 => cfg_err_locked ,
  cfg_err_acs                    => cfg_err_acs ,
  cfg_err_internal_uncor         => cfg_err_internal_uncor ,
  cfg_err_tlp_cpl_header         => cfg_err_tlp_cpl_header ,
  cfg_err_cpl_rdy                => cfg_err_cpl_rdy ,
  cfg_interrupt                  => cfg_interrupt ,
  cfg_interrupt_rdy              => cfg_interrupt_rdy ,
  cfg_interrupt_assert           => cfg_interrupt_assert ,
  cfg_interrupt_di               => cfg_interrupt_di ,
  cfg_interrupt_do               => cfg_interrupt_do ,
  cfg_interrupt_mmenable         => cfg_interrupt_mmenable ,
  cfg_interrupt_msienable        => cfg_interrupt_msienable ,
  cfg_interrupt_msixenable       => cfg_interrupt_msixenable ,
  cfg_interrupt_msixfm           => cfg_interrupt_msixfm ,
  cfg_interrupt_stat             => cfg_interrupt_stat ,
  cfg_pciecap_interrupt_msgnum   => cfg_pciecap_interrupt_msgnum ,
  cfg_turnoff_ok                 => cfg_turnoff_ok ,
  cfg_to_turnoff                 => cfg_to_turnoff ,

  cfg_trn_pending                => cfg_trn_pending ,
  cfg_pm_halt_aspm_l0s           => cfg_pm_halt_aspm_l0s ,
  cfg_pm_halt_aspm_l1            => cfg_pm_halt_aspm_l1 ,
  cfg_pm_force_state_en          => cfg_pm_force_state_en ,
  cfg_pm_force_state             => cfg_pm_force_state ,

  cfg_pm_wake                    => cfg_pm_wake ,
  cfg_bus_number                 => cfg_bus_number ,
  cfg_device_number              => cfg_device_number ,
  cfg_function_number            => cfg_function_number ,
  cfg_status                     => cfg_status ,
  cfg_command                    => cfg_command ,
  cfg_dstatus                    => cfg_dstatus ,
  cfg_dcommand                   => cfg_dcommand ,
  cfg_lstatus                    => cfg_lstatus ,
  cfg_lcommand                   => cfg_lcommand ,
  cfg_dcommand2                  => cfg_dcommand2 ,
  cfg_pcie_link_state            => cfg_pcie_link_state ,
  cfg_dsn                        => cfg_dsn ,

  --------------------------------------------------------------------------------------------------------------------
  -- 3. Management (MGMT) Interface                                                                                 --
  --------------------------------------------------------------------------------------------------------------------
  cfg_mgmt_di                    => cfg_mgmt_di ,
  cfg_mgmt_byte_en               => cfg_mgmt_byte_en ,
  cfg_mgmt_dwaddr                => cfg_mgmt_dwaddr ,
  cfg_mgmt_wr_en                 => cfg_mgmt_wr_en ,
  cfg_mgmt_rd_en                 => cfg_mgmt_rd_en ,
  cfg_mgmt_wr_readonly           => cfg_mgmt_wr_readonly ,

  -------------------------------------------------------------------------------------------------------------------
  -- 3. Advanced Error Reporting (AER) Interface                                                                   --
  -------------------------------------------------------------------------------------------------------------------
  cfg_err_aer_headerlog          => cfg_err_aer_headerlog ,
  cfg_aer_interrupt_msgnum       => cfg_aer_interrupt_msgnum ,
  cfg_err_aer_headerlog_set      => cfg_err_aer_headerlog_set ,
  cfg_aer_ecrc_check_en          => cfg_aer_ecrc_check_en ,
  cfg_aer_ecrc_gen_en            => cfg_aer_ecrc_gen_en ,

  -------------------------------------------------------------------------------------------------------------------
  -- 4. Physical Layer Control and Status (PL) Interface                                                           --
  -------------------------------------------------------------------------------------------------------------------
  pl_initial_link_width          => pl_initial_link_width ,
  pl_lane_reversal_mode          => pl_lane_reversal_mode ,
  pl_link_gen2_cap               => pl_link_gen2_cap ,
  pl_link_partner_gen2_supported => pl_link_partner_gen2_supported ,
  pl_link_upcfg_cap              => pl_link_upcfg_cap ,
  pl_ltssm_state                 => pl_ltssm_state ,
  pl_received_hot_rst            => pl_received_hot_rst ,
  pl_sel_lnk_rate                => pl_sel_lnk_rate ,
  pl_sel_lnk_width               => pl_sel_lnk_width ,
  pl_directed_link_auton         => pl_directed_link_auton ,
  pl_directed_link_change        => pl_directed_link_change ,
  pl_directed_link_speed         => pl_directed_link_speed ,
  pl_directed_link_width         => pl_directed_link_width ,
  pl_upstream_prefer_deemph      => pl_upstream_prefer_deemph,
  
  
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
           
-- HOST DMA Data 
	-- RX FIFO
	m_axi_tvalid	=> m_axi_tvalid,
	m_axi_tready	=> m_axi_tready,
	m_axi_tdata		=> m_axi_tdata,
	 
	-- TX FIFO
	s_axi_tvalid	=> s_axi_tvalid,
	s_axi_tready	=> s_axi_tready,
	s_axi_tdata		=> s_axi_tdata,
		   


---------------------------------------------------
-- User IP interface

	ip_clk		    => ip_clk,
	user_rst_n		=> user_rst_n_sig,

-- HOST DMA Data 	
	m_user_tvalid		=> m_user_tvalid,--m_tvalid,--m_user_tvalid,
	m_user_tready		=> m_user_tready,--m_tready,--m_user_tready,
	m_user_tdata		=> m_user_tdata,--m_tdata,--m_user_tdata,
	 
	-- TX FIFO
	s_user_tvalid		=> s_user_tvalid,--s_tvalid,--s_user_tvalid,
	s_user_tready		=> s_user_tready,--s_tready,--s_user_tready,
	s_user_tdata		=> s_user_tdata,--s_tdata,--s_user_tdata,

  --  Registers interface
  	Reg_RE		=> Reg_RE,
  	Reg_WE		=> Reg_WE,	
  	Reg_Addr		=> Reg_Addr,
  	Reg_Dout		=> Reg_Dout,
  	Reg_Din		=> Reg_Din,
  	Reg_Valid	=> Reg_Valid,
  	
  	pcie_int_req	=> pcie_int_req,
  	pcie_int_compl	=> pcie_int_compl,
  
  	-- EP tx interface
  	-- EP TX CMD FIFO interface 
  	ep_tx_cmd_wr_en		=> ep_tx_cmd_wr_en,
  	ep_tx_cmd_data		=> ep_tx_cmd_data,
  	ep_tx_cmd_full		=> ep_tx_cmd_full,
  	
  	ep_tx_error			=> ep_tx_error,
  	ep_tx_tag_error		=> ep_tx_tag_error,
  	
  	ep_txd_s_tvalid		=> ep_txd_s_tvalid,
  	ep_txd_s_tdata		=> ep_txd_s_tdata,
  	ep_txd_s_tready		=> ep_txd_s_tready,
  	
  	-- EP rx interface	
  	ep_rxd_m_tvalid	=> ep_rxd_m_tvalid,
  	ep_rxd_m_tready => ep_rxd_m_tready,
  	ep_rxd_m_tdata	=> ep_rxd_m_tdata,
  	InitDoneBtm     => InitDoneBottom_s,
  	InitDoneTop    => InitDoneTop_s,
  	
  	use_AXI         => use_AXI	  
  
  );
  
  
  --m_axi_tready  <= '0';
  
  --s_axi_tvalid  <= '0';
  --s_axi_tdata    <= (others => '0'); 
  
--cntl :  cbus 
--	port map (
--	cbus_clk  	=> cbus_clk,

--	id      => id,
--	strtcmd => strtcmd,
--	command	=> command,
--	dval    => dval,

--    -- CBus 
--    -- to USER IP
--    hardid	=> open,
--    mac		=> open,
--    userid	=> open,

--    OutReg0	=> open,
--    OutReg1	=> open,
--    OutReg2	=> open,
--    OutReg3	=> open,
--    OutReg4	=> open,
--    OutReg5	=> open,
--    OutReg6	=> open,

--    -- from USER IP
--    InReg0	=> InReg0,
--   	InReg1	=> InReg1,
--   	InReg2	=> InReg2,
--   	InReg3	=> InReg3,
--   	InReg4	=> InReg4,
--   	InReg5	=> InReg5,
--   	InReg6	=> InReg6,
   	
--    -- S6 FIFO Interface
--    fifo_clk 	=> ip_clk,
--    fifo_rst 	=> fifo_rst,
--    -- RX FIFO
--    s6_v7_empty => s6_v7_empty,
--    s6_v7_dout 	=> s6_v7_dout,
--    s6_v7_rd_en => s6_v7_rd_en,
--    s6_v7_data_count => s6_v7_data_count,
--    -- TX FIFO
--    v7_s6_din 	=> v7_s6_din,
--    v7_s6_wr_en => v7_s6_wr_en,
--    v7_s6_full  => v7_s6_full
    

--    );  
 
-- InReg0 <= X"12345678";
-- InReg1 <= X"12345678";
-- InReg2 <= X"12345678";
-- InReg3 <= X"12345678";
-- InReg4 <= X"12345678";
-- InReg5 <= X"12345678";
-- InReg6 <= X"12345678";
          
    
    axi_system : axi_system_wrapper
    port map (
    sys_rst_n        =>		user_rst_n_sig      ,
--    sys_rst_out_n    =>     open,--sys_rst_out_n  ,
    ip_clk          =>     ip_clk ,                             
    clk_in         =>     user_clk       ,
    --clk_out          =>     axi_clk_out        ,
    
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
	  
      -- S02_AXI_wid(0 downto 0)                          => S02_AXI_wid(0 downto 0),       
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
      S02_AXI_rready                         =>         S02_AXI_rready,
          
  -- User interface
  
      -- MM2S CMD interface
      StartPulse_mm2s                 =>  StartPulse_mm2s       ,   
      StartAddrB_mm2s                 =>  StartAddrB_mm2s       ,
      BytesToTransfer_mm2s          =>  BytesToTransfer_mm2s    ,
      CmdTag_mm2s                   =>  CmdTag_mm2s             ,
      CmdAck_mm2s                   =>  CmdAck_mm2s             ,
          
          
      -- S2MM CMD interface
      StartPulse_s2mm                 => StartPulse_s2mm        ,
      StartAddrB_s2mm               => StartAddrB_s2mm          ,
      BytesToTransfer_s2mm            => BytesToTransfer_s2mm   ,
      CmdTag_s2mm                    => CmdTag_s2mm             ,    
      CmdAck_s2mm                    => CmdAck_s2mm             ,        
                             
      -- AXI Stream Master interface (Read Status from AXI)
      -- To User IP
      m_axis_s2mm_sts_tvalid         =>   m_axis_s2mm_sts_tvalid  ,
      m_axis_s2mm_sts_tready        =>   m_axis_s2mm_sts_tready   ,
      m_axis_s2mm_sts_tdata          =>   m_axis_s2mm_sts_tdata   ,
                                                                 
      m_axis_mm2s_sts_tvalid         =>   m_axis_mm2s_sts_tvalid  ,
      m_axis_mm2s_sts_tready        =>   m_axis_mm2s_sts_tready   ,
      m_axis_mm2s_sts_tdata          =>   m_axis_mm2s_sts_tdata   ,
                                                                 
      -- AXI Stream Master Fifo interface (Write to fifo)        
      m_axis_txd_tvalid         =>  s_axi_tvalid,  --m_axis_txd_tvalid          ,
      m_axis_txd_tready         =>  s_axi_tready,--m_axis_txd_tready            ,
      m_axis_txd_tdata          =>  s_axi_tdata,--m_axis_txd_tdata             ,
                                                                 
      -- AXI Stream Slave Fifo interface (Read from fifo)        
      s_axis_rxd_tvalid         => m_axi_tvalid,--s_axis_rxd_tvalid            ,         
      s_axis_rxd_tready         => m_axi_tready,--s_axis_rxd_tready            ,
      s_axis_rxd_tdata          => m_axi_tdata,--s_axis_rxd_tdata             ,
          
  
      dm_mm2s_err    => dm_mm2s_err       ,                                
      dm_s2mm_err    => dm_s2mm_err       ,
                                            
      
  -- DDR PHY Interface    
    
      Top_ddr_dq       =>    Top_ddr_dq         ,
      Top_ddr_dqs_p    =>    Top_ddr_dqs_p      ,
      Top_ddr_dqs_n    =>    Top_ddr_dqs_n      ,
      Top_ddr_addr     =>    Top_ddr_addr       ,
      Top_ddr_ba       =>    Top_ddr_ba         ,
      Top_ddr_ras_n    =>    Top_ddr_ras_n      ,
      Top_ddr_cas_n    =>    Top_ddr_cas_n      ,
      Top_ddr_we_n     =>    Top_ddr_we_n       ,
      Top_ddr_reset_n  =>    Top_ddr_reset_n ,
      Top_ddr_ck_p     =>    Top_ddr_ck_p    ,
      Top_ddr_ck_n     =>    Top_ddr_ck_n    ,
      Top_ddr_cke      =>    Top_ddr_cke     ,
      Top_ddr_cs_n     =>    Top_ddr_cs_n    ,
      Top_ddr_dm       =>    Top_ddr_dm      ,
      Top_ddr_odt      =>    Top_ddr_odt     ,
                                            
  
--      Btm_ddr_dq      =>    Btm_ddr_dq       ,
--      Btm_ddr_dqs_p   =>    Btm_ddr_dqs_p    ,
--      Btm_ddr_dqs_n   =>    Btm_ddr_dqs_n    ,
--      Btm_ddr_addr    =>    Btm_ddr_addr     ,
--      Btm_ddr_ba      =>    Btm_ddr_ba       ,
--      Btm_ddr_ras_n   =>    Btm_ddr_ras_n    ,
--      Btm_ddr_cas_n   =>    Btm_ddr_cas_n    ,
--      Btm_ddr_we_n    =>    Btm_ddr_we_n     ,
--      Btm_ddr_reset_n =>    Btm_ddr_reset_n  ,
--      Btm_ddr_ck_p    =>    Btm_ddr_ck_p     ,
--      Btm_ddr_ck_n    =>    Btm_ddr_ck_n     ,
--      Btm_ddr_cke     =>    Btm_ddr_cke      ,
--      Btm_ddr_cs_n    =>    Btm_ddr_cs_n     ,
--      Btm_ddr_dm      =>    Btm_ddr_dm       ,
--      Btm_ddr_odt     =>    Btm_ddr_odt      ,
  
      InitDoneTop     =>    InitDoneTop_s       ,
      InitDoneBtm     =>    InitDoneBottom_s    ,
      clk_axi         =>    clk_axi
      --clk_axi_ref     =>    clk_axi_ref    
      
    
    ); 
    
    --axi_clk_out<= clk_axi;

end pcie_7x;