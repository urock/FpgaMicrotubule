/*
 * rc47_find_devices.c
 *
 *  Created on: May 31, 2012
 *      Author: urock
 */

#include "RostaPCIeAPI.h"
#include "rc47-api.h"
#ifdef USE_PLX
	#include "PlxApi.h"
#endif

struct pci_device pd[MAX_PCIE_DEVICES];
struct rc47_board_t rc47[MAX_RC47_BOARDS];


int main(int argc, char *argv[])
{
	int i, k, ret_val, dev;
	stm_status_t    stm_status;



	printf("\nSearching for RC47 boards\n");

	k = rc47_search_for_boards(pd,rc47);

	if (k<0) {
		printf("No boards in system!\n\n");
		return 0;
	} else 
		if (k==1)		
			printf("Found %d board\n",k);
		else
			printf("Found %d boards\n",k);


	rc47_print_device_info(k,rc47);
	
	for( i =0; i<k; i++){
                if (RD_OpenDevice(rc47[i].SP_pd)<0)
                        printf("OpenDevice error\n");
                dev = rc47[i].SP_pd->intfd;
                ret_val = rc47_read_stm_status(dev, &stm_status);
                if( ret_val==0) 
            		printf("RC47 # %d SN  \t = %s\n",i,stm_status.serial_number);
				else
					printf("Error in rc47_read_stm_status\n");
				
				RD_CloseDevice(rc47[i].SP_pd);
        }
 

	return k;
}
