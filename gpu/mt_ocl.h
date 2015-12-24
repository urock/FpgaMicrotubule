#ifndef MT_OCL_H
#define MT_OCL_H

// GLOBALS 
#define SUCCESS 0
#define FAILURE 1

#define SDK_SUCCESS 0
#define SDK_FAILURE 1

#include <CL/cl.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

#define ASSERT_CL( ret )\
   if( (ret) != CL_SUCCESS )\
   {\
      fprintf( stderr, "%s:%d: error: %s\n", \
             __FILE__, __LINE__, cluErrorString(ret) );\
      exit(FAILURE);\
   }



#include "mt_defines.h"

#define MT_KERNELS_FILE "mt_kernels.cl"
//#define MT_KERNELS_FILE "mt_kernels_f3.cl"
#define MT_KERNEL2D_1G_NAME "mt_kernel2d_1g"
#define MT_KERNEL2D_MG_NAME "mt_kernel2d_mg"

#define MT_KERNELS2_FILE "mt_kernels2.cl"
#define MT_KERNEL1_NAME "mt_kernel2d1"
#define MT_KERNEL2_NAME "mt_kernel2d2"
//#define MT_KERNEL3_NAME "mt_kernel2d_run"

#define MT_ENABLE_KERNEL_PROFILING
const char *cluErrorString(cl_int err);
std::string convertToString(const char * filename);


class mtOcl {

public:

int pl_idx;
int dev_idx;

double prof_time;

size_t gszX;
size_t gszY;
size_t lszX;
size_t lszY;
int numIter;

int bufSz;
int bufSzNg;

cl_mem   xBuf;
cl_mem   yBuf;
cl_mem   tBuf;

cl_mem   lat_l_Buf;
cl_mem   lat_r_Buf;
cl_mem   long_u_Buf;
cl_mem   long_d_Buf;


float* _x;
float* _y;
float* _t;
float* _ll;
float* _lr;
float* _lu;
float* _ld;
cl_int* _f;


cl_mem   fBuf;
cl_int* flags;

cl_context          context;
cl_device_id        *devices;
cl_command_queue    commandQueue;

//int dev_idx;
int use_rw_buffer;
int use_kernel_profiling;

//int use_program;
cl_program program1;
cl_kernel  kernel2d_1g;
cl_kernel  kernel2d_mg;

cl_program program2;
cl_kernel  kernel1;
cl_kernel  kernel2;
cl_kernel  kernel3;


int initializeHost(void);

int initializeCL(void);


// 1g kernel
int preRunCLKernels0(float*  x,float*  y, float*  t);
int runCLKernels0(float*  x_in, float*  y_in, float*  t_in);
int postRunCleanup0(void);

// mg kernel
int preRunCLKernels1(float*  x,float*  y, float*  t);
int runCLKernels1(float*  x_in, float*  y_in, float*  t_in);
int postRunCleanup1(void);

// two kernels
int preRunCLKernels2(float*  x,float*  y, float*  t);
int runCLKernels2(float*  x_in, float*  y_in, float*  t_in);
int postRunCleanup2(void);

// two kernels inside
/*
int preRunCLKernels3(float*  x,float*  y, float*  t);
int runCLKernels3(float*  x_in, float*  y_in, float*  t_in);
int postRunCleanup3(void);
*/

int cleanupCL(void);

void cleanupHost(void);

int run(		int algo,int		n_step,	
				int 	load_coords,

				float* 	x_inout,
				float* 	y_inout,
				float* 	t_inout

			);

int run2(	int		n_step,
				int 	load_coords,
				float* 	x_inout,
				float* 	y_inout,
				float* 	t_inout

			);


int postRun(int algo);


};


#endif
