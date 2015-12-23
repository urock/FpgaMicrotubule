/*****************************************************
* ux_types			         	     *
* Author:  Alexey Shmatok <alexey.shmatok@gmail.com> *
******************************************************/

#ifndef __UX_TYPES__H
#define __UX_TYPES__H

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned long long u64;

typedef union {u8 u8_val;u16 u16_val;u32 u32_val;u64 u64_val;} u_t;
typedef union {u8 u8_val[4];u16 u16_val[2];u32 u32_val;} u_t2;
typedef union {u8* u8_ptr;u16* u16_ptr;u32* u32_ptr;u64* u64_ptr;} u_ptr_t;


#endif
