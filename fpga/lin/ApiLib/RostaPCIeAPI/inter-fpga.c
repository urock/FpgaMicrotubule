/*
 * inter_fpga.c
 *
 *  Created on: May 14, 2013
 *      Author: urock
 *
 *
 *
 *      File containing functions for Inter FPGA data transfer over PCI Express
 *
 *      Uses pthreads
 *
 */



#include "RostaPCIeAPI.h"
#include "inter-fpga.h"


struct fpga_dma_t	fpga_read;
struct fpga_dma_t	fpga_write;


int device_write_status[MAX_PCIE_DEVICES];
int device_read_status[MAX_PCIE_DEVICES];



// Internal functions prototypes

void *fpga_read_thread(void *arg);
void *fpga_write_thread(void *arg);


// fpga writes to system memory
void *fpga_write_thread(void *arg)
{

	int ret_val=0;


	struct fpga_dma_t	*fpga_dma;


	fpga_dma = (struct fpga_dma_t *) arg;

/*	printf("fpga_write_thread: dev = %d, rd_buf = 0x%p, len = %d\n",fpga_dma->src_dev,
			fpga_dma->buffer,
			fpga_dma->len_dwords);*/


	// read data from dev
	ret_val = RD_ReadBlockDma(	fpga_dma->src_dev,
								fpga_dma->buffer,
								fpga_dma->len_dwords*4);

	if (ret_val < 0) {
		printf("fpga_write_thread: Error writing data to fpga\n");
		device_write_status[fpga_dma->src_dev] = DMA_STATUS_ERROR;
	} else {
		//printf("fpga_write_thread: Exit successful\n");
		device_write_status[fpga_dma->src_dev] = DMA_STATUS_OK;
	}



	pthread_exit(NULL);


}


// fpga reads system memory
void *fpga_read_thread(void *arg)
{

	int ret_val=0;


	struct fpga_dma_t	*fpga_dma;


	fpga_dma = (struct fpga_dma_t *) arg;

	/*printf("fpga_read_thread: dev = %d, wr_buf = 0x%p, len = %d\n",fpga_dma->src_dev,
																	fpga_dma->buffer,
																	fpga_dma->len_dwords);*/



	ret_val = RD_WriteBlockDma(	fpga_dma->src_dev,
								fpga_dma->buffer,
								fpga_dma->len_dwords*4);
	if (ret_val < 0) {
		printf("fpga_read_thread: Error writing data to fpga\n");
		device_read_status[fpga_dma->src_dev] = DMA_STATUS_ERROR;
	} else {
		//printf("fpga_read_thread: Exit successful\n");
		device_read_status[fpga_dma->src_dev] = DMA_STATUS_OK;
	}

	pthread_exit(NULL);

}


// Externals

/**
 * 	Non blocking write to FPGA.
 *	Device reads host memory
 *
 */
int RD_DeviceReadHostMemory(int dev, unsigned int *buffer, int len_dwords)
{
	int ret_val;
	pthread_t write_thread;

	pthread_attr_t thread_attr;

	ret_val = pthread_attr_init(&thread_attr);
	if (ret_val != 0) {
		printf("pthread_attr_init failed\n");
		return -1;
	}

	ret_val = pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if (ret_val != 0) {
		printf("pthread_attr_setdetachstate failed\n");
		return -1;
	}

	fpga_read.src_dev = dev;
	fpga_read.buffer = buffer;
	fpga_read.len_dwords = len_dwords;


	device_read_status[dev] = 0;

	ret_val = pthread_create(&write_thread,&thread_attr,fpga_read_thread,(void *)&fpga_read);
	if (ret_val != 0) {

		printf("pthread_create failed\n\n");
		return -1;

	}

	(void)pthread_attr_destroy(&thread_attr);

	return 0;
}



/**
 * 	Non blocking read from FPGA.
 *	Device writes host memory
 *
 *	*buffer - ptr to read buffer
 *
 *
 *
 */
int RD_DeviceWriteHostMemory(int dev, unsigned int *buffer, int len_dwords)
{
	int ret_val;
	pthread_t read_thread;

	pthread_attr_t thread_attr;

	ret_val = pthread_attr_init(&thread_attr);
	if (ret_val != 0) {
		printf("pthread_attr_init failed\n");
		return -1;
	}

	ret_val = pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if (ret_val != 0) {
		printf("pthread_attr_setdetachstate failed\n");
		return -1;
	}

	fpga_write.src_dev = dev;
	fpga_write.buffer = buffer;
	fpga_write.len_dwords = len_dwords;



	// reset dma status
	device_write_status[dev] = 0;


	ret_val = pthread_create(&read_thread,&thread_attr,fpga_write_thread,(void *)&fpga_write);
	if (ret_val != 0) {

		printf("pthread_create failed\n\n");
		return -1;

	}
	(void)pthread_attr_destroy(&thread_attr);

	return 0;
}


int RD_CheckDeviceWriteStatus(int dev)
{
	int t = 0;

	//printf("Waiting for completion FPGA (dev = %d) writting to RAM\n...", dev);

	while(t++<20000) {	// timeout = 2s

		if (device_write_status[dev] != 0)
			return device_write_status[dev];

		usleep(100); // 1000 us
	}

	return DMA_STATUS_TIMEOUT;
}


int RD_CheckDeviceReadStatus(int dev)
{
	int t = 0;

	//printf("Waiting for completion FPGA (dev = %d) reading RAM\n...", dev);

	while(t++<20000) {	// timeout = 2s

		if (device_read_status[dev] != 0)
			return device_read_status[dev];

		usleep(100); // 1000 us
	}

	return DMA_STATUS_TIMEOUT;
}

