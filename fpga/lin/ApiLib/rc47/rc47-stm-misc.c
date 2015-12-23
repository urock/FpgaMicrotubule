/*
 * rc47-stm-misc.c
 *
 *  Created on: May 21, 2012
 *      Author: urock
 */


#include "RostaPCIeAPI.h"
#include "rc47-api.h"
#include "rc47-stm-misc.h"



// doesn't wait for ack right after sending cmd_code
// instead ack is expected as first byte of reply
int rc47_send_cmd_frame(int dev, struct cmd_frame_s * frame)
{
	unsigned char val8;




	//msg_dbg("\n\nSending cmd frame...\n");

	if (rc47_send_byte_stm(dev,frame->cmd_code) < 0) {
		printf("rc47_send_cmd_frame: Error sending cmd code to sp6\n");
		return -3;
	}


	switch (frame->cmd_code) {


	case CMD_V7_RST:

		if (rc47_send_byte_stm(dev,frame->cmd_data.v7_rst_data.v7_index) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}


		break;

	case CMD_V7_RECONFIG:

		if (rc47_send_byte_stm(dev,frame->cmd_data.v7_reconfig_data.v7_index) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}
		if (rc47_send_byte_stm(dev,frame->cmd_data.v7_reconfig_data.bit_id) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}
		break;

	case CMD_READ_SDIO:

	case CMD_WRITE_SDIO:


		if (rc47_send_byte_stm(dev,(unsigned char)frame->cmd_data.sdio_access.addr) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}

		if (rc47_send_byte_stm(dev,(unsigned char)(frame->cmd_data.sdio_access.addr >> 8)) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}

		if (rc47_send_byte_stm(dev,(unsigned char)(frame->cmd_data.sdio_access.addr >> 16)) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}

		if (rc47_send_byte_stm(dev,(unsigned char)(frame->cmd_data.sdio_access.addr >> 24)) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}

		if (rc47_send_byte_stm(dev,(unsigned char)frame->cmd_data.sdio_access.length) < 0) {
			printf("rc47_send_cmd_frame: Error sending byte to sp6\n");
			return -3;
		}


		break;


	default:					break;

	}

	if (rc47_receive_byte_stm(dev,&val8) < 0) {
		printf("rc47_send_cmd_frame: Error receiving acknowledge\n");
		return -3;
	}



	if (( val8 == CMD_ACK) ) {
		//printf("ACK OK\n");
		return 0;
	}

	if ( val8 == CMD_NACK ) {	// check acknowledge
		printf("CMD_NACK\n");
		return -5;
	} else {
		printf("Bad ACK from STm: %x\n",val8);
		return -6;
	}

	return 0;
}




inline int rc47_send_byte_stm(int dev, unsigned char data_byte)
{


	return RD_WriteDeviceReg32(dev,SP6_BAR,WRITEDATA_STM_REG,(unsigned int)data_byte);

}



int rc47_send_buffer_stm(int dev, unsigned char *src_buf, int byte_count)
{
	int i;

	for (i=0;i<byte_count; i++) {

		if (rc47_send_byte_stm(dev,(unsigned int)src_buf[i]) < 0) {
			printf("rc47_send_buf_stm: Error sending byte to sp6; i = %d\n",i);
			return -3;
		}

	}

	return 0;
}



/**********************************************************
 *
 * function rc47_wait_for_host_tx_fifo_empty
 *
 *
 * this function should be called after long data writes
 * to host tx fifo in spartan6
 *
 *
 */


int rc47_wait_for_host_tx_fifo_empty(int dev)
{
	unsigned int 	val32;
	int 			t = 0;

	// wait for data
	while(t<=STM_TIMEOUT) {
		if (RD_ReadDeviceReg32(dev,SP6_BAR,RD_DATA_STATUS_REG,&val32) < 0) {
			printf("rc47_wait_for_host_tx_fifo_empty: Error reading from RD_DATA_STATUS_REG\n");
			return -1;
		}
		if (val32 & SP6_HOST_TX_EMPTY)	// if fifo was read out by mcu
			break;
		t++;
		usleep(500);
	}

	if (t >= STM_TIMEOUT) {
		printf("rc47_wait_for_host_tx_fifo_empty: Timeout!!\n");
		return -2;
	}


	return 0;

}


int rc47_receive_byte_stm(int dev, unsigned char *data_byte)
{
	unsigned int 	val32;
	int 			t = 0;

	// wait for data
	while(t<=(10*STM_TIMEOUT)) {
		if (RD_ReadDeviceReg32(dev,SP6_BAR,RD_DATA_COUNT_REG,&val32) < 0) {
			printf("rc47_receive_byte_stm: Error reading from RD_DATA_COUNT_REG\n");
			return -1;
		}
		if (val32 >= 1)	// if fifo has at least 1 byte
			break;
		t++;
		usleep(100);
	}

	if (t >= (10*STM_TIMEOUT)) {
		printf("rc47_receive_byte_stm: Timeout!!\n");
		return -2;
	}


	if (RD_ReadDeviceReg32(dev,SP6_BAR,READDATA_STM_REG,&val32) < 0) {
		printf("rc47_receive_byte_stm: Error reading from READDATA_STM_REG\n");
		return -1;
	}


	*data_byte = (unsigned char)val32;

	return 0;
}



int rc47_receive_buffer_stm(int dev, unsigned char *dst_buf, int num)
{
	unsigned int 	val32;
	int 			ret_val = 0, i, t =0;
	int 			bytes_transfered = 0; // total bytes transfered
	unsigned int 	xfer = 0;	// bytes to transfer in current phase



	while(bytes_transfered < num) {

		// wait for data
		while(t<=STM_TIMEOUT) {
			if (RD_ReadDeviceReg32(dev,SP6_BAR,RD_DATA_COUNT_REG,&val32) < 0) {
				printf("rc47_receive_buffer_stm: Error reading from RD_DATA_COUNT_REG\n");
				return -1;
			}
			if (val32 >= 1)	// if fifo has at least 1 byte
				break;
			t++;
		}

		if (t >= STM_TIMEOUT) {
			printf("rc47_receive_buffer_stm: Timeout!!\n");
			return -2;
		}



		if ((bytes_transfered + val32) <= num)
			xfer = val32;
		else
			xfer = num - bytes_transfered;

		// read xfer bytes from FIFO
		for (i=0;i<xfer;i++) {
			if (RD_ReadDeviceReg32(dev,SP6_BAR,READDATA_STM_REG,&val32) < 0) {
				printf("rc47_receive_buffer_stm: Error reading from READDATA_STM_REG\n");
				return -1;
			}
			dst_buf[bytes_transfered + i] = (unsigned char)val32;
		}

		bytes_transfered += xfer;

	}



	return ret_val;
}

