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

    return num_boards; 
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

      std::cerr << "Error getting index by dev num" << std::endl;
      return -1; 
    }

    free(wr_buf_free[index]);
    free(rd_buf_free[index]);

    close(dev);

    devs[index] = -1; 

    return 0; 

  }


  int FpgaDev::StartRandomGenerator(int dev, int L, unsigned int *seeds) {
    
    unsigned int reg_val; 

    for (int i =0; i < L; ++i){
      RD_WriteDeviceReg32m(dev, CNTRL_BAR, SEED_REG + 4*i, seeds[i]); 
    }

    RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
   
    // deassert rand reset
    reg_val |= (1<<4);
    RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
    
    // set rand_enable flag
    reg_val |= (1<<7);
    RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);

    // start rand core
    reg_val |= (1<<5);   
    RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);         

    return 0; 
  }


  int FpgaDev::StopRandomGenerator(int dev) {
    
    unsigned int reg_val; 

    RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
    
    // assert rand reset
    reg_val &= ~(1<<4);
    RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
       
    // reset rand_enable flag
    reg_val &= ~(1<<7);
    RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);         

    return 0; 
  }


  int FpgaDev::LoadCoeffs(int dev, int NumCoeffs, float *coeffs) {

    unsigned int reg_val;

    for (int i = 0; i < NumCoeffs; ++i) {
      wr_buf[dev][i] = reinterpret_cast<unsigned int&>(coeffs[i]); 
      // equal??
      // wr_buf[dev][i] = *reinterpret_cast<unsigned int*>(&coeffs[i]); 
    }


    if (fpga_write_to_axi(dev, wr_buf[dev], 64*sizeof(float), 0x20000000 + coeffs_ddr_offset) < 0){
       fprintf (stderr,"Error in fpga_write_to_axi\n");
       RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, reg_val);
       fprintf (stderr,"MasterMemRdTotalCnt 0x%x\n", reg_val);
       RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, reg_val);
       fprintf (stderr,"FifoMemRdTotalCnt 0x%x\n", reg_val);
       RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, reg_val);
       fprintf (stderr,"FifoMemWrTotalCnt 0x%x\n", reg_val);
       RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, reg_val);
       fprintf (stderr,"MasterMemWrTotalCnt 0x%x\n", reg_val);
       
       return -2;
    } 

    // set use_coeffs_from_json flag in hls core
    RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_OFFSET_I, 1);

    return 0; 
  }



int FpgaDev::CalcDynamics(  int                  dev,
                            unsigned int         n_step,           //time steps to run 
                            unsigned int         n_layers,        // number of layers to calculate. should be a multiple of 12
                
                            std::vector<std::vector<float> > &x,
                            std::vector<std::vector<float> > &y,
                            std::vector<std::vector<float> > &t,
                            std::vector<std::vector<int> > &type
                          ) 
{  

  int index = GetIndexByDev(dev);

  if (index < 0) {
    std::cerr << "CalcDynamics: bad dev and index" << std::endl;
    return -2;
  }

   two_floats tmp; 
   two_floats w0, w1;
   
   unsigned int reg_val; 
   unsigned int i,j; 

   int cnt, hls_done;
   
   int k = 0; // ddr buffer index
   
   two_floats * buf_in = (two_floats *)wr_buf;
   
   // 16 bytes for each molecule 
         
   
   for (i=0; i<13; i++)
   for (j=0; j<n_layers; j++) {
      
      tmp.d0 = x[i][j];
      tmp.d1 = y[i][j];    
      
      
      buf_in[k++] = tmp;
      tmp.d0 = t[i][j];
      if (type[i][j] == -1) {
         tmp.d1 = 1;
      } else {
         tmp.d1 = type[i][j];
      }     
      buf_in[k++] = tmp; 
   }


   // deassert mt hls reset
   RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
   reg_val |= (1<<6);
   RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);


      
   //////////////////////////////////////////////////////////////////////////////
   RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_A, n_step);
   
   int p2 = 0;
   switch(n_layers) {
      case 12: p2 = 0; break;
      case 24: p2 = 1; break;
      case 36: p2 = 2; break;
      case 48: p2 = 3; break;
      case 60: p2 = 4; break;
      case 72: p2 = 5; break;
      case 84: p2 = 6; break;
      case 108: p2 = 7; break;
      case 156: p2 = 8; break;
      case 216: p2 = 9; break;
      default: p2 = 2;  
   }
   
   RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_B, p2);


   unsigned int size_dw = (((4*n_layers*13)+32)/32); // pcie transfer size should be multiple of 128 bytes 
   
   // fpga_write_to_axi needs transfers size in bytes
   if (fpga_write_to_axi(dev, wr_buf[index], 4*size_dw, 0x20000000) < 0){
      fprintf (stderr,"Error in fpga_write_to_axi\n");
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, reg_val);
      fprintf (stderr,"MasterMemRdTotalCnt 0x%x\n", reg_val);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, reg_val);
      fprintf (stderr,"FifoMemRdTotalCnt 0x%x\n", reg_val);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, reg_val);
      fprintf (stderr,"FifoMemWrTotalCnt 0x%x\n", reg_val);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, reg_val);
      fprintf (stderr,"MasterMemWrTotalCnt 0x%x\n", reg_val);
      
      return -2;
   }
   

   

   //////////////////////////////////start hls///////////////////////////////////
   RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
   reg_val |= (1<<2);
   RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
   ///////////////////////////////////////////////////////////////////////////////
   

   //////////////////////////wait hls to finish///////////////////////////////////
   cnt =0;
   do{
      usleep(1000);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
      cnt++;
      hls_done = (reg_val&(1<<3)) ? 1 : 0; 
      
   }while ((!hls_done)&&(cnt!=1000000));
   //printf("Command reg is 0x%x\n", reg_val);
   if (cnt ==1000000) {fprintf(stderr,"Timeout error\n"); return -1;}
   ///////////////////////////////////////////////////////////////////////////////

RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);

printf("hls done cnt = %d, reg_val = 0x%x\n",cnt, reg_val); 
   
   ///////////////////////read data from fpga ddr 
   if (fpga_read_from_axi(dev, 0x20000000, 4*size_dw, rd_buf[index]) < 0){
      fprintf (stderr,"Error in fpga_read_from_axi\n");
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, reg_val);
      fprintf (stderr,"MasterMemRdTotalCnt 0x%x\n", reg_val);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, reg_val);
      fprintf (stderr,"FifoMemRdTotalCnt 0x%x\n", reg_val);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, reg_val);
      fprintf (stderr,"FifoMemWrTotalCnt 0x%x\n", reg_val);
      RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, reg_val);
      fprintf (stderr,"MasterMemWrTotalCnt 0x%x\n", reg_val);
      return -3; 
   }
   

   two_floats * buf_out = (two_floats *)rd_buf;
   
   k = 0;
   for (i=0; i<13; i++)
     for (j=0; j<n_layers; j++) {
        
        w0 = buf_out[k++];
        w1 = buf_out[k++]; 
        
        x[i][j] = w0.d0; 
        y[i][j] = w0.d1; 
        t[i][j] = w1.d0; 
     }     

   return 0;            

}


  


} // namespace microtubule