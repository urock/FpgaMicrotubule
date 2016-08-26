/*
 * RostaPCIeAPI.h
 *
 *  Created on: May 16, 2012
 *      Author: urock
 */
#ifndef PLX_LINUX
    #define PLX_LINUX         
#endif
#ifdef USE_PLX
	#include "PlxApi.h"
#endif
#ifndef COMMONAPI_H_
#define COMMONAPI_H_

#ifdef __cplusplus
#   define API extern "C"
#else
#   define API
#endif


#include "sys-include.h"
#include "rosta-ioctl.h"
//#include "rosta-common.h"

//typedef unsigned long long uint64_t; 

// comment this to turn off debug printf
#define ROSTA_DEBUG

#ifdef ROSTA_DEBUG
	#define msg_dbg(format, arg...) printf(format, ## arg)
#else
	#define msg_dbg(format, arg...)
#endif


#define MAX_PCIE_DEVICES	128
#define MAX_RC47_BOARDS		12


#define MAX_DMA_TRANSFER_LENGTH		0x400000		// 4 MB


#define PAGE_MASK 	0xFFFFF000




#define DEVICE_NAME "rosta_dev"


struct pci_region
{
	unsigned base;
	unsigned length;

	int fd;

	unsigned char *localbase;
	unsigned localoffset;
};


struct parent_pci
{
unsigned parent_phys_domain;
unsigned parent_phys_bus;
unsigned parent_phys_device;
unsigned parent_phys_function;
};
struct pci_device
{
	// driver numbers
	int major;
	int minor;
	int version; 

	// Set during locate

	unsigned vendor_id, device_id;
	unsigned subsys_vendor_id, subsys_device_id;

	unsigned phys_domain, phys_bus, phys_device, phys_function;
	struct   parent_pci parent[100];
	int 	 num_parent;
	unsigned instance;

	// Set in RD_FindDevices

	struct pci_region config;
	struct pci_region regions[6];

	// device handle, set in RD_OpenDevice
	int intfd;
};


/* ************** Exported Functions *****************/

// RD - Rosta Device API functions

API int RD_FindDevices(struct pci_device *pd, int verbose);
API int RD_PrintDeviceInfo(struct pci_device *p);
API int RD_OpenDevice(struct pci_device *p);
API void RD_CloseDevice(struct pci_device *p);
API int RD_DisableDevice(struct pci_device *p);
API int RD_RescanBus(void);
API int RD_EnableDevice(struct pci_device *p);

API int RD_WriteDeviceReg32(int fd, unsigned int bar, unsigned int offset, unsigned int data);
API int RD_ReadDeviceReg32(int fd, unsigned int bar, unsigned int offset, unsigned int* data);
API int RD_ReadBlockDma(int dev,unsigned int *buf,unsigned int len_bytes);
API int RD_WriteBlockDma(int dev,unsigned int *buf,unsigned int len_bytes);


// dual dma transfer is not supported in Windows driver - don't implement this function under Win
API int RD_DualDmaTransfer(int fd, unsigned int * src, unsigned int * dst,
		unsigned int len_write, unsigned len_read);

API int RD_SetPacketLength(int dev, unsigned int sizeB);

API int create_page_alligned_buffer(int buf_len, unsigned int **buf_out, unsigned int **buf_free);






// not used functions
int rsp5xx_get_bar_info(int fd, unsigned int bar, unsigned int *addr, unsigned int *size);
int device_register_set_bits(int fd, unsigned int bar, unsigned int offset, unsigned int *reg_val, unsigned int set_bits);
int device_register_reset_bits(int fd, unsigned int bar, unsigned int offset, unsigned int *reg_val, unsigned int reset_bits);
int rsp_p2p_dma(int fd, unsigned int src, unsigned int dst, unsigned int size_bytes);


// internal functions
API int pcid_device_open(struct pci_device *p);
API int pcid_device_close(struct pci_device *p);



// macro defines
#define do_and_test(xx,yy) \
	do { \
		r = xx yy; \
		if (r < 0) \
		{ \
			printf(# xx # yy ": error %d\n",r); \
			return r; \
		} \
	} while (0)



#define	RD_ReadDeviceReg32m(dev, bar, addr, reg_val);									\
	if (RD_ReadDeviceReg32(dev, bar, addr, &reg_val) < 0) {								\
		fprintf(stderr,"Error in RD_ReadDeviceReg32: dev=%d, bar=%d, addr=%d\n",dev,bar,addr);	\
		return -3;																		\
	}


#define	RD_WriteDeviceReg32m(dev, bar, addr, reg_val);									\
	if (RD_WriteDeviceReg32(dev, bar, addr, reg_val) < 0) {								\
		fprintf(stderr,"Error in RD_WriteDeviceReg32: dev=%d, bar=%d, addr=%d\n",dev,bar,addr);	\
		return -3;																		\
	}



#endif /* COMMONAPI_H_ */

