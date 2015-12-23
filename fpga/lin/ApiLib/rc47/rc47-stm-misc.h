/*
 * rc47-stm-misc.h
 *
 *  Created on: May 21, 2012
 *      Author: urock
 */

#ifndef RC47STMMISC_H_
#define RC47STMMISC_H_

/***********************************************************************************
 *
 * STM over SP6 communication sector
 *
 **********************************************************************************/

#define STM_TIMEOUT				200



// cmd_code_defines
#define CMD_READ_STATUS			0x01
#define CMD_READ_MAX_DATA		0x02
#define CMD_READ_ADC_DATA		0x03


#define CMD_V7_RST				0x08
#define CMD_V7_RECONFIG			0x09

#define CMD_WRITE_XSVF			0x10
#define CMD_READ_XSVF			0x11
#define CMD_PROG_CPLD			0x12

#define CMD_READ_SDIO			0x19
#define CMD_WRITE_SDIO			0x1A


#define CMD_CHECK_READY			0xA0
#define STM_READY				0xA1
#define STM_BUSY				0xA2

#define CMD_ACK					0xFF
#define CMD_CMLP				0xEE
#define CMD_NACK				0xF0
#define CMD_ERROR				0xE0




	// CMD_V7_RST
		struct v7_rst_str {
			unsigned char	v7_index; 	// one hot encoding
		};


	// CMD_V7_RECONFIG
		struct v7_reconfig_str {
			unsigned char	v7_index; 	// one hot encoding
			unsigned char	bit_id;
		};


	// CMD_WRITE_XSVF, CMD_READ_XSVF
		struct xsvf_str {
			unsigned short		xsvf_position; 	// current transfer position
			unsigned short		xsvf_len;		// current transfer length in bytes; = 128
		};


	// CMD_xxxx_SDIO
		struct sdio_access_str {
			unsigned int	addr; 					// in blocks of 512 bytes
			unsigned char	length; 		// in blocks; max - SDIO_MAX_BLOCKS_RX
		};


	// command frame
		struct cmd_frame_s {
			unsigned char cmd_code;					// from cmd_code_defines
			union {
				struct v7_rst_str			v7_rst_data;
				struct v7_reconfig_str		v7_reconfig_data;
				struct xsvf_str				xsvf_data;
				struct sdio_access_str		sdio_access;
			} cmd_data;
		};




int rc47_send_cmd_frame(int dev, struct cmd_frame_s * frame);

int rc47_send_byte_stm(int dev, unsigned char data_byte);
int rc47_send_buffer_stm(int dev, unsigned char *src_buf, int byte_count);

int rc47_receive_byte_stm(int dev, unsigned char *data_byte);
int rc47_receive_buffer_stm(int dev, unsigned char *dst_buf, int num);
int rc47_wait_for_host_tx_fifo_empty(int dev);

#endif /* RC47STMMISC_H_ */
