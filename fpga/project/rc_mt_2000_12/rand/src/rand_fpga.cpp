#include "rand_fpga.h"

void ex_gen_uni_num(unsigned long *A, unsigned long  B, unsigned long C);
unsigned long tempering(unsigned long x);



//
// Generates uniformly distributed pseudo random numbers
class fpga_uni_num {

	unsigned long mt3[N_period - M_period];
	unsigned long mt2[2*M_period - N_period];
	unsigned long mt1[N_period - M_period];



	int mti;
	unsigned long prev_val;


public:

	void set_seed (unsigned int);
	float get_uni_num();
};


void fpga_uni_num::set_seed (unsigned int s) {
	
	unsigned long read_reg;
	unsigned long write_reg;
	
	mti=0;
    mt1[0]= s & 0xffffffffUL;
    for (int mtii=1; mtii<N_period; mtii++) {
    	if ((mtii-1) < (N_period - M_period))
    		read_reg = mt1[mtii-1];
    	else if ((mtii-1) < M_period)
    		read_reg = mt2[mtii-1-(N_period-M_period)];
    	else
    		read_reg = mt3[mtii-1-M_period];


    	write_reg = (1812433253UL * (read_reg ^ (read_reg >> 30)) + mtii) & 0xffffffffUL;

       	if (mtii < (N_period - M_period))
       		mt1[mtii] = write_reg;
       	else if (mtii < M_period)
       		mt2[mtii-(N_period-M_period)] = write_reg;
       	else
       		mt3[mtii-M_period] = write_reg;
    }

    prev_val = mt1[0];


}

const unsigned long mag01[2]={0x0UL, MATRIX_A};


float fpga_uni_num::get_uni_num(void) {
#pragma HLS PIPELINE
#pragma HLS RESOURCE variable=mt1 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=mt2 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=mt3 core=RAM_2P_BRAM

//#pragma HLS DEPENDENCE variable=mt WAR inter
#pragma HLS INLINE
    float y;
    unsigned long reg1, reg2, reg3, m1_temp, m2_temp, m3_temp;

    unsigned long	temper, temper1;

    unsigned int i1, i2;
	unsigned int a1, a2, a3, s2, s3;
//do{
    i1=mti;

	if 	(i1!=(N_period-1)) 	i2=i1+1;
	else 					i2=0;

	
	// calc bram read addresses and mux control signals			
	if (i2==0) {
		s2=1; s3=2;
		a1=i2; a2=2*M_period-N_period-1; a3=0;
	}
	else if (i2 < (N_period - M_period)) {
		s2 = 1; s3 = 3;
		a1 = i2; a2 = 0;	a3 = i2-1;
	}else if (i2== (N_period - M_period)){
		s2 = 2; s3 = 3;
		a2 = 0; a1 = 0;	a3 = N_period-M_period-1;
	}
	else if (i2 < M_period) {
		s2 = 2; s3 = 1;
		a1 = i2 - (N_period - M_period)-1; a2 = i2 - (N_period - M_period);	a3 = 0;
	} else if (i2 < (2*(N_period-M_period)+1)) {
		s2 = 3; s3 = 1;
		a1 = i2 - (N_period - M_period)-1; a2 = 0;	a3 = i2 - M_period;
	} else {
		s2 = 3; s3 = 2;
		a1 = 0; a2 = i2 - (2*(N_period-M_period)+1);	a3 = i2 - M_period;
	}


	// read data from bram
	m1_temp = mt1[a1];
	m2_temp = mt2[a2];
	m3_temp = mt3[a3];
	
	// save data to regs
	reg1=prev_val;

	if 		(s2 == 1) 	reg2 = m1_temp;
	else if (s2 == 2) 	reg2 = m2_temp;
	else 				reg2 = m3_temp;

	if 		(s3 == 1) 	reg3 = m1_temp;
	else if (s3 == 2) 	reg3 = m2_temp;
	else      			reg3 = m3_temp;

	// calc new val 
	ex_gen_uni_num(&reg1, reg2, reg3);

	// write process
	if (i1 < (N_period - M_period))
   		mt1[i1] = reg1;
   	else if (i1 < M_period)
   		mt2[i1-(N_period-M_period)] = reg1;
   	else
   		mt3[i1-M_period] = reg1;
 
    prev_val=reg2;

    if (mti == (N_period - 1)) mti = 0;
    else  mti=mti+1;
    // Tempering with moving to [-1;1] range to save resources
 //   temper = tempering(reg1) - 0x80000000;
  //  y=(float)temper*(2.0/4294967296.0);
    temper=tempering(reg1);

//} while (temper==0);

  temper1 = (temper==0) ? 1 : temper;

y=(float) temper1/4294967296.0;

    return y;

}


fpga_uni_num	uni_num[2];

// generate normal pseudo random numbers
#ifdef SYNTH_NOT_COMPARE
int rand_top(int s0, int s1, two_floats *stream_out)
#else
int rand_top(int N, int s0, int s1, float 	*stream_out)
#endif
{
#pragma HLS DATA_PACK variable=stream_out struct_level
#pragma HLS INTERFACE ap_ack port=stream_out

#pragma HLS DEPENDENCE variable=uni_num WAR inter
#pragma HLS ARRAY_PARTITION variable=uni_num complete dim=1

	float r1, r2, s;
	two_floats val_out;
	float logfs;
	float divs;
	float mults, mults1;
	float rho;
	float r12, r22;
	float d0, d1;
	float sinfs, cosfs;
#pragma HLS RESOURCE variable=logfs core=FLog_nodsp
#pragma HLS RESOURCE variable=mults core=FMul_nodsp
#pragma HLS RESOURCE variable=mults1 core=FMul_nodsp
#pragma HLS RESOURCE variable=d0 core=FMul_nodsp
#pragma HLS RESOURCE variable=d1 core=FMul_nodsp
#pragma HLS RESOURCE variable=rho core=FRSqrt_nodsp

	int tm = 0;


	uni_num[0].set_seed(s0);
	uni_num[1].set_seed(s1);


// generate normal numbers
#ifdef SYNTH_NOT_COMPARE
	while(1) {
#else
	n_loop: for(int t=0; t<N; t++) {
#endif

#pragma HLS PIPELINE

		r1 = uni_num[0].get_uni_num();
		r2 = uni_num[1].get_uni_num();


			logfs = logf(r1);
			mults = -2.0f * logfs;
			rho = sqrtf(mults);

			//rho = sqrtf((float)-2.0 * logf(s)/s);

			mults1=2*pii*r2;

			sinfs=sinf(mults1);
			cosfs=cosf(mults1);

			d0 = rho*sinfs;
			d1 = rho*cosfs;

			val_out.d0 = d0;
			val_out.d1 = d1;

#ifdef SYNTH_NOT_COMPARE
			*stream_out=val_out;
#else
			stream_out[2*tm] = val_out.d0;
			stream_out[2*tm + 1] = val_out.d1;
			tm++;
#endif



	}

#ifdef SYNTH_NOT_COMPARE
	return 0;
#else
	return tm;
#endif

}


void ex_gen_uni_num(unsigned long *A, unsigned long  B, unsigned long C) {
#pragma HLS PIPELINE


        unsigned long y;

        y = ((*A) & UPPER_MASK)|((B) & LOWER_MASK);
        (*A) = (C) ^ (y >> 1) ^ mag01[y & 0x1UL];


        return;
}



unsigned long tempering(unsigned long x){
#pragma HLS PIPELINE
        unsigned long y;
        y=x;
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);

        return y;
}

