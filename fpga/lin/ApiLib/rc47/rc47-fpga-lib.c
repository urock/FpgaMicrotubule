

#include "rc47-fpga-lib.h"







void get_time(struct timeval *tt)
{
	gettimeofday(tt,NULL); 
}


void calc_time_speed(struct timeval *tt1, struct timeval *tt2, int len_dwords, double *tsec, double *speed)
{
	double ttv1 = ((double) tt1->tv_usec) / 1000000 + tt1->tv_sec;
	double ttv2 = ((double) tt2->tv_usec) / 1000000 + tt2->tv_sec;
	*tsec = ttv2 - ttv1;
	*speed = (double) len_dwords * 4 / 1024 / 1024 / *tsec;

}





/********************************************

 * @param	dev
 *
 * @return
 * 		- -1 is returned for a failure
 *
 *******************************************/
int fpga_release_user_reset(int dev) {

	int ret_val; 
	unsigned int reg_val;

	// release user reset
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, USER_CNTRL_REG, 1);
	if (ret_val < 0) {
		printf("Error writting to 0x44 reg!\n");
		return -1;
	}

	ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, USER_CNTRL_REG, &reg_val);
	if (ret_val < 0) {
		printf("Error reading 0x44 reg!\n");
		return -1;
	}

	if (reg_val != 1) {
		printf("Error releasing user reset!\n");
		printf("Reg val = 0x%x\n",reg_val);
		return -1;
	}
//	else
//		printf("OK releasing user reset\n");

	return 0;
}



/********************************************
 *	fpga_check_user_ip_version
 *
 * Read user_ip core from device

	COMMAND_REG(31 downto 20) - RO field

 * @param	dev
 *
 * @return
 * 		- -1 is returned for a failure
 * 		- version (12 bits)
 *
 *******************************************/
int fpga_check_user_ip_version(int dev, int expected_version) {
	int ret_val;


	unsigned int version, reg_val;

	ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, COMMAND_REG, &reg_val);
	if (ret_val < 0) {
		printf("Error reading COMMAND reg!\n");
		return -1;
	}

	version = (reg_val >> 20); 

	//printf("Dev1 user ip version is 0x%x\n",version);
	if (version != expected_version) {
		
		printf("User_ip version is not equal to expected = %d\n", expected_version); 
		return -2;
	}

	return version; 
}



/********************************************
 *	fpga_set_device_index
 *
 * Set FPGA index 

	COMMAND_REG(19 downto 12) - WR field

 * 	@param	dev
	@param	index
 *
 * @return
 * 		- -1 is returned for a failure
 * 		- 0 otherwise
 *
 *******************************************/

int fpga_set_device_index(int dev, unsigned char index) {

#define FPGA_INDEX_MASK		0xFFF00FFF
	int ret_val;
	unsigned int reg_val;


	ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, COMMAND_REG, &reg_val);
	if (ret_val < 0) {
		printf("Error reading COMMAND reg!\n");
		return -1;
	}

	reg_val = (reg_val & FPGA_INDEX_MASK) | ((unsigned int)index << 12); 

	// release user reset
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	if (ret_val < 0) {
		printf("Error writting to COMMAND reg!\n");
		return -1;
	}

	return 0; 
}


int fpga_start_hls(int dev) {

	return fpga_set_register_bit(dev, COMMAND_REG, 0, 1); 
}


// wait for end of working of hls on FPGA
// return 0 on success
int fpga_wait_for_ap_done(int dev, int *ap_return)
{
	unsigned int reg_out;
	int j = 0;
	do {
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, HLS_STATUS, reg_out);
		j++;
		usleep(10);
	} while ( ((reg_out & 0x2) != 0x2) && j< 10000);
	
	if (j == 10000) {
		printf ("\nError ap_done was not set!    \n\n");
		return -2;
	}  
	
	printf ("\n\n     ap_done was set!  j = %d  \n\n", j);
	
	RD_ReadDeviceReg32m(dev, CNTRL_BAR, HLS_AP_RETURN, reg_out);
	
	*ap_return = reg_out;
	return 0;
}

/********************************************
 *	fpga_set_register_bit
 *

 * 	@param	dev
	@param	reg_addr
	@param	bit = {31...0}
	@param  state = {0, 1}

 *
 * @return
 * 		- -1 is returned for a failure
 * 		- 0 otherwise
 *
 *******************************************/

int fpga_set_register_bit(int dev, unsigned int reg_addr, int bit, int state) {

	int ret_val;
	unsigned int reg_val;

	ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, reg_addr, &reg_val);
	if (ret_val < 0) {
		printf("Error reading 0x%x reg!\n",reg_addr);
		return -1;
	}
	
	if (state == 1) // set bit
		reg_val = reg_val | (1<<bit); 
	else if (state == 0) // reset bit
		reg_val = reg_val & ~(1<<bit); 
	

	// release user reset
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, reg_addr, reg_val);
	if (ret_val < 0) {
		printf("Error writting to 0x%x reg!\n",reg_addr);
		return -1;
	}

	return 0; 
}



/********************************************
 *	fpga_user_reset
 *
 * Put user_ip in Reset state

 * @param	dev
 *
 * @return
 * 		- -1 is returned for a failure
 * 		- 0 is returned for a pass
 *
 *******************************************/
int fpga_user_reset(int dev) {
	int ret_val;

	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, USER_CNTRL_REG, 0);
	if (ret_val < 0) {
		printf("fpga_user_reset: Error writting to 0x44 reg!\n");
		return -1;
	}
	return 0;
}



void fpga_mode_reg_check_error(unsigned int reg_val)
{

	if (reg_val & (1<<4))
		printf("Master Memory Write Init Error\n");

	if (reg_val & (1<<5))
		printf("Master Read Request Error\n");

	if (reg_val & (1<<6))
		printf("Interrupt Error\n");

	if (reg_val & (1<<7))
		printf("Timeout Error\n");


	if (reg_val & (1<<26))
		printf("TX_INIT_EP_MEM_WR_ERROR\n");
	if (reg_val & (1<<27))
		printf("TX_EP_MEM_WR_ERROR\n");
	if (reg_val & (1<<28))
		printf("init_rd_error\n");
	if (reg_val & (1<<29))
		printf("Master Memory Write Error\n");
	if (reg_val & (1<<30))
		printf("WaitAnswer\n");


}






int fpga_write_to_axi(int dev, unsigned int *src_buf, unsigned int bytes_to_transfer, unsigned int dest_addr) {

	int i, ret_val = 0;
	unsigned int reg_val = 0; 
	unsigned char rx_status = 0; 

	int timeout = 6; 

	//assert(bytes_to_transfer <= MAX_DMA_TRANSFER_LENGTH); 

	//printf("fpga_write_to_axi enter, src_buf = 0x%x, dest_addr=0x%x\n", src_buf, dest_addr); 

	//printf("fpga_write_to_axi length=0x%x\n", bytes_to_transfer); 


	// set axi address
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, AXI_RX_ADDR, dest_addr);
	if (ret_val < 0) {
		printf("fpga_write_to_axi: Error writting to reg!\n");
		return -1;
	}

	// set transfer length in bytes - this makes controller start working
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, AXI_RX_LEN_B, bytes_to_transfer);
	if (ret_val < 0) {
		printf("fpga_write_to_axi: Error writting to reg!\n");
		return -1;
	}
	// max bytes to transfer is (2^23 - 1 byte) = (8 MB - 1 byte) > 4 MB max DMA data transfer 


	ret_val = RD_WriteBlockDma(dev, src_buf, bytes_to_transfer);


	if (ret_val != bytes_to_transfer) {
		printf("Error in fpga_write_to_axi: RD_WriteBlockDma return != bytes_to_transfer\n");
		return -1;
	}
		

	
 	i = 0;

	while(i++<timeout) {
		ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, AXI_RX_STATUS, &reg_val);
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error reading from reg!\n");
			return -1;
		}
		//printf("fpga_write_to_axi: i = %d, reg_val = 0x%x\n",i,reg_val); 
		if (reg_val & AXI_STATUS_VALID) 
			break; 
	}


	//RD_WriteDeviceReg32(dev, CNTRL_BAR, AXI_RX_STATUS, 0);

	if (i>(timeout-1)) {
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);		
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);		
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_write_to_axi: waiting for AXI rx status timeout\n\n");
		printf("rx_status = 0x%x\n\n",reg_val);

		return -1; 
	}


	rx_status = (unsigned char)reg_val;

	if (rx_status & (1<<4)) {
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);			
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_write_to_axi: Internal error in AXI status\n");
		ret_val = -1;
	}

	if (rx_status & (1<<5)) {
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);			
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_write_to_axi: Decode error in AXI status\n");
		ret_val = -1;
	}

	if (rx_status & (1<<6)) {
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);			
		if (ret_val < 0) {
			printf("fpga_write_to_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_write_to_axi: Slave error in AXI status\n");
		ret_val = -1;
	}




	
	return ret_val;
}




int fpga_read_from_axi(int dev, unsigned int src_addr, unsigned int bytes_to_transfer, unsigned int *dest_buf) {

	int i, ret_val = 0;
	unsigned int reg_val = 0; 
	unsigned char tx_status; 

	int timeout = 6; 

	// set axi address
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, AXI_TX_ADDR, src_addr);
	if (ret_val < 0) {
		printf("fpga_read_from_axi: Error writting to reg!\n");
		return -1;
	}

	// set transfer length in bytes - this makes controller start working
	ret_val = RD_WriteDeviceReg32(dev, CNTRL_BAR, AXI_TX_LEN_B, bytes_to_transfer);
	if (ret_val < 0) {
		printf("fpga_read_from_axi: Error writting to reg!\n");
		return -1;
	}


	ret_val = RD_ReadBlockDma(dev, dest_buf, bytes_to_transfer);

	if (ret_val < 0) {
		printf("Error in fpga_read_from_axi: RD_ReadBlockDma returns negative\n");
		return -1;
	}


	i = 0;

	while(i++<timeout) {
		ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, AXI_TX_STATUS, &reg_val);
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error reading from reg!\n");
			return -1;
		}
		//printf("fpga_read_from_axi: i = %d, reg_val = 0x%x\n",i,reg_val); 
		if (reg_val & AXI_STATUS_VALID)
			break;

	}

	RD_WriteDeviceReg32(dev, CNTRL_BAR, AXI_TX_STATUS, 0);

	 if (i>(timeout-1)) {
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);		
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);		
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_read_from_axi: waiting for AXI tx status timeout\n");
		printf("tx_status = 0x%x\n\n",reg_val);
		return -2; 
	}
		

	tx_status = (unsigned char)reg_val;

	if (tx_status & (1<<4)) {
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);			
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_read_from_axi: Internal error in AXI status\n");
		ret_val = -1;
	}

	if (tx_status & (1<<5)) {
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);
		if (ret_val < 0) {
			printf("fpga_read_from_axit: Error writting to reg!\n");
		}
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);			
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_read_from_axi: Decode error in AXI status\n");
		ret_val = -1;
	}

	if (tx_status & (1<<6)) {
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 1);
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		RD_WriteDeviceReg32(dev, CNTRL_BAR, 0x150, 0);			
		if (ret_val < 0) {
			printf("fpga_read_from_axi: Error writting to reg!\n");
		}
		printf("Error in fpga_read_from_axi: Slave error in AXI status\n");
		ret_val = -1;
	}



	

	return ret_val; 
}




int fpga_wait_for_ddr_init (int dev) {

	int i=0, ret_val; 
	unsigned int reg_val; 

	// wait for DDR INIT DONE	
	while(i++<100) {
		ret_val = RD_ReadDeviceReg32(dev, CNTRL_BAR, MASTER_MODE, &reg_val);
		if (ret_val < 0) {
			printf("fpga_wait_for_ddr_init: Error reading 0x100 reg!\n");
			return -1;
		}	
		//printf("Command Reg val = 0x%8x\n",reg_val); 
		if ((reg_val & DDR_INIT_DONE_BIT) && (reg_val & DDR_INIT_DONE_BIT_2))  
			break; 
		usleep(10000);	
	}
	
	if (i>99) { 
		printf("fpga_wait_for_ddr_init: Timeout error\n");
		return -1; 
	}

	return 0; 
}






