#include "rand.h"
#include <unistd.h>

namespace microtubule {

// float Random :: Normal_dist(void)
// {
//    if (!valid)
//    {
//       do {
//       r1 = 2.0*genrand_real2() - 1;
//       r2 = 2.0*genrand_real2() - 1;
//       s = r1 * r1 + r2 * r2;
//       } 
//       while (s > 1);
//       rho = sqrtf((float)-2.0 * logf(s)/s);
//       valid = true;
//    } else {
//    valid = false;
//    }  
//    return rho * (valid ? r1 : r2); //return rho * (valid ? r1 : r2) * m_sigma + m_mean;
// };
// double Random :: genrand_real2(void)
// {
//     unsigned long y;
//     static unsigned long mag01[2]={0x0UL, MATRIX_A};
//     /* mag01[x] = x * MATRIX_A  for x=0,1 */

//     if (mti >= N_period) { /* generate N_period words at one time */
//         int kk;
//         if (mti == N_period+1)   /* if init_genrand() has not been called, */
//             init_genrand(5489UL); /* a default initial seed is used */

//         for (kk=0;kk<N_period-M_period;kk++) {
//             y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
//             mt[kk] = mt[kk+M_period] ^ (y >> 1) ^ mag01[y & 0x1UL];
//         }
//         for (;kk<N_period-1;kk++) {
//             y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
//             mt[kk] = mt[kk+(M_period-N_period)] ^ (y >> 1) ^ mag01[y & 0x1UL];
//         }
//         y = (mt[N_period-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
//         mt[N_period-1] = mt[M_period-1] ^ (y >> 1) ^ mag01[y & 0x1UL];
//         mti = 0;
//     }  
//     y = mt[mti++];
//     /* Tempering */
//     y ^= (y >> 11);
//     y ^= (y << 7) & 0x9d2c5680UL;
//     y ^= (y << 15) & 0xefc60000UL;
//     y ^= (y >> 18);
//     return y*(1.0/4294967296.0); 
//    //return y*2.328306436538696e-10;
// };
// void Random :: init_genrand(unsigned long s)
// {
//     mt[0]= s & 0xffffffffUL;
//     for (mti=1; mti<N_period; mti++) {
//         mt[mti] = 
//        (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
//         mt[mti] &= 0xffffffffUL;
//         /* for >32 bit machines */
//     }
// };



void UniRandom::init_genrand(unsigned int seed)
{
    mt[0]= seed & 0xffffffffUL;
    for (mti=1; mti<N_period; mti++) {
        mt[mti] =
      (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }


};

float UniRandom::get_uni_num()
{
    unsigned int y;
    static unsigned int mag01[2]={0x0UL, MATRIX_A};
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

  y = (y==0) ? 1 : y;

   return y*(1.0/4294967296.0);
};


float UniRandom::Normal_dist(void)
{
   if (!valid)
   {
      do {
      r1 = 2.0*get_uni_num() - 1;
      r2 = 2.0*get_uni_num() - 1;
      s = r1 * r1 + r2 * r2;
      } 
      while (s > 1);
      rho = sqrtf((float)-2.0 * logf(s)/s);
      valid = true;
   } else {
   valid = false;
   }  
   return rho * (valid ? r1 : r2); //return rho * (valid ? r1 : r2) * m_sigma + m_mean;
};

} // namespace microtubule
