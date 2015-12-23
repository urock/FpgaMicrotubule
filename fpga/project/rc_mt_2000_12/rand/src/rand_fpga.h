#ifndef RAND_FPGA_H
#define RAND_FPGA_H


#include <cmath>
#include <stdio.h>
#include <string.h>
#include "hls_stream.h"

#define SYNTH_NOT_COMPARE	// defines synth flow, stream interface, otherwise compare flow with pointer interface
#define pii 3.14159265359f

/* Period parameters for randomiser*/
#define N_period 624
#define M_period 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

#define X 454


typedef struct {
	float d0;
	float d1;
} two_floats;


int rand_cpu(int N, int s0, int s1, float *buf_out);

#ifdef SYNTH_NOT_COMPARE
	int rand_top(int s0, int s1, hls::stream<two_floats> 	&stream_out);
#else
	int rand_top(int N, int s0, int s1, float 	*stream_out);
#endif

#endif //RAND_FPGA_H
