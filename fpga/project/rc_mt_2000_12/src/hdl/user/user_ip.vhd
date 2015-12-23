----------------------------------------------------------------------------------
-- Project     : Rosta Base PCIe project Ver.9
-- File        : user_ip.vhd
-- Version     : 9.0
-- Company     : Rosta
-- Engineer    : urock & rkob & tasha
-- Create Date : 31.10.2013 
-- Comments    : User IP Interface 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library unisim;
use unisim.vcomponents.all;

entity user_ip is 

        port (

                user_rst_n              : in  std_logic;                -- active low
                
                clk_in                          : in  std_logic;
                clk_out                         : out  std_logic;  
                
                      
                
                -- PCIe FIFO interface
                -- AXIs Slave interface (read from RX_FIFO)
                host_rxd_s_tvalid       : in  std_logic;
                host_rxd_s_tready       : out std_logic;
                host_rxd_s_tdata        : in  std_logic_vector(63 downto 0);

                
                -- AXIs Master interface (write to TX_FIFO)
                host_txd_m_tvalid       : out std_logic; 
                host_txd_m_tready       : in  std_logic;
                host_txd_m_tdata        : out std_logic_vector(63 downto 0);
                
        --  PCIe Registers interface
                Reg_RE          : in  std_logic;
                Reg_WE          : in  std_logic;
                Reg_Addr                : in  std_logic_vector( 7 downto 0);
                Reg_Dout                : in  std_logic_vector(31 downto 0); 
                Reg_Din         : out std_logic_vector(31 downto 0);
                Reg_Valid       : out std_logic;        
                                
                user_int_req    : out std_logic;
                user_int_compl  : in  std_logic;
                
                -- EP tx interface
        -- EP TX CMD FIFO interface 
                ep_tx_cmd_wr_en         : out std_logic;
                ep_tx_cmd_data          : out std_logic_vector(63 downto 0);    -- (addr_32_bits & length_24_bits & tag_8_bits)
                ep_tx_cmd_full          : in  std_logic;  
        
                ep_tx_error                     : in std_logic; 
                ep_tx_tag_error         : in std_logic_vector(7 downto 0); 
                
        -- EP TXD AXIS Master (write to other EP)
                ep_txd_m_tvalid         : out std_logic;
                ep_txd_m_tdata          : out std_logic_vector(63 downto 0);
                ep_txd_m_tready         : in  std_logic;
                
                -- EP RXD AXIS Slave (receive data from other EP) 
                ep_rxd_s_tvalid : in  std_logic;
                ep_rxd_s_tready : out std_logic;
                ep_rxd_s_tdata  : in  std_logic_vector(63 downto 0);
                


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

end user_ip;



architecture rtl of user_ip is

component clk_wiz
        port
                (-- Clock in ports
                        CLK_IN1           : in     std_logic;
                        -- Clock out ports
                        CLK_OUT1          : out    std_logic
                );
end component;





component massive_rand_gen 
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
end component;


  
 



-- Version 
constant        UserIPVersion   : std_logic_vector(11 downto 0) := X"00A"; 

--- Registers address map
--- mapped to BAR0 region with offset = X"100"
constant CommandReg                     : std_logic_vector(7 downto 0) := X"00"; 

-- CommandReg(31 downto 19) - 12 bits - UserIPVersion - RO - set by hardware
-- CommandReg(18 downto 11) - 8  bits - FPGA device index - RW - set by CPU



-- EP TX
constant EpTxAddress            : std_logic_vector(7 downto 0) := X"1C"; 
constant EpTxLength             : std_logic_vector(7 downto 0) := X"20"; 
constant EpTxTag                : std_logic_vector(7 downto 0) := X"24"; 
constant EpTxStatus             : std_logic_vector(7 downto 0) := X"28"; 

-- Reg test
constant TestRegIn              : std_logic_vector(7 downto 0) :=X"2C"; 
constant TestRegOut             : std_logic_vector(7 downto 0) :=X"30";

constant hls_length_set  : std_logic_vector(7 downto 0)      :=X"54";
constant hls_offset_i_set  : std_logic_vector(7 downto 0)      :=X"58";
constant hls_offset_o_set  : std_logic_vector(7 downto 0)      :=X"5C";
constant hls_p1_set  : std_logic_vector(7 downto 0)      :=X"60";
constant hls_p2_set  : std_logic_vector(7 downto 0)      :=X"64";
constant hls_cp_size_set : std_logic_vector(7 downto 0)  :=X"68";


constant hls_return  : std_logic_vector(7 downto 0)      :=X"6C";

constant seed0		: std_logic_vector(7 downto 0)      :=X"70";
constant seed1		: std_logic_vector(7 downto 0)      :=X"74";
constant seed2		: std_logic_vector(7 downto 0)      :=X"78";
constant seed3		: std_logic_vector(7 downto 0)      :=X"7C";
constant seed4		: std_logic_vector(7 downto 0)      :=X"80";
constant seed5		: std_logic_vector(7 downto 0)      :=X"84";
constant seed6		: std_logic_vector(7 downto 0)      :=X"88";
constant seed7		: std_logic_vector(7 downto 0)      :=X"8C";
constant seed8		: std_logic_vector(7 downto 0)      :=X"90";
constant seed9		: std_logic_vector(7 downto 0)      :=X"94";


-- DWORD SWAP BYTE function : ( big-endian -> little-endian )   -- DONE in DMA master unit
-- host_rd(31 downto 0):= trn_rd(7 downto 0)& trn_rd(15 downto 8)& trn_rd(23 downto 16)& trn_rd(31 downto 24); 

signal CommandReg_c                     : std_logic_vector(31 downto 0); 

-- bit		function
-- 0 		ep_tx_cmd_wr_en
-- 2		hls start
-- 3 		hls_done
-- 4		rand_ap_rst_n
-- 5		rand_ap_start
-- 6		hls_ap_rst_n
-- 7		rand_enable
-- 8 - 12    rand idle
-- 29-31 	intercon_selector

signal rand_enable		: std_logic;


signal RxCmdTag, TxCmdTag       : std_logic_vector(3 downto 0); 

signal EpTxAddress_c            : std_logic_vector(31 downto 0); -- addr_32_bits        - ????? (????????) ??? ??????
signal EpTxLength_c             : std_logic_vector(31 downto 0); -- length_24_bits      - ?????????? DWORDs ??? ??????
signal EpTxTag_c                        : std_logic_vector( 7 downto 0); -- tag_8_bits          - ??? ??????

signal EpTxStatus_c                     : std_logic_vector(31 downto 0); -- tag_8_bits          - ??? ??????            



signal TestRegIn_c                      : std_logic_vector(15 downto 0);
-- TestRegIn_c(15::0)   - input_data 16 bits

signal TestRegOut_c                     : std_logic_vector(31 downto 0);
-- TestRegOut_c(15::0)  - input_data + 1
-- TestRegOut_c(31)             - data valid - set on second cyle after writting to TestRegIn_c, priority reset after host reads data TestRegOut_c


signal test_data_set            : std_logic;



signal intercon_selector                : std_logic_vector(2 downto 0);         



signal hls_length       : std_logic_vector(31 downto 0);
signal hls_offset_i     : std_logic_vector(31 downto 0);
signal hls_offset_o     : std_logic_vector(31 downto 0);
signal hls_p1           : std_logic_vector(31 downto 0);
signal hls_p2           : std_logic_vector(31 downto 0);
signal hls_return_c     : std_logic_vector(31 downto 0);
signal hls_return_d     : std_logic_vector(31 downto 0);

signal hls_done         : std_logic;

signal hls_cp_size : std_logic_vector(31 downto 0);


signal rand_ap_rst_n		: std_logic; 
signal rand_ap_start		: std_logic; 


signal seed0_c				: std_logic_vector(31 downto 0);
signal seed1_c				: std_logic_vector(31 downto 0);
signal seed2_c				: std_logic_vector(31 downto 0);
signal seed3_c				: std_logic_vector(31 downto 0);
signal seed4_c				: std_logic_vector(31 downto 0);
signal seed5_c				: std_logic_vector(31 downto 0);
signal seed6_c				: std_logic_vector(31 downto 0);
signal seed7_c				: std_logic_vector(31 downto 0);
signal seed8_c				: std_logic_vector(31 downto 0);
signal seed9_c				: std_logic_vector(31 downto 0);


signal ip_clk           : std_logic; 



signal rand_done	: std_logic_vector(4 downto 0);
signal rand_idle	: std_logic_vector(4 downto 0);
signal rand_return	: std_logic_vector(31 downto 0);




signal clk              : std_logic;



begin





ip_clk_gen: clk_wiz
port map
(
        CLK_IN1 => clk_in,
        CLK_OUT1 => ip_clk
);



---------------------------------------------------------------------------------
-- USER IP registers 
-----------------------------
process(ip_clk, user_rst_n)
begin
        
        if user_rst_n = '0' then
                                                          
                CommandReg_c    <= (others => '0'); 
                

                EpTxAddress_c   <= (others => '0'); 
                EpTxLength_c    <= (others => '0'); 
                EpTxTag_c               <= (others => '0'); 
                EpTxStatus_c    <= (others => '0'); 

                Reg_Din                         <= (others => '1'); 
                Reg_Valid                       <= '0'; 
                
                
                RxCmdTag                <= (others => '0');
                TxCmdTag                <= (others => '0');

                TestRegIn_c             <= (others => '0'); 
                TestRegOut_c    <= (others => '0');
                test_data_set   <= '0'; 
			      
                                  
        elsif rising_edge(ip_clk) then
        

        
        -- set default values
		test_data_set   <= '0';

		Reg_Din         <= (others => '1');
		Reg_Valid       <= '0';

                
        -- Write 
		if Reg_WE = '1' then
			case Reg_Addr is 
			
				when CommandReg         => CommandReg_c         <= Reg_Dout;
				when EpTxAddress        => EpTxAddress_c        <= Reg_Dout;                            
				when EpTxLength         => EpTxLength_c         <= Reg_Dout;
				when EpTxTag            => EpTxTag_c            <= Reg_Dout(7 downto 0);
				
                when hls_length_set 	=> hls_length 			<= Reg_Dout; 
                when hls_offset_i_set 	=> hls_offset_i  		<= Reg_Dout; 
                when hls_offset_o_set 	=> hls_offset_o 		<= Reg_Dout; 
                when hls_p1_set 		=> hls_p1 				<= Reg_Dout;
                when hls_p2_set 		=> hls_p2 				<= Reg_Dout;
                when hls_cp_size_set 	=> hls_cp_size			<= Reg_Dout;



				when  seed0	 			=> seed0_c				<= Reg_Dout; 		 
				when  seed1	 			=> seed1_c				<= Reg_Dout; 		 
				when  seed2	 			=> seed2_c				<= Reg_Dout; 		 
				when  seed3	 			=> seed3_c				<= Reg_Dout; 		 
				when  seed4	 			=> seed4_c				<= Reg_Dout; 		 
				when  seed5	 			=> seed5_c				<= Reg_Dout; 		 
				when  seed6	 			=> seed6_c				<= Reg_Dout; 		 
				when  seed7	 			=> seed7_c				<= Reg_Dout; 		 
				when  seed8	 			=> seed8_c				<= Reg_Dout; 		 
				when  seed9	 			=> seed9_c				<= Reg_Dout; 		 
				
				--  TestRegs                
				when TestRegIn  => 
					TestRegIn_c                     <= Reg_Dout(15 downto 0);
					test_data_set           <= '1';  

	
				when others             => null;
						
				end case;
		
                                
        -- Read 
		elsif Reg_RE = '1' then
				
			case Reg_Addr is 

				when CommandReg => Reg_Din      <= X"0000"& "000" & rand_idle & CommandReg_c(7 downto 0);                   

				
				when EpTxAddress        => Reg_Din      <= EpTxAddress_c;       
				when EpTxLength         => Reg_Din      <= EpTxLength_c;        
				when EpTxTag            => Reg_Din      <= X"000000" & EpTxTag_c;                       
						
				when EpTxStatus         => 
					Reg_Din                 <= EpTxStatus_c;                                
					EpTxStatus_c    <= (others => '0');
																								
						 
				when TestRegOut =>      
					Reg_Din         <= TestRegOut_c;
					if TestRegOut_c(31) = '1' then
							TestRegOut_c(31)        <= '0';
					end if; 
						
																				
				when hls_return		=> Reg_Din 	<= hls_return_c; 
						
				when others => Reg_Din  <= X"12345678"; 
				
			
			end case;
			
			Reg_Valid       <= '1'; 
						
		end if; 
                        
                
                ----------------
                -- EP TX status
                if ep_tx_error = '1' then
                        EpTxStatus_c(31)                        <= '1';
                        EpTxStatus_c(7 downto 0)        <= X"FF";
                end if; 
                        
                -----------------------------   
                -- start EP TX bit
                if CommandReg_c(0) = '1' then
                        CommandReg_c(0) <= '0';
                end if;         
                
               if CommandReg_c(2) = '1' then
                        CommandReg_c(2) <= '0';
                        CommandReg_c(3) <= '0';
  
                end if;
                
                if (CommandReg_c(2) = '0' and hls_done = '1') then
                     CommandReg_c(3) 	<= '1';
					 hls_return_c		<= hls_return_d;
                end if;     

                -- Test Regs process
                if test_data_set = '1' then
                        TestRegOut_c(15 downto 0)       <= TestRegIn_c + 1;
                        TestRegOut_c(31)                        <= '1';                         -- test data out valid
                end if; 
                
                
				ap_rst_n			<= CommandReg_c(6);
				ap_ctrl_start       <= CommandReg_c(2);

				hls_done 	<= ap_ctrl_done;
                       
				hls_return_d	<= ap_return; 	

				cp_size	<= hls_cp_size;
 
				offset_i 		<= hls_offset_i;
				offset_o 		<= hls_offset_o;
				p1 				<= hls_p1;
				p2 				<= hls_p2;
				InputLength 	<= hls_length;
                
                -- rand start and rst_n
				if CommandReg_c(5) = '1' then
					CommandReg_c(5)	<= '0';
				end if;
				
				rand_ap_rst_n	<= CommandReg_c(4);		
				rand_ap_start	<= CommandReg_c(5);

				
				rand_enable		<= CommandReg_c(7);

                        
        end if;
end process; 



ep_tx_cmd_wr_en         <= CommandReg_c(0); 

ep_tx_cmd_data          <= EpTxAddress_c & EpTxLength_c(23 downto 0) & EpTxTag_c; 



----------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------


mass_rand_core: massive_rand_gen 
	port map (
		ap_clk 					=> ip_clk,
		ap_rst_n 				=> rand_ap_rst_n,

		ap_start				=> rand_ap_start,
		ap_idle 				=> rand_idle,
		
		rand_enable				=> rand_enable, 


		seed0					=> seed0_c,
		seed1					=> seed1_c,
		seed2					=> seed2_c,
		seed3					=> seed3_c,
		seed4					=> seed4_c,
		seed5					=> seed5_c,
		seed6					=> seed6_c,
		seed7					=> seed7_c,
		seed8					=> seed8_c,
		seed9					=> seed9_c,

		
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
	
	

clk_out <= ip_clk; 

user_int_req    <= '0'; 

------------------------------------
-- Data FIFOs interconnection
------------------------------------
intercon_selector <= CommandReg_c(31 downto 29);

fifo_intercon: process(intercon_selector, host_rxd_s_tdata, host_rxd_s_tvalid, host_txd_m_tready,  
                       ep_rxd_s_tdata, ep_rxd_s_tvalid, ep_txd_m_tready      )
begin
 CASE intercon_selector IS
when "000"=>
        
        

        host_txd_m_tvalid       <= host_rxd_s_tvalid;
        host_rxd_s_tready       <= host_txd_m_tready; 
        host_txd_m_tdata        <= host_rxd_s_tdata;    
                
        
        -- EP TX        <= EX RX; 
        ep_txd_m_tvalid         <= ep_rxd_s_tvalid; 
        ep_txd_m_tdata          <= ep_rxd_s_tdata; 
        ep_rxd_s_tready         <= ep_txd_m_tready; 
                
                                
when "100"=>
        
        -- HOST TX      <= EP RX;
        host_txd_m_tdata        <= ep_rxd_s_tdata; 
        host_txd_m_tvalid       <= ep_rxd_s_tvalid;     
        ep_rxd_s_tready         <= host_txd_m_tready;
        
        -- EP TX        <= HOST RX; 
        ep_txd_m_tvalid         <= host_rxd_s_tvalid; 
        ep_txd_m_tdata          <= host_rxd_s_tdata; 
        host_rxd_s_tready       <= ep_txd_m_tready; 
        
                        
when others =>
                        host_txd_m_tvalid       <= host_rxd_s_tvalid;
                        host_rxd_s_tready     <= host_txd_m_tready; 
                        host_txd_m_tdata    <= host_rxd_s_tdata;     
    

                        -- EP TX     <= EX RX; 
                        ep_txd_m_tvalid        <= ep_rxd_s_tvalid; 
                        ep_txd_m_tdata         <= ep_rxd_s_tdata; 
                        ep_rxd_s_tready     <= ep_txd_m_tready;                                        
        
        
end case; 
end process;





end rtl; 
