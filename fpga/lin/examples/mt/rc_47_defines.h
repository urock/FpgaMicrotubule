#ifndef RC_47_DEF_H
#define RC_47_DEF_H



#define HLS_LENGTH 0x154
#define HLS_A 0x160
#define HLS_B 0x164
#define HLS_OFFSET_I 0x158
#define HLS_OFFSET_O 0x15C
#define HLS_CP_SIZE 0x168


#define SEED_REG 0x170

#define SIZE_DWORD  0x80*(int)(((2*N_d*13+(NUM_SEEDS)/2)/(float)0x80+1)) // размер  передаваемых данных  в fpga_write_to_axi  должен быть кратен 1024 байтам (=128=0x80 двойных слов)
#define SIZE_BYTE		8*SIZE_DWORD	

#define coeffs_ddr_offset 8096

// во время дебага TOTAL_STEPS можно делать меньше




#endif