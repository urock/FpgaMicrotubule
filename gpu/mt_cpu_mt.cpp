#include "mt_cpu_mt.h"


#include <iostream>       
#include <atomic> 
#include <thread>         
#include <vector> 
#include <stdlib.h>

//#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define CHECK_FATAL(s,msg)                                    \
  if(s){                                                  \
    fprintf(stderr,                                       \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    fflush(stderr);                                       \
    abort();                                              \
  }

namespace mt{



bit type[13][N_d];
float x[13][N_d+3];
float y[13][N_d+3];
float t[13][N_d+3];
float lat_l_x[13][N_d+1];
float lat_l_y[13][N_d+1];
float lat_l_t[13][N_d+1];
float lat_r_x[13][N_d+3];
float lat_r_y[13][N_d+3];
float lat_r_t[13][N_d+3];
float long_u_x[13][N_d+1];
float long_u_y[13][N_d+1];
float long_u_t[13][N_d+1];
float long_d_x[13][N_d+1];
float long_d_y[13][N_d+1];
float long_d_t[13][N_d+1];




void calc_grad_c(		int i1, 		// i index правой молекулы
					int j1,			// j index правой молекулы

					int i2,			// i index левой молекулы

					bit type, 		// dimer type: 0 - 'D', 1 - 'T'
					bit pos,		// monomer position in dimer: 0 - bottom, 1 - top

					float x_1,		// правая молекула		mol1
					float y_1,
					float teta_1,

					float x_2,		// левая молекула		mol2
					float y_2,
					float teta_2,

					float x_3,		// верхняя молекула 	mol3
					float y_3,
					float teta_3,

					float *grad_lat_x_1,			// left component of mol1
					float *grad_lat_y_1,
					float *grad_lat_teta_1,

					float *grad_lat_x_2,			// right component of mol2
					float *grad_lat_y_2,
					float *grad_lat_teta_2,

					float *grad_long_x_1,			// up component of mol1
					float *grad_long_y_1,
					float *grad_long_teta_1,

					float *grad_long_x_3,			// down component of mol3
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



	//	[nd] 	- 	mol3
	//	[nd-1] 	- 	mol1


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

	if (pos==0) {		// bottom monomer (interaction inside dimer)
		dUdr_C = C_Koeff*r_long;
	} else {			// top monomer (interaction with upper dimer)

		float tmp1 = r_long *  expf(-r_long*inv_ro0_long)*(2 - r_long*inv_ro0_long);
		float tmp2	= r_long * clong_dlong_ro0 * expf(-(r_long*r_long) * d_long_ro0 );

		if (type==0)	// dimer type 'D'
			dUdr_C = (tmp1*b_long_D + tmp2) * A_long_D;
		else 			// dimer type 'T'
			dUdr_C = (tmp1*b_long_T + tmp2) * A_long_T;
	}



	float Grad_tmp_x = drdx_long * dUdr_C;
	float Grad_tmp_y = drdy_long * dUdr_C;

	float GradU_C_teta_1 = -dUdr_C*( drdx_long*(-Rad*cos_t_1) + drdy_long*(Rad*sin_t_1));
	float GradU_C_teta_3 =  dUdr_C*(-drdx_long*(-Rad*cos_t_3) - drdy_long*(Rad*sin_t_3));

	float Grad_tmp;
	if (type==0)		// dimer type 'D'
		Grad_tmp = B_Koeff*(teta_3 - teta_1 - teta0_D);
	else				// dimer type 'T'
		Grad_tmp = B_Koeff*(teta_3 - teta_1 - teta0_T);

	// поменял тут знак - все заработало!
	float GradU_B_teta_1 = - Grad_tmp;
	float GradU_B_teta_3 = + Grad_tmp;


	if (j1 == (N_d-1)) {

		*grad_long_x_1 		= 0.0f;
		*grad_long_y_1 		= 0.0f;
		*grad_long_teta_1	= 0.0f;

		*grad_long_x_3 		= 0.0f;
		*grad_long_y_3 		= 0.0f;
		*grad_long_teta_3	= 0.0f;

	} else {

		*grad_long_x_1 		= Grad_tmp_x;
		*grad_long_y_1 		= Grad_tmp_y;
		*grad_long_teta_1	= GradU_C_teta_1 + GradU_B_teta_1;

		*grad_long_x_3 		= - Grad_tmp_x;
		*grad_long_y_3 		= - Grad_tmp_y;
		*grad_long_teta_3	= GradU_C_teta_3 + GradU_B_teta_3;


	}


}




void run_step_c(int step)
{

	int i, j;

	float f_x, f_y, f_t;
	bit pos = 0; 
	for (i=0; i<13; i++){
		for (j=0; j<N_d; j++) {

			int i2 = (i==12)? 0 : (i+1);
			int j2 = (i==12)? (j+3) : j;
                        pos =(j % 2);

			calc_grad_c(i, j, i2, type[i][j],  pos,

							 x[i ][j],  y[i][j], t[i][j],
							 x[i2][j2], y[i2][j2], t[i2][j2],
							 x[i][j+1],  y[i][j+1], t[i][j+1],

							 &lat_l_x[i ][j], &lat_l_y[i ][j], &lat_l_t[i ][j],
							 &lat_r_x[i2][j2], &lat_r_y[i2][j2], &lat_r_t[i2][j2],

							 &long_u_x[i][j  ], &long_u_y[i][j  ], &long_u_t[i][j  ],
							 &long_d_x[i][j+1], &long_d_y[i][j+1], &long_d_t[i][j+1]);

		}


	}



	for (j=1; j<N_d; j++)
		for (i=0; i<13; i++) {

			f_x = lat_l_x[i][j] + lat_r_x[i][j] + long_u_x[i][j] + long_d_x[i][j];
			f_y = lat_l_y[i][j] + lat_r_y[i][j] + long_u_y[i][j] + long_d_y[i][j];
			f_t = lat_l_t[i][j] + lat_r_t[i][j] + long_u_t[i][j] + long_d_t[i][j];


			x[i][j] -= dt_viscPF * f_x;
			y[i][j] -= dt_viscPF * f_y;
			t[i][j] -= dt_viscPF_teta * f_t;
		}



}




class SpinningBarrier
{
public:
  SpinningBarrier (unsigned int n) : n_ (n), nwait_ (0), step_(0) {}

  bool Wait () {
    unsigned int step = step_.load();
    if (nwait_.fetch_add(1) == n_ - 1) {
      /* OK, last thread to come.  */
      nwait_.store(0); // XXX: maybe can use relaxed ordering here ??
      step_.fetch_add(1);
      return true;
    } else {
      /* Run in circles and scream like a little girl. */
      while (step_.load() == step){
       std::this_thread::yield();
       //timespec sleepValue = {0};
       //sleepValue.tv_nsec = 1;
       //nanosleep(&sleepValue, NULL);
      }
          
      return false;
    }
  }

protected:
  /* Number of synchronized threads. */
  const unsigned int n_;

  /* Number of threads currently spinning.  */
  std::atomic<unsigned int> nwait_;

  /* Number of barrier syncronizations completed so far, 
   ** it's OK to wrap.  */
  std::atomic<unsigned int> step_;
};


SpinningBarrier* sBarrier;

namespace JSB{

  std::atomic<unsigned int> nwait_;
  std::atomic<unsigned int> step_;

bool just_spinning_barrier(int tid, int gn)
{
    unsigned int step = step_.load();
    if (nwait_.fetch_add(1) == gn - 1) {
      nwait_.store(0);
      step_.fetch_add(1);
      return true;
    } else {
      while (step_.load() == step){
       std::this_thread::yield();
      }
          
      return false;
    }

}

} // ns

#define TH_MAX 32


int gszx,gszy,lszx,lszy;
int gnx,gny,gn;
int iterNum=1;

struct ginfo_t{int id0;int id1;};
//pthread_t ths[TH_MAX];
std::thread* ths[TH_MAX];
ginfo_t gifs[TH_MAX];

std::atomic<int> thread_run[TH_MAX];
std::atomic<int> thread_kill(0);
//int thread_run[TH_MAX];
//int thread_counter=0;

/*
//std::atomic<int> sync_counter(0);
std::atomic<int> step_run[TH_MAX];


void global_synch(int tid,int s)
{
 int i,done;
// s++;
 step_run[tid].store(s);
 printf("%d tid%d:%d,%d\n",s,tid,step_run[0].load(),step_run[1].load());
 while(1){
         done=1;
         for (i=0; i < gn; i++)if(i!=tid){
          if(step_run[i].load()!=s){done=0;break;};
         }
         if(done){
          break;
         }
//         std::this_thread::yield();
//         usleep(1);
 }
 printf("_%d tid%d:%d,%d\n",s,tid,step_run[0].load(),step_run[1].load());
}
*/



void run_step_mt(int tid,int tid0,int tid1, int n_steps)
{

	int i, j;

        int i0=tid0*lszx;
        int in=(tid0*lszx+lszx);

   
        int j0=(tid1*lszy);
        int jn=(tid1*lszy+lszy);

//        printf("run_step_mt %d..%d\n",j0,jn);

	float f_x, f_y, f_t;
	bit pos = 0;
        int sc=1; 

for(int s=0;s<n_steps;s++){


	for (i=i0; i<in; i++) if(i<13){
		for (j=j0; j<jn; j++) if(j<N_d){

			int i2 = (i==12)? 0 : (i+1);
			int j2 = (i==12)? (j+3) : j;
                        pos =(j % 2);

			calc_grad_c(i, j, i2, type[i][j],  pos,

							 x[i ][j],  y[i][j], t[i][j],
							 x[i2][j2], y[i2][j2], t[i2][j2],
							 x[i][j+1],  y[i][j+1], t[i][j+1],

							 &lat_l_x[i ][j], &lat_l_y[i ][j], &lat_l_t[i ][j],
							 &lat_r_x[i2][j2], &lat_r_y[i2][j2], &lat_r_t[i2][j2],

							 &long_u_x[i][j  ], &long_u_y[i][j  ], &long_u_t[i][j  ],
							 &long_d_x[i][j+1], &long_d_y[i][j+1], &long_d_t[i][j+1]);

		}


	}


// global synch
//if(gn>1) sBarrier->Wait();//global_synch(tid,2*s);
//if(gn>1) global_synch(tid,sc++);
if(gn>1)JSB::just_spinning_barrier(tid,gn);

	for (j=j0; j<jn; j++) if(j>0) if (j<N_d){
		for (i=i0; i<in; i++) if(i<13){

			f_x = lat_l_x[i][j] + lat_r_x[i][j] + long_u_x[i][j] + long_d_x[i][j];
			f_y = lat_l_y[i][j] + lat_r_y[i][j] + long_u_y[i][j] + long_d_y[i][j];
			f_t = lat_l_t[i][j] + lat_r_t[i][j] + long_u_t[i][j] + long_d_t[i][j];


			x[i][j] -= dt_viscPF * f_x;
			y[i][j] -= dt_viscPF * f_y;
			t[i][j] -= dt_viscPF_teta * f_t;
		}
        } 

//if(gn>1) sBarrier->Wait();//global_synch(tid,2*s+1);
//if(gn>1) global_synch(tid,sc++);
if(gn>1)JSB::just_spinning_barrier(tid,gn);

}// for s

}

void processing_thread(ginfo_t* gi)
{
  int id0=gi->id0;int id1=gi->id1;
  int id=id0*gny+id1;
  printf("thread tid %d %d,%d\n",id,id0,id1);
  while(thread_kill.load() == 0){
   if(thread_run[id].load()==1){
    //printf("thread_run %d, tid %d\n",got,gi->id);
     run_step_mt(id,id0,id1,iterNum);
    //run_step_c(0);
    thread_run[id].store(0);
   }
   //usleep(1);
 }
  printf("processing_thread %d exiting...\n",id);
}

void mt_cpu_mt_init(int _gszx,int _gszy,int _lszx,int _lszy)
{

int i,j,s;
printf("mt_cpu_mt_init, hw concurency %d\n",std::thread::hardware_concurrency());
gszx=_gszx;gszy=_gszy;
lszx=_lszx;lszy=_lszy;
gnx=gszx/lszx;gny=gszy/lszy;
gn=gnx*gny;
printf("mt_cpu_mt_init, gsz(%d,%d) lsz(%d,%d) gn(%d,%d)=%d\n",gszx,gszy,lszx,lszy,gnx,gny,gn);

sBarrier=new SpinningBarrier(gn);
JSB::nwait_=0;
JSB::step_=0;
for(i=0;i<gn;i++){
  thread_run[i].store(0);
//  step_run[i].store(0);
}
for(i=0;i<gnx;i++)
 for(j=0;j<gny;j++){
  gifs[i*gny+j].id0=i;
  gifs[i*gny+j].id1=j;
  ths[i*gny+j]=new std::thread(processing_thread,&(gifs[i*gny+j]));
  CHECK_FATAL(ths[i*gny+j]==NULL,"std::thread failed.");
} // for


}

void mt_cpu_mt_finit()
{
  thread_kill.store(0);
}



void mt_cpu_mt(	int		n_step,	
				int 	load_coords,

				float 	x_in[][N_d],
				float 	y_in[][N_d],
				float 	t_in[][N_d],

				float 	x_out[][N_d],
				float 	y_out[][N_d],
				float 	t_out[][N_d]
			)

{
	int i,j;

	if (load_coords) {

		for (j=0; j<N_d; j++)
			for (i=0; i<13; i++) {
				type[i][j] = 0;
				x[i][j] = x_in[i][j];
				y[i][j] = y_in[i][j];
				t[i][j] = t_in[i][j];
			}
	}
/*
	for (int step=0; step < n_step; step++){
		run_step_c(step);
	}

*/
	//printf("mt_cpu_mt gn=%d, n_step=%d\n",gn,n_step);
        iterNum=n_step;

        // run signal
        for (i=0; i < gn; i++){
         thread_run[i].store(1);
        }

        // wait
        while(1){
         int done=1;for (i=0; i < gn; i++){if(thread_run[i].load()!=0){done=0;break;};}
         if(done)break;
         //usleep(1);
        }

	for (i=0; i<13; i++)
		for (j=0; j<N_d; j++) 	{
			x_out[i][j] = x[i][j];
			y_out[i][j] = y[i][j];
			t_out[i][j] = t[i][j];
		}
}




} // ns mt
