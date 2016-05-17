#ifdef _DEBUG
   #undef _DEBUG
   #include <omp.h>
   #define _DEBUG
#else
   #include <omp.h>
#endif


#define RunOpenMP

#include "mt_cpu.h"
#include <assert.h>
#include <algorithm>
#include <iostream>

class uni_num {

public:

   unsigned long mt[N_period]; /* the array for the state vector  */
   int mti;

   void init_genrand(unsigned long seed);

   float get_uni_num(void);

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

   y = (y==0) ? 1 : y;

   return y*(1.0/4294967296.0);
};


uni_num ar_uni[4*N_threads];



int is_nan(float val){
 return (val!=val);
}

int inline get_norm_vals(int index, float *n0, float *n1) {


   float r1, r2;
   float logfs;
   float mults, mults1;
   float rho;
   float d0, d1;
   float sinfs, cosfs;

   int tid = omp_get_thread_num();

   r1 = ar_uni[tid*(2*index)].get_uni_num();
   r2 = ar_uni[tid*(2*index + 1)].get_uni_num();

   logfs = logf(r1);
   mults = -2.0f * logfs;
   rho = sqrtf(mults);

   mults1=2*pii*r2;

   sinfs=sinf(mults1);
   cosfs=cosf(mults1);

   d0 = rho*sinfs;
   d1 = rho*cosfs;

   *n0 = d0;
   *n1 = d1;

   return (is_nan(d0) || is_nan (d1));
}


void run_step_c();



inline void calc_grad_c(   int i1,     // i index правой молекулы
                           int j1,        // j index правой молекулы

                           int i2,        // i index левой молекулы

                           bit type,      // dimer type: 0 - 'D', 1 - 'T'
                           bit pos,    // monomer position in dimer: 0 - bottom, 1 - top

                           float x_1,     // правая молекула      mol1
                           float y_1,
                           float teta_1,

                           float x_2,     // левая молекула    mol2
                           float y_2,
                           float teta_2,

                           float x_3,     // верхняя молекула  mol3
                           float y_3,
                           float teta_3,

                           float *grad_lat_x_1,       // left component of mol1
                           float *grad_lat_y_1,
                           float *grad_lat_teta_1,

                           float *grad_lat_x_2,       // right component of mol2
                           float *grad_lat_y_2,
                           float *grad_lat_teta_2,

                           float *grad_long_x_1,         // up component of mol1
                           float *grad_long_y_1,
                           float *grad_long_teta_1,

                           float *grad_long_x_3,         // down component of mol3
                           float *grad_long_y_3,
                           float *grad_long_teta_3

   );






void mt_cpu(   int      n_step,
               int      load_coords,

               float    x_in[][N_d],      // входные массивы координат, используются при load_coords = 1
               float    y_in[][N_d],
               float    t_in[][N_d],

               float    x_out[][N_d],
               float    y_out[][N_d],
               float    t_out[][N_d],

               int      flag_seed_c,

               int      seeds[],
               bool     ompflag

         )

{

   bit type[13][N_d];

   float ** x = new float * [13];
   float ** y = new float * [13];
   float ** t = new float *[13];
   float ** lat_l_x = new float *[13];
   float ** lat_l_y = new float * [13];
   float ** lat_l_t = new float * [13];
   float ** lat_r_x = new float * [13];
   float ** lat_r_y = new float * [13];
   float ** lat_r_t = new float * [13];
   float ** long_u_x = new float * [13];
   float ** long_u_y = new float * [13];
   float ** long_u_t = new float * [13];
   float ** long_d_t = new float * [13];




   for (int i =0; i < 13; i++) {
      x[i] = new  float [N_d+3];
      y[i] = new  float [N_d+3];
      t[i] = new  float [N_d+3];
      lat_l_x[i] = new  float [N_d+3];
      lat_l_y[i] = new  float [N_d+3];
      lat_l_t[i] = new  float [N_d+3];
      lat_r_x[i] = new  float [N_d+3];
      lat_r_y[i] = new  float [N_d+3];
      lat_r_t[i] = new  float [N_d+3];
      long_u_x[i] = new  float [N_d+3];
      long_u_y[i] = new  float [N_d+3];
      long_u_t[i] = new  float [N_d+3];
      long_d_t[i] = new  float [N_d+3];
   }


   int i,j;



   if ((flag_seed_c == 1)) {

      for (int jj=0; jj<4*N_threads; jj++)
         ar_uni[jj].init_genrand(seeds[jj]);
   }



   if (load_coords) {
      for (j=0; j<N_d; j++) {
         for (i=0; i<13; i++) {
            type[i][j] = 0;
            x[i][j] = x_in[i][j];
            y[i][j] = y_in[i][j];
            t[i][j] = t_in[i][j];
         }
      }
   }




   double t1= 0, t2 = 0, delta_t = 0;
   t1=omp_get_wtime();
   float f_x=0, f_y=0, f_t=0;
   int thr_num = 0;

#ifdef RunOpenMP

   omp_set_nested(1);
   omp_set_dynamic(0);
   omp_set_num_threads(N_threads);

   #pragma omp parallel default(shared) private(f_x, f_y, f_t)  num_threads(N_threads) if (ompflag)//for  schedule(static, 3) //default(none) firstprivate() shared() shared (x, lat_l_x, lat_l_y, lat_l_t, lat_r_x, lat_r_y, lat_r_t, long_u_x, long_u_y, long_u_t, long_d_x, long_d_y, long_d_t)
   {
#endif


      //////****Main parallel cycle********///////
      for (int step=1; step <= n_step; step++) {

         #pragma omp  for  schedule(static)
         //collapse(2)
         //lastprivate(lat_l_x,lat_l_y,lat_l_t) firstprivate(x,y,t)
         //#pragma omp simd

         for (int i=0; i<13; i++) {
            for (int j=0; j<N_d; j++) {

               bit pos = j%2;
               int i2 = (i==12)? 0 : (i+1);
               int j2 = (i==12)? (j+3) : j;



               calc_grad_c(i, j, i2, type[i][j],  pos,

                           x[i ][j],  y[i][j], t[i][j],                                // mol1 (right) - правая молекула, для нее обновляем силы для взаимодействия слева и вверху

                           x[i2][j2], y[i2][j2], t[i2][j2],                            // mol2 (left) - молекуля слева по горизонтали, для нее обновляем силу взаимодейтствия справа

                           x[i][j+1],  y[i][j+1], t[i][j+1],                              // mol3 (up)  - молекула сверху по вертикали, для нее обновляем силу взаимодействия снизу

                           &lat_l_x[i ][j], &lat_l_y[i ][j], &lat_l_t[i ][j],
                           &lat_r_x[i2][j2], &lat_r_y[i2][j2], &lat_r_t[i2][j2],
                           &long_u_x[i][j  ], &long_u_y[i][j  ], &long_u_t[i][j  ],
                           NULL, NULL, &long_d_t[i][j+1]    );

              } // j
           } // i



         #pragma omp  for  schedule(static)
         //collapse(2)
         //firstprivate(lat_l_x,lat_l_y,lat_l_t)
         //#pragma omp simd

         for (int i=0; i<13; i++) {
            for (int j=1; j<N_d; j++) {

               float rand_buf[4];

               if (get_norm_vals(0, &rand_buf[0], &rand_buf[1])!=0) {printf("NaN error!!!!\n"); return -1;}
               if (get_norm_vals(1, &rand_buf[2], &rand_buf[3])!=0) {printf("NaN error!!!!\n"); return -1;}


               float temp1 = - long_u_x[i][j-1];
               float temp2 = - long_u_y[i][j-1];

               f_x = lat_l_x[i][j] + lat_r_x[i][j] + long_u_x[i][j] + temp1;
               f_y = lat_l_y[i][j] + lat_r_y[i][j] + long_u_y[i][j] + temp2;
               f_t = lat_l_t[i][j] + lat_r_t[i][j] + long_u_t[i][j] + long_d_t[i][j];


               x[i][j] -= dt_viscPF * f_x          +  sqrt_PF_xy*rand_buf[0];///rand1*sqrt_PF_xy;
               y[i][j] -= dt_viscPF * f_y          + sqrt_PF_xy*rand_buf[1];///rand2*sqrt_PF_xy;
               t[i][j] -= dt_viscPF_teta * f_t  + sqrt_PF_teta*rand_buf[2];///rand3*sqrt_PF_teta;

            }
         }

      } // n_step

#ifdef RunOpenMP
   }
#endif

   t2=omp_get_wtime();
   delta_t = t2 - t1;
   printf("Elapsed time = %f\n", delta_t);


   for (int i=0; i<13; i++)
      for (int j=0; j<N_d; j++)  {
         x_out[i][j] =  x[i][j];
         y_out[i][j] =  y[i][j];
         t_out[i][j] =  t[i][j];
      }


}








inline  void calc_grad_c(     int i1,     // i index правой молекулы
               int j1,        // j index правой молекулы

               int i2,        // i index левой молекулы

               bit type,      // dimer type: 0 - 'D', 1 - 'T'
               bit pos,    // monomer position in dimer: 0 - bottom, 1 - top

               float x_1,     // правая молекула      mol1
               float y_1,
               float teta_1,

               float x_2,     // левая молекула    mol2
               float y_2,
               float teta_2,

               float x_3,     // верхняя молекула  mol3
               float y_3,
               float teta_3,

               float *grad_lat_x_1,       // left component of mol1
               float *grad_lat_y_1,
               float *grad_lat_teta_1,

               float *grad_lat_x_2,       // right component of mol2
               float *grad_lat_y_2,
               float *grad_lat_teta_2,

               float *grad_long_x_1,         // up component of mol1
               float *grad_long_y_1,
               float *grad_long_teta_1,

               float *grad_long_x_3,         // down component of mol3
               float *grad_long_y_3,
               float *grad_long_teta_3

                     )


{



   // теперь PE_left - это индекс i2, а PF_right - индекс i1

   float cos_t_A = cosf(teta_2);
   float sin_t_A = sinf(teta_2);
   float cos_t_B = cosf(teta_1);
   float sin_t_B = sinf(teta_1);


   float cos_t_1 = cos_t_B;
   float sin_t_1 = sin_t_B;

   float cos_t_3 = cosf(teta_3);
   float sin_t_3 = sinf(teta_3);

   // swap i1 <=> i2


   float Ax_left = Ax_1[i2]*cos_t_A + Ax_3[i2]*sin_t_A - Ax_2[i2] +
            (x_2 + R_MT) * A_Bx_4[i2];

   float Ay_left = Ay_1[i2]*cos_t_A + Ay_2[i2]*sin_t_A + Ay_3[i2] +
            (x_2 + R_MT) * A_By_4[i2];

   float Az_left = -Az_1*sin_t_A + Az_2*cos_t_A + y_2;


   float Bx_right = Bx_1[i1]*cos_t_B + Bx_3[i1]*sin_t_B - Bx_2[i1] +
            (x_1 + R_MT) * A_Bx_4[i1];

   float By_right = By_1[i1]*cos_t_B + By_2[i1]*sin_t_B + By_3[i1] +
            (x_1 + R_MT) * A_By_4[i1];

   float Bz_right = -Bz_1*sin_t_B + Bz_2*cos_t_B + y_1;

   float Dx = Ax_left - Bx_right;
   float Dy = Ay_left - By_right;
   float Dz = Az_left - Bz_right;


   float dist = sqrtf(( pow(Dx, 2) + pow(Dy, 2) + pow(Dz, 2) ));


   if (dist <=1e-7 ){
      dist = 1e-5;
   }

   float inv_dist = 1/dist;

   float drdAx = Dx * inv_dist;
   float drdAy = Dy * inv_dist;
   float drdAz = Dz * inv_dist;
   float drdBx = -drdAx;
   float drdBy = -drdAy;
   float drdBz = -drdAz;

   float dA_X_dteta = -sin_t_A*Ax_1[i2] + cos_t_A*Ax_3[i2];
   float dA_Y_dteta = -sin_t_A*Ay_1[i2] + cos_t_A*Ay_2[i2];
   float dA_Z_dteta = -cos_t_A*Az_1 - sin_t_A*Az_2;

   float drdx_A = drdAx*A_Bx_4[i2] + drdAy*A_By_4[i2];
   float drdy_A = drdAz;
   float drdteta_A = drdAx*dA_X_dteta + drdAy*dA_Y_dteta + drdAz*dA_Z_dteta;

   //================================================
   float dB_X_dteta = -sin_t_B*Bx_1[i1] + cos_t_B*Bx_3[i1];
   float dB_Y_dteta = -sin_t_B*By_1[i1] + cos_t_B*By_2[i1];
   float dB_Z_dteta = -cos_t_B*Bz_1 - sin_t_B*Bz_2;

   float drdx_B = drdBx*A_Bx_4[i1] + drdBy*A_By_4[i1];
   float drdy_B = drdBz;
   float drdteta_B = drdBx*dB_X_dteta + drdBy*dB_Y_dteta + drdBz*dB_Z_dteta;


   float Grad_U_tmp = (b_lat* dist *expf(-dist*inv_ro0)*(2.0f - dist*inv_ro0) +
            dist* clat_dlat_ro0 * expf( - (dist*dist) * d_lat_ro0 )  ) * A_Koeff;



   if ((i1==12)&&(j1>=(N_d-3))) {

      *grad_lat_x_2 = 0.0f;
      *grad_lat_y_2 = 0.0f;
      *grad_lat_teta_2 = 0.0f;

      *grad_lat_x_1 = 0.0f;
      *grad_lat_y_1 = 0.0f;
      *grad_lat_teta_1 = 0.0f;

   } else {

      *grad_lat_x_2 = Grad_U_tmp * drdx_A;
      *grad_lat_y_2 = Grad_U_tmp * drdy_A;
      *grad_lat_teta_2 = Grad_U_tmp * drdteta_A;

      *grad_lat_x_1 = Grad_U_tmp * drdx_B;
      *grad_lat_y_1 = Grad_U_tmp * drdy_B;
      *grad_lat_teta_1 = Grad_U_tmp * drdteta_B;

   }



   // [nd]  -  mol3
   // [nd-1]   -  mol1


   // longitudinal gradient

   float r_long_x = (x_3 - x_1) - Rad*(sin_t_1 + sin_t_3);
   float r_long_y = (y_3 - y_1) - Rad*(cos_t_1 + cos_t_3);
   float r_long = sqrtf( r_long_x*r_long_x + r_long_y*r_long_y);

   if (r_long <=1e-15 ){
      r_long = 1e-7;
   }

   float drdx_long = - r_long_x/r_long;
   float drdy_long = - r_long_y/r_long;

   float dUdr_C;

   if (pos==0) {     // bottom monomer (interaction inside dimer)
      dUdr_C = C_Koeff*r_long;
   } else {       // top monomer (interaction with upper dimer)

      float tmp1 = r_long *  expf(-r_long*inv_ro0_long)*(2 - r_long*inv_ro0_long);
      float tmp2  = r_long * clong_dlong_ro0 * expf(-(r_long*r_long) * d_long_ro0 );

      if (type==0)   // dimer type 'D'
         dUdr_C = (tmp1*b_long_D + tmp2) * A_long_D;
      else        // dimer type 'T'
         dUdr_C = (tmp1*b_long_T + tmp2) * A_long_T;
   }



   float Grad_tmp_x = drdx_long * dUdr_C;
   float Grad_tmp_y = drdy_long * dUdr_C;

   float GradU_C_teta_1 = -dUdr_C*( drdx_long*(-Rad*cos_t_1) + drdy_long*(Rad*sin_t_1));
   float GradU_C_teta_3 =  dUdr_C*(-drdx_long*(-Rad*cos_t_3) - drdy_long*(Rad*sin_t_3));

   float Grad_tmp;
   if (type==0)      // dimer type 'D'
      Grad_tmp = B_Koeff*(teta_3 - teta_1 - teta0_D);
   else           // dimer type 'T'
      Grad_tmp = B_Koeff*(teta_3 - teta_1 - teta0_T);

   // поменял тут знак - все заработало!
   float GradU_B_teta_1 = - Grad_tmp;
   float GradU_B_teta_3 = + Grad_tmp;


   if (j1 == (N_d-1)) {

      *grad_long_x_1       = 0.0f;
      *grad_long_y_1       = 0.0f;
      *grad_long_teta_1 = 0.0f;

      //*grad_long_x_3     = 0.0f;
      //*grad_long_y_3     = 0.0f;
      *grad_long_teta_3 = 0.0f;

   } else {

      *grad_long_x_1       = Grad_tmp_x;
      *grad_long_y_1       = Grad_tmp_y;
      *grad_long_teta_1 = GradU_C_teta_1 + GradU_B_teta_1;

      //*grad_long_x_3     = - Grad_tmp_x;
      //*grad_long_y_3     = - Grad_tmp_y;
      *grad_long_teta_3 = GradU_C_teta_3 + GradU_B_teta_3;


   }


}







