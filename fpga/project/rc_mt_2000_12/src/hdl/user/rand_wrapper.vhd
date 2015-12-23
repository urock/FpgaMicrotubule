----------------------------------------------------------------------------------
-- Company: 
-- Engineer:	urock
-- Create Date: 06/26/2015 04:44:01 PM
-- Module Name: rand_wrapper - Behavioral

-- Project Name: 

-- Description: wrapper for hls rand generator + output axis reg slice
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.std_logic_1164.ALL;


entity rand_wrapper is
	port (
		ap_clk 					: in  std_logic;
		ap_rst_n 				: in  std_logic;
		ap_start 				: in  std_logic;
		ap_done 				: out std_logic;
		ap_idle 				: out std_logic;
		ap_ready 				: out std_logic;
		ap_return 				: out std_logic_vector (31 downto 0);

		rand_enable				: in  std_logic; 
		
		s0 						: in  std_logic_vector (31 downto 0);
		s1 						: in  std_logic_vector (31 downto 0);
		
		m_axis_tdata 			: out std_logic_vector(63 downto 0);
		m_axis_tvalid 			: out std_logic;
		m_axis_tready 			: in  std_logic		
		

	); 
end rand_wrapper;

architecture Behavioral of rand_wrapper is


component rand_core
	port (
		ap_clk 					: in std_logic;
		--ap_rst_n 				: in std_logic;
		ap_rst                  : in std_logic;
		ap_start 				: in std_logic;
		ap_done 				: out std_logic;
		ap_idle 				: out std_logic;
		ap_ready 				: out std_logic;
		ap_return 				: out std_logic_vector (31 downto 0);

		s0 						: in std_logic_vector (31 downto 0);
		s1 						: in std_logic_vector (31 downto 0);
		
		stream_out              : out std_logic_vector (63 downto 0);
        stream_out_ap_ack        : in std_logic
		


	);
end component;






	
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



signal s_axis_tvalid 	: std_logic;
signal s_axis_tready 	: std_logic;
signal s_axis_tdata 	: std_logic_vector(63 downto 0)	:= (others => '0');



signal stream_out              : STD_LOGIC_VECTOR (63 downto 0);
signal stream_out_ap_ack       : STD_LOGIC;
		
signal ap_rst : std_logic;

signal reg_slice_rst_n	: std_logic;

signal cnt	: integer range 0 to 8195;

signal ap_idle_s	: std_logic;

begin

ap_rst<= not ap_rst_n;

process(ap_clk)
begin
	if rising_edge(ap_clk) then
		if ap_rst_n = '0' then
			cnt		<= 0;
			reg_slice_rst_n	<= '0';
		else 
			if ap_idle_s = '0' then 
				if cnt = 8195 then 
					reg_slice_rst_n	<= '1';
				else
					cnt	<= cnt + 1;
				end if;		
			end if;
		end if;
	end if;
end process;

hls_inst: rand_core
port map (
		ap_clk 					=> ap_clk,
		--ap_rst_n 				=> ap_rst_n,
		ap_rst 				=> ap_rst,
		ap_start 				=> ap_start,
		ap_done 				=> ap_done,
		ap_idle 				=> ap_idle,
		ap_ready 				=> ap_ready,
		ap_return 				=> ap_return,

		s0 						=> s0,
		s1 						=> s1,
		
        stream_out              => stream_out,
        stream_out_ap_ack       => stream_out_ap_ack
	
	);
	
	s_axis_tdata		<= stream_out;
	s_axis_tvalid		<= '1';
	stream_out_ap_ack	<= s_axis_tready;
	
reg_slice: axis_reg_slice_64
port map (
		s_aclk 			=> ap_clk,
		s_aresetn 		=> reg_slice_rst_n,
		              
		s_axis_tvalid 	=> s_axis_tvalid,
		s_axis_tready 	=> s_axis_tready,
		s_axis_tdata 	=> s_axis_tdata,
		               
		m_axis_tvalid 	=> m_axis_tvalid,
		m_axis_tready 	=> m_axis_tready,
		m_axis_tdata 	=> m_axis_tdata
);


	
end Behavioral;
