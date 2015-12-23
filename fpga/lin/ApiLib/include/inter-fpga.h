#ifndef INTER_FPGA_H
#define INTER_FPGA_H

/********************************************
 * Header file for Inter FPGA data transfer over PCI Express
 *
 * FPGA projects supported:
 *		rc_pcie_ep2ep_585_0
 *
 *
 *
 *
 *******************************************/


#include <pthread.h>



#define DMA_STATUS_OK		1
#define DMA_STATUS_ERROR	-1
#define DMA_STATUS_TIMEOUT	-2



 #ifdef __cplusplus
#   define API extern "C"
#else
#   define API
#endif


API int RD_DeviceReadHostMemory(int dev, unsigned int *buffer, int len_dwords);
API int RD_CheckDeviceReadStatus(int dev);

API int RD_DeviceWriteHostMemory(int dev, unsigned int *buffer, int len_dwords);
API int RD_CheckDeviceWriteStatus(int dev);
//API void RD_Init_end_of_programm_dma_transfer_handler(int fdev1); //use it only with  RD_DeviceWriteHostMemory





struct fpga_dma_t{

	int 			src_dev;		//!< Source device file descriptor
	unsigned int 	*buffer;		//!< Pointer to user space write buffer
	unsigned int 	len_dwords;		//!< Number of DWORDs to transfer

};



#endif // INTER_FPGA_H
