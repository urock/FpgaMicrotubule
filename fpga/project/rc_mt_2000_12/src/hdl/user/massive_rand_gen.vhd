----------------------------------------------------------------------------------
-- Company: 
-- Engineer:	urock
-- Create Date: 06/26/2015 04:44:01 PM
-- Module Name: massive_rand_gen - Behavioral

-- Project Name: 

-- Description: 5 rand gen + seed, start and idle control logic
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.std_logic_1164.ALL;


entity massive_rand_gen is
	port (
		ap_clk 					: in  std_logic;
		ap_rst_n 				: in  std_logic;

		ap_start 				: in  std_logic;

		ap_idle 				: out std_logic_vector(4 downto 0);
		
		rand_enable				: in  std_logic; 

		seed0					: in  std_logic_vector (31 downto 0);
		seed1					: in  std_logic_vector (31 downto 0);
		seed2					: in  std_logic_vector (31 downto 0);
		seed3					: in  std_logic_vector (31 downto 0);
		seed4					: in  std_logic_vector (31 downto 0);
		seed5					: in  std_logic_vector (31 downto 0);
		seed6					: in  std_logic_vector (31 downto 0);
		seed7					: in  std_logic_vector (31 downto 0);
		seed8					: in  std_logic_vector (31 downto 0);
		seed9					: in  std_logic_vector (31 downto 0);
			
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
end massive_rand_gen;

architecture Behavioral of massive_rand_gen is


component rand_wrapper
	port (
		ap_clk 					: in std_logic;
		ap_rst_n 				: in std_logic;
		ap_start 				: in std_logic;
		ap_done 				: out std_logic;
		ap_idle 				: out std_logic;
		ap_ready 				: out std_logic;
		ap_return 				: out std_logic_vector (31 downto 0);

		rand_enable				: in  std_logic; 
		
		s0 						: in std_logic_vector (31 downto 0);
		s1 						: in std_logic_vector (31 downto 0);
		
		m_axis_tdata 			: out std_logic_vector(63 downto 0);
		m_axis_tvalid 			: out std_logic;
		m_axis_tready 			: in  std_logic	
	);
end component;


		
signal rand1_ap_done	: std_logic;
signal rand1_ap_idle	: std_logic;
signal rand1_ap_return	: std_logic_vector(31 downto 0);

signal rand2_ap_done	: std_logic;
signal rand2_ap_idle	: std_logic;
signal rand2_ap_return	: std_logic_vector(31 downto 0);

signal rand3_ap_done	: std_logic;
signal rand3_ap_idle	: std_logic;
signal rand3_ap_return	: std_logic_vector(31 downto 0);

signal rand4_ap_done	: std_logic;
signal rand4_ap_idle	: std_logic;
signal rand4_ap_return	: std_logic_vector(31 downto 0);


signal rand5_ap_done	: std_logic;
signal rand5_ap_idle	: std_logic;
signal rand5_ap_return	: std_logic_vector(31 downto 0);

signal ap_rst_n_s		: std_logic;
signal ap_start_s		: std_logic;

begin

process(ap_clk)
begin
if rising_edge(ap_clk) then

	ap_rst_n_s	<= ap_rst_n; 
	ap_start_s	<= ap_start; 
				
	ap_idle		<= rand5_ap_idle & rand4_ap_idle & rand3_ap_idle & rand2_ap_idle & rand1_ap_idle;
	

end if;
end process;




rand1: rand_wrapper
port map (
		ap_clk 					=> ap_clk,
		ap_rst_n 				=> ap_rst_n_s,
		
		ap_start 				=> ap_start_s,
		ap_done 				=> rand1_ap_done,
		ap_idle 				=> rand1_ap_idle,
		ap_ready 				=> open,
		ap_return 				=> rand3_ap_return,

		rand_enable				=> rand_enable,
		
		s0 						=> seed0,
		s1 						=> seed1,
	
        m_axis_tdata            => rand1_tdata,
        m_axis_tvalid       	=> rand1_tvalid,		
		m_axis_tready			=> rand1_tready
	
	);


	
rand2: rand_wrapper
port map (
		ap_clk 					=> ap_clk,
		ap_rst_n 				=> ap_rst_n_s,
		
		ap_start 				=> ap_start_s,
		ap_done 				=> rand2_ap_done,
		ap_idle 				=> rand2_ap_idle,
		ap_ready 				=> open,
		ap_return 				=> rand3_ap_return,

		rand_enable				=> rand_enable,		
		
		s0 						=> seed2,
		s1 						=> seed3,

        m_axis_tdata            => rand2_tdata,
        m_axis_tvalid       	=> rand2_tvalid,		
		m_axis_tready			=> rand2_tready

	
	);


rand3: rand_wrapper
port map (
		ap_clk 					=> ap_clk,
		ap_rst_n 				=> ap_rst_n_s,
		
		ap_start 				=> ap_start_s,
		ap_done 				=> rand3_ap_done,
		ap_idle 				=> rand3_ap_idle,
		ap_ready 				=> open,
		ap_return 				=> rand3_ap_return,

		rand_enable				=> rand_enable,		
		
		s0 						=> seed4,
		s1 						=> seed5,

        m_axis_tdata            => rand3_tdata,
        m_axis_tvalid       	=> rand3_tvalid,		
		m_axis_tready			=> rand3_tready

	
	);
	

rand4: rand_wrapper
port map (
		ap_clk 					=> ap_clk,
		ap_rst_n 				=> ap_rst_n_s,
		
		ap_start 				=> ap_start_s,
		ap_done 				=> rand4_ap_done,
		ap_idle 				=> rand4_ap_idle,
		ap_ready 				=> open,
		ap_return 				=> rand4_ap_return,

		rand_enable				=> rand_enable,		
		
		s0 						=> seed6,
		s1 						=> seed7,

        m_axis_tdata            => rand4_tdata,
        m_axis_tvalid       	=> rand4_tvalid,		
		m_axis_tready			=> rand4_tready

	
	);

	
rand5: rand_wrapper
port map (
		ap_clk 					=> ap_clk,
		ap_rst_n 				=> ap_rst_n_s,
		
		ap_start 				=> ap_start_s,
		ap_done 				=> rand5_ap_done,
		ap_idle 				=> rand5_ap_idle,
		ap_ready 				=> open,
		ap_return 				=> rand5_ap_return,

		rand_enable				=> rand_enable,		
		
		s0 						=> seed8,
		s1 						=> seed9,

        m_axis_tdata            => rand5_tdata,
        m_axis_tvalid       	=> rand5_tvalid,		
		m_axis_tready			=> rand5_tready

	
	);	

end Behavioral;
