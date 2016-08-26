/*
 * rc47-api.h
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
//#define USE_PLX
#define MAX_DEVICES_TO_LIST        100

#ifndef RC47API_H_
#define RC47API_H_

#ifdef __cplusplus
#   define API extern "C"
#else
#   define API
#endif

#include "rc47-fpga-lib.h"
#include "misc-utils.h"
#include "sd_file.h"

#define Time(tt) gettimeofday(&tt,NULL)

#define MAX_RC47_BOARDS		4

// Virtex7 registers
#define CNTRL_BAR 			0

//#define TEST_REG			0x2C
#define USER_CNTRL_REG		0x44
//#define COMMAND_REG			0x100

// pear 2 pear transfers
//#define P2P_START_DMA_REG			0x30
// bit 0 - enable control over start DMA from SP6
// bit 1 - Start DMA line from SP6. read only
//#define P2P_DWORD_CNT_REG			0x34		// counts received dwords; DST
//#define P2P_EXPECT_LENGTH_REG		0x38		// transfer length dwords; host writes to DST
//#define P2P_WRITE_TIMER_REG			0x3C		// transfer ticks for speed meas; fclk = 250 MHz; dt = 4ns; DST




// SPARTAN 6 defines

#define SP6_DEVICE_ID			0xB007
#define V7_DEVICE_ID			0x0747


#define SP6_BAR					0

// Virtex 7 Chip Selects
#define CS_LD					(1<<0)
#define CS_LU					(1<<1)
#define CS_RD					(1<<2)
#define CS_RU					(1<<3)

// new virtex-7 names
#define CS_C0					CS_LD
#define CS_C1					CS_LU
#define CS_C2					CS_RD
#define CS_C3					CS_RU

// Spartan 6 Registers Addresses
#define SP6_TEST_REG1			0x2C
#define SP6_TEST_REG2			0x30

#define SP6_SYNC_REG			0x34

#define CONFIG_STATUS_R			0x40
#define CONFIG_DATA_R			0x44

#define WRITEDATA_STM_REG		0x50	// 8 bits
#define READDATA_STM_REG		0x54	// 8 bits
#define RD_DATA_COUNT_REG		0x58	// byte counter; 15 bits => max 16 kbytes per one read
#define RD_DATA_STATUS_REG		0x5C	// bit[0] = 1 => data ready


#define RESET_FIFO_REG			0x60	// WO: bit[0] = 1 => reset fifo

#define V7_PCIE_RESET_N			0x88	// RW: bit[3:0] : reset_n[3:0] (active low); default = "1111"



#define V7_LNK_TGL_CNT_0		0x8C
#define V7_LNK_TGL_CNT_1		0x90
#define V7_LNK_TGL_CNT_2		0x94
#define V7_LNK_TGL_CNT_3		0x98


#define V7_LNK_STATUS			0x9C

// bit  0	: C0 lnk_up
// bit  1	: C0 time fifo read enable 
// bit  2	: C0 time fifo empty
// bit  3	: C0 time fifo overflow
// bit  4	: C0 link toggle overflow

// bit  8	: C1 lnk_up
// bit  9	: C1 time fifo read enable 
// bit 10	: C1 time fifo empty
// bit 11	: C1 time fifo overflow
// bit 12	: C1 link toggle overflow

// bit 16	: C2 lnk_up
// bit 17	: C2 time fifo read enable 
// bit 18	: C2 time fifo empty
// bit 19	: C2 time fifo overflow
// bit 20	: C2 link toggle overflow

// bit 24	: C3 lnk_up
// bit 25	: C3 time fifo read enable 
// bit 26	: C3 time fifo empty
// bit 27	: C3 time fifo overflow
// bit 28	: C3 link toggle overflow

#define TIME_FIFO_0		0xA0
#define TIME_FIFO_1		0xA4
#define TIME_FIFO_2		0xA8
#define TIME_FIFO_3		0xAC



// Spartan 6 RD_DATA_STATUS_REG bits
#define SP6_STM_DATA_READY		(1<<0)
#define SP6_HOST_TX_EMPTY		(1<<2)


// CONFIG STATUS Register Bits
#define DONE_LD					(1<<0)
#define DONE_LU					(1<<1)
#define DONE_RD					(1<<2)
#define DONE_RU					(1<<3)
#define INIT_LD					(1<<4)
#define INIT_LU					(1<<5)
#define INIT_RD					(1<<6)
#define INIT_RU					(1<<7)
//#define CS_LD					(1<<8)
//#define CS_LU					(1<<9)
//#define CS_RD					(1<<10)
//#define CS_RU					(1<<11)
#define V7_PROG					(1<<12)


#define CS_MASK					0x00000F00
#define INIT_MASK				0x000000F0
#define DONE_MASK				0x0000000F


// Spartan 6 CBUS registers

#define SP6_MAC_RB8			0x38

#define SP6_V7_RULE			0x64
// -- (3 downto 0) - Chip Select "0000" - С0, "0001" - С1, "0010" - С2, "0011" - С3.
// Старшие биты регистра используются для определения команды на шине CBus.
// (8 downto 4) = "00001" - передача уникального номера User ID в ПЛИС V7
// (8 downto 4) = "00010-01000" - 7 32-битных регистров, доступных для записи в User_IP
// (8 downto 4) = "01001-01111" - 7 32-битных регистров, доступных для чтения из User_IP

# define SP6_WRITE_V7_FIFO_CMD		0x10	// should be shifted 4 bits to the left
// (8 downto 4) = "10000" - передача пакетных данных в ПЛИС V7

#define SP6_FETCH_V7_FIFO_DATA_CMD		0x11
// (8 downto 4) = "10001" - чтение пакетных данных из ПЛИС V7

#define SP6_FETCH_V7_FIFO_STATUS_CMD	0x12
// (8 downto 4) = "10010" - чтение статуса фифо из ПЛИС V7

#define SP6_V7_LD_ID0		0x70
#define SP6_V7_LU_ID0		0x74
#define SP6_V7_RD_ID0		0x78
#define SP6_V7_RU_ID0		0x7C


#define V7_WR_FD			0x90	// WO; data reg for v7 fifo
#define V7_WR_ST			0x94	// RO; bit 0 - empty, bit 1 - full; 
#define V7_RD_FD			0x98	// RO
#define V7_RD_ST			0x9C	// 
// bits 9:0 - read data count (in dwords)
// bit 15 - empty

#define V7_F_STS			0xA0	// fifo status in virtex7
// bits 11:0 - read data count (in bytes)?
// bit 15 - empty
// bit 16 - full











// RECORD_DESC_DEFINES for BITFILE_V7
#define V7_485_START			11
#define V7_585_START			2








typedef struct {
  unsigned char	fversion; 	// firmware version
  unsigned char	i2c_error;	// 1 - error,
  unsigned char	cpld_error;
  unsigned char	sdio_error;
  unsigned char	sdio_card_vaild_code;
  unsigned char plx_error;
  unsigned int  plx_id;
  unsigned char mac0;		//!< RDM03 MAC Address byte 0 MSB
  unsigned char mac1;
  unsigned char mac2;
  unsigned char mac3;
  unsigned char mac4;
  unsigned char mac5; 		//!< RDM03 MAC Address byte 5 LSB
  char serial_number[9];

} stm_status_t;





typedef struct {
  unsigned char	i2c_error_38;	// 1 - error,
  unsigned char	i2c_error_39;	// 1 - error,
  unsigned char	i2c_error_40;	// 1 - error,
  unsigned char	id_error;			// 1 - error,

  unsigned char	u38_id;
  unsigned char	u39_id;
  unsigned char	u40_id;

  unsigned char	u38_v2_1v0_lu;
  unsigned char	u38_v3_1v0_ld;
  unsigned char	u39_v2_1v0_ru;
  unsigned char	u39_v3_1v0_rd;

  unsigned char	u40_v1_12v;
  unsigned char	u40_v2_5v;
  unsigned char	u40_v3_18v;

  unsigned char	u38_Tint;
  unsigned char	u38_Text1;		// FPGA LD		C0
  unsigned char	u38_Text2;		// FPGA LU		C1

  unsigned char	u39_Tint;
  unsigned char	u39_Text1;		// FPGA RD		C2
  unsigned char	u39_Text2;		// FPGA RU		C3 

  unsigned char	u40_Tint;
  unsigned char	u40_Text1;		// PLX
  unsigned char	u40_Text2;		// not connected


} max_status_t;


// all voltages are in millivolts
typedef struct {

	unsigned short Vref_25_raw; 		// adc data for reference voltage of 2.5 V
	unsigned short VDC_12;
	unsigned short VDC_5;
	unsigned short VDC_33;
	unsigned short V_18;
	unsigned short V_1v0_lu;
	unsigned short V_1v0_ld;
	unsigned short V_1v0_ru;
	unsigned short V_1v0_rd;
	unsigned short V_12va_lu;
	unsigned short V_12va_ld;
	unsigned short V_12va_ru;
	unsigned short V_12va_rd;
	unsigned short V_10va_lu;
	unsigned short V_10va_ld;
	unsigned short V_10va_ru;
	unsigned short V_10va_rd;
	unsigned short V_uem1_L;
	unsigned short V_uem2_L;
	unsigned short V_uem1_R;
	unsigned short V_uem2_R;
	unsigned short V_15v_L;
	unsigned short V_12va_s;
	unsigned short V_15v_R;
	unsigned short V_18va;
	unsigned short V_12;
	unsigned short dummy1;
	unsigned short dummy2;
	unsigned short dummy3;

} adc_data_t, *adc_data_p;







// RECORD_DESC_DEFINES for BITFILE_V7
#define V7_485_START			11
#define V7_585_START			2




// RC47 Error codes
#define RC47_ERRORS

#define RC47_FILE_ERROR			-1
#define RC47_INIT_DOWN_ERROR	-2
#define RC47_INIT_UP_ERROR		-3
#define RC47_DONE_UP_ERROR		-4
#define RC47_CS_PARAM_ERROR		-5







// data structure to describe RC47 board
struct rc47_board_t{

	int				index;
//	int 			plx_up_port;		// PLX upstream port number
//	int				plx_up_bus_num;		// PLX upstream bus number

	struct pci_device	*SP_pd;			// pointer to pci device Spartan6
	struct pci_device	*LU_pd;			// Virtex 7 LU
	struct pci_device	*LD_pd;			// ..
	struct pci_device	*RU_pd;
	struct pci_device	*RD_pd;

	stm_status_t	stm_status;
	max_status_t	max_status;
	adc_data_t		adc_data;
	#ifdef USE_PLX
	PLX_DEVICE_KEY    *PLX_dev1;
	PLX_DEVICE_KEY    *PLX_dev2;
	PLX_DEVICE_KEY    *PLX_dev3;
	PLX_DEVICE_KEY    *PLX_dev4;
	PLX_DEVICE_KEY    *PLX_dev5;
	#endif
};


/*
	On RC-47 issue 2 Virtex-7 chips were renamed

	LD = C0
	LU = C1
	RD = C2
	RU = C3

*/




#define SDIO_BLOCK_SIZE		512



API int rc47_search_for_boards(struct pci_device *pd, struct rc47_board_t *rc47);
API void rc47_print_device_info(int NumBoards, struct rc47_board_t *rc47); 

API int rc47_reset_v7(struct rc47_board_t *rc47, unsigned char chip_select);
API int rc47_reconfig_v7_mSD(struct rc47_board_t *rc47, unsigned char chip_select, unsigned char v7_bit_id);
API int rc47_config_v7(struct rc47_board_t *rc47, int chip_select, char *bit_file_path);
API int rc47_link_reset(struct rc47_board_t *rc47, int chip_select);

API int rc47_read_stm_status(int dev, stm_status_t* stm_status_ptr);
API int rc47_read_max_status(int dev, max_status_t* max_status_ptr);
API int rc47_read_adc_data(int dev, adc_data_t* adc_data_ptr);

API void rc47_remove_v7_devices(struct rc47_board_t *rc47, unsigned char chip_select);

API int rc47_write_buf_to_sdio(struct rc47_board_t *rc47, unsigned char *src_buf, unsigned int sdio_block_addr, unsigned char block_num);
API int rc47_read_buf_from_sdio(struct rc47_board_t *rc47, unsigned char *dst_buf, unsigned int sdio_block_addr, unsigned char block_num);

API int rc47_programm_sdio(struct rc47_board_t *rc47, char *bit_file_path, int verify);

API int build_sd(char* sp6_file_name_ptr, char* v7_file_name_ptr,unsigned int* v7_description, char* sd_card_file );

//API int rc47_test_stm(int dev);
API int wait_for_link_up(struct rc47_board_t *rc47, int chip_select);
#ifdef USE_PLX
void rc47_set_plx_dma_param(unsigned int addr, unsigned int len, int * wr_buf, PLX_DMA_PARAMS *   DmaParams);
int rc47_release_plx(PLX_DEVICE_OBJECT * Device,  PLX_NOTIFY_OBJECT * NotifyObject);
int rc47_init_plx (PLX_DEVICE_OBJECT * Device, PLX_NOTIFY_OBJECT * NotifyObject, PLX_INTERRUPT  *   PlxInterrupt, PLX_DMA_PROP    *  DmaProp);
#endif
// CBUS functions

API int cbus_write_v7_rx_fifo(int dev, int chip_select, int length, unsigned int *wr_buf);
API int cbus_read_v7_tx_fifo(int dev, int chip_select, int length, unsigned int *rd_buf);
API int cbus_get_v7_tx_fifo_word_count(int dev, int chip_select);



#endif /* RC47API_H_ */
