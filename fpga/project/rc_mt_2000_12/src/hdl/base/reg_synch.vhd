----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:17:36 11/02/2013 
-- Design Name: 
-- Module Name:    reg_synch - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.Vcomponents.all;

entity reg_synch is
    Port ( 
	 
			rst_n			: in  std_logic; 
			
			d1_clk 		: in  std_logic;
			d1_we 		: in  std_logic;
			d1_addr 		: in  std_logic_vector ( 7 downto 0);
			d1_dout 		: in  std_logic_vector (31 downto 0);
			d1_re 		: in  std_logic;
			d1_din 		: out std_logic_vector (31 downto 0);
			d1_valid 	: out std_logic;
			d1_ready		: in  std_logic;
			
			d2_clk 		: in  std_logic;
			d2_we 		: out std_logic;
			d2_addr 		: out std_logic_vector ( 7 downto 0);
			d2_dout 		: out std_logic_vector (31 downto 0);
			d2_re 		: out std_logic;
			d2_din 		: in  std_logic_vector (31 downto 0);
			d2_valid 	: in  std_logic
		);
end reg_synch;

architecture Behavioral of reg_synch is

component fifo_to_user_ip
  PORT (
    rst 		: in  std_logic;
    wr_clk 	: in  std_logic;
    rd_clk 	: in  std_logic;
    din 		: in  std_logic_vector(40 downto 0);
    wr_en 	: in  std_logic;
    rd_en 	: in  std_logic;
    dout 	: out std_logic_vector(40 downto 0);
    full 	: out std_logic;
    empty 	: out std_logic
  );
end component;

component fifo_from_user_ip
  PORT (
    rst 		: in  std_logic;
    wr_clk 	: in  std_logic;
    rd_clk 	: in  std_logic;
    din 		: in  std_logic_vector(31 downto 0);
    wr_en 	: in  std_logic;
    rd_en 	: in  std_logic;
    dout 	: out std_logic_vector(31 downto 0);
    full 	: out std_logic;
    empty 	: out std_logic
  );
end component;

signal reset		: std_logic; 

signal fifo_to_user_we			: std_logic;
signal fifo_to_user_din			: std_logic_vector(40 downto 0);

signal fifo_to_user_re			: std_logic;
signal fifo_to_user_empty		: std_logic;
signal fifo_to_user_empty_d_n	: std_logic;

signal fifo_to_user_dout		: std_logic_vector(40 downto 0);

signal fifo_from_user_re			: std_logic;
signal fifo_from_user_empty		: std_logic;
signal fifo_from_user_empty_d_n	: std_logic;


begin

reset	<= not rst_n; 

fifo_to_user_we	<= d1_we or d1_re;
fifo_to_user_din	<= d1_we & d1_addr & d1_dout;

fifo_to_user : fifo_to_user_ip
  PORT MAP (
    rst 			=> reset,
    wr_clk 		=> d1_clk,
    wr_en 		=> fifo_to_user_we,
    din 			=> fifo_to_user_din,
    full 		=> open,

    rd_clk 		=> d2_clk,
    rd_en 		=> fifo_to_user_re,
    dout 		=> fifo_to_user_dout,
    empty 		=> fifo_to_user_empty
  );


process(d2_clk)
begin
	if rising_edge(d2_clk) then
		fifo_to_user_empty_d_n	<= not fifo_to_user_empty; 
	end if;
end process;

fifo_to_user_re	<= fifo_to_user_empty_d_n and (not fifo_to_user_empty); 

d2_we	<= fifo_to_user_re and 		 fifo_to_user_dout(40); 
d2_re	<= fifo_to_user_re and (not fifo_to_user_dout(40));

d2_addr	<= fifo_to_user_dout(39 downto 32); 
d2_dout	<= fifo_to_user_dout(31 downto  0); 



fifo_from_user : fifo_from_user_ip
  PORT MAP (
    rst 			=> reset,
    wr_clk 		=> d2_clk,
    wr_en 		=> d2_valid,
    din 			=> d2_din,
    full 		=> open,

    rd_clk 		=> d1_clk,
    rd_en 		=> fifo_from_user_re,
    dout 		=> d1_din,
    empty 		=> fifo_from_user_empty
  );

process(d1_clk)
begin
	if rising_edge(d1_clk) then
		d1_valid	<= not fifo_from_user_empty; 
	end if;
end process;

fifo_from_user_re	<= d1_ready; 



end Behavioral;

