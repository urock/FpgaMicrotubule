----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 		urock
-- 
-- Create Date:    16:47:37 08/09/2012 

-- Tool versions: 
-- Description: 
--
-- Dependencies: 	DataMover IP Core HW_VER = 3.00.a
--
-- Revision: 
-- Revision 0.02 - Don't use move signal and tlast is formed in a different way

-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity stream_axi_bridge is
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
	
end stream_axi_bridge;

architecture rtl of stream_axi_bridge is


  -------------------------------------------------------------------
  -- Function
  -------------------------------------------------------------------
  function calc_btt_lsb (input_value : integer) return integer is
    Variable lbs : Integer := 2;
  begin

    if (input_value = 32) then

       lbs := 2;

    elsif (input_value = 64) then

       lbs := 3;

    else

       lbs := 3;

    end if;
	 
    Return (lbs);
  end function calc_btt_lsb;
  -------------------------------------------------------------------
  
  
   Constant BTT_LSB_s2mm       : integer range 0 to 4 := calc_btt_lsb(C_AXIS_DWIDTH_s2mm);
   Constant BTT_LSB_mm2s       : integer range 0 to 4 := calc_btt_lsb(C_AXIS_DWIDTH_mm2s);





component axi_cmd_fifo
  PORT (
    rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(71 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(71 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC
  );
end component;


component axi_data_fifo
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

component axi_sts_fifo IS
  PORT (
    m_aclk : IN STD_LOGIC;
    s_aclk : IN STD_LOGIC;
    s_aresetn : IN STD_LOGIC;
    s_axis_tvalid : IN STD_LOGIC;
    s_axis_tready : OUT STD_LOGIC;
    s_axis_tdata : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    m_axis_tvalid : OUT STD_LOGIC;
    m_axis_tready : IN STD_LOGIC;
    m_axis_tdata : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END component;


signal move_mm2s		: std_logic;
signal move_s2mm		: std_logic;
signal tlast	: std_logic;

signal transfer_count 	: integer range 0 to 4*1024*1024; 

signal transfer_limit 	: integer range 0 to 4*1024*1024; 

type s2mm_stm is (IDLE_ST, TRANSFER_ST);
signal s2mm_state	: s2mm_stm; 


type mm2s_stm is (IDLE_ST, TRANSFER_ST);
signal mm2s_state	: mm2s_stm; 


signal inc	: std_logic; 

signal m_axis_cmd_mm2s_tvalid_n : std_logic;
signal m_axis_cmd_s2mm_tvalid_n : std_logic;
signal m_axis_s2mm_tvalid_n : std_logic;
signal mm2s_full : std_logic;
signal m_axis_txd_tvalid_n : std_logic;
signal s2mm_full : std_logic;
signal s2mm_d_wr_en : std_logic;
signal s2mm_d_rd_en : std_logic;

signal mm2s_d_wr_en : std_logic;
signal mm2s_d_rd_en : std_logic;

signal mm2s_c_din : std_logic_vector(71 downto 0);
signal mm2s_c_rd_en : std_logic;

signal s2mm_c_din : std_logic_vector(71 downto 0);
signal s2mm_c_rd_en : std_logic;


signal s2mm_axi_cmd_fifo_dout   :  std_logic_vector(71 downto 0);



attribute KEEP : string;
attribute KEEP of m_axis_s2mm_tvalid			: signal is "TRUE";
attribute KEEP of m_axis_s2mm_tready			: signal is "TRUE";
attribute KEEP of s_axis_rxd_tready			: signal is "TRUE";
attribute KEEP of s_axis_rxd_tvalid			: signal is "TRUE";
attribute KEEP of m_axis_s2mm_tdata			: signal is "TRUE";
attribute KEEP of move_mm2s			: signal is "TRUE";
attribute KEEP of move_s2mm			: signal is "TRUE";
attribute KEEP of tlast			: signal is "TRUE";
attribute KEEP of transfer_count			: signal is "TRUE";
attribute KEEP of inc			: signal is "TRUE";

attribute KEEP of m_axis_cmd_s2mm_tvalid			: signal is "TRUE";
attribute KEEP of m_axis_cmd_s2mm_tready			: signal is "TRUE";
attribute KEEP of m_axis_cmd_s2mm_tdata			: signal is "TRUE";

attribute KEEP of m_axis_cmd_mm2s_tvalid			: signal is "TRUE";
attribute KEEP of m_axis_cmd_mm2s_tready			: signal is "TRUE";
attribute KEEP of m_axis_cmd_mm2s_tdata			: signal is "TRUE";

--attribute KEEP of s_axis_sts_tvalid			: signal is "TRUE";
--attribute KEEP of s_axis_sts_tready			: signal is "TRUE";
--attribute KEEP of s_axis_sts_tdata			: signal is "TRUE";





begin



m_axis_s2mm_sts_tkeep 	 <= m_axis_s2mm_sts_tkeep_i;
m_axis_s2mm_sts_tlast 	 <= m_axis_s2mm_sts_tlast_i;
m_axis_mm2s_sts_tkeep 	 <= m_axis_mm2s_sts_tkeep_i;
m_axis_mm2s_sts_tlast 	 <= m_axis_mm2s_sts_tlast_i;

s2mm_axi_sts_fifo: axi_sts_fifo
port map (

    m_aclk              => aclk,
    s_aclk              => clk_axi,
    s_aresetn           => aReset_n, 
    s_axis_tvalid       => m_axis_s2mm_sts_tvalid_i,
    s_axis_tready       => m_axis_s2mm_sts_tready_i,
    s_axis_tdata        => m_axis_s2mm_sts_tdata_i,
    m_axis_tvalid       => m_axis_s2mm_sts_tvalid,
    m_axis_tready       => m_axis_s2mm_sts_tready,
    m_axis_tdata        => m_axis_s2mm_sts_tdata
); 

mm2s_axi_sts_fifo: axi_sts_fifo
port map (

    m_aclk              => aclk,
    s_aclk              => clk_axi,
    s_aresetn           => aReset_n, 
    s_axis_tvalid       => m_axis_mm2s_sts_tvalid_i,
    s_axis_tready       => m_axis_mm2s_sts_tready_i,
    s_axis_tdata        => m_axis_mm2s_sts_tdata_i,
    m_axis_tvalid       => m_axis_mm2s_sts_tvalid,
    m_axis_tready       => m_axis_mm2s_sts_tready,
    m_axis_tdata        => m_axis_mm2s_sts_tdata
); 





s_axis_rxd_tready<= not s2mm_full; 

s_axis_mm2s_tready<= (not mm2s_full) ;--and move_mm2s;
mm2s_axi_cmd_fifo: axi_cmd_fifo
port map (
	rst 	=> not aReset_n,
	wr_clk 	=> aclk,
	rd_clk 	=> clk_axi,
	din 	=> mm2s_c_din,
	wr_en 	=> StartPulse_mm2s,
	
	rd_en 	=> mm2s_c_rd_en,
	dout 	=> m_axis_cmd_mm2s_tdata,
	
	full 	=> open,
	empty 	=> m_axis_cmd_mm2s_tvalid_n
);	

mm2s_c_din<="0000" & CmdTag_mm2s & StartAddrB_mm2s & '1' & '1' & "000000" & '1' & BytesToTransfer_mm2s;
mm2s_c_rd_en<=m_axis_cmd_mm2s_tready and (not m_axis_cmd_mm2s_tvalid_n);

s2mm_axi_cmd_fifo: axi_cmd_fifo
port map (
	rst 	=> not aReset_n,
	wr_clk 	=> aclk,
	rd_clk 	=> clk_axi,
	din 	=> s2mm_c_din,
	wr_en 	=> StartPulse_s2mm,
	
	rd_en 	=> s2mm_c_rd_en,
	dout 	=> s2mm_axi_cmd_fifo_dout,
	
	full 	=> open,
	empty 	=> m_axis_cmd_s2mm_tvalid_n
);	

m_axis_cmd_s2mm_tdata   <= s2mm_axi_cmd_fifo_dout; 

--process(clk_axi, aReset_n)
--begin
--if aReset_n = '0' then
--m_axis_s2mm_tvalid<='0';
--m_axis_cmd_s2mm_tvalid<='0';
--m_axis_cmd_mm2s_tvalid<= '0';

--elsif rising_edge(clk_axi) then

--m_axis_s2mm_tvalid<=not m_axis_s2mm_tvalid_n;
--m_axis_cmd_s2mm_tvalid <= not m_axis_cmd_s2mm_tvalid_n;
--m_axis_cmd_mm2s_tvalid <= not m_axis_cmd_mm2s_tvalid_n;


--end if;
--end process;

    m_axis_s2mm_tvalid<=not m_axis_s2mm_tvalid_n;
    m_axis_cmd_s2mm_tvalid <= not m_axis_cmd_s2mm_tvalid_n;
    m_axis_cmd_mm2s_tvalid <= not m_axis_cmd_mm2s_tvalid_n;

s2mm_c_din<="0000" & CmdTag_s2mm & StartAddrB_s2mm & '1' & '1' & "000000" & '1' & BytesToTransfer_s2mm;
s2mm_c_rd_en<=m_axis_cmd_s2mm_tready and (not m_axis_cmd_s2mm_tvalid_n);
mm2s_axi_data_fifo: axi_data_fifo
port map (
	rst 	=> not aReset_n,
	wr_clk 	=> clk_axi,
	rd_clk 	=> aclk,
	din 	=> s_axis_mm2s_tdata,
	wr_en 	=> mm2s_d_wr_en,
	
	rd_en 	=> mm2s_d_rd_en,
	dout 	=> m_axis_txd_tdata,
	
	full 	=> mm2s_full,
	empty 	=> m_axis_txd_tvalid_n
);

mm2s_d_wr_en<= s_axis_mm2s_tvalid and (not mm2s_full) ;--and move_mm2s;
mm2s_d_rd_en<= (not m_axis_txd_tvalid_n) and m_axis_txd_tready;
m_axis_txd_tvalid<= not m_axis_txd_tvalid_n;

s2mm_axi_data_fifo: axi_data_fifo
port map (
	rst 	=> not aReset_n,
	wr_clk 	=> aclk,
	rd_clk 	=> clk_axi,
	din 	=> s_axis_rxd_tdata,
	wr_en 	=> s2mm_d_wr_en,
	
	rd_en 	=> s2mm_d_rd_en,
	dout 	=> m_axis_s2mm_tdata,
	
	full 	=> s2mm_full,
	empty 	=> m_axis_s2mm_tvalid_n
);



s2mm_d_wr_en<= s_axis_rxd_tvalid and (not s2mm_full);
s2mm_d_rd_en<= (not m_axis_s2mm_tvalid_n) and m_axis_s2mm_tready;

-----------------------------------------------------------------
--  Datapath s2mm
	--m_axis_s2mm_tvalid	<= s_axis_rxd_tvalid and move_s2mm; 
	--s_axis_rxd_tready		<= m_axis_s2mm_tready and move_s2mm; 
--	m_axis_s2mm_tvalid	<= s_axis_rxd_tvalid; 
--	s_axis_rxd_tready		<= m_axis_s2mm_tready; 
	
	--m_axis_s2mm_tdata		<= s_axis_rxd_tdata; 
	m_axis_s2mm_tkeep		<= X"FF"; 
	m_axis_s2mm_tlast		<= tlast; 
	
	
-- Datapath mm2s

	--m_axis_txd_tvalid		<= s_axis_mm2s_tvalid and move_mm2s;
	--s_axis_mm2s_tready		<= m_axis_txd_tready and move_mm2s; 
	--m_axis_txd_tdata		<= s_axis_mm2s_tdata; 	
	
	
---------------------------------------------------------------------
-- FIFO to S2MM Transfer controller
-- main issue is to generate tlast signal and stop data transfer 
-- after moving required ammount of data
---------------------------------------------------------------------	

	move_s2mm 	<= '1' when s2mm_state = TRANSFER_ST else '0'; 
	
	tlast	<= '1' when 	(transfer_count = transfer_limit) and 
							(m_axis_s2mm_tvalid_n = '0') and 
							(m_axis_s2mm_tready = '1') 
				else '0'; 
	
	inc	<= '1' when ((not m_axis_s2mm_tvalid_n) and m_axis_s2mm_tready) = '1' else '0';
	
	---------------------------------------------------------------------
-- MM2S Transfer controller
-- After starting transfer wait for tlast signal from Datamover
---------------------------------------------------------------------	

	move_mm2s 	<= '1' when mm2s_state = TRANSFER_ST else '0'; 
	
	process(clk_axi, aReset_n)
	begin
		if aReset_n = '0' then
			transfer_count <= 0;
			transfer_limit <= 0; 	
			
			s2mm_state	<= IDLE_ST; 
			
		elsif rising_edge(clk_axi) then
		
			
					
			case s2mm_state is
				
				when IDLE_ST => 
				
					if (m_axis_cmd_s2mm_tvalid_n = '0') then
						transfer_limit	<= conv_integer(s2mm_axi_cmd_fifo_dout(22 downto BTT_LSB_s2mm)) - 1; 
						s2mm_state		<= TRANSFER_ST;
					end if;
				
				when TRANSFER_ST	=>
					
--					move	<= '1'; 
					if tlast = '1' then
						s2mm_state	<= IDLE_ST;
						transfer_count <= 0;
					elsif inc = '1' then
						transfer_count <= transfer_count + 1;						-- counts DWORDS (4 bytes) 
					end if;				
				
				when others =>
					s2mm_state	<= IDLE_ST;
				
			end case; 
		
		end if;
	end process;
	
	

	process(clk_axi, aReset_n)
	begin
		if aReset_n = '0' then
			
			mm2s_state	<= IDLE_ST; 
			
		elsif rising_edge(clk_axi) then
					
			case mm2s_state is
				
				when IDLE_ST => 
				
					if (m_axis_cmd_mm2s_tvalid_n = '0')then
						mm2s_state	<= TRANSFER_ST;
					end if;
				
				when TRANSFER_ST	=>
					
					if s_axis_mm2s_tlast = '1' then
						mm2s_state	<= IDLE_ST;
					end if;
				
				when others =>
					mm2s_state	<= IDLE_ST;
				
			end case; 
		
		end if;
	end process;	

end rtl;

