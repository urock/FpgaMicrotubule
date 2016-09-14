#include "mt.h"

using namespace std;
using namespace microtubule;

namespace microtubule {

  // coefficients
extern float viscPF;   
extern float viscPF_teta;   
extern float B_Koeff; 
extern float dt; 
extern float dt_viscPF_teta; 
extern float dt_viscPF; 
extern float sqrt_PF_xy; 
extern float sqrt_PF_teta;   
extern float R_MT; 
extern float A_Koeff; 
extern float b_lat; 
extern float A_long_D; 
extern float b_long_D; 
extern float A_long_T; 
extern float b_long_T; 
extern float ro0; 
extern float ro0_long; 
extern float inv_ro0_long; 
extern float c_lat; 
extern float d_lat; 
extern float C_Koeff; 
extern float Rad; 
extern float inv_ro0; 
extern float clat_dlat_ro0;   
extern float clong_dlong_ro0; 
extern float d_lat_ro0; 
extern float d_long_ro0; 
extern float fi_r; 
extern float psi_r; 
extern float fi_l; 
extern float psi_l; 
extern float rad_mon; 
extern float teta0_D; 
extern float teta0_T; 	


vector<vector<int> > type;//[13][N_d];		


vector<vector<float> >lat_l_x;//[13][N_d+1];
vector<vector<float> > lat_l_y;//[13][N_d+1];
vector<vector<float> > lat_l_t;//[13][N_d+1];

vector<vector<float> > lat_r_x;//[13][N_d+3];
vector<vector<float> > lat_r_y;//[13][N_d+3];
vector<vector<float> > lat_r_t;//[13][N_d+3];

vector<vector<float> > long_u_x;//[13][N_d+1];
vector<vector<float> > long_u_y;//[13][N_d+1];
vector<vector<float> > long_u_t;//[13][N_d+1];

vector<vector<float> > long_d_x;//[13][N_d+1];
vector<vector<float> > long_d_y;//[13][N_d+1];
vector<vector<float> > long_d_t;//[13][N_d+1];


UniRandom *ar_uni[10];

unsigned int N_d_choose = 0;

int is_nan(float val){
 return (val!=val);
}


int get_norm_vals(int index, float *n0, float *n1) {

	float r1, r2;
	float logfs;
	float mults, mults1;
	float rho;
	float d0, d1;
	float sinfs, cosfs;


	r1 = ar_uni[2*index]->get_uni_num();
	r2 = ar_uni[2*index + 1]->get_uni_num();



	logfs = logf(r1);
	mults = -2.0f * logfs;
	rho = sqrtf(mults);

	mults1=2*pi*r2;

	sinfs=sinf(mults1);
	cosfs=cosf(mults1);

	d0 = rho*sinfs;
	d1 = rho*cosfs;

	*n0 = d0;
	*n1 = d1;

	//printf("%f %f\n", d0, d1);

 return (is_nan(d0) || is_nan (d1));

}




void calc_grad_c(		unsigned int i1, 		// i index i?aaie iieaeoeu
						unsigned int j1,			// j index i?aaie iieaeoeu

						unsigned int i2,			// i index eaaie iieaeoeu

						int type, 		// dimer type: 0 - 'D', 1 - 'T'
						unsigned int pos,		// monomer position in dimer: 0 - bottom, 1 - top

						float x_1,		// i?aaay iieaeoea		mol1
						float y_1,
						float teta_1,

						float x_2,		// eaaay iieaeoea		mol2
						float y_2,
						float teta_2,

						float x_3,		// aa?oiyy iieaeoea 	mol3
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


	// oaia?u PE_left - yoi eiaaen i2, a PF_right - eiaaen i1

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



	if ((i1==12)&&(j1>=(N_d_choose-3))) {

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

	// iiiaiye ooo ciae - ana ca?aaioaei!
	float GradU_B_teta_1 = - Grad_tmp;
	float GradU_B_teta_3 = + Grad_tmp;


	if (j1 == (N_d_choose-1)) {

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






int run_step_c(mt_coords_t  &mt_coords, vector<vector<int> > type_mol, bool flag_rand_c)
{
	unsigned int i, j;

	float f_x, f_y, f_t;

	unsigned int pos = 0; 

	// calculate gradients
	for (i=0; i<13; i++){

		pos = 0;

		for (j=0; j<N_d_choose; j++) {


			unsigned int i2 = (i==12)? 0 : (i+1);
			unsigned int j2 = (i==12)? (j+3) : j;

			calc_grad_c(i, j, i2, type_mol[i][j],  pos,

			mt_coords.x[i ][j],  mt_coords.y[i][j], mt_coords.t[i][j],											// mol1 (right) - i?aaay iieaeoea, aey iaa iaiiaeyai neeu aey acaeiiaaenoaey neaaa e aaa?oo

			mt_coords.x[i2][j2], mt_coords.y[i2][j2], mt_coords.t[i2][j2],										// mol2 (left) - iieaeoey neaaa ii ai?eciioaee, aey iaa iaiiaeyai neeo acaeiiaaeonoaey ni?aaa

			mt_coords.x[i][j+1],  mt_coords.y[i][j+1], mt_coords.t[i][j+1],										// mol3 (up)  - iieaeoea naa?oo ii aa?oeeaee, aey iaa iaiiaeyai neeo acaeiiaaenoaey nieco

			&lat_l_x[i ][j], &lat_l_y[i ][j], &lat_l_t[i ][j],
			&lat_r_x[i2][j2], &lat_r_y[i2][j2], &lat_r_t[i2][j2],

			&long_u_x[i][j  ], &long_u_y[i][j  ], &long_u_t[i][j  ],
			&long_d_x[i][j+1], &long_d_y[i][j+1], &long_d_t[i][j+1]    );

			if (pos==1)
			pos = 0;
			else
			pos = 1;

		}


	}



	// update coordinates
	// float norm_val1[3], norm_val2[3], norm_val3[3];
	float temp_val1, temp_val2, temp_val3;
	
	float rand_buf[10];



	for (j=0; j<N_d_choose; j+=3)	{	// coordinates are fixed at j=0
		for (i=0; i<13; i++) {
			if (flag_rand_c){
				for (int ii=0; ii<5; ii++) {		
					if (get_norm_vals(ii, &rand_buf[2*ii], &rand_buf[2*ii+1])!=0) {
					 	printf("NaN error!!!!\n"); 
					 	return -1;
					}
				}
			}
		


			temp_val1=rand_buf[0];
			temp_val2=rand_buf[1];
			temp_val3=rand_buf[2];


			f_x = lat_l_x[i][j] + lat_r_x[i][j] + long_u_x[i][j] + long_d_x[i][j];
			f_y = lat_l_y[i][j] + lat_r_y[i][j] + long_u_y[i][j] + long_d_y[i][j];
			f_t = lat_l_t[i][j] + lat_r_t[i][j] + long_u_t[i][j] + long_d_t[i][j];

		
			if (j > 0) {
				if (flag_rand_c){
					mt_coords.x[i][j] -= dt_viscPF * f_x + sqrt_PF_xy*temp_val1;
					mt_coords.y[i][j] -= dt_viscPF * f_y + sqrt_PF_xy*temp_val2;
					mt_coords.t[i][j] -= dt_viscPF_teta* f_t + sqrt_PF_teta*temp_val3;
				} else{
					mt_coords.x[i][j] -= dt_viscPF * f_x ;
					mt_coords.y[i][j] -= dt_viscPF * f_y;
					mt_coords.t[i][j] -= dt_viscPF_teta* f_t;
					
				}
			}

			temp_val1=rand_buf[3];
			temp_val2=rand_buf[4];
			temp_val3=rand_buf[5];

			f_x = lat_l_x[i][j+1] + lat_r_x[i][j+1] + long_u_x[i][j+1] + long_d_x[i][j+1];
			f_y = lat_l_y[i][j+1] + lat_r_y[i][j+1] + long_u_y[i][j+1] + long_d_y[i][j+1];
			f_t = lat_l_t[i][j+1] + lat_r_t[i][j+1] + long_u_t[i][j+1] + long_d_t[i][j+1];

		 	if (flag_rand_c){
				mt_coords.x[i][j+1] -= dt_viscPF * f_x + sqrt_PF_xy*temp_val1;
				mt_coords.y[i][j+1] -= dt_viscPF * f_y + sqrt_PF_xy*temp_val2;
				mt_coords.t[i][j+1] -= dt_viscPF_teta* f_t + sqrt_PF_teta*temp_val3;
		 	} else {
				mt_coords.x[i][j+1] -= dt_viscPF * f_x ;
				mt_coords.y[i][j+1] -= dt_viscPF * f_y;
				mt_coords.t[i][j+1] -= dt_viscPF_teta* f_t;
		 	}



			temp_val1=rand_buf[6];
			temp_val2=rand_buf[7];
			temp_val3=rand_buf[8];
	
			f_x = lat_l_x[i][j+2] + lat_r_x[i][j+2] + long_u_x[i][j+2] + long_d_x[i][j+2];
			f_y = lat_l_y[i][j+2] + lat_r_y[i][j+2] + long_u_y[i][j+2] + long_d_y[i][j+2];
			f_t = lat_l_t[i][j+2] + lat_r_t[i][j+2] + long_u_t[i][j+2] + long_d_t[i][j+2];

			if (flag_rand_c){
				mt_coords.x[i][j+2] -= dt_viscPF * f_x + sqrt_PF_xy*temp_val1;
				mt_coords.y[i][j+2] -= dt_viscPF * f_y + sqrt_PF_xy*temp_val2;
				mt_coords.t[i][j+2] -= dt_viscPF_teta* f_t + sqrt_PF_teta*temp_val3;
			} else {
				mt_coords.x[i][j+2] -= dt_viscPF * f_x;
				mt_coords.y[i][j+2] -= dt_viscPF * f_y;
				mt_coords.t[i][j+2] -= dt_viscPF_teta* f_t;
			
		  }

		} // i

 	} // j


return 0;
}




int mt::calc_dynamics_cpu(unsigned int n_layers) {

	static bool first_run = true;

	if (first_run) {

		first_run = false; 

		lat_l_x.resize(13);
		lat_l_y.resize(13);
		lat_l_t.resize(13);
		lat_r_x.resize(13);
		lat_r_y.resize(13);
		lat_r_t.resize(13);
		long_u_x.resize(13);
		long_u_y.resize(13);
		long_u_t.resize(13);
		long_d_x.resize(13);
		long_d_y.resize(13);
		long_d_t.resize(13);
		type.resize(13);

		if (brownian_en) {
			for (int jj=0; jj<10; jj++) 
				ar_uni[jj] = new UniRandom(seeds[jj]);
		}
	}

	if (N_d_choose != n_layers) {

		N_d_choose = n_layers;

		for (int p = 0; p < 13; p++) {

			lat_l_x[p].resize(N_d_choose+1);
			lat_l_y[p].resize(N_d_choose+1);
			lat_l_t[p].resize(N_d_choose+1);
			lat_r_x[p].resize(N_d_choose+3);
			lat_r_y[p].resize(N_d_choose+3);
			lat_r_t[p].resize(N_d_choose+3);
			long_u_x[p].resize(N_d_choose+1);
			long_u_y[p].resize(N_d_choose+1);
			long_u_t[p].resize(N_d_choose+1);
			long_d_x[p].resize(N_d_choose+1);
			long_d_y[p].resize(N_d_choose+1);
			long_d_t[p].resize(N_d_choose+1);
			type[p].resize(N_d_choose);
			
		}

	}

	for (int step=1; step <= STEPS_TO_WRITE; step++) {
		if (run_step_c(coords, type_mol, brownian_en)<0) 
			return -1;	
	}

	return 0;	
}





} // microtubule


