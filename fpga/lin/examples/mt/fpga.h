#pragma once
#include "rc47-api.h"

#include <iostream>

#define HLS_LENGTH 0x154
#define HLS_A 0x160
#define HLS_B 0x164
#define HLS_OFFSET_I 0x158
#define HLS_OFFSET_O 0x15C
#define HLS_CP_SIZE 0x168


#define SEED_REG 0x170

#define SIZE_DWORD  0x80*(int)(((2*N_d*13)/(float)0x80+1)) 
#define SIZE_BYTE    8*SIZE_DWORD   

#define coeffs_ddr_offset 8096


#define MAX_BOARDS   (MAX_RC47_BOARDS)
#define MAX_DEVS     (MAX_RC47_BOARDS * 5)

namespace microtubule {


  class FpgaDev {

  public:

    FpgaDev();
    int FindDevices(void);
    int GetIndexByDev(int);

    int open(int, int);
    int SetSeeds(int, int, unsigned int *);
    int close(int);


  private: 

    struct rc47_board_t rc47[MAX_BOARDS];

    struct pci_device pd[MAX_DEVS];
    

    unsigned int   *wr_buf[MAX_DEVS], *wr_buf_free[MAX_DEVS];
    unsigned int   *rd_buf[MAX_DEVS], *rd_buf_free[MAX_DEVS];      

    int num_boards = -1; 

    int devs[MAX_DEVS]; 


  };

} // namespace microtubule

