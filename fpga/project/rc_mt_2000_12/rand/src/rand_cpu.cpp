#include "rand_fpga.h"



// 32bits Linear Feedback Shift Register
// Generates uniformly distributed pseudo random numbers
class uni_num {

	bool valid;
	unsigned long mt[N_period]; /* the array for the state vector  */
	int mti;

public:
	uni_num(unsigned long seed) {
		mti=N_period+1;
		init_genrand(seed);
		valid = false;
	};
	float get_uni_num(void);
	void init_genrand(unsigned long seed);




};





void uni_num::init_genrand(unsigned long seed)
{
    mt[0]= seed & 0xffffffffUL;
    for (mti=1; mti<N_period; mti++) {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }


};

float uni_num::get_uni_num()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N_period) { /* generate N_period words at one time */
        int kk;


        for (kk=0;kk<N_period-M_period;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M_period] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N_period-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M_period-N_period)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N_period-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N_period-1] = mt[M_period-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti];
    mti=mti+1;


    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y*(1.0/4294967296.0);
};


// generate normal pseudo random numbers
int rand_cpu(int N, int s0, int s1, float *buf_out)
{

	uni_num * ar_uni[2];
	ar_uni[0]=new uni_num(s0);
	ar_uni[1]=new uni_num(s1);

	float r1, r2, s, rho;
	two_floats val_out;

	int t, tm=0;

	for(t=0; t<N; t++) {


		r1 = 2*ar_uni[0]->get_uni_num() - 1;
		r2 = 2*ar_uni[1]->get_uni_num() - 1;
		s = r1 * r1 + r2 * r2;

		if (s < 1.0f) {

			rho = sqrtf((float)-2.0 * logf(s)/s);

			val_out.d0 = rho*r1;
			val_out.d1 = rho*r2;


			buf_out[2*tm] = val_out.d0;
			buf_out[2*tm + 1] = val_out.d1;
			tm++;

		}




	}

	return tm;

}
