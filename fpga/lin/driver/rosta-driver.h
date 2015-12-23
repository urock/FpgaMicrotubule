#ifndef __rsp_H
#define __rsp_H

#include <linux/cdev.h>
#include <linux/module.h> 
#include <linux/init.h>   
#include <linux/version.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>  
#include <linux/dma-mapping.h>  
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <asm/delay.h>
#include <linux/list.h>
#include <linux/ioport.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/version.h>


//#include "../include/rosta-common.h" 
#include "../ApiLib/include/rosta-ioctl.h"

#define MAX_rsp_DEV 128
#define MAX_rsp_BAR 5


#define MAX_DEVICES 128


#define CNTL_BAR	0

// FPGA register addresses
#define M_WR_ADDR	0x0014
#define M_RD_ADDR	0x0018
#define M_WR_CNT	0x001C
#define M_RD_CNT	0x0020 // here length in bytes
#define M_MODE		0x0024

// Debug register in Master Engine
#define MasterMemRdTotalCnt		0x50
#define MasterEpTxTotalCnt		0x54
#define MasterEpRxTotalCnt		0x58
#define MasterMemWrTotalCnt		0x5C

#define FifoMemRdTotalCnt		0x60
#define FifoEpTxTotalCnt		0x64
#define FifoEpRxTotalCnt		0x68
#define FifoMemWrTotalCnt		0x6C


// Control flags in MODE register
#define STARD_WR	1	//  0		device writes to RAM
#define COMPL_WR	2	//  1
#define STARD_RD	4	//  2		device reads from RAM
#define COMPL_RD	8	//  3
#define ERROR_WR	16	//  4
#define ERROR_RD	32	//  5
#define ERROR_INT	64	//  6
#define ERROR_TIMEOUT	128	//  7
#define DUAL_DMA_BIT		256 
		//	512	//  9
		//	1024	// 10
		//	2048	// 11
		//	4096	// 12
		//	8192	// 13
#define RST_DMA_FIFO	16384	// 14
#define RST_LIST_FIFO	32768	// 15

#define STARD_P2P		(1<<24)	// peer to peer dma transfer
#define COMPL_P2P		(1<<25)



struct rsp_bar {
 unsigned long addr;
 unsigned long size;
 volatile unsigned long* vaddr;
 int io_port; // added by Yuri to distinguish between MEM and PORT regions
};

struct rsp_dma{
    int offset;
    struct page **pages;
    struct scatterlist *sglist;
    int sglen;
    int nr_pages;
    int direction;
    int tail;
};

typedef struct rsp_dma rsp_dma_t;


struct rsp_device {

	// Device number
	int instance;
	
	// Device type/handler data
	struct pci_dev *pdev;
 	struct cdev cdev;

	int 	dev_num;
	char 	name[64];

//	struct 	pci_dev* dev;

//-------------------------
	spinlock_t lock;

	struct rsp_bar bar[5];
	int 	bar_count;
	int 	cur_bar;
	int 	is_opened;
	int 	is_present;

//	int 	use_dma;
	int 	use_int;
	int 	dma_ch;
//	u32 	intr_reg,
	u32	intr_mask,intr;

	wait_queue_head_t 	intr_wait;
	spinlock_t 		intr_lock;
	dma_addr_t 		dma_addr[2];    
	u32 			*bus_addr[2];
  
	int 	current_buffer_a;
	int 	current_buffer_b;
	int 	irq_count;
	char 	*user_buffer;
    
	int 	direction;
    
	u32 	r_current;
	u32 	dma_w_process;
	u32 	int_waiting;
	struct 	rsp_dma dma_w;
	struct 	rsp_dma dma_r;
   
	int 	flag;
	int 	inter_handled;
	u32 	IRQ_MMR;
	u32 	position[6];

	u32	PacketLengthDW; // pcie packet length in DW
	u32	PLenShift; 

	int rsp_device_access;

};


// function prototypes
static ssize_t show_cdev(struct device_driver *driver, char *buf); 
ssize_t show_instance(struct device *dev, struct device_attribute *attr, char *buf);


#endif

