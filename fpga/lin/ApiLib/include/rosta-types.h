#ifndef __RSP5XX_TYPES_H
#define __RSP5XX_TYPES_H

/*
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned long long u64;
*/

typedef struct {
	unsigned short device_id;
	unsigned short vendor_id;
} rsp5xx_device_info_t, *rsp5xx_device_info_p;

//typedef rsp5xx_device_info_t* rsp5xx_device_info_t_ptr

typedef struct {
	unsigned int id;
	unsigned int addr;
	unsigned int size;
} rsp5xx_bar_info_t, *rsp5xx_bar_info_p;




typedef struct {
	unsigned int dword1;
	unsigned int dword2;
} two_dwords_t, *two_dwords_p;

typedef struct {
	unsigned int bar;
	unsigned int offset;
	unsigned int data;
} rsp5xx_reg32_io_t, *rsp5xx_reg32_io_p;

typedef struct {

	unsigned long long dest;
	unsigned long long src;
	unsigned long long size_read;
	unsigned long long size_write;
//	u32 * retval;


} RD_DualDmaTransfer_t, *RD_DualDmaTransfer_p;


typedef struct {

	unsigned long src;
	unsigned long dest;
	unsigned long size;

} rsp_p2p_dma_t, *rsp_p2p_dma_p;




typedef enum { // fpga kind
	fk_top, fk_bot,
} generic_fpga_kind_t;

typedef enum { // fpga kind
	gio_read = 1, gio_write = 2,
} generic_io_t;

typedef struct {
	unsigned int offset, value;
} rsp5xx_request;

typedef struct {
	unsigned int addr;
	unsigned int data;
} rsp5xx_ad;

struct rsp5xx_dma_cfg {
	int direction;
	unsigned int zbtoffset;
	unsigned int count;
	unsigned int addsetsize;
	rsp5xx_ad ** addset;
};

typedef struct rsp5xx_dma_cfg rsp5xx_dma_cfg_t;

#endif

