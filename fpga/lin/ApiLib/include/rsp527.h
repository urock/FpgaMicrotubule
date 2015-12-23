/*
 * rsp527.h
 *
 *  Created on: Sep 3, 2012
 *      Author: urock
 */

#ifndef RSP527_H_
#define RSP527_H_



// Virtex registers
#define CNTRL_BAR 			0

#define TEST_REG			0x28
#define USER_CNTRL_REG		0x44
#define COMMAND_REG			0x100


// AXI Datamover Controller Registers
#define AXI_RX_ADDR			0x104
#define AXI_RX_LEN_B		0x108
#define AXI_RX_STATUS		0x10C

#define AXI_TX_ADDR			0x110
#define AXI_TX_LEN_B		0x114
#define AXI_TX_STATUS		0x118


// status valid
#define STATUS_VALID		(1<<8)



#endif /* RSP527_H_ */
