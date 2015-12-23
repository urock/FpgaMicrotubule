#ifndef FPGA_LIB_H_
#define FPGA_LIB_H_

#include "RostaPCIeAPI.h"



#define CNTRL_BAR	0



// Master Engine Register
#define MASTER_VERSION		0x00
#define MASTER_MODE			0x24
#define USER_CNTRL_REG		0x44
#define EpTxTimeCount 		0x74
#define EpRxTimeCount 		0x78
#define TxTimeCount 		0x7C
#define RxTimeCount 		0x80
#define LenCount 			0x84
#define LenGen				0x148


// Debug register in Master Engine
#define MasterMemRdTotalCnt		0x50
#define MasterEpTxTotalCnt		0x54
#define MasterEpRxTotalCnt		0x58
#define MasterMemWrTotalCnt		0x5C

#define FifoMemRdTotalCnt		0x60
#define FifoEpTxTotalCnt		0x64
#define FifoEpRxTotalCnt		0x68
#define FifoMemWrTotalCnt		0x6C









// User_ip reg
#define COMMAND_REG				0x100


// AXI Datamover Controller Registers
#define AXI_RX_ADDR			0x74
#define AXI_RX_LEN_B		0x78
#define AXI_RX_STATUS		0x7C

#define AXI_TX_ADDR			0x80
#define AXI_TX_LEN_B		0x84
#define AXI_TX_STATUS		0x88

#define	EP_TXADDRESS			0x11C
#define EP_TXLENGTH				0x120
#define EP_TXTAG				0x124
#define EP_TXSTATUS_CNTR		0x128
#define TESTREGIN				0x12C
#define TESTREGOUT				0x130

#define RXDERROR			0x134



#define START_STRESS_REG	0x134
#define STRESS_ERRORS_REG	0x138
#define COUNT_LSB_REG		0x13C
#define COUNT_MSB_REG		0x140
#define CLK_PERIOD_REG		0x144


#define HLS_STATUS 			0x134
#define HLS_AP_RETURN		0x1E8   
#define HLS_ARG1			0x1F0
#define HLS_ARG2			0x1F4
#define HLS_ARG3			0x1F8   

#define HLS_DONE_BIT		(1<<1)






// status valid
#define AXI_STATUS_VALID		(1<<8)



// DDR section
#define AXI_BRAM_BASE		0xA0000000
#define AXI_DDR_BASE		0x20000000

#ifdef HALF
#define AXI_DDR_LENGTH		0x20000000
#else
#define AXI_DDR_LENGTH		0x40000000	// 512 MB
#endif

#define DDR_INIT_DONE_BIT	0x4000//0x400000
#define DDR_INIT_DONE_BIT_2	0x8000//0x800000




API void get_time(struct timeval *tt); 
API void calc_time_speed(struct timeval *tt1, struct timeval *tt2, int len_dwords, double *tsec, double *speed);


// fpga functions
API int fpga_user_reset(int dev);
API int fpga_release_user_reset(int dev);
API void fpga_mode_reg_check_error(unsigned int reg_val);
API int fpga_check_user_ip_version(int dev, int expected_version); 
API int fpga_set_device_index(int dev, unsigned char index);
API int fpga_set_register_bit(int dev, unsigned int reg_addr, int bit, int state); 
API int fpga_start_hls(int dev); 

API int fpga_wait_for_ap_done(int dev, int *ap_return);

API int fpga_wait_for_ddr_init (int dev); 

API int fpga_write_to_axi(int dev, unsigned int *src_buf, unsigned int bytes_to_transfer, unsigned int dest_addr);
API int fpga_read_from_axi(int dev, unsigned int src_addr, unsigned int bytes_to_transfer, unsigned int *dest_buf);



#endif // FPGA_LIB_H_
