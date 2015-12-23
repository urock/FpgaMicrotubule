/*****************************************

Rosta SD loader main header file


*******************************************/



#ifndef _SDLOADER_H_
#define _SDLOADER_H_

#include <fcntl.h>  
#include <stdlib.h>     
#include <unistd.h>     
#include <stdio.h>
#include <string.h>





int load_file_to_mem(char *file_name);
int get_biffile_length(char *file_name); 


#define MAX_V7_BITFILES			5


// msd - Micro

#define SDIO_BLOCK_SIZE		512
#define MSD_VALID_CODE		0x19841211

typedef struct {
	unsigned int 	valid_code; 		// static valid code
	unsigned int	card_type;			// some type from CARD_TYPE_DEFINES
	unsigned int 	num_records;		// number of records on card
	unsigned int	next_record_addr;	// 
	

} msd_master_record_str, *msd_master_record_str_p; 

typedef struct {
	unsigned int	record_type;		// some type from RECORD_TYPE_DEFINES
	unsigned int	record_description;	// from RECORD_DESC_DEFINES
	unsigned int	bit_file_addr; 		// bit_file address in blocks (512 bytes)
	unsigned int 	bit_file_length;	// in bytes
	unsigned int	next_record_addr;	// 0 - end of record list
	unsigned int	dummy_1;
	unsigned int	dummy_2;
	unsigned int	dummy_3;

} msd_record_str, *msd_record_str_p;




// RECORD_TYPE_DEFINES
#define BITFILE_V7			1
#define BITFILE_SP6			2

// RECORD_DESC_DEFINES for V7
#define XC7VX485T_ALL		11
#define XC7VX485T_TEST		21

#define XC7V585T_ALL		12
#define XC7V585T_TEST		22

// RECORD_DESC_DEFINES for SP6
#define LX25T			1
#define LX45T			2


#endif




