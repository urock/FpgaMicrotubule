#pragma once

#include <cmath>



#define N_period 624
#define M_period 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

namespace microtubule {


class Random
{     
  public:
    Random(unsigned long s) {
      mti=N_period+1;
      init_genrand(s);
      r1 = 0; r2 = 0; s = 0; rho = 0;
      valid = false;
    };
    ~Random(){};      
    float Normal_dist(void);
    double genrand_real2(void);

  private:
    float r1, r2, s, rho;
    bool valid;
    void init_genrand(unsigned long s);
    unsigned long mt[N_period]; /* the array for the state vector  */
    int mti;             /* mti==N_period+1 means mt[N_period] is not initialized */
};


} // namespace microtubule
