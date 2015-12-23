/*
 * rc47-api.c
 *
 *  Created on: May 18, 2012
 *      Author: urock <rumyantsev@rosta.ru>
 */


#include "RostaPCIeAPI.h"
#include "rc47-api.h"
#include "rc47-stm-misc.h"




/* internal functions*/
int rc47_check_init_up(int dev, int cs);
int rc47_check_init_down(int dev, int cs);
int rc47_check_done_up(int dev, int cs);
void rc47_remove_v7_devices(struct rc47_board_t *rc47, unsigned char chip_select);
static void TimeNow();
int  check_file(unsigned long* code);
#ifdef USE_PLX
S16 SelectDevice_DMA(PLX_DEVICE_KEY    *DevKey_DMA);
#endif



// Global arrays
#ifdef USE_PLX	
PLX_DEVICE_KEY    PLX_Key[MAX_DEVICES_TO_LIST];
#endif



/********************************************************************************
 *	Searches for RC47 boards and fill array of rc47_board_t structures
 *
 *	struct pci_device *pd - pointer to array of struct pci_device
 *	struct rc47_board_t *rc47 - pointer to array of rc47_board_t structures
 *
 *
 *  Returns number of rc47 boards in system on success, < 0 on error
 *
 ********************************************************************************/
int rc47_search_for_boards(struct pci_device *pd, struct rc47_board_t *rc47)
{
	int k;			// number of all rosta pcie devices
	int i;			// pcie device index searching sp6
	int m = 0;  	// number of all rc47 boards
	int j;  		// pcie device index searching v7
	int p;  		// pcie device index searching plx
	int l;  		// index searching parent
#ifdef USE_PLX
	S16               num_plxdevice;
#endif
	unsigned int tmp_parent_bus;

	unsigned char *ptr;
#ifdef USE_PLX
	num_plxdevice= SelectDevice_DMA(PLX_Key);
	if (num_plxdevice == -1) return -1;
#endif	
	ptr = (unsigned char*)rc47;

	for (i=0; i<4*sizeof(struct rc47_board_t); i++)
		*ptr++ = 0;

	k = RD_FindDevices(pd, 0);
	if (k < 0) {
		fprintf(stderr,"rc47_search_for_boards: Error in RD_FindDevices!\n");
		return -1;
	} 


	for (i=(k-1);i>=0;i--) {

		if (pd[i].device_id == SP6_DEVICE_ID) { 	// found SP6 device

			rc47[m].SP_pd = &pd[i];		// save pointer to sp6 pcie device

			tmp_parent_bus = pd[i].parent[0].parent_phys_bus;
			//printf ("\nOK1! %d\n", pd[i].parent[0].parent_phys_bus);

			for (j=0;j<k;j++) {			// search again. now for Virtex7

				if (pd[j].device_id == V7_DEVICE_ID) {					// found V7

					if ( pd[j].parent[0].parent_phys_bus == tmp_parent_bus  )  	{	// v7 and sp6 are from one rc47 board
					//printf ("\nOK! %d\n", pd[j].parent[0].parent_phys_device);

						if (pd[j].parent[0].parent_phys_device == 0) 

							rc47[m].LU_pd = &pd[j];

						else if (pd[j].parent[0].parent_phys_device == 1) 

							rc47[m].LD_pd = &pd[j];

						else if (pd[j].parent[0].parent_phys_device == 9) 

							rc47[m].RD_pd = &pd[j];

						else if (pd[j].parent[0].parent_phys_device == 0xa) 
							rc47[m].RU_pd = &pd[j];
					}
				}
			}

#ifdef USE_PLX				
			for (p = 0; p < num_plxdevice; p++) {// search again. Now for PLX 
				
			if (pd[i].parent[1].parent_phys_domain==PLX_Key[p].domain && 
				pd[i].parent[1].parent_phys_bus==PLX_Key[p].bus && 
				pd[i].parent[1].parent_phys_device==PLX_Key[p].slot )  {
				
				rc47[m].PLX_dev5= &PLX_Key[p]; 

			}
				
			for (l = 2; l< pd[i].num_parent; l++){
				if  (pd[i].parent[l].parent_phys_domain==PLX_Key[p].domain && pd[i].parent[l].parent_phys_bus==PLX_Key[p].bus && pd[i].parent[l].parent_phys_device==PLX_Key[p].slot ) {
					if ( PLX_Key[p].function==((U8)1) )  {rc47[m].PLX_dev1= &PLX_Key[p]; }
					if ( PLX_Key[p].function==((U8)2) )  {rc47[m].PLX_dev2= &PLX_Key[p]; }
					if ( PLX_Key[p].function==((U8)3) )  {rc47[m].PLX_dev3= &PLX_Key[p]; }
					if ( PLX_Key[p].function==((U8)4) )  {rc47[m].PLX_dev4= &PLX_Key[p]; }
					
				}
				}				
				}
#endif				
			m++;						// increment rc47 board counter

		}

	}


	return m;


}


/** 
 Also tries to open all device files - thus creating device files if not present in system
 first time after system startup should be called with root permissions

*/
void rc47_print_device_info(int NumBoards, struct rc47_board_t *rc47)
{
	int i;


	printf("\n\t\t\tBus Address\tChip ID\n"); 

	for (i=0; i< NumBoards; i++) {

		printf("RC47 #%d:::\n",i);
		

		if (rc47[i].SP_pd != NULL) {
			printf("\tSystem Chip\t%02X:%02x.%d\t\t%04x\n",
					rc47[i].SP_pd->phys_bus, rc47[i].SP_pd->phys_device,
					rc47[i].SP_pd->phys_function, rc47[i].SP_pd->subsys_device_id);
			
			if (RD_OpenDevice(rc47[i].SP_pd)<0) 
				printf("SP_pd OpenDevice error\n");		
			else
				RD_CloseDevice(rc47[i].SP_pd);					
		}

		if (rc47[i].LD_pd != NULL) {
			printf("\tVirtex-7 C0\t%02X:%02x.%d\t\t%04x\n",
					rc47[i].LD_pd->phys_bus, rc47[i].LD_pd->phys_device,
					rc47[i].LD_pd->phys_function, rc47[i].LD_pd->subsys_device_id);

			if (RD_OpenDevice(rc47[i].LD_pd)<0) 
				printf("LD_pd OpenDevice error\n");		
			else
				RD_CloseDevice(rc47[i].LD_pd);					
		}

		if (rc47[i].LU_pd != NULL) {
			printf("\tVirtex-7 C1\t%02X:%02x.%d\t\t%04x\n",
					rc47[i].LU_pd->phys_bus, rc47[i].LU_pd->phys_device,
					rc47[i].LU_pd->phys_function, rc47[i].LU_pd->subsys_device_id);

			if (RD_OpenDevice(rc47[i].LU_pd)<0) 
				printf("LU_pd OpenDevice error\n");		
			else
				RD_CloseDevice(rc47[i].LU_pd);					
		}

		if (rc47[i].RD_pd != NULL) {
			printf("\tVirtex-7 C2\t%02X:%02x.%d\t\t%04x\n",
					rc47[i].RD_pd->phys_bus, rc47[i].RD_pd->phys_device,
					rc47[i].RD_pd->phys_function, rc47[i].RD_pd->subsys_device_id);

			if (RD_OpenDevice(rc47[i].RD_pd)<0) 
				printf("RD_pd OpenDevice error\n");		
			else
				RD_CloseDevice(rc47[i].RD_pd);					
		}

		if (rc47[i].RU_pd != NULL) {
			printf("\tVirtex-7 C3\t%02X:%02x.%d\t\t%04x\n",
					rc47[i].RU_pd->phys_bus, rc47[i].RU_pd->phys_device,
					rc47[i].RU_pd->phys_function, rc47[i].RU_pd->subsys_device_id);

			if (RD_OpenDevice(rc47[i].RU_pd)<0) 
				printf("RU_pd OpenDevice error\n");		
			else
				RD_CloseDevice(rc47[i].RU_pd);					
		}
		#ifdef USE_PLX
		if (rc47[i].PLX_dev1 != NULL) {
			printf("\tPLX 8725 \t%02X:%02x.%d\t\t%04x\n",
					rc47[i].PLX_dev1->bus, rc47[i].PLX_dev1->slot,
					rc47[i].PLX_dev1->function, rc47[i].PLX_dev1->DeviceId);
		}
				if (rc47[i].PLX_dev2 != NULL) {
			printf("\tPLX 8725 \t%02X:%02x.%d\t\t%04x\n",
					rc47[i].PLX_dev2->bus, rc47[i].PLX_dev2->slot,
					rc47[i].PLX_dev2->function, rc47[i].PLX_dev2->DeviceId);
		}
				if (rc47[i].PLX_dev3 != NULL) {
			printf("\tPLX 8725 \t%02X:%02x.%d\t\t%04x\n",
					rc47[i].PLX_dev3->bus, rc47[i].PLX_dev3->slot,
					rc47[i].PLX_dev3->function, rc47[i].PLX_dev3->DeviceId);
		}
				if (rc47[i].PLX_dev4 != NULL) {
			printf("\tPLX 8725 \t%02X:%02x.%d\t\t%04x\n",
					rc47[i].PLX_dev4->bus, rc47[i].PLX_dev4->slot,
					rc47[i].PLX_dev4->function, rc47[i].PLX_dev4->DeviceId);
		}
				if (rc47[i].PLX_dev5 != NULL) {
			printf("\tPLX 8732 \t%02X:%02x.%d\t\t%04x\n",
					rc47[i].PLX_dev5->bus, rc47[i].PLX_dev5->slot,
					rc47[i].PLX_dev5->function, rc47[i].PLX_dev5->DeviceId);
		}
		#endif
	}

	printf("\n"); 

}

/************************************************************************
 * Configures Virtex 7
 *
 * Params: 	rc47 - pointer to rc47_board_t - current board to work with
 * 			chip_select - must be any combination of flags CS_LD, CS_LU, CS_RD, CS_RU
 * 			bit_file_path - relative path to bitstream file
 *
 * Returns 0 on success or negative error code from RC47_ERRORS defines
 *
 *
 ************************************************************************/

int rc47_config_v7(struct rc47_board_t *rc47, int chip_select, char *bit_file_path)
{
	int fd, fileLength;
	int bytes_read, ret_code;

	unsigned char *bitstreem_buf;
	int i,  t=0, dev;

	unsigned int status_reg;
	// check parameters
	if ((chip_select < 0) || (chip_select>15)) {
		fprintf(stderr,"rc47_config_v7: Assert on chip_select parameter!\n");
		return RC47_CS_PARAM_ERROR;
	}
	
	fd = open((const char *)bit_file_path,O_RDWR);

	if (fd < 0) {
		fprintf(stderr,"Error opening file %s\n",bit_file_path);
		return RC47_FILE_ERROR;
	}
	

	printf("\n\n-------------\nEntered rc47_config_v7\n\nWill now configure:");
	if (chip_select & CS_LD) printf(" C0");
	if (chip_select & CS_LU) printf(" C1");
	if (chip_select & CS_RD) printf(" C2");
	if (chip_select & CS_RU) printf(" C3");
	printf("\n\n"); 

	rc47_remove_v7_devices(rc47, chip_select);

	usleep(100);


	dev = rc47->SP_pd->intfd;

	// assert hardware reset
	RD_WriteDeviceReg32m(dev, SP6_BAR, V7_PCIE_RESET_N, 0);

	// set all chip select  bits; they are active low
	device_register_set_bits(dev, SP6_BAR, CONFIG_STATUS_R, &status_reg, CS_MASK | V7_PROG);


/*********** Reading bit file *****************/
	printf("\n\nload_bitstream: bit_file_path = %s\n",bit_file_path);



	fileLength = lseek(fd,0,SEEK_END);


	printf("file length = %d\n",fileLength);
	ret_code = lseek(fd,0,0);


	bitstreem_buf = (unsigned char *) malloc(fileLength);
	if (!bitstreem_buf) {
		fprintf(stderr,"Error allocting memory for Write Buffer\n");
		close(fd);
		return RC47_FILE_ERROR;
	}



	bytes_read = read(fd,(void *)bitstreem_buf,fileLength);
	if (bytes_read != fileLength) {
		fprintf(stderr,"Error reading file! bytes_read\n");
		free(bitstreem_buf);
		close(fd);
		return RC47_FILE_ERROR;
	}
	close(fd);
/************ DONE Reading bit file *****************/


	ret_code = 0;

	printf("\nStart configuration...\n\n");

	do {

		RD_ReadDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);

		//msg_dbg("Start config, status = 0x%x\n", status_reg);


		//msg_dbg("Set Chip select\n\n");
		// set chip select CS_LU
		device_register_reset_bits(dev, SP6_BAR, CONFIG_STATUS_R, &status_reg, (chip_select << 8));

		//msg_dbg("chip select set, status = 0x%x\n", status_reg);

		//msg_dbg("Set Prog LOW\n\n");
		// toggle PROG. PROG is active low
		status_reg &= ~V7_PROG; // reset PROG
		RD_WriteDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);

		t = 0;
		// wait for INIT bits in status reg to go down
		while(rc47_check_init_down(dev, chip_select) && (t++<10))  // Timeout 1 ms
			usleep(100); 			// 100 microsecs

		if (t>=10) {
			fprintf(stderr,"rc47_config_v7: Error checking for INIT DOWN!\n");
			ret_code = RC47_INIT_DOWN_ERROR;
			break;
		}

		//msg_dbg("Set Prog HIGH\n\n");
		status_reg |= V7_PROG; // set PROG
		RD_WriteDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);



		t = 0;
		// wait for INIT bits in status reg to go up
		while(rc47_check_init_up(dev, chip_select) && (t++<50))  // Timeout 50 ms
			usleep(1000); 			// 1000 microsecs

		if (t++ >= 50) {
			fprintf(stderr,"rc47_config_v7: Error checking for INIT UP!\n");
			ret_code = RC47_INIT_UP_ERROR;
			break;
		}


		RD_ReadDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);
		//msg_dbg("Init OK, status = 0x%x\n",status_reg);



		for (i = 0; i < fileLength; i++)
			RD_WriteDeviceReg32(dev,SP6_BAR,CONFIG_DATA_R,bitstreem_buf[i]);

		RD_ReadDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);
		//msg_dbg("\nDATA sent, status = 0x%x\n",status_reg);


		t=0;
		while(rc47_check_done_up(dev, chip_select) && (t++<10000)); // Timeout 100 ms
			//usleep(10000); 			// 10000 microsecs = 10 ms

		if (t >= 10000) {
			fprintf(stderr,"rc47_config_v7: Error checking for DONE UP\n");
			ret_code = RC47_DONE_UP_ERROR;
			break;
		} else
			printf("Config End OK\n\n");

	} while(0);



	// set all chip select  bits; they are active low
	device_register_set_bits(dev, SP6_BAR, CONFIG_STATUS_R, &status_reg, CS_MASK);

	// deassert hardware reset
	RD_WriteDeviceReg32m(dev, SP6_BAR, V7_PCIE_RESET_N, 0xFFFFFFFF);

	free(bitstreem_buf);

	return ret_code;
}





/************************************************************************
 * Reset V7 Link over Spartan6
 *
 * Params: 	rc47 - pointer to rc47_board_t - current board to work with
 * 			chip_select - must be any combination of flags CS_LD, CS_LU, CS_RD, CS_RU
 *
 * Returns 0 on success
 *
 *
 ************************************************************************/

int rc47_link_reset(struct rc47_board_t *rc47, int chip_select)
{
	int ret_code = 0;

	int dev;

	// check parameters
	if ((chip_select < 0) || (chip_select>15)) {
		fprintf(stderr,"rc47_config_v7: Assert on chip_select parameter!\n");
		return RC47_CS_PARAM_ERROR;
	}

	printf("\n\n-------------\nEntered rc47_config_v7\n\nWill now configure:");
	if (chip_select & CS_LD) printf(" C0");
	if (chip_select & CS_LU) printf(" C1");
	if (chip_select & CS_RD) printf(" C2");
	if (chip_select & CS_RU) printf(" C3");
	printf("\n\n");

	rc47_remove_v7_devices(rc47, chip_select);

	usleep(100);


	dev = rc47->SP_pd->intfd;

	RD_WriteDeviceReg32m(dev, SP6_BAR, V7_PCIE_RESET_N, 0);

	usleep(100000);		// 100 ms

	RD_WriteDeviceReg32m(dev, SP6_BAR, V7_PCIE_RESET_N, 0xFFFFFFFF);





	return ret_code;
}








//int rc47_read_stm_status(int dev, stm_status_t* stm_status_ptr, plx_status_t* plx_status_ptr)
int rc47_read_stm_status(int dev, stm_status_t* stm_status_ptr)
{
	struct cmd_frame_s	frame;

	frame.cmd_code = CMD_READ_STATUS;

	if (rc47_send_cmd_frame(dev,&frame)) {
		printf("Error sending cmd CMD_READ_STATUS \n");
		return -1;
	}

	// receive data from stm
	if (rc47_receive_buffer_stm(dev, (unsigned char *)stm_status_ptr, sizeof(stm_status_t)) <0 ) {
		printf("Error receiving stm_status\n");
		return -2;
	}
/*	if (rc47_receive_buffer_stm(dev, (unsigned char *)plx_status_ptr, sizeof(plx_status_t)) <0 ) {
		printf("Error receiving stm_status\n");
		return -2;
	}
*/
	return 0;

}

int rc47_read_max_status(int dev, max_status_t* max_status_ptr)
{
	struct cmd_frame_s	frame;

	frame.cmd_code = CMD_READ_MAX_DATA;

	if (rc47_send_cmd_frame(dev,&frame)) {
		printf("Error sending cmd CMD_READ_MAX_DATA \n");
		return -1;
	}

	// receive data from stm
	if (rc47_receive_buffer_stm(dev, (unsigned char *)max_status_ptr, sizeof(max_status_t)) <0 ) {
		printf("Error receiving buffer\n");
		return -2;
	}

	return 0;

}


int rc47_read_adc_data(int dev, adc_data_t* adc_data_ptr)
{
	struct cmd_frame_s	frame;

	frame.cmd_code = CMD_READ_ADC_DATA;

	if (rc47_send_cmd_frame(dev,&frame)) {
		printf("Error sending cmd CMD_READ_ADC_DATA \n");
		return -1;
	}

	// receive data from stm
	if (rc47_receive_buffer_stm(dev, (unsigned char *)adc_data_ptr, sizeof(adc_data_t)) <0 ) {
		printf("Error receiving buffer\n");
		return -2;
	}

	return 0;

}




/*****************************************************************************************
 * 	Function to send command to STM to start Virtex 7 reconfiguration from microSD
 *
 *  It is assumed that corresponding pcie devices have already been removed from OS.
 *
 * Params: 	rc47 - pointer to rc47_board_t - current board to work with
 * 			chip_select - must be any combination of flags CS_LD, CS_LU, CS_RD, CS_RU
 * 			v7_bit_id - v7 bit file description code to match one on microSD card
 *
 *****************************************************************************************/

int rc47_reconfig_v7_mSD(struct rc47_board_t *rc47, unsigned char chip_select, unsigned char v7_bit_id)
{
	struct cmd_frame_s	frame;

	frame.cmd_code = CMD_V7_RECONFIG;

	frame.cmd_data.v7_reconfig_data.v7_index = chip_select;
	frame.cmd_data.v7_reconfig_data.bit_id = v7_bit_id;

	// check parameters
	if ((chip_select < 0) || (chip_select>15)) {
		printf("rc47_config_v7: Assert on chip_select parameter!\n");
		return RC47_CS_PARAM_ERROR;
	}


	printf("\n\n-------------\nEntered rc47_reconfig_v7_mSD\n\nWill now reconfigure from microSD:");
	if (chip_select & CS_LD) printf(" C0");
	if (chip_select & CS_LU) printf(" C1");
	if (chip_select & CS_RD) printf(" C2");
	if (chip_select & CS_RU) printf(" C3");
	printf("\n\n"); 

	rc47_remove_v7_devices(rc47, chip_select);

	usleep(100);


	printf("\n\n");


	if (rc47_send_cmd_frame(rc47->SP_pd->intfd, &frame)) {
		printf("Error sending cmd CMD_V7_RECONFIG \n");
		return -1;
	}

	//wait for reconfiguration to finish
	sleep(2);

	return 0;
}



/*****************************************************************************************
 * 	Function to send command to STM to reset Virtex 7 configuration
 *
 *  It is assumed that corresponding pcie devices have already been removed from OS.
 *
 * Params: 	rc47 - pointer to rc47_board_t - current board to work with
 * 			chip_select - must be any combination of flags CS_LD, CS_LU, CS_RD, CS_RU
 *
 *****************************************************************************************/


int rc47_reset_v7(struct rc47_board_t *rc47, unsigned char chip_select)
{
	struct cmd_frame_s	frame;

	frame.cmd_code = CMD_V7_RST;

	frame.cmd_data.v7_rst_data.v7_index = chip_select;

	// check parameters
	if ((chip_select < 0) || (chip_select>15)) {
		printf("rc47_config_v7: Assert on chip_select parameter!\n");
		return RC47_CS_PARAM_ERROR;
	}



	printf("\n\n-------------\nEntered rc47_reset_v7\n\nWill now reset:");
	if (chip_select & CS_LD) printf(" C0");
	if (chip_select & CS_LU) printf(" C1");
	if (chip_select & CS_RD) printf(" C2");
	if (chip_select & CS_RU) printf(" C3");
	printf("\n\n"); 

	rc47_remove_v7_devices(rc47, chip_select);

	usleep(100);


	if (rc47_send_cmd_frame(rc47->SP_pd->intfd, &frame)) {
		printf("Error sending cmd CMD_V7_RST \n");
		return -1;
	}

	return 0;
}





/**************************

SDIO address in blocks (512 байт) – 4 байта
SDIO length in blocks (SDIO_MAX_BLOCKS_RX = 24; transfer length <= 12 KB) – 1 байт

****************************/

int rc47_write_buf_to_sdio(
		struct 			rc47_board_t *rc47,
		unsigned char 	*src_buf,
		unsigned int 	sdio_block_addr,
		unsigned char 	block_num				// number of blocks to transfer
		)

{
	int dev;

	unsigned char read_buf;
	struct cmd_frame_s	frame;


	if (block_num > 24) {
		printf("rc47_write_buf_to_sdio: assert on param block_num\n");
		return -9;
	}

	dev = rc47->SP_pd->intfd;


	frame.cmd_code = CMD_WRITE_SDIO;

	frame.cmd_data.sdio_access.addr = sdio_block_addr;
	frame.cmd_data.sdio_access.length = block_num;


	if (rc47_send_cmd_frame(dev, &frame)) {
		printf("rc47_write_buf_to_sdio: Error sending cmd CMD_WRITE_SDIO \n");
		return -1;
	}



	// send data to stm
	if (rc47_send_buffer_stm(dev, src_buf, block_num*SDIO_BLOCK_SIZE) < 0) {
		printf("rc47_write_buf_to_sdio: Error sending buffer\n");
		return -2;
	}



	//wait for completion

	if (rc47_wait_for_host_tx_fifo_empty(dev) < 0)
		return -3;

	while(1) {


		if (rc47_send_byte_stm(dev,CMD_CHECK_READY) < 0) {
			printf("rc47_write_buf_to_sdio: Error sending cmd CMD_CHECK_READY\n");
			return -3;
		}


		if (rc47_receive_byte_stm(dev,&read_buf) < 0) {
			printf("rc47_send_cmd_frame: Error receiving ack on CMD_CHECK_READY\n");
			return -3;
		}

		if (read_buf == STM_READY) {
			break;
		}

		if ( read_buf == STM_BUSY ) {	// chech acknowledge
			//printf("STM_BUSY\n");
			continue;
		} else {
			printf("Bad ACK from STm: 0x%x\n",read_buf);
			return -5;
		}

		usleep(100);

	} // end wait


	// get ack from stm
	if (rc47_receive_byte_stm(dev,&read_buf) < 0) {
		printf("rc47_send_cmd_frame: Error receiving acknowledge on CMD_WRITE_SDIO\n");
		return -3;
	}

	if (( read_buf == CMD_CMLP) )
		return 0;


	if ( read_buf == CMD_ERROR ) {	// chech acknowledge

		printf("Error completing  CMD_WRITE_SDIO ");

		if (rc47_receive_byte_stm(dev,&read_buf) < 0) {
			printf("rc47_send_cmd_frame: Error receiving acknowledge\n");
			return -3;
		}

		 printf("Error code = %d\n", read_buf);

		return -7;
	} else {
		printf("Bad ACK from STm: 0x%x\n",read_buf);
		return -5;
	}



	return 0;
}




int rc47_read_buf_from_sdio(
		struct 			rc47_board_t *rc47,
		unsigned char 	*dst_buf,
		unsigned int 	sdio_block_addr,
		unsigned char 	block_num			// number of blocks to transfer
	)

{
	int dev;

	unsigned char read_buf;
	struct cmd_frame_s	frame;

	if (block_num > 24) {
		printf("rc47_read_buf_from_sdio: assert on param block_num\n");
		return -9;
	}

	dev = rc47->SP_pd->intfd;


	frame.cmd_code = CMD_READ_SDIO;

	frame.cmd_data.sdio_access.addr = sdio_block_addr;
	frame.cmd_data.sdio_access.length = block_num;


	if (rc47_send_cmd_frame(dev, &frame)) {
		printf("rc47_read_buf_from_sdio: Error sending cmd CMD_READ_SDIO \n");
		return -1;
	}



	//wait for completion
	while(1) {
		if (rc47_send_byte_stm(dev,CMD_CHECK_READY) < 0) {
			printf("rc47_read_buf_from_sdio: Error sending cmd CMD_CHECK_READY\n");
			return -3;
		}


		// get ack from stm
		if (rc47_receive_byte_stm(dev,&read_buf) < 0) {
			printf("rc47_read_buf_from_sdio: Error receiving acknowledge on CMD_CHECK_READY\n");
			return -3;
		}

		if (( read_buf == STM_READY) ) {
			break;
		}


		if ( read_buf == STM_BUSY ) {	// chech acknowledge
			continue;
		} else {
			printf("Bad ACK from STm: 0x%x\n",read_buf);
			return -5;
		}


	} // end wait


	if (rc47_receive_byte_stm(dev,&read_buf) < 0) {
		printf("rc47_read_buf_from_sdio: Error receiving acknowledge on CMD_READ_SDIO\n");
		return -3;
	}

	if (( read_buf == CMD_CMLP) ) {

		// read data from STM
		if (rc47_receive_buffer_stm(dev, dst_buf, block_num*SDIO_BLOCK_SIZE) < 0) {
			printf("rc47_read_buf_from_sdio: Error receiving buffer\n");
			return -2;
		}

		return 0;
	}

	if ( read_buf == CMD_ERROR ) {	// chech acknowledge

		printf("Error completing  CMD_READ_SDIO ");

		if (rc47_receive_byte_stm(dev,&read_buf) < 0) {
			printf("rc47_read_buf_from_sdio: Error receiving acknowledge\n");
			return -3;
		}

		 printf("Error code = %d\n", read_buf);

		return -7;
	} else {
		printf("Bad ACK from STm: 0x%x\n",read_buf);
		return -5;
	}






	return 0;
}





/////// INTERNALS ////////////////////////////



// return 0 when all needed inits are up
// 1 otherwise
int rc47_check_init_up(int dev, int cs)
{
	unsigned int 	status_reg;

	RD_ReadDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);

	//msg_dbg("check init up: status reg = 0x%x; \n\n", status_reg);


	do {

		if ( (cs & CS_LD) && !(status_reg & INIT_LD) )
			return 1;
		if ( (cs & CS_LU) && !(status_reg & INIT_LU) )
			return 1;
		if ( (cs & CS_RD) && !(status_reg & INIT_RD) )
			return 1;
		if ( (cs & CS_RU) && !(status_reg & INIT_RU) )
			return 1;


	} while (0);

	return 0;

}


// return 0 when all needed inits are down
// 1 otherwise
int rc47_check_init_down(int dev, int cs)
{
	unsigned int 	status_reg;

	RD_ReadDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);

	//msg_dbg("check init down: status reg = 0x%x; \n\n", status_reg);


	do {

		if ( (cs & CS_LD) && (status_reg & INIT_LD) )
			return 1;
		if ( (cs & CS_LU) && (status_reg & INIT_LU) )
			return 1;
		if ( (cs & CS_RD) && (status_reg & INIT_RD) )
			return 1;
		if ( (cs & CS_RU) && (status_reg & INIT_RU) )
			return 1;


	} while (0);

	return 0;

}



// return 0 when all needed dones are up
// 1 otherwise
int rc47_check_done_up(int dev,int cs)
{
	unsigned int 	status_reg;


	RD_ReadDeviceReg32m(dev,SP6_BAR,CONFIG_STATUS_R,status_reg);

	//msg_dbg("check done up: status reg = 0x%x; \n\n", status_reg);


	do {

		if ( (cs & CS_LD) && !(status_reg & DONE_LD) )
			return 1;
		if ( (cs & CS_LU) && !(status_reg & DONE_LU) )
			return 1;
		if ( (cs & CS_RD) && !(status_reg & DONE_RD) )
			return 1;
		if ( (cs & CS_RU) && !(status_reg & DONE_RU) )
			return 1;


	} while (0);

	// this will toggle CCLK during FPGA startup sequence
	RD_WriteDeviceReg32m(dev,SP6_BAR,CONFIG_DATA_R,0xff );

	return 0;

}



void rc47_remove_v7_devices(struct rc47_board_t *rc47, unsigned char chip_select)
{



//	if (chip_select & CS_LD) {
//		if (rc47->LD_pd) {
//			printf("C0 device diabled\t%02x:%02x.%d\n",
//					rc47->LD_pd->phys_bus, rc47->LD_pd->phys_device,
//					rc47->LD_pd->phys_function);
//			RD_DisableDevice(rc47->LD_pd);
//		}
//	}
//	if (chip_select & CS_LU) {
//		if (rc47->LU_pd) {
//
//			printf("C1 device diabled\t%02x:%02x.%d\n",
//					rc47->LU_pd->phys_bus, rc47->LU_pd->phys_device,
//					rc47->LU_pd->phys_function);
//			RD_DisableDevice(rc47->LU_pd);
//		}
//	}
//	if (chip_select & CS_RD) {
//		if (rc47->RD_pd) {
//
//			printf("C2 device diabled\t%02x:%02x.%d\n",
//					rc47->RD_pd->phys_bus, rc47->RD_pd->phys_device,
//					rc47->RD_pd->phys_function);
//			RD_DisableDevice(rc47->RD_pd);
//		}
//	}
//	if (chip_select & CS_RU) {
//		if (rc47->RU_pd) {
//			printf("C3 device diabled\t%02x:%02x.%d\n",
//					rc47->RU_pd->phys_bus, rc47->RU_pd->phys_device,
//					rc47->RU_pd->phys_function);
//			RD_DisableDevice(rc47->RU_pd);
//		}
//	}

	if (chip_select & CS_RU) {
		if (rc47->RU_pd) {
			printf("C3 device diabled\t%02x:%02x.%d\n",
					rc47->RU_pd->phys_bus, rc47->RU_pd->phys_device,
					rc47->RU_pd->phys_function);
			RD_DisableDevice(rc47->RU_pd);
		}
	}

	if (chip_select & CS_RD) {
		if (rc47->RD_pd) {

			printf("C2 device diabled\t%02x:%02x.%d\n",
					rc47->RD_pd->phys_bus, rc47->RD_pd->phys_device,
					rc47->RD_pd->phys_function);
			RD_DisableDevice(rc47->RD_pd);
		}
	}

	if (chip_select & CS_LU) {
		if (rc47->LU_pd) {

			printf("C1 device diabled\t%02x:%02x.%d\n",
					rc47->LU_pd->phys_bus, rc47->LU_pd->phys_device,
					rc47->LU_pd->phys_function);
			RD_DisableDevice(rc47->LU_pd);
		}
	}

	if (chip_select & CS_LD) {
		if (rc47->LD_pd) {
			printf("C0 device diabled\t%02x:%02x.%d\n",
					rc47->LD_pd->phys_bus, rc47->LD_pd->phys_device,
					rc47->LD_pd->phys_function);
			RD_DisableDevice(rc47->LD_pd);
		}
	}

}


struct timeval tt1, tt2;
int tms, times;
double tsec2;

int  check_file(unsigned long* code){
 
        const unsigned long rcode = 0x19841211;
        printf("code is %x\n", (unsigned int)*code);
        if (*code !=  rcode)
                return -2;
        else
                return 0;
 
}


int rc47_programm_sdio(struct rc47_board_t *rc47, char *bit_file_path, int verify)
{
	int r;
	int fd, fileLength;
	int bytes_read, ret_val;

	unsigned char *wr_buf, *rd_buf;



	/*********** Reading bit file *****************/
	printf("\n\nFile to load to SD card = %s\n",bit_file_path);



	fd = open((const char *)bit_file_path,O_RDWR);

	if (fd < 0) {
		printf("Error opening file\n");
		return RC47_FILE_ERROR;
	}

	fileLength = lseek(fd,0,SEEK_END);

	printf("file length = %d\n",fileLength);
	lseek(fd,0,0);


	wr_buf = (unsigned char *) malloc(fileLength + 512);
	if (!wr_buf) {
		printf("Error allocting memory for Write Buffer\n");
		close(fd);
		return RC47_FILE_ERROR;
	}

	rd_buf = (unsigned char *) malloc(fileLength + 512);
	if (!rd_buf) {
		printf("Error allocting memory for Read Buffer\n");
		close(fd);
		return RC47_FILE_ERROR;
	}
	memset((void*)wr_buf,0,fileLength + 512);
	memset((void*)rd_buf,0,fileLength + 512);


	bytes_read = read(fd,(void *)wr_buf,fileLength);
	if (bytes_read != fileLength) {
		printf("Error reading file! bytes_read = %d\n", bytes_read);
		free(wr_buf);
		close(fd);
		return RC47_FILE_ERROR;
	}
	close(fd);
	/************ DONE Reading bit file *****************/

	// if filelength is not a multiple of 512 -> extend it
	if ((fileLength % 512) != 0) {
		while ((fileLength % 512) != 0 )
			fileLength++;
	}


     // check file
        
        unsigned long *code;
        code = (unsigned long *)(&wr_buf[512]);
 
 
        if (check_file(code) < 0){
                printf("Checking code failed. Wrong file for loading?\n");
                return -2;
        }
        // end check file
		

	int i=0, error=0, filePointer=0;
	// walk through the file


	int transfer_length = 24;		// 24 * SDIO_BLOCK_SIZE = 12 KB

	int n = (int)((fileLength >> 9)/transfer_length); // total rounds of transfer
	if (n==0) n=1;

	int x = 0; // round completed
	int x1; // foor loop index
	int u = 20; // number of updates

	float ratio;
	int   c;
	int	  w = 50;	// width of bar

	printf("\nStart programming SD card by blocks of %2.1f KB\n\n", (float)transfer_length/2.0 );




	// open SP6 device
	do_and_test(RD_OpenDevice,(rc47->SP_pd));




	Time(tt1);

	while(1) {

		if (filePointer == fileLength)
			break;
		else {

			if ((filePointer + (transfer_length << 9)) <= fileLength) {

				//printf("filePointer = 0x%x\n",filePointer);

				//transfer data

				if (rc47_write_buf_to_sdio(rc47, &wr_buf[filePointer],(filePointer >> 9), transfer_length) != 0) {
					printf("Error rc47_write_buf_to_sdio\n");
					return -3;
				}


				if (verify) {

					if (rc47_read_buf_from_sdio(rc47, &rd_buf[filePointer],(filePointer >> 9), transfer_length) != 0) {
						printf("Error rc47_read_buf_from_sdio\n");
						return -3;
					}

					for (i=0; i<(transfer_length << 9); i++) {
						if (wr_buf[filePointer+i] != rd_buf[filePointer+i])
							error++;
					}
				}


				if (x % (n/u) == 0) {

					// Calculuate the ratio of complete-to-incomplete.
					ratio = x/(float)n;
					c     = ratio * w;

					// Show the percentage complete.
					printf("%3d%% [", (int)(ratio*100) );

					// Show the load bar.
					for (x1=0; x1<c; x1++)
					   printf("=");

					for (x1=c; x1<w; x1++)
					   printf(" ");
 					// ANSI Control codes to go back to the
					// previous line and clear it.
					// printf("]\n33[F33[J");
					printf("]\r"); // Move to the first column
					fflush(stdout);

				}

				filePointer += (transfer_length << 9);
				x++;

			} else
				transfer_length -= 1;

		}


	}

	TimeNow();

	if (!error) {
		printf("\n\nSD card programming completed successfully\n\n");
		ret_val = 0;
	} else {
		printf("\n\nSD card programming Errors = %d\n\n",error);
		ret_val = -10;
	}

	printf(" *** SD card programming ended over %.1f sec\n\n\n",tsec2);










	RD_CloseDevice(rc47->SP_pd);

	return ret_val;
}







static void TimeNow(void) {
	Time(tt2);
	double ttv1 = ((double) tt1.tv_usec) / 1000000 + tt1.tv_sec;
	double ttv2 = ((double) tt2.tv_usec) / 1000000 + tt2.tv_sec;
	//  tsec=(int)(ttv2-ttv1);
	tsec2 = ttv2 - ttv1;
}





#define MAX_LINK_HOLD		5


// return 0 on successfull waiting on link up
// (cmp_c3_ok << 3) + (cmp_c2_ok << 2) + (cmp_c1_ok << 1) + (cmp_c0_ok << 0) on timeout error
int wait_for_link_up(struct rc47_board_t *rc47, int chip_select)
{


	int i,j,  t=0;

	int p=0;

	int c3_tgl[MAX_LINK_HOLD], c2_tgl[MAX_LINK_HOLD], c1_tgl[MAX_LINK_HOLD], c0_tgl[MAX_LINK_HOLD];
	int cmp_val, cmp_c3_ok=0, cmp_c2_ok=0, cmp_c1_ok=0, cmp_c0_ok=0;

	unsigned int reg_val, lnk_status;

	for(j=1;j<MAX_LINK_HOLD; j++) {
		c0_tgl[j] = ~j; 
		c1_tgl[j] = ~j; 
		c2_tgl[j] = ~j; 
		c3_tgl[j] = ~j; 
	}

	int	dev = rc47->SP_pd->intfd;

while(1) {	// timeout is 20 s

	usleep(100000);		// 100 ms

	t++; 

	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_STATUS,lnk_status);
	printf("V7_LNK_STATUS = 0x%8x\n", lnk_status);

	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_0,reg_val);
	printf("V7_LNK_TGL_CNT_0 = 0x%8x\n", reg_val);
	// save c0 tgl cnt to fifo
	c0_tgl[p] = reg_val;

	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_1,reg_val);
	printf("V7_LNK_TGL_CNT_1 = 0x%8x\n", reg_val);
	// save c1 tgl cnt to fifo
	c1_tgl[p] = reg_val;

	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_2,reg_val);
	printf("V7_LNK_TGL_CNT_2 = 0x%8x\n", reg_val);
	// save c2 tgl cnt to fifo
	c2_tgl[p] = reg_val;

	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_3,reg_val);
	printf("V7_LNK_TGL_CNT_3 = 0x%8x\n", reg_val);
	// save c3 tgl cnt to fifo
	c3_tgl[p] = reg_val;

	if (p==MAX_LINK_HOLD-1) {
		p=0;
	} else 
		p++; 

	if (lnk_status&(1<<24)) {
		cmp_c3_ok = 1; cmp_val = c3_tgl[0];
		for(j=1;j<MAX_LINK_HOLD; j++)
			if (cmp_val != c3_tgl[j]) {
				cmp_c3_ok = 0;
				break;
			} else 
				cmp_val = c3_tgl[j]; 
					
	}

	if (lnk_status&(1<<16)) {
		cmp_c2_ok = 1; cmp_val = c2_tgl[0];
		for(j=1;j<MAX_LINK_HOLD; j++)
			if (cmp_val != c2_tgl[j]) {
				cmp_c2_ok = 0;
				break;
			} else 
				cmp_val = c2_tgl[j]; 
					
	}

	if (lnk_status&(1<<8)) {
		cmp_c1_ok = 1; cmp_val = c1_tgl[0];
		for(j=1;j<MAX_LINK_HOLD; j++)
			if (cmp_val != c1_tgl[j]) {
				cmp_c1_ok = 0;
				break;
			} else 
				cmp_val = c1_tgl[j]; 
					
	}

	if (lnk_status&(1<<0)) {
		cmp_c0_ok = 1; cmp_val = c0_tgl[0];
		for(j=1;j<MAX_LINK_HOLD; j++)
			if (cmp_val != c0_tgl[j]) {
				cmp_c0_ok = 0;
				break;
			} else 
				cmp_val = c0_tgl[j]; 
					
	}

	if (cmp_c3_ok && cmp_c2_ok && cmp_c1_ok && cmp_c0_ok)
		break;
	
}

	if (cmp_c3_ok && cmp_c2_ok && cmp_c1_ok && cmp_c0_ok) {
		
		printf("Link is stable after t = %d\n",t);
		printf("Tgl C0 cnt = %d\n",c0_tgl[0]);
		printf("Tgl C1 cnt = %d\n",c1_tgl[0]);
		printf("Tgl C2 cnt = %d\n",c2_tgl[0]);
		printf("Tgl C3 cnt = %d\n",c3_tgl[0]);
		printf("LNK_STATUS = 0x%8x\n\n",lnk_status);

		printf("ticks \t\t ms \n");

		// read lnk toggle moments from Spartan FIFO
		for (i=0; i<c3_tgl[0]; i++) {
			RD_WriteDeviceReg32m(dev, SP6_BAR, V7_LNK_STATUS, (1<<1)); // reverse?? check in spartan
			RD_ReadDeviceReg32m(dev,SP6_BAR,TIME_FIFO_3,reg_val);
			printf("%9d\t%f \n",reg_val, ((double)reg_val)*0.000016);

		}

		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_STATUS,lnk_status);
		printf("\n\nLNK_STATUS = 0x%8x\n\n",lnk_status);

		return 0;
	}


	return (cmp_c3_ok << 3) + (cmp_c2_ok << 2) + (cmp_c1_ok << 1) + (cmp_c0_ok << 0); 

/*
	printf("Starting waiting on link for 10 min\n\n");

	int c3_tg, c2_tg, c1_tg, c0_tg; 

	for (i=0;i<10*60; i++) {

		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_STATUS,lnk_status);
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_0,c0_tg);
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_1,c1_tg);
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_2,c2_tg);
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_LNK_TGL_CNT_3,c3_tg);
		printf("%d: LNK_STATUS = 0x%8x\t C0 tgl_cnt = %d\t C1 tgl_cnt = %d\t C2 tgl_cnt = %d\t C3 tgl_cnt = %d\t\n",i, lnk_status, c0_tg, c1_tg, c2_tg, c3_tg);


		sleep(1);

	}
*/


}



// CBUS functions




// int dev - spartan6 device id
// int chip_select - virtex7 chip select
// return v7 cbus TX_FIFO word count through cbus and sp6 pcie or -1 on error
int cbus_get_v7_tx_fifo_word_count(int dev, int chip_select)
{
	unsigned int read_val, count;
	
	int cs;

	switch (chip_select) {
		case CS_C0: cs = 0; break;
		case CS_C1: cs = 1; break;
		case CS_C2: cs = 2; break;
		case CS_C3: cs = 3; break;

		default: fprintf(stderr,"Chip select error\n"); return -1;
	}
	
	
	// issue cmd to fetch v7 TX_FIFO status to sp6
	read_val = (SP6_FETCH_V7_FIFO_STATUS_CMD << 4) + (cs & 0x0000000F);
	RD_WriteDeviceReg32m(dev, SP6_BAR, SP6_V7_RULE, read_val);	
	
	usleep(1000);
	
	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_F_STS,read_val);
	count = read_val & 0x00001FFF;
	//printf("\tstatus = 0x%x, %d, %.1f, \n",count,count,((float)count)/4.0 );
	
	return count;
}


// read v7 TX_FIFO through cbus and sp6 pcie
// int dev - spartan6 device id
// int chip_select - virtex7 chip select
// return actually read word count or -1 on error
int cbus_read_v7_tx_fifo(int dev, int chip_select, int length, unsigned int *rd_buf)
{
	unsigned int read_val;
	int cs;
	int i;

	switch (chip_select) {
		case CS_C0: cs = 0; break;
		case CS_C1: cs = 1; break;
		case CS_C2: cs = 2; break;
		case CS_C3: cs = 3; break;

		default: fprintf(stderr,"Chip select error\n"); return -1;
	}	
	
	// Issue cmd to fetch data from Virtex7 TX_FIFO
	read_val = (SP6_FETCH_V7_FIFO_DATA_CMD << 4) + (cs & 0x0000000F);	
	RD_WriteDeviceReg32m(dev, SP6_BAR, SP6_V7_RULE, read_val);	
	
	// wait
	//sleep(1); 
	
	//printf("Checking status of Spartan6 Read fifo\n"); 
	RD_ReadDeviceReg32m(dev,SP6_BAR,V7_RD_ST,read_val);
	//printf("data count = %d\n",(read_val & 0x000003FF));	
	
	
	for (i=0; i<length; i++) {
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_RD_ST,read_val);
		if (!(read_val & (1<<16))) { // not empty
			RD_ReadDeviceReg32m(dev,SP6_BAR,V7_RD_FD,read_val);
			rd_buf[i] = read_val; 
		} else  
			break; 
	}
	
	return i; 

}


// int dev - spartan6 device id
// int chip_select - virtex7 chip select
// return actually wrote word count or -1 on error
int cbus_write_v7_rx_fifo(int dev, int chip_select, int length, unsigned int *wr_buf)
{
	int i, t;
	unsigned int read_val;	
	
	int cs;

	for (i=0; i<length; i++) {
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_WR_ST,read_val);
		
		if (!(read_val & (1<<1))) { // not full
			RD_WriteDeviceReg32m(dev, SP6_BAR, V7_WR_FD, wr_buf[i]);
		} else 
			break;
	}

	switch (chip_select) {
		case CS_C0: cs = 0; break;
		case CS_C1: cs = 1; break;
		case CS_C2: cs = 2; break;
		case CS_C3: cs = 3; break;

		default: fprintf(stderr,"Chip select error\n"); return -1;
	}
	
	// issue cmd to start transfer from sp6 to v7
	read_val = (SP6_WRITE_V7_FIFO_CMD << 4) + (cs & 0x0000000F);
	RD_WriteDeviceReg32m(dev, SP6_BAR, SP6_V7_RULE, read_val);
	
	// wait for transfer complete
	for(t=0; t<100; t++){
		RD_ReadDeviceReg32m(dev,SP6_BAR,V7_WR_ST,read_val);
		//printf("\tstatus = 0x%x\n",read_val);
		if (read_val == 1)	// empty
			break;
	}	
	
	if (t>99) {
		fprintf(stderr,"Timeout waiting for SP6->V7 transfer complete!\n");
		return -1;
	}
	
	return i;
}



#ifdef USE_PLX
S16
SelectDevice_DMA(
	PLX_DEVICE_KEY    *DevKey_DMA
    )
{
    S32               i;
    S32               NumDevices;
    BOOLEAN           bAddDevice;
	BOOLEAN			  check;
    PLX_STATUS        status;
    PLX_DEVICE_KEY    DevKey;
    //PLX_DEVICE_KEY    DevKey_DMA[MAX_DEVICES_TO_LIST];
    PLX_DRIVER_PROP   DriverProp;
    PLX_DEVICE_OBJECT Device;


    printf("\n");

    i          = 0;
    NumDevices = 0;

    do
    {
        // Setup for next device
        memset(&DevKey, PCI_FIELD_IGNORE, sizeof(PLX_DEVICE_KEY));

        // Check if device exists
        status =
            PlxPci_DeviceFind(
                &DevKey,
                (U16)i
                );

        if (status == ApiSuccess)
        {
            // Default to add device
            bAddDevice = TRUE;
			check	   = TRUE;

            if (bAddDevice)
            {
                // Verify supported chip type



                        if ((DevKey.PlxChip& 0xFFFF) == 0x8725)
                        {							
						
                            // DMA is always function 1 or higher
                            if (DevKey.function == 0)
                                bAddDevice = FALSE;
                        }
                        else
                        {
							bAddDevice = FALSE;
							
							if ((DevKey.PlxChip& 0xFFFF) == 0x8732){
								bAddDevice = TRUE;
								check = FALSE;
							} 
                           
                        }

            }
			
			
			//printf(bAddDevice ? "true" : "false");
            if (bAddDevice)
            {;
                // Open device to get its properties
                PlxPci_DeviceOpen(
                    &DevKey,
                    &Device
                    );
            }

            // Verify driver used is PnP and not Service driver
            if (bAddDevice && check)
            {
                PlxPci_DriverProperties(
                    &Device,
                    &DriverProp
                    );

                if (DriverProp.bIsServiceDriver)
                {
                    bAddDevice = FALSE;
                }
            }

            // Close device
            PlxPci_DeviceClose(
                &Device
                );



            if ((bAddDevice))
            {
                // Copy device key info
                DevKey_DMA[NumDevices] = DevKey;


                // Increment device count
                NumDevices++;
            }
        }

        // Go to next devices
        i++;
    }
    while ((status == ApiSuccess) && (NumDevices < MAX_DEVICES_TO_LIST));

    if (NumDevices == 0)
        return 0;




    // Return selected device information
    //*pKey = DevKey_DMA[i - 1];

    return (S16)NumDevices;
}

int rc47_init_plx(PLX_DEVICE_OBJECT * Device, PLX_NOTIFY_OBJECT * NotifyObject, PLX_INTERRUPT  *   PlxInterrupt, PLX_DMA_PROP    *  DmaProp){

U8     DmaChannel;
PLX_STATUS        status;

	DmaChannel = (U8)0;
	
 		
    // Open the DMA channel
	printf("  Open Channel %i for DMA......... ", DmaChannel);
	status = PlxPci_DmaChannelOpen(Device,DmaChannel,NULL);

if (status == ApiSuccess)
{
        printf("Ok\n");
}
else
{
        printf("*ERROR* - API failed\n");
		return -1;
}

printf("  Get DMA propeties.............. ");
status =PlxPci_DmaGetProperties(Device,DmaChannel,DmaProp);

if (status == ApiSuccess)
{
        printf("Ok\n");
}
else
{
        printf("*ERROR* - API failed\n");
		return -2;
}


// Set to support 128B TLP read request size
DmaProp->MaxSrcXferSize = PLX_DMA_MAX_SRC_TSIZE_2K;
DmaProp->MaxDestWriteSize = PLX_DMA_MAX_SRC_TSIZE_128B;
	
    // Set DMA properties
printf("  Set DMA propeties.............. ");
status = PlxPci_DmaSetProperties(Device,DmaChannel,DmaProp);

    if (status == ApiSuccess)
    {
        printf("Ok\n");
    }
    else
    {
        printf("*ERROR* - API failed\n");
		return -3;
    }
	
	
printf("  Register for notification...... ");

    // Clear interrupt fields
    memset( PlxInterrupt, 0, sizeof(PLX_INTERRUPT) );

    // Setup to wait for selected DMA channel
    PlxInterrupt->DmaDone = (1 << DmaChannel);

status =PlxPci_NotificationRegisterFor(Device, PlxInterrupt, NotifyObject);

 if (status != ApiSuccess)
    {
        printf("*ERROR* - API failed\n");
		return -4;
    }
    else
    {
        printf( "Ok\n" );
    }


return 0;

}


int rc47_release_plx(PLX_DEVICE_OBJECT * Device,  PLX_NOTIFY_OBJECT * NotifyObject){
PLX_STATUS        status;
// Release the interrupt wait object
     printf("  Cancelling Int Notification.... ");
    status =PlxPci_NotificationCancel(Device,NotifyObject);

    if (status != ApiSuccess)
    {
        printf("*ERROR* - API failed\n");
		return -1;
    }
    else
    {
        printf("Ok\n");
    } 
	
	

    // Close DMA Channel
    printf("  Close DMA Channel.............. ");
    status = PlxPci_DmaChannelClose(Device, 0);

if (status == ApiSuccess)
{
        printf("Ok\n");
}
else
{
        printf("*ERROR* - API failed\n");
		return -1;
}

return 0;

}

void rc47_set_plx_dma_param(unsigned int addr, unsigned int len, int * wr_buf, PLX_DMA_PARAMS *   DmaParams){
memset(DmaParams, 0, sizeof(PLX_DMA_PARAMS));	
DmaParams->UserVa    = (PLX_UINT_PTR)wr_buf;
DmaParams->PciAddr   = addr;
DmaParams->ByteCount = len*4;
DmaParams->Direction = PLX_DMA_USER_TO_PCI;
DmaParams->bConstAddrDest = (U8)1;
			
}

#endif











