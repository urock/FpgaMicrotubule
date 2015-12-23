#ifndef __RSP_IOCTL_H
#define __RSP_IOCTL_H

#include "rosta-types.h"

#define RSP_DRV_IOCTL_BASE 'r'
//#define RSP_DEVNAME "/dev/rsp5xx"

#define DRIVER_NAME "rosta_drv"


#define RSP_DEV_INFO 		_IOR  (RSP_DRV_IOCTL_BASE, 0, rsp5xx_device_info_p)
#define RSP_BAR_INFO 		_IOWR (RSP_DRV_IOCTL_BASE, 1, rsp5xx_bar_info_p)
#define RSP_REG32_READ 		_IOWR (RSP_DRV_IOCTL_BASE, 2, rsp5xx_reg32_io_p)
#define RSP_REG32_WRITE 	_IOW  (RSP_DRV_IOCTL_BASE, 3, rsp5xx_reg32_io_p)
#define RSP_SET_PACKET_LEN 	_IOW  (RSP_DRV_IOCTL_BASE,10, two_dwords_p)
#define RSP_DUAL_DMA		_IOWR (RSP_DRV_IOCTL_BASE,13, RD_DualDmaTransfer_p)
#define RSP_P2P_DMA			_IOWR (RSP_DRV_IOCTL_BASE,14, rsp_p2p_dma_p)


#endif



