#include "fpga.h"



namespace microtubule {

	FpgaDev::FpgaDev() {}

	int FpgaDev::FindDevices() {


	   printf("\nSearching for RC47 boards\n");

	   num_boards = rc47_search_for_boards(pd,rc47);

	   if (num_boards<0) {
	      std::cerr << "No boards in system!\n\n";
	      return -1;
	   } else 
	   	std::cerr << "Boards in system -> " << num_boards << "\n"; 


	   rc47_print_device_info(num_boards,rc47);		

		return 0; 
	}

	// returns file descriptor or negative num on error
	int FpgaDev::open(int board, int chip_select) {

		int r; 
		unsigned int reg_val;

		struct pci_device *pd_ptr;

		int index = board * 4 + chip_select; 

		if (num_boards <= 0) {
	      std::cerr << "No boards in system!\n\n";
	      return -2;			
		}

		if (board >= num_boards) {
			std::cerr << "Invalid selected board number\n";
			return -3;
		}

	   if ((chip_select == 0) && (rc47[board].LD_pd != NULL)) {
	      pd_ptr = rc47[board].LD_pd;
	      printf("Virtex Chip\t:::\t%s\n", "C0");
	   } else if ((chip_select == 1) && (rc47[board].LU_pd != NULL)) {
	      pd_ptr = rc47[board].LU_pd;
	      printf("Virtex Chip\t:::\t%s\n", "C1");
	   } else if ((chip_select == 2) && (rc47[board].RD_pd != NULL)) {
	      pd_ptr = rc47[board].RD_pd;
	      printf("Virtex Chip\t:::\t%s\n", "C2");
	   } else if ((chip_select == 3) && (rc47[board].RU_pd != NULL)) {
	      pd_ptr = rc47[board].RU_pd;
	      printf("Virtex Chip\t:::\t%s\n", "C3");
	   } else {
	      std::cerr << "Invalid selected virtex chip\n";
	      return -4;
	   }

	   // open pcie device
	   do_and_test(RD_OpenDevice,(pd_ptr));      

	   int dev = pd_ptr->intfd;	   

	   devs[index] = dev; 


	   // reset user part
	   if (fpga_user_reset(dev) < 0)
	   return -5;
	   
	   if (fpga_release_user_reset(dev) < 0) 
	   return -6;     

	   if (fpga_wait_for_ddr_init(dev) < 0)
	   return -7;
	   else
	   printf("Init DDR OK!\n"); 
	   
	   
	   // set interconnection 
	   RD_ReadDeviceReg32m(dev, CNTRL_BAR, 0x4, reg_val);
	   
	   reg_val |= (1<<5);
	   
	   RD_WriteDeviceReg32m(dev, CNTRL_BAR, 0x4, reg_val);
	   

	   /////////////////////////////////////set all counters to zero///////////////////
	   
	   RD_WriteDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, 0);
	   RD_WriteDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, 0);
	   RD_WriteDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, 0);
	   RD_WriteDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, 0);
	   
	   ///////////////////////////////////////////////////////////////////////////////
	   
	   if (create_page_alligned_buffer(0x100000, &wr_buf[index], &wr_buf_free[index])) {
	      fprintf (stderr,"Error memory allocation for wr_buf\n");
	      return -8;
	   }

	   if (create_page_alligned_buffer(0x100000, &rd_buf[index], &rd_buf_free[index])) {
	      fprintf (stderr,"Error memory allocation for rd_buf\n");
	      return -8;
	   }  

	   return dev;
	}

	int FpgaDev::GetIndexByDev(int dev) {

		int i;
		bool found_index = false; 

		for (i = 0; i < MAX_DEVS; ++i) 
			if (devs[i] == dev) {
				found_index = true;
				break;
			}

		return (found_index) ? i : -1; 

	}

	int FpgaDev::close(int dev) {

		int index = GetIndexByDev(dev); 

		if (index == -1) {

			std::cerr << "Error getting index by dev num\n";
			return -1; 
		}

		free(wr_buf_free[index]);
		free(rd_buf_free[index]);

		close(dev);

		return 0; 

	}


} // namespace microtubule