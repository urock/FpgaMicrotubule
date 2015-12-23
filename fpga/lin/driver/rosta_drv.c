/* *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */



#include "rosta-driver.h"

#define ROSTA_DRIVER_VERSION		102 	// 1.02


#define RSP5XX_DEBUG

#include "msg.h"

#define MAX_DMA_PAGES 1024
#define TIME_MAX 1000
#define REG_SHIFT 6
#define REG_SHIFT_R 10

#define TEST_ERROR(val) if (val < 0) return val;

MODULE_DESCRIPTION("Generic device driver module for Rosta rsp cards <www.rosta.ru>");
MODULE_AUTHOR("Alexey Shmatok <alexey.shmatok@gmail.com>, Yuri Rumyantsev <yarumyantsev@gmail.com>, "
		"Vladimir Platonov <shaera.epine@gmail.com>");
MODULE_LICENSE("GPL");

static char dev_present[MAX_DEVICES];
static struct semaphore rsp_sem;

static int rsp_major = 0;
static int rsp_minor = 0;
static int rsp_nr_devs = MAX_DEVICES;
static struct fasync_struct *async_queue=NULL;

module_param(rsp_major, int, S_IRUGO);
module_param(rsp_minor, int, S_IRUGO);
module_param(rsp_nr_devs, int, S_IRUGO);


static DRIVER_ATTR(rosta_drv_cdev , S_IRUGO, show_cdev, NULL);
static DEVICE_ATTR(rosta_dev_instance, S_IRUGO, show_instance, NULL);



static int rsp_open(struct inode *inode, struct file *filp);
static int rsp_fasync(int fd, struct file *filp, int mode);
static int rsp_release(struct inode *inode, struct file *filp);
static ssize_t rsp_read(struct file *filp, char *buf, size_t count,
		loff_t *f_pos);
static ssize_t rsp_write(struct file *filp, const char *buf, size_t count,
		loff_t *f_pos);
//static int     rsp_ioctl(struct inode *inode, struct file *filp,
//		   unsigned int cmd, unsigned long arg);
static long rsp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static const struct file_operations rsp_fops = { .owner = THIS_MODULE, .read =
		rsp_read, .write = rsp_write, .unlocked_ioctl = rsp_ioctl, .open =
		rsp_open, .release = rsp_release, .fasync = rsp_fasync,};



void fpga_mode_reg_check_error(unsigned int reg_val)
{

	if (reg_val & (1<<4))
		msg_dbg("Master Memory Write Init Error\n");

	if (reg_val & (1<<5))
		msg_dbg("Master Read Request Error\n");

	if (reg_val & (1<<6))
		msg_dbg("Interrupt Error\n");

	if (reg_val & (1<<7))
		msg_dbg("Timeout Error\n");


	if (reg_val & (1<<26))
		msg_dbg("TX_INIT_EP_MEM_WR_ERROR\n");
	if (reg_val & (1<<27))
		msg_dbg("TX_EP_MEM_WR_ERROR\n");
	if (reg_val & (1<<28))
		msg_dbg("init_rd_error\n");
	if (reg_val & (1<<29))
		msg_dbg("Master Memory Write Error\n");
	if (reg_val & (1<<30))
		msg_dbg("WaitAnswer\n");


}



// data - pointer to buffer - must be unsigned long
int rsp_setup_dma(struct rsp_dma *dma, int direction, unsigned long data,
		u32 size) {
	u32 first, last;
	int msg_err;

	// PAGE_SHIFT = 12
	// PAGE_SIZE = 4k
	// PAGE_MASK = ~(PAGE_SIZE - 1) = ~(0x0000 0FFF) = 0xFFFF F000

	dma->direction = direction;
	first = (data & PAGE_MASK) >> PAGE_SHIFT; // first page number
	last = ((data + size - 1) & PAGE_MASK) >> PAGE_SHIFT; // last page number
	dma->tail = 1 + ((data + size - 1) & ~PAGE_MASK);
	dma->offset = data & ~PAGE_MASK;
	dma->nr_pages = last - first + 1;
	msg_dbg("dma nr_pages %d",dma->nr_pages);
	dma->pages = kmalloc(dma->nr_pages * sizeof(struct page*), GFP_KERNEL);

	if (!dma->pages) {
		msg_err("NULL == dma->pages");
		return -ENOMEM;
	}
	msg_dbg("init user [0x%lx+0x%x => %d pages]",data,size,dma->nr_pages);

	down_read(&current->mm->mmap_sem);
	msg_err
			= get_user_pages(current, current->mm, data & PAGE_MASK,
					dma->nr_pages, direction == PCI_DMA_FROMDEVICE, 1,
					dma->pages, NULL);
	up_read(&current->mm->mmap_sem);

	if (msg_err == dma->nr_pages) {
		msg_dbg("get_user_pages ok, %d pages",msg_err);
		return 0;
	} else {
		msg_err("get_user_pages failed, msg_err=%d must be %d",msg_err,dma->nr_pages);
		return -EINVAL;
	}

}

int dma_uninit_user(struct rsp_dma *dma) {
	if (dma == NULL) {
		msg_err("dma_uninit_user dma==NULL");
		return 0;
	}
	kfree(dma->sglist);
	kfree(dma->pages);
	return 0;
}

struct scatterlist* rsp_setup_sglist(struct page **pages, int nr_pages,
		int offset, int tail) {
	struct scatterlist *sglist;
	int i;

	if (pages == NULL) {
		msg_err("rsp_setup_sglist error pages == NULL");
		return NULL;
	}

	sglist = kmalloc(sizeof(*sglist) * nr_pages, GFP_KERNEL);

	if (NULL == sglist) {
		msg_err("rsp_setup_sglist kmalloc error sglist==NULL");
		return NULL;
	}

	memset(sglist, 0, sizeof(*sglist) * nr_pages);

	for (i = 0; i < nr_pages; i++) {
		if (pages[i] == NULL) {
			msg_err("rsp_setup_sglist error page %d == NULL",i);
			kfree(sglist);
			return NULL;
		}
		if (i == 0) {
			sg_set_page(&sglist[i], pages[i], PAGE_SIZE - offset, offset);
		} else if (i == nr_pages - 1) {
			sg_set_page(&sglist[i], pages[i], tail, 0);
		} else
			sg_set_page(&sglist[i], pages[i], PAGE_SIZE, 0);
	}
	return sglist;
}

irq_handler_t rsp_intmsg_errupt(int irq, void *dev_id, struct pt_regs *regs) {

	u32 v32;

	struct rsp_device *rsp_device = (struct rsp_device *) dev_id;
	if (irq != rsp_device->intr) {
		msg_err("msg_error in intmsg_errupt:irq:%i!=intr:%i",irq,rsp_device->intr);
		return IRQ_NONE;
	}


	if (!rsp_device->rsp_device_access)
		return IRQ_NONE;

	v32 = ioread32( (void *) ((unsigned long) rsp_device->bar[CNTL_BAR].vaddr
							+ M_MODE));

	if (!((v32 & COMPL_WR) || (v32 & COMPL_RD) ||(v32 & COMPL_P2P)))
		return IRQ_NONE;


	if (v32 != 0xFFFFFFFF) {

		msg_dbg("Interrupt: IRQ_MMR = 0x%x\n",v32);

		if (v32 & COMPL_WR) {
			v32 &= ~COMPL_WR;
			msg_dbg("interrupt: dma_wr compl");
		}
		if (v32 & COMPL_RD) {
			v32 &= ~COMPL_RD;
			msg_dbg("interrupt: dma_rd compl");
		}
		if (v32 & DUAL_DMA_BIT) {
			v32 &= ~DUAL_DMA_BIT;
			msg_dbg("interrupt: dual cleared");
		}
		if (v32 & COMPL_P2P) {
			v32 &= ~COMPL_P2P;
			msg_dbg("interrupt: p2p transfer completed");
		}
		iowrite32(v32,
				(void *) ((unsigned long) rsp_device->bar[CNTL_BAR].vaddr + M_MODE));

	}

	rsp_device->IRQ_MMR = v32;
	rsp_device->flag=0;


	wake_up_interruptible(&rsp_device->intr_wait);

	return (irq_handler_t) IRQ_HANDLED;
}

static int rsp_request_irq(unsigned int irq, struct rsp_device *dev) {
	int res = 0;
	dev->use_int = 0;
	//dev->intr_lock = SPIN_LOCK_UNLOCKED;
	/*res = request_irq(irq, (irq_handler_t) rsp_intmsg_errupt,
			IRQF_DISABLED | IRQF_SHARED, "rsp_intr", (void*) dev);*/
	res = request_irq(irq, (irq_handler_t) rsp_intmsg_errupt,
				IRQF_SHARED, "rsp_intr", (void*) dev);
	if (res < 0) {
		dev->use_int = 0;
		msg_err("request irq %d FAILED",irq);
		return res;
	}
	msg_dbg("request irq %d success",irq);
	dev->use_int = 1;
	return res;
}

// file operations

static int rsp_open(struct inode *inode, struct file *filp) {
	struct rsp_device *rsp_dev;

	msg_dbg("Entering function:%s\n", __FUNCTION__);

	rsp_dev = container_of(inode->i_cdev, struct rsp_device, cdev);

	if (!rsp_dev) {
		msg_err("Error opening device, no such device! \n");
		return -ENODEV;
	}

	if (!rsp_dev->is_opened) {
		msg_dbg("Opening rsp device with instance %d\n",rsp_dev->instance);
		rsp_dev->is_opened = 1;
	} else {
		msg_err("Device %d already opened. Close first\n",rsp_dev->instance);
		return -EBUSY;
	}

	filp->private_data = rsp_dev;


	msg_dbg ("requesting IRQ...");
	if (rsp_request_irq(rsp_dev->pdev->irq, rsp_dev) >= 0) {
		rsp_dev->intr = rsp_dev->pdev->irq;
		msg_dbg("IRQ OK");
		return 0;
	} else {
		msg_err(" device %d irq trouble\n",rsp_dev->instance);
		return -EBUSY;
	}


}

static int rsp_release(struct inode *inode, struct file *filp) {
	struct rsp_device *rsp_dev;

	msg_dbg(" Entering function:%s\n", __FUNCTION__);
	rsp_dev = filp->private_data;
	rsp_fasync(-1, filp, 0);

	if (!rsp_dev) {
		msg_err(" Release rsp device not found");
		return -ENODEV;
	}

	if (!rsp_dev->is_opened) {
		msg_err(" Device %d is not opened\n",rsp_dev->instance);
		return -ENODEV;
	} else {
		msg_dbg(" Released rsp device %d\n",rsp_dev->instance);
		rsp_dev->is_opened = 0;
	}

	if (rsp_dev->use_int) {
		free_irq(rsp_dev->pdev->irq, (void*) rsp_dev);
		msg_dbg(" Released freed irq\n");
		rsp_dev->use_int = 0;
	}

	return 0;
}

static ssize_t dma_setup(struct file *filep, RD_DualDmaTransfer_p dma_data,
		char wr) {
	struct rsp_device *device = filep->private_data;
	ssize_t retval = 0;
	size_t count;
	unsigned long buf, dev_dma_addr, dev_dma_cnt;
	u32 v32, l32, CntVal = 0, r = 0, l32_1 = 0;
	u32 cur_buf_addr, cur_buf_length, tail;
	u32 i, n;

	struct rsp_dma *rma = 0;

	msg_dbg(" Entering function:%s\n", __FUNCTION__);

	if (!device) {
		msg_err(" device==NULL");
		return -1;
	}

	if (!wr) {
		device->direction = WRITE;
		device->dma_w.direction = PCI_DMA_FROMDEVICE;
		device->intr_mask = 0x02 << REG_SHIFT;
		buf = dma_data->dest;
		count = dma_data->size_read;
		rma = &device->dma_w;
		dev_dma_addr = M_WR_ADDR;
		dev_dma_cnt = M_WR_CNT;
	} else {
		device->direction = READ;
		device->dma_r.direction = PCI_DMA_TODEVICE;
		device->intr_mask = 0x02 << REG_SHIFT_R;
		buf = dma_data->src;
		count = dma_data->size_write;
		rma = &device->dma_r;
		dev_dma_addr = M_RD_ADDR;
		dev_dma_cnt = M_RD_CNT;
	}

	if (rsp_setup_dma(rma, rma->direction, (unsigned long) buf,
			count) == 0) {

		msg_dbg(" mapping ok");
		msg_dbg(" offset:%X",rma->offset);

		rma->sglist = rsp_setup_sglist(rma->pages, rma->nr_pages, rma->offset,
				rma->tail);

		if (rma->sglist == NULL) {
			msg_err(" msg_error generating ScatterGather List");
			retval = -EIO;
			goto __xexit;
		}
		rma->sglen = pci_map_sg(device->pdev, rma->sglist, rma->nr_pages,
				rma->direction);

		if (rma->sglen == 0) {
			kfree(rma->sglist);
			rma->sglist = NULL;
			msg_err(" pci_map_sg failed\n");
			retval = -EIO;
			goto __xexit;
		}
		msg_dbg(" mapping OK: %i buffers",rma->sglen);

		pci_dma_sync_sg_for_device(device->pdev, rma->sglist, rma->sglen,
				rma->direction);

		n = rma->nr_pages;
		if (n > MAX_DMA_PAGES)
			n = MAX_DMA_PAGES;

		for (i = 0; i < n; i++) {
			msg_dbg(" PCI_DMA page %d go",i);
			cur_buf_addr = sg_dma_address(&rma->sglist[i]);
			cur_buf_length = sg_dma_len(&rma->sglist[i]);
			msg_dbg(" DMA allocated buffer v32=%x, l32=%x", cur_buf_addr, cur_buf_length);

			//l32 - текущее значение для записи в регистр
			//l32_1 - значение для предыдущей страницы

			while (cur_buf_length > 0) {
				tail = PAGE_SIZE - (cur_buf_addr & (PAGE_SIZE - 1));
				l32 = (cur_buf_length < tail) ? cur_buf_length : tail;
				v32 = cur_buf_addr;
				msg_dbg(" DMA transaction buffer v32=%x, l32=%x, tail=%x, cbl=%x",
						v32, l32, tail, cur_buf_length);

				if (!wr) {
					if (l32 != l32_1) {
						l32_1 = l32;
						r = l32 >> device->PLenShift;
						CntVal = (device->PacketLengthDW << 24) + r;
					}
				} else
					CntVal = l32;
				iowrite32(
						v32,
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ dev_dma_addr));
				iowrite32(
						CntVal,
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ dev_dma_cnt));
				cur_buf_addr += l32;
				cur_buf_length -= l32;
			}
		}
	}
	else
	{
		msg_err("DMA setup failed");
		return -1;
	}

	return 0;

	__xexit: kfree(rma->sglist);
	kfree(rma->pages);

	return -1;
}
static void dma_start(struct file *filep, char wr) {
	struct rsp_device *device = filep->private_data;
	u32 v32 = 0;

	msg_dbg(" prepare wait queue");
	DECLARE_WAITQUEUE(wait,current);
	add_wait_queue(&device->intr_wait, &wait);

	device->flag = 1;
	msg_dbg(" go task intmsg_erruptable");
	current->state = TASK_INTERRUPTIBLE;

	msg_dbg(" start dma transfer");

	device->rsp_device_access = 1;

	v32 = ioread32(
			(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));
	switch (wr) {
		case 0:
			v32 |= STARD_WR;
			break;
		case 1:
			v32 |= STARD_RD;
			break;
		case 2:
			v32 |= (STARD_WR | STARD_RD | DUAL_DMA_BIT);
		default:
			break;
	}
	iowrite32(v32,
			(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));

	// START WAITING
	schedule_timeout(msecs_to_jiffies(TIME_MAX));

	// DONE WAITING
	current->state = TASK_RUNNING;
	remove_wait_queue(&device->intr_wait, &wait);

	device->rsp_device_access = 0;

	msg_dbg(" waiting finished");

	if (device->flag > 0) {
		msg_err(" DMA timed out.., abort!!! flag = %d", device->flag);
		//	   	break;
	}



	msg_dbg(" PCI_DMA loop pages finished");

}

static ssize_t dma_finish(struct file *filep, char wr) {

	int i = 0;
	struct rsp_device *device = filep->private_data;
	struct rsp_dma *rma = 0;
	ssize_t retval = 0;


	if (!wr)
		rma = &device->dma_w;
	else
		rma = &device->dma_r;

	pci_dma_sync_sg_for_cpu(device->pdev, rma->sglist,
			rma->sglen, rma->direction);

	for (i = 0; i < rma->nr_pages; i++) {
		if (!PageReserved(rma->pages[i]))
			SetPageDirty(rma->pages[i]);
		page_cache_release(rma->pages[i]);
	}

	pci_unmap_sg(device->pdev, rma->sglist,
			rma->nr_pages, rma->direction);

	if (device->flag > 0)
		retval = -EIO;
	else
		retval = 0;

	kfree(rma->sglist);
	kfree(rma->pages);


	return retval;
}

static ssize_t rsp_dual_dma(struct file *filep, RD_DualDmaTransfer_p dma, char wr) {

	ssize_t retval = 0;

	if (wr == 1 || wr == 0) {
		retval = dma_setup(filep, dma, wr);
		TEST_ERROR(retval);
		dma_start(filep, wr);
		retval = dma_finish(filep, wr);
	}
	else
	{
		retval = dma_setup(filep, dma, 1);
		TEST_ERROR(retval);
		retval = dma_setup(filep, dma, 0);
		TEST_ERROR(retval);
		dma_start(filep, 2);
		retval = dma_finish(filep, 1);
		TEST_ERROR(retval);
		retval = dma_finish(filep, 0);
	}

	return retval;
}




static ssize_t rsp_p2p_dma(struct file *filep, rsp_p2p_dma_p dma) {

	ssize_t retval = 0;

	u32 CntVal = 0, r = 0, v32;

	struct rsp_device *device = filep->private_data;

	// program device for write operation

	// send source address to device
	// not implemented

	// send destination address to device
	iowrite32(dma->dest, (void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_WR_ADDR));

	// send length to device
	r = dma->size >> device->PLenShift;
	CntVal = (device->PacketLengthDW << 24) + r;

	msg_dbg("rsp_p2p_dma: size = 0x%x, CntVal=0x%x\n",dma->size, CntVal);

	iowrite32(CntVal, (void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_WR_CNT));



	// wait for completion


	DECLARE_WAITQUEUE(wait,current);
	add_wait_queue(&device->intr_wait, &wait);

	device->flag = 1;
	current->state = TASK_INTERRUPTIBLE;

	msg_dbg("Start P2P DMA transfer");

	device->rsp_device_access = 1;

	// send cmd to start
	v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));
	v32 |= STARD_P2P;
	iowrite32(v32, (void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));

	// START WAITING
//	schedule_timeout(msecs_to_jiffies(TIME_MAX));
	schedule();

	// DONE WAITING
	current->state = TASK_RUNNING;
	remove_wait_queue(&device->intr_wait, &wait);

	device->rsp_device_access = 0;

	msg_dbg(" waiting finished");

	if (device->flag == 1) {
		msg_err(" read timed out.., abort!!!");
		return -1;
	}



	return retval;
}



static long rsp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	u32 retval = 0;
	struct rsp_device *rsp_dev;

	//msg_dbg(" Entering function:%s\n", __FUNCTION__);
	rsp_dev = filp->private_data;

	if (!rsp_dev) {
		msg_err(" No such device!");
		return -ENODEV;
	}

	switch (cmd) // +++++
	{

	//********************************************************************************************************
	case RSP_DEV_INFO: {
		rsp5xx_device_info_t device_info;
		pci_read_config_word(rsp_dev->pdev, PCI_DEVICE_ID,
				&(device_info.device_id));
		pci_read_config_word(rsp_dev->pdev, PCI_VENDOR_ID,
				&(device_info.vendor_id));
		if (copy_to_user((void *) arg, &device_info,
				sizeof(rsp5xx_device_info_t))) {
			msg_err(" rsp_DEV_INFO copy_to_user failed\n");
			return -EFAULT;
		}
		retval = 1;
		break;
	}

		//*********************************************************************************************************
	case RSP_BAR_INFO: {
		rsp5xx_bar_info_t bar_info;

		if (copy_from_user(&bar_info, (void *) arg, sizeof(rsp5xx_bar_info_t))) {
			msg_err(" rsp_BAR_msg_dbg copy_from_user failed\n");
			return -EFAULT;
		}
		if (bar_info.id >= 0 && bar_info.id < rsp_dev->bar_count) {
			bar_info.addr = rsp_dev->bar[bar_info.id].addr;
			bar_info.size = rsp_dev->bar[bar_info.id].size;
			if (copy_to_user((void *) arg, &bar_info, sizeof(rsp5xx_bar_info_t))) {
				msg_err(" rsp_BAR_INFO copy_to_user failed\n");
				return -EFAULT;
			}

		} else {
			msg_err("rsp_BAR_INFO wrong bar id %d, bar_count=%d\n",bar_info.id,rsp_dev->bar_count);
			return -EFAULT;
		}
		retval = 1;
		break;
	}
		//*****************************************************************************************************
	case RSP_REG32_READ: {
		rsp5xx_reg32_io_t reg32_io;
		unsigned int bar, offset;

		if (copy_from_user(&reg32_io, (void *) arg, sizeof(rsp5xx_reg32_io_t))) {
			msg_err("rsp_REG32_READ copy_from_user failed\n");
			return -EFAULT;
		}
		if (!(reg32_io.bar >= 0 && reg32_io.bar < rsp_dev->bar_count)) {
			msg_err("rsp_REG32_READ wrong bar id=%d, bar_count=%d\n",reg32_io.bar,rsp_dev->bar_count);
			return -EFAULT;
		}
		if (!(reg32_io.offset >= 0 && reg32_io.offset
				< rsp_dev->bar[reg32_io.bar].size)) {
			msg_err("rsp_REG32_READ wrong offset=%x in bar id=%d, bar_count=%d\n",reg32_io.offset,reg32_io.bar,rsp_dev->bar_count);
			return -EFAULT;

		}

		bar = reg32_io.bar;
		offset = reg32_io.offset;
		reg32_io.data = ioread32(
				(void *) ((unsigned long) rsp_dev->bar[bar].vaddr + offset));

		if (copy_to_user((void *) arg, &reg32_io, sizeof(rsp5xx_reg32_io_t))) {
			msg_err(" rsp_REG32_READ copy_to_user failed\n");
			return -EFAULT;
		}

		retval = 1;
		break;
	}
		//******************************************************************************************
	case RSP_REG32_WRITE: {
		rsp5xx_reg32_io_t reg32_io;
		unsigned int bar, offset;

		if (copy_from_user(&reg32_io, (void *) arg, sizeof(rsp5xx_reg32_io_t))) {
			msg_err(" rsp_REG32_WRITE copy_from_user failed\n");
			return -EFAULT;
		}
		if (!(reg32_io.bar >= 0 && reg32_io.bar < rsp_dev->bar_count)) {
			msg_err(" rsp_REG32_WRITE wrong bar id=%d, bar_count=%d\n",reg32_io.bar,rsp_dev->bar_count);
			return -EFAULT;
		}
		if (!(reg32_io.offset >= 0 && reg32_io.offset
				< rsp_dev->bar[reg32_io.bar].size)) {
			msg_err(" rsp_REG32_WRITE wrong offset=%x in bar id=%d, bar_count=%d\n",reg32_io.offset,reg32_io.bar,rsp_dev->bar_count);
			return -EFAULT;

		}

		bar = reg32_io.bar;
		offset = reg32_io.offset;
		iowrite32(reg32_io.data,
				(void *) ((unsigned long) rsp_dev->bar[bar].vaddr + offset));

		retval = 1;
		break;
	}
		//************************************************************************************************

	case RSP_SET_PACKET_LEN: {
		two_dwords_t tmp;
		if (copy_from_user(&tmp, (void *) arg, sizeof(two_dwords_t))) {
			msg_err("RSP_SET_PACKET_LEN copy_from_user failed\n");
			return -EFAULT;
		}
		rsp_dev->PacketLengthDW = tmp.dword1;
		rsp_dev->PLenShift = tmp.dword2;
		msg_dbg("Set packet length = %d, shift = %d\n",rsp_dev->PacketLengthDW,rsp_dev->PLenShift);

		break;
	}

	case RSP_DUAL_DMA: {
		RD_DualDmaTransfer_t dma;
		retval = 1;
		if (copy_from_user(&dma, (void *) arg, sizeof(RD_DualDmaTransfer_t))) {
			msg_err("RSP_DUAL_DMA copy_from_user failed");
			return -EFAULT;
		}
		msg_dbg("RSP_DUAL_DMA launching");
		retval = rsp_dual_dma(filp , &dma, 2);

		if (retval < 0)
			return -EFAULT;

		retval = 1;
		break;
	}

	case RSP_P2P_DMA: {
		rsp_p2p_dma_t dma_p2p;
		retval = 1;
		if (copy_from_user(&dma_p2p, (void *) arg, sizeof(rsp_p2p_dma_t))) {
			msg_err("RSP_P2PL_DMA copy_from_user failed");
			return -EFAULT;
		}
		msg_dbg("RSP_P2P_DMA launching");
		retval = rsp_p2p_dma(filp , &dma_p2p);

		if (retval < 0)
			return -EFAULT;

		retval = 1;
		break;
	}
	default:
		msg_dbg(" rmb_base_ioctl ENOIOCTLCMD\n");
		return -ENOIOCTLCMD;

	} // switch

	return retval;
}

static int rsp_fasync(int fd, struct file *filp, int mode)
{
 return fasync_helper(fd, filp, mode, &async_queue);
}

static ssize_t rsp_read(struct file *filep, char *buf, size_t count,
		loff_t *offset) {
	struct rsp_device *device = filep->private_data;
	ssize_t retval = 0;
	u32 v32, l32, CntVal = 0, r = 0, l32_1 = 0, bytes_to_go = count;
	u32 i, n;
	u32 cur_buf_addr, cur_buf_length, tail;

	msg_dbg(" Entering function:%s\n", __FUNCTION__);

	if (!device) {
		msg_err(" rsp_read device==NULL");
		return 0;
	}
	if (buf == NULL) {
		msg_err(" rsp_read buf address == NULL");
		return 0;
	}

	if (count == 0) {
		msg_err(" rsp_read count==0");
		return 0;
	}

	device->direction = WRITE;
	device->dma_w.direction = PCI_DMA_FROMDEVICE;
	device->intr_mask = 0x02 << REG_SHIFT;

	if (rsp_setup_dma(&(device->dma_w), PCI_DMA_FROMDEVICE,
			(unsigned long) buf, count) == 0) {

		//	msg_dbg("mapped %d pages",dma->nr_pages);
		//	msg_dbg("offset:%X",device->dma_w.offset);
		msg_dbg(" mapping ok");
		msg_dbg(" offset:%X",device->dma_w.offset);

		device->dma_w.sglist = rsp_setup_sglist(device->dma_w.pages,
				device->dma_w.nr_pages, device->dma_w.offset,
				device->dma_w.tail);

		if (device->dma_w.sglist == NULL) {
			msg_err(" msg_error generating ScatterGather List, aborting reading");
			retval = -EIO;
			goto __xexit;
		}
		device->dma_w.sglen = pci_map_sg(device->pdev, device->dma_w.sglist,
				device->dma_w.nr_pages, device->dma_w.direction);

		if (device->dma_w.sglen == 0) {
			kfree(device->dma_w.sglist);
			device->dma_w.sglist = NULL;
			msg_err(" pci_map_sg failed\n");
			retval = -EIO;
			goto __xexit;
		}
		msg_dbg(" mapping OK: %i buffers",device->dma_w.sglen);

		pci_dma_sync_sg_for_device(device->pdev, device->dma_w.sglist,
				device->dma_w.sglen, device->dma_w.direction);
				
		//kill_fasync(&async_queue, SIGIO, POLL_IN);		

		msg_dbg(" setup dma controller for write");

		n = device->dma_w.nr_pages;
		if (n > MAX_DMA_PAGES)
			n = MAX_DMA_PAGES;

		for (i = 0; i < n; i++) {
			//msg_dbg(" PCI_DMA page %d go",i);
			//			v32=sg_dma_address(&device->dma_w.sglist[i]);
			//			l32=sg_dma_len(&device->dma_w.sglist[i]);
			cur_buf_addr = sg_dma_address(&device->dma_w.sglist[i]);

			if (i == (n-1))	 // programming last page
				cur_buf_length = bytes_to_go;
			else
				cur_buf_length = sg_dma_len(&device->dma_w.sglist[i]);


			//msg_dbg(" DMA read allocated buffer v32=%x, l32=%x", cur_buf_addr, cur_buf_length);

			//l32 - текущее значение для записи в регистр
			//l32_1 - значение для предыдущей страницы

			while (cur_buf_length > 0) {
				tail = PAGE_SIZE - (cur_buf_addr & (PAGE_SIZE - 1));
				l32 = (cur_buf_length < tail) ? cur_buf_length : tail;
				v32 = cur_buf_addr;
				//msg_dbg(" DMA read transaction buffer v32=%x, l32=%x, tail=%x, cbl=%x",
//						v32, l32, tail, cur_buf_length);

				if (l32 != l32_1) {
					l32_1 = l32;
					r = l32 >> device->PLenShift;
					CntVal = (device->PacketLengthDW << 24) + r;
				}
				iowrite32(
						v32,
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ M_WR_ADDR));
				iowrite32(
						CntVal,
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ M_WR_CNT));
				bytes_to_go -= l32;
				cur_buf_addr += l32;
				cur_buf_length -= l32;
			}

		}
//		kill_fasync(&async_queue, SIGIO, POLL_IN);
		msg_dbg(" prepare wait queue");
		DECLARE_WAITQUEUE(wait,current);
		add_wait_queue(&device->intr_wait, &wait);

		device->flag = 1;
		msg_dbg(" go task intmsg_erruptable");
		current->state = TASK_INTERRUPTIBLE;

		msg_dbg(" start dma write transfer");

		device->rsp_device_access = 1;

		v32
				= ioread32(
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ M_MODE));
		v32 |= STARD_WR;
		iowrite32(v32,
				(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));

		kill_fasync(&async_queue, SIGIO, POLL_IN);		
		// START WAITING
		schedule_timeout(msecs_to_jiffies(TIME_MAX));

		// DONE WAITING
		current->state = TASK_RUNNING;
		remove_wait_queue(&device->intr_wait, &wait);

		device->rsp_device_access = 0;

		msg_dbg(" waiting finished");

		if (device->flag == 1) {
			msg_err(" read timed out.., abort!!!");

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));

			fpga_mode_reg_check_error(v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + MasterEpRxTotalCnt));
			msg_dbg(" dev 2 MasterEpRxTotalCnt 0x%x\n",v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + FifoEpRxTotalCnt));
			msg_dbg(" dev 2 FifoEpRxTotalCnt 0x%x\n",v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + FifoMemWrTotalCnt));
			msg_dbg(" dev 2 FifoMemWrTotalCnt 0x%x\n",v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + MasterMemWrTotalCnt));
			msg_dbg(" dev 2 MasterMemWrTotalCnt 0x%x\n\n",v32);


			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + 0x70));
			msg_dbg(" dev 2 TxdDataCount 0x%x\n\n",v32);



		}

		msg_dbg(" PCI_DMA_FROMDEVICE loop pages %d finished",n);

		pci_dma_sync_sg_for_cpu(device->pdev, device->dma_w.sglist,
				device->dma_w.sglen, device->dma_w.direction);

		for (i = 0; i < device->dma_w.nr_pages; i++) {
			if (!PageReserved(device->dma_w.pages[i]))
				SetPageDirty(device->dma_w.pages[i]);
			page_cache_release(device->dma_w.pages[i]);
		}

		pci_unmap_sg(device->pdev, device->dma_w.sglist,
				device->dma_w.nr_pages, device->dma_w.direction);

		if ((device->flag == 1) || device->IRQ_MMR == 0xFFFFFFFF)
			retval = -EIO;
		else
			retval = count;

		__xexit: kfree(device->dma_w.sglist);
		kfree(device->dma_w.pages);

	} else {
		msg_err(" mapping msg_error");
	}

	return retval;
}


//count is in bytes
static ssize_t rsp_write(struct file *file, const char __user *buf,
		size_t count, loff_t *offset) {
	struct rsp_device *device = file->private_data;
	ssize_t retval = 0;
	u32 v32, l32, bytes_to_go = count;
	u32 cur_buf_addr, cur_buf_length, tail;
	int i, n;

	msg_dbg(" Entering function:%s\n", __FUNCTION__);

	if (device == NULL) {
		msg_err(" rsp_write device==NULL");
		return 0;
	}

	if (buf == NULL) {
		msg_err(" rsp_write buf address == NULL");
		return 0;
	}

	if (count == 0) {
		msg_err(" rsp_write count==0");
		return 0;
	}
	msg_dbg(" User buffer address = 0x%x, length = 0x%x\n", buf,count);

	device->direction = READ;
	device->dma_r.direction = PCI_DMA_TODEVICE;
	device->intr_mask = 0x02 << REG_SHIFT_R;

	if (rsp_setup_dma(&(device->dma_r), PCI_DMA_TODEVICE, (unsigned long) buf,
			count) == 0) {

		msg_dbg(" mapping ok");
		msg_dbg(" offset:%X",device->dma_r.offset);

		unsigned long l_addr, p_addr;

		l_addr = (unsigned long) page_address( device->dma_r.pages[0]);
		if (l_addr != NULL) {
//			msg_dbg(" page logical addr = 0x%x\n",l_addr);
			p_addr = __pa(l_addr);
//			if (p_addr != NULL)
//				msg_dbg(" page phy addr = 0x%x\n",p_addr);
//			else
//				msg_dbg(" page phy addr = NULL\n");
		} else
			msg_dbg(" page logical addr = NULL\n");

		device->dma_r.sglist = rsp_setup_sglist(device->dma_r.pages,
				device->dma_r.nr_pages, device->dma_r.offset,
				device->dma_r.tail);
		if (device->dma_r.sglist == NULL) {
			msg_err(" msg_error generating ScatterGather List, aborting writing");
			retval = -EIO;
			goto __xexit;
		}

		device->dma_r.sglen = pci_map_sg(device->pdev, device->dma_r.sglist,
				device->dma_r.nr_pages, device->dma_r.direction);

		if (device->dma_r.sglen == 0) {
			msg_err(" pci_map_sg failed\n");
			kfree(device->dma_r.sglist);
			device->dma_r.sglist = NULL;
			retval = -EIO;
			goto __xexit;
		}
		msg_dbg(" mapping OK: %i buffers",device->dma_r.sglen);

		pci_dma_sync_sg_for_device(device->pdev, device->dma_r.sglist,
				device->dma_r.sglen, device->dma_r.direction);

		msg_dbg(" setup dma controller for read");

		n = device->dma_r.nr_pages;
		if (n > MAX_DMA_PAGES)
			n = MAX_DMA_PAGES;



		for (i = 0; i < n; i++) {
			//msg_dbg(" PCI_DMA_FROMDEVICE page %d go",i);
			cur_buf_addr = sg_dma_address(&device->dma_r.sglist[i]);

			if (i == (n-1))	 // programming last page
				cur_buf_length = bytes_to_go;
			else
				cur_buf_length = sg_dma_len(&device->dma_r.sglist[i]);

			//msg_dbg(" DMA write allocated buffer v32=%x, l32=%x", cur_buf_addr, cur_buf_length);

			while (cur_buf_length > 0) {
				tail = PAGE_SIZE - (cur_buf_addr & (PAGE_SIZE - 1));
				l32 = (cur_buf_length < tail) ? cur_buf_length : tail;
				v32 = cur_buf_addr;
				//msg_dbg(" DMA write transaction buffer v32=%x, l32=%x, tail=%x, cbl=%x",
//						v32, l32, tail, cur_buf_length);

				iowrite32(
						v32,
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ M_RD_ADDR));
				iowrite32(
						l32,
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ M_RD_CNT));
				bytes_to_go -= l32;
				cur_buf_addr += l32;
				cur_buf_length -= l32;
			}
		}

		msg_dbg(" prepare wait queue");
		DECLARE_WAITQUEUE(wait,current);
		add_wait_queue(&device->intr_wait, &wait);

		device->flag = 1;

		device->rsp_device_access = 1;

		msg_dbg(" go task intmsg_erruptable");
		current->state = TASK_INTERRUPTIBLE;

		msg_dbg(" start dma read transfer");

		v32
				= ioread32(
						(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr
								+ M_MODE));
		v32 |= STARD_RD;
		iowrite32(v32,
				(void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));

		schedule_timeout(msecs_to_jiffies(TIME_MAX));

		current->state = TASK_RUNNING;
		remove_wait_queue(&device->intr_wait, &wait);

		device->rsp_device_access = 0;
		msg_dbg(" waiting finished");

		if (device->flag == 1) {
			msg_err(" write timed out.., abort!!!");

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + M_MODE));

			fpga_mode_reg_check_error(v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + MasterMemRdTotalCnt));
			msg_dbg(" dev 1 MasterMemRdTotalCnt 0x%x\n",v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + FifoMemRdTotalCnt));
			msg_dbg(" dev 1 FifoMemRdTotalCnt 0x%x\n",v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + FifoEpTxTotalCnt));
			msg_dbg(" dev 1 FifoEpTxTotalCnt 0x%x\n",v32);

			v32 = ioread32((void *) ((unsigned long) device->bar[CNTL_BAR].vaddr + MasterEpTxTotalCnt));
			msg_dbg(" dev 1 MasterEpTxTotalCnt 0x%x\n\n",v32);

		}

		msg_dbg(" PCI_DMA_TODEVICE loop pages %d finished",n);

		__xclenup_dma:

		msg_dbg(" pci_dma_sync_sg_for_cpu start");
		pci_dma_sync_sg_for_cpu(device->pdev, device->dma_r.sglist,
				device->dma_r.sglen, device->dma_r.direction);

		msg_dbg(" pci_dma_sync_sg_for_cpu ok");
		for (i = 0; i < device->dma_r.nr_pages; i++) {
			if (!PageReserved(device->dma_r.pages[i]))
				SetPageDirty(device->dma_r.pages[i]);
			page_cache_release(device->dma_r.pages[i]);
		}

		msg_dbg(" pci_unmap_sg start");
		pci_unmap_sg(device->pdev, device->dma_r.sglist,
				device->dma_r.nr_pages, device->dma_r.direction);
		msg_dbg(" pci_unmap_sg ok");

		if ((device->flag == 1) || device->IRQ_MMR == 0xFFFFFFFF)
			retval = -EIO;
		else
			retval = count;

		__xexit: kfree(device->dma_r.sglist);
		kfree(device->dma_r.pages);

	} else {
		msg_err(" mapping msg_error");
	}
	return retval;
}

static int rsp_board_map_bar(struct pci_dev* dev, struct rsp_device* dev_ptr,
		int num) {
	unsigned long resource_flag;
	msg_dbg(" mapping BAR #%d ... \n",num);

	// get BAR address and size
	dev_ptr->bar[num].addr = pci_resource_start(dev, num);
	dev_ptr->bar[num].size = pci_resource_len(dev, num);

	resource_flag = pci_resource_flags(dev,num);

	if (resource_flag & IORESOURCE_IO) { // Resource IO PORT

/*
		msg_dbg(" found PORT BAR%d region at phys. addr. %lx with size %lx.\n",num,
				dev_ptr->bar[num].addr, dev_ptr->bar[num].size);
		dev_ptr->bar[num].io_port = 1;

		if (!request_region(dev_ptr->bar[num].addr, dev_ptr->bar[num].size,
				dev_ptr->name)) {
			msg_err(" request_region of BAR #%d failed.\n",num);
			goto msg_err_bar_region;
		}

		dev_ptr->bar[num].vaddr = iopoRD_map(dev_ptr->bar[num].addr,
				dev_ptr->bar[num].size);

		if (!dev_ptr->bar[num].vaddr) {
			msg_err(" iopoRD_map for BAR%d failed. \n", num);
			goto msg_err_bar_map;
		}

		msg_dbg(" remapped PORT BAR%d: %lx to vaddr %p\n",num,
				dev_ptr->bar[num].addr,dev_ptr->bar[num].vaddr);
*/

	// we don't support IO ports

		msg_err(" we don't support IO ports  \n");
		goto msg_err_bar_map;

	} else { // Resource MEMORY
		msg_dbg(" found MEM BAR%d region at phys. addr. %lx with size %lx.\n",num,
				dev_ptr->bar[num].addr, dev_ptr->bar[num].size);
		dev_ptr->bar[num].io_port = 0;

		if (!request_mem_region(dev_ptr->bar[num].addr, dev_ptr->bar[num].size,
				dev_ptr->name)) {
			msg_err(" request_mem_region of BAR #%d failed.\n",num);
			goto msg_err_bar_region;
		}

		dev_ptr->bar[num].vaddr = ioremap_nocache(dev_ptr->bar[num].addr,
				dev_ptr->bar[num].size);

		if (!dev_ptr->bar[num].vaddr) {
			msg_err(": ioremap_nocache for BAR%d failed.\n",num);
			goto msg_err_bar_map;
		}

		msg_dbg(" remapped MEM BAR%d: %lx to vaddr %p\n",num,
				dev_ptr->bar[num].addr,dev_ptr->bar[num].vaddr);
	}

	return 0;

	msg_err_bar_map: if (dev_ptr->bar[num].io_port)
		release_region(dev_ptr->bar[num].addr, dev_ptr->bar[num].size);
	else
		release_mem_region(dev_ptr->bar[num].addr, dev_ptr->bar[num].size);
	msg_dbg(" BAR%d released \n",num);
	msg_err_bar_region: msg_dbg(" BAR%d mapping failed!\n\n",num);
	return -EBUSY;
}

static int rsp_board_unmap_bar(struct pci_dev* dev, struct rsp_device* dev_ptr,
		int num) {
	if (dev_ptr->bar[num].vaddr != 0) {
	//	if (dev_ptr->bar[num].io_port)
	//		iopoRD_unmap((void*) dev_ptr->bar[num].vaddr);
	//	else
		iounmap(dev_ptr->bar[num].vaddr);
		msg_dbg(" device %d BAR%d unmapped\n",dev_ptr->dev_num,num);
	}

	if (dev_ptr->bar[num].addr != 0) {
		if (dev_ptr->bar[num].io_port)
			release_region(dev_ptr->bar[num].addr, dev_ptr->bar[num].size);
		else
			release_mem_region(dev_ptr->bar[num].addr, dev_ptr->bar[num].size);
		msg_dbg(" device %d BAR%d released\n",dev_ptr->dev_num,num);
	}

	return 0;
}

static int rsp_board_map_bars(struct pci_dev* dev, struct rsp_device* rsp_dev) {
	int i;
	int pci_msg_err;

	msg_dbg("Entering function:%s\n", __FUNCTION__);
	rsp_dev->bar_count = 0;
	for (i = 0; i < MAX_rsp_BAR; i++)
		if (pci_resource_len(dev, i) > 0) {
			pci_msg_err = rsp_board_map_bar(dev, rsp_dev, i);
			if (pci_msg_err) {
				msg_dbg(" failed rsp_board_map_bar %d.\n",i);
				return -1;
			}
			rsp_dev->bar_count++;
		}
	return 0;
}

static int rsp_board_unmap_bars(struct pci_dev* dev, struct rsp_device* rsp_dev) {
	int i;

	msg_dbg("Entering function:%s\n", __FUNCTION__);
	for (i = 0; i < MAX_rsp_BAR; i++)
		if (rsp_dev->bar[i].size > 0)
			rsp_board_unmap_bar(dev, rsp_dev, i);
	return 0;
}

ssize_t show_instance(struct device *dev, struct device_attribute *attr, char *buf) {
	struct rsp_device *rsp_dev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", rsp_dev->instance);
}



static int rsp_board_probe(struct pci_dev* dev,
		const struct pci_device_id* entry) {

	int pci_msg_err, err, i;
	dev_t devno;
	struct rsp_device* rsp_dev;

	msg_dbg("Entering function:%s\n", __FUNCTION__);
	msg_dbg(" board_probe vendor=%04x,device=%04x,defn=%x,ss_vendor=%04x,dev->ss_device=%04x\n",
			dev->vendor,dev->device,dev->devfn,dev->subsystem_vendor,dev->subsystem_device);

	pci_msg_err = pci_enable_device(dev);
	if (pci_msg_err) {
		msg_err(" could not  pci_enable_device\n");
		return pci_msg_err;
	}

	pci_set_master(dev);

	rsp_dev = kmalloc(sizeof(struct rsp_device), GFP_KERNEL);
	memset(rsp_dev, 0, sizeof(struct rsp_device));

	if (rsp_dev == NULL) {
		msg_err(" rsp_dev kmalloc failed.\n");
		return -ENOMEM;
	}

	msg_dbg("before sem\n");
	// calculate new instance number
	down(&rsp_sem);
	i = 0;
	while ((dev_present[i] == 1) && (i < MAX_DEVICES))
		i++;
	if (i == MAX_DEVICES) {
		msg_err("Device limit reached\n");
		kfree(rsp_dev);
	}
	dev_present[i] = 1;
	rsp_dev->instance = i;
	up(&rsp_sem);

	msg_dbg(" instance %d\n", rsp_dev->instance);
	sprintf(rsp_dev->name, "rsp5xx%d", rsp_dev->instance);

	rsp_dev->PLenShift = 7;
	rsp_dev->PacketLengthDW = 32;
	rsp_dev->pdev = dev;

	rsp_dev->rsp_device_access = 0;
	pci_set_drvdata(dev, rsp_dev);

	cdev_init(&rsp_dev->cdev, &rsp_fops);
	rsp_dev->cdev.owner = THIS_MODULE;
	rsp_dev->cdev.ops = &rsp_fops;

	devno = MKDEV(rsp_major, rsp_minor + rsp_dev->instance);
	err = cdev_add(&rsp_dev->cdev, devno, 1);

	if (err < 0) {
		msg_err(" can't create cdev (%d)\n", err);
		kfree(rsp_dev);
		return -1;
	}

	/* Initialize  */

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,0,0)
	if (!pci_set_consistent_dma_mask(dev, DMA_64BIT_MASK)) {
		msg_dbg(" using 64bit DMA addressing\n");
	} else if (!pci_set_consistent_dma_mask(dev, DMA_32BIT_MASK)) {
		msg_dbg(" using 32bit DMA addressing\n");
	} else {
		msg_err(" No suitable DMA available.\n");
	}
#else
	if (!pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(64))) {
		msg_dbg(" using 64bit DMA addressing\n");
	} else if (!pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(32))) {
		msg_dbg(" using 32bit DMA addressing\n");
	} else {
		msg_err(" No suitable DMA available.\n");
	}
#endif
	pci_msg_err = rsp_board_map_bars(dev, rsp_dev);
	if (pci_msg_err) {
		msg_err(" failed rsp_board_map_bars.\n");
		goto map_bars_msg_err;
	} else {
		msg_dbg(" rsp_board_map_bars %d sux.\n",rsp_dev->bar_count);

	}

	init_waitqueue_head(&rsp_dev->intr_wait);

	// Add the instance number to the device
	pci_msg_err = device_create_file(&dev->dev, &dev_attr_rosta_dev_instance);
	if (pci_msg_err) {
		msg_err("Failed to create device file \n");
		goto map_bars_msg_err;
	} else {
		msg_dbg("Device file created sux\n");

	}

	return 0;

	map_bars_msg_err: rsp_board_unmap_bars(dev, rsp_dev);
	pci_disable_device(dev);
	kfree(rsp_dev);
	return pci_msg_err;
}

static void rsp_board_remove(struct pci_dev *dev) {

	int pci_msg_err;
	struct rsp_device* rsp_dev;

	msg_dbg("Entering function:%s\n", __FUNCTION__);

	rsp_dev = pci_get_drvdata(dev);

	if (rsp_dev == NULL) {
		msg_err(" problem no device data!\n");
	} else {
		// do cleanup
		pci_msg_err = rsp_board_unmap_bars(dev, rsp_dev);
	}

	msg_dbg(" board_remove vendor=%04x,device=%04x,defn=%x,ss_vendor=%04x,dev->ss_device=%04x\n",
			dev->vendor,dev->device,dev->devfn,dev->subsystem_vendor,dev->subsystem_device);

	device_remove_file(&dev->dev, &dev_attr_rosta_dev_instance);

	pci_disable_device(dev);

	cdev_del(&rsp_dev->cdev);

	down(&rsp_sem);
	dev_present[rsp_dev->instance] = 0;
	up(&rsp_sem);

	kfree(rsp_dev);

	msg_dbg(" board_remove exit\n");

}

static struct pci_device_id rsp_board_pci_tbl[] = { 
		{ 0x1819, 0x0527, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, 
		{ 0x1819, 0x0529, PCI_ANY_ID, PCI_ANY_ID, 0, 0,	0 }, 
		{ 0x1819, 0xB007, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
		{ 0x1819, 0x0047, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
		{ 0x1819, 0x0747, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
		{ 0x1819, 0xB003, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, { 0, } };

// pci_driver definition
static struct pci_driver rsp_driver = { .name = DRIVER_NAME, .id_table =
		rsp_board_pci_tbl, .probe = rsp_board_probe, .remove =
		__devexit_p(rsp_board_remove), };

static ssize_t show_cdev(struct device_driver *driver, char *buf) {
	return snprintf(buf, PAGE_SIZE, "%d %d %d\n", rsp_major, rsp_minor, ROSTA_DRIVER_VERSION);
}



// initialization of the module
static int __init rsp_init_module(void) {

	dev_t dev = 0;
	int result, i;

	msg_err("Entering function:%s\n", __FUNCTION__);
	sema_init(&rsp_sem, 1);

	for (i = 0; i < MAX_DEVICES; i++)
		dev_present[i] = 0;

	if (rsp_major) {
		dev = MKDEV(rsp_major, rsp_minor);
		result = register_chrdev_region(dev, rsp_nr_devs, DRIVER_NAME);
	} else {
		result = alloc_chrdev_region(&dev, rsp_minor, rsp_nr_devs, DRIVER_NAME);
		rsp_major = MAJOR(dev);
	}

	if (result < 0) {
		msg_err("rsp: can't get major %d\n", rsp_major);
		return result;
	}

	result = pci_register_driver(&rsp_driver);

	if (result < 0) {
		msg_err("Can't register PCI device (%d)\n", result);
		return result;
	}

	result = driver_create_file(&rsp_driver.driver, &driver_attr_rosta_drv_cdev );

	if (result < 0) {
		msg_err("Can't create attribute (%d)\n", result);
		return result;
	}

	msg_err("rsp: Loaded with major %d\n", rsp_major);

	return 0;
}

// clean up the module
static void __exit rsp_module_cleanup(void) {
	msg_err("Entering function:%s\n", __FUNCTION__);

	driver_remove_file(&rsp_driver.driver, &driver_attr_rosta_drv_cdev );

	pci_unregister_driver(&rsp_driver);

	unregister_chrdev_region(MKDEV(rsp_major, rsp_minor), rsp_nr_devs);

	msg_err(" module_cleanup exit\n");
}
module_init(rsp_init_module)
;
module_exit(rsp_module_cleanup)
;

