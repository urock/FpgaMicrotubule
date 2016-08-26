#pragma once
#include "rc47-api.h"

#include <iostream>


#define MAX_BOARDS   (MAX_RC47_BOARDS)
#define MAX_DEVS     (MAX_RC47_BOARDS * 5)

namespace microtubule {


   class FpgaDev {

   public:

      FpgaDev();
      int FindDevices(void);
      int GetIndexByDev(int);

      int open(int, int);
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

