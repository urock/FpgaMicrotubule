
#include "RostaPCIeAPI.h"


int RD_WriteDeviceReg32(int fd, unsigned int bar, unsigned int offset,
		unsigned int data) {
	rsp5xx_reg32_io_t reg32_io;

	reg32_io.bar = bar;
	reg32_io.offset = offset;
	reg32_io.data = data;

	return ioctl(fd, RSP_REG32_WRITE, &reg32_io);
}

int RD_ReadDeviceReg32(int fd, unsigned int bar, unsigned int offset,
		unsigned int* data) {
	int ret;
	rsp5xx_reg32_io_t reg32_io;

	reg32_io.bar = bar;
	reg32_io.offset = offset;
	reg32_io.data = 0;

	ret = ioctl(fd, RSP_REG32_READ, &reg32_io);
	*data = reg32_io.data;
	return ret;
}

int RD_DualDmaTransfer(int fd, unsigned int * src, unsigned int * dst,
		unsigned int len_write, unsigned len_read)
{

	RD_DualDmaTransfer_t dma;
	dma.src = (unsigned long long) src;
	dma.dest = (unsigned long long) dst;
	dma.size_write = len_write;
	dma.size_read = len_read;
	return ioctl(fd, RSP_DUAL_DMA ,&dma);

}

int RD_ReadBlockDma(int dev, unsigned int *buf, unsigned int len_bytes) {
	return read(dev, (void*) buf, len_bytes);
}

int RD_WriteBlockDma(int dev, unsigned int *buf, unsigned int len_bytes) {

	return write(dev, (void*) buf, len_bytes);
}


int RD_SetPacketLength(int dev, unsigned int sizeB) // sizeB - pcie packet size in bytes
{
	two_dwords_t plen;
	unsigned int tmp;

	switch (sizeB) {

	case 4: 	tmp = 2; break;
	case 8: 	tmp = 3; break;
	case 16: 	tmp = 4; break;
	case 32:	tmp = 5; break;
	case 64:	tmp = 6; break;
	case 128:	tmp = 7; break;
	case 256:	tmp = 8; break;
	default:	return -10;
	}

	plen.dword1 = (unsigned int) sizeB >> 2; // driver needs value of packet length in DWORDs
	plen.dword2 = tmp; // this tells driver how many bits to shift
	// to calc number of packets to transfer

	return ioctl(dev, RSP_SET_PACKET_LEN, &plen);
}




int create_page_alligned_buffer(int buf_len, unsigned int **buf_out, unsigned int **buf_free)
{
	int 			wset;
	unsigned long 	page_addr;
	unsigned long 	page_offset;

	unsigned int *buf, *buf_alligned;

	buf = (unsigned int *) malloc(buf_len * 4 + 4096); // allocated len*4 bytes
	if (!buf)
		return -1;

	*buf_free = buf;

	page_addr = (unsigned long) (&buf[0]) & PAGE_MASK;

	page_offset = (unsigned long) &buf[0] - page_addr;

	wset = (4096 - ((int) page_offset)) >> 2;

	buf_alligned = &buf[wset];

	*buf_out = buf_alligned;

	return 0;
}


// not used functions

int rsp5xx_get_bar_info(int fd, unsigned int bar, unsigned int *addr,
		unsigned int *size) {
	rsp5xx_bar_info_t bar_info;
	int ret;

	bar_info.id = bar;
	bar_info.addr = 0;
	bar_info.size = 0;
	ret = ioctl(fd, RSP_BAR_INFO, &bar_info);
	*addr = bar_info.addr;
	*size = bar_info.size;
	return ret;
}


int device_register_set_bits(int fd, unsigned int bar, unsigned int offset,
		unsigned int *reg_val, unsigned int set_bits)
{

	RD_ReadDeviceReg32(fd,bar,offset,reg_val);

	*reg_val |=  set_bits;

	RD_WriteDeviceReg32(fd,bar,offset,*reg_val);

	return 0;

}

int device_register_reset_bits(int fd, unsigned int bar, unsigned int offset,
		unsigned int *reg_val, unsigned int reset_bits)
{

	RD_ReadDeviceReg32(fd,bar,offset,reg_val);

	*reg_val &=  ~reset_bits;

	RD_WriteDeviceReg32(fd,bar,offset,*reg_val);

	return 0;
}


int rsp_p2p_dma(int fd, unsigned int src, unsigned int dst,
		unsigned int size_bytes)
{

	rsp_p2p_dma_t p2p_dma;
	p2p_dma.src = (unsigned long) src;
	p2p_dma.dest = (unsigned long) dst;
	p2p_dma.size = size_bytes;

	return ioctl(fd, RSP_P2P_DMA ,&p2p_dma);

}



