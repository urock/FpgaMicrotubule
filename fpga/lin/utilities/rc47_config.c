/*
 * rc47_config.c
 *
 *  Created on: May 18, 2012
 *      Author: urock
 */

#include "RostaPCIeAPI.h"
#include "rc47-api.h"


struct cmd_params_t {
	int		board_index;
	int		chip_select;
	char 	*bit_file_path;

};




void print_usage(void)
{
	printf("Command line args error!\n");
	printf("Usage: <executable name> -b [board index] -v [chip select] -f [bit file name]\n");
	printf("chip select: any combination of C0, C1, C2, C3 or all\n");

}


int process_cmd_line(int argc, char *argv[], struct cmd_params_t *cmd_p)
{
	int i,j, index, chip_select = 0;

	int got_index=0, got_cs=0, got_file_name=0, max_j=0;


	for (i=0;i<argc; i++) {

		if (!strncmp("-b",argv[i],2)) {
			sscanf(argv[i+1],"%d",&index);
			cmd_p->board_index = index;
			got_index = 1;

		}
		if (!strncmp("-v",argv[i],2)) {
			if ((i+4) < (argc-1))  //
				max_j = i+4;
			else
				max_j = argc-1;


			for(j=i;j<=max_j;j++) {
				if (!strncmp("C0",argv[j],2))		// LD
					chip_select |= CS_LD;
				if (!strncmp("C1",argv[j],2))		// LU
					chip_select |= CS_LU;
				if (!strncmp("C2",argv[j],2))		// RD
					chip_select |= CS_RD;
				if (!strncmp("C3",argv[j],2))		// RU
					chip_select |= CS_RU;
				if (!strncmp("all",argv[j],3))
					chip_select = CS_LD | CS_LU | CS_RD | CS_RU;
			}
			cmd_p->chip_select = chip_select;
			got_cs = 1;


		}
		if (!strncmp("-f",argv[i],2)) {
			cmd_p->bit_file_path = argv[i+1];
			got_file_name = 1;
		}
	}


	if (got_index && got_cs && got_file_name)
		return 1;
	else
		return -1;
}


struct pci_device pd[MAX_PCIE_DEVICES];
struct rc47_board_t rc47[MAX_RC47_BOARDS];

struct rc47_board_t rc47n[MAX_RC47_BOARDS];		// after reconfig



int main(int argc, char *argv[])
{
	int r, k, i, index, chip_select, ret_val, kn, error = 0;

	struct cmd_params_t	cmd_p;




	char cmd_str[100];



	if (process_cmd_line(argc, argv, &cmd_p) < 0) {
		print_usage();
		return -1;
	}


	index = cmd_p.board_index;

	chip_select = cmd_p.chip_select;

	//printf("\nindex = %d\ncs = %d\nfile_name=%s\n\n",index,chip_select,cmd_p.bit_file_path);


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


	if (index >= k) {
		fprintf(stderr,"\nInvalid board index! Exitting...\n\n");
		return -2;
	} else {
		printf("Board selected: %d\n",index);
	}


	// open SP6 device
	do_and_test(RD_OpenDevice,(rc47[index].SP_pd));


	ret_val = rc47_config_v7(&rc47[index], chip_select, cmd_p.bit_file_path);


	if (ret_val) {

		fprintf(stderr,"Error in rc47_config_v7\n");
		return -8;
	}

//	ret_val = wait_for_link_up(&rc47[index], chip_select);

	RD_CloseDevice(rc47[index].SP_pd);

/*
	if (ret_val != 0) {

		fprintf(stderr,"Timeout waiting for link up! link_status = 0x%x ",ret_val);
		if ((ret_val & (1<<0)) == 0) fprintf(stderr,"C0 Error ");
		if ((ret_val & (1<<1)) == 0) fprintf(stderr,"C1 Error ");
		if ((ret_val & (1<<2)) == 0) fprintf(stderr,"C2 Error ");
		if ((ret_val & (1<<3)) == 0) fprintf(stderr,"C3 Error ");
		fprintf(stderr,"\n");
		return -9;
	} 
*/

	printf("Wait for link\n");
	sleep(1); 


	printf("\nRescan PCI Express bus after reconfig...\n");
	do_and_test(RD_RescanBus,());
	sleep(1);

	printf("\nSearching for RC47 boards\n");
	

	kn = rc47_search_for_boards(pd,rc47n);
	

	rc47_print_device_info(kn,rc47n);
	

	if (kn != k) {		// error

		fprintf(stderr, "Error in rc47_config k != kn \n");
		return -3;

	} else {

		for(i=0; i<kn; i++) {

			if ((rc47[i].LD_pd != NULL) && (rc47n[i].LD_pd == NULL)) {
				fprintf(stderr, "Error in rc47 link reset board %d: no C0 after reset\n",i);
				error=1;
			}
			if ((rc47[i].LU_pd != NULL) && (rc47n[i].LU_pd == NULL)) {
				fprintf(stderr, "Error in rc47 link reset board %d: no C1 after reset\n",i);
				error=1;
			}
			if ((rc47[i].RD_pd != NULL) && (rc47n[i].RD_pd == NULL)) {
				fprintf(stderr, "Error in rc47 link reset board %d: no C2 after reset\n",i);
				error=1;
			}
			if ((rc47[i].RU_pd != NULL) && (rc47n[i].RU_pd == NULL)) {
				fprintf(stderr, "Error in rc47 link reset board %d: no C3 after reset\n",i);
				error=1;
			}

		}
	}

	if (error) {
		return -5;
	}

	
	

	// workaroud Device Control Register Issue
	if (chip_select & CS_LD ) { // printf(" C0");
		sprintf(cmd_str,"setpci -s %x:00.0 68.L=00003820",rc47n[index].LD_pd->phys_bus);
		printf("%s\n",cmd_str);
		ret_val = system(cmd_str);
	}

	if (chip_select & CS_LU) { //printf(" C1");
		sprintf(cmd_str,"setpci -s %x:00.0 68.L=00003820",rc47n[index].LU_pd->phys_bus);
		printf("%s\n",cmd_str);
		ret_val = system(cmd_str);
	}

	if (chip_select & CS_RD) { //printf(" C2");
		sprintf(cmd_str,"setpci -s %x:00.0 68.L=00003820",rc47n[index].RD_pd->phys_bus);
		printf("%s\n",cmd_str);
		ret_val = system(cmd_str);
	}

	if (chip_select & CS_RU) { //printf(" C3");
		sprintf(cmd_str,"setpci -s %x:00.0 68.L=00003820",rc47n[index].RU_pd->phys_bus);
		printf("%s\n",cmd_str);
		ret_val = system(cmd_str);
	}



	return 0;
}



