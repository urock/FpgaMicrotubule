#include "mt_cpu.h"

extern  unsigned int N_d;

//#define DYN_RAND
// нету разницы между динамическим и статическим объявлением объектов uni_num

FILE * f_r;
vector<vector<bit> > type;//[13][N_d];		

vector<vector<float> > x;//[13][N_d+3];
vector<vector<float> > y;//[13][N_d+3];
vector<vector<float> > t;//[13][N_d+3];

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


int run_step_c(int flag_rand_c);

unsigned int N_d_choose = 0;


class uni_num {




public:
//	uni_num(unsigned long seed) {
//		mti=N_period+1;
//		init_genrand(seed);
//		valid = false;
//	};
	float get_uni_num(void);
	void init_genrand(unsigned int seed);


	unsigned int mt[N_period]; /* the array for the state vector  */
	int mti;

};





void uni_num::init_genrand(unsigned int seed)
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



uni_num ar_uni[10];


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



	r1 = ar_uni[2*index].get_uni_num();
	r2 = ar_uni[2*index + 1].get_uni_num();



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

	//printf("%f %f\n", d0, d1);

 return (is_nan(d0) || is_nan (d1));

}




void calc_grad_c(		unsigned int i1, 		// i index i?aaie iieaeoeu
						unsigned int j1,			// j index i?aaie iieaeoeu

						unsigned int i2,			// i index eaaie iieaeoeu

						bit type, 		// dimer type: 0 - 'D', 1 - 'T'
						bit pos,		// monomer position in dimer: 0 - bottom, 1 - top

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

);




// ooo ia?aue eiaaen ii?aaaeyao i?ioioeeaiaio (ii ai?eciioaee)
// a aoi?ie - iiia? iieaeoeu aioo?e yoiai i?ioioeeaiaioa (ii aa?oeeaee)




// ooieeoey au?eney?uay eii?aeiaou a oa?aiea caaaiiiai eiee?anoaa oaaia ii a?aiaie - n_step
// ia?aue ?ac aie?ia aucuaaouny n ia?aiao?ii load_coords = 1, ?oiau i?ieieoeaeece?iaaou ianneau eii?aeiao
// iineaao?uea auciau aie?iu auou n load_coords = 0, ?oiau nio?aiyou eii?aeiaou
int mt_cpu(	int		n_step,				// iieiia eiee?anoai oaaia ii a?aiaie
			int 	load_coords,		//
			int		flag_rand_c,
			int		flag_seed_c,

			unsigned int seeds[],

			vector<vector<float> >  & x_in,		// aoiaiua ianneau eii?aeiao, eniieuco?ony i?e load_coords = 1
			vector<vector<float> >  & y_in,
			vector<vector<float> >  & t_in,

			vector<vector<float> >  & x_out,		// auoiaiua ianneau eii?aeiao
			vector<vector<float> >  & y_out,
			vector<vector<float> >  & t_out,
			unsigned int N_d_chooseInput
)

{
N_d_choose = N_d_chooseInput;
	
printf("load_coords %d flag_rand %d flag_seed %d\n", load_coords, flag_rand_c, flag_seed_c);

f_r=fopen("rand_coords.txt","a");
	

	static int allocate = 0;

	if (allocate==0){
		allocate=1;

		
		x.resize(13);
		y.resize(13);
		t.resize(13);
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
		for (int p = 0; p < 13; p++){
			x[p].resize(N_d_choose+3);
			y[p].resize(N_d_choose+3);
			t[p].resize(N_d_choose+3);
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
	
	if ((flag_seed_c == 1) && (flag_rand_c==1)) {

		for (int jj=0; jj<10; jj++)
			ar_uni[jj].init_genrand(seeds[jj]);
	}	
	// for (int p=0; p<10;p++)
	// printf("start gen %d %d\n",p,ar_uni[p].mti);
	
	// for (int p =0; p<10; p++)
		// printf("seed is %d\n", seeds[p]);	


	

	unsigned int i,j;

	if (load_coords) {

		for (j=0; j<N_d_choose; j++)
		for (i=0; i<13; i++) {
			type[i][j] = 0;		// ana aeia?u oeia D


			x[i][j] = x_in[i][j];
			y[i][j] = y_in[i][j];
			t[i][j] = t_in[i][j];

		}

	}
	



	for (int step=1; step <= n_step; step++){
		if (run_step_c(flag_rand_c)<0) 
			return -1;	
	}




	for (i=0; i<13; i++)
	for (j=0; j<N_d_choose; j++) 	{
		x_out[i][j] = x[i][j];
		y_out[i][j] = y[i][j];
		t_out[i][j] = t[i][j];
	}
	
	

	// for (int p=0; p<10;p++)
	// printf("end gen %d %d\n",p,ar_uni[p].mti);
fclose(f_r);	
return 0;	
}





int run_step_c(int flag_rand_c)
{
	unsigned int i, j;

	float f_x, f_y, f_t;

	// ia?aiao?, ii?aaaey?uee n?eoaai ee eiiaeooaaeuiia (ii aa?oeeaee) acaeiiaaenoaea ia?ao iieaeoeaie (iiiiia?aie) 
	// aioo?e iaiie aaioaee (aeia?a) (pos = 0), eee ia?ao iiiiia?aie ec ?aciuo aeia?ia (pos = 1)
	bit pos = 0; 

	// calculate gradients
	for (i=0; i<13; i++){

		pos = 0;

		for (j=0; j<N_d_choose; j++) {


			// auai? ninaaa neaaa aey eaoa?aeuiiai acaeiiaaenoaey 
			// anaai i?ioeeaiaioia (IO) 13 oooe: io 0 ai 12
			// ie?iee ?ya iieaeoe ec i?ioioeeaiaioia ia?acoao ia eieuoi, a nie?aeu. y eii?aeiaoa 12 IO aey iaiiai ?yaa anaaaa aieuoa y eii?aeiaou 0 IO
			// nie?aeu cae?o?eaaaony iaeaai aaa?o
			// iiyoiio iiiiia? 12 IO neaaa acaeiiaaenoaoao n iiiiia?ii ia ec naiaai ?yaa, a ec a?oaiai - ni naaeaii ia 3 ?yaa.
			// o.a. iieaeoea (0,12) neaaa acaeiiaaenoaoao n (3,0); (1,12) - (4,0) e oa
			unsigned int i2 = (i==12)? 0 : (i+1);
			unsigned int j2 = (i==12)? (j+3) : j;

			calc_grad_c(i, j, i2, type[i][j],  pos,

			x[i ][j],  y[i][j], t[i][j],											// mol1 (right) - i?aaay iieaeoea, aey iaa iaiiaeyai neeu aey acaeiiaaenoaey neaaa e aaa?oo

			x[i2][j2], y[i2][j2], t[i2][j2],										// mol2 (left) - iieaeoey neaaa ii ai?eciioaee, aey iaa iaiiaeyai neeo acaeiiaaeonoaey ni?aaa

			x[i][j+1],  y[i][j+1], t[i][j+1],										// mol3 (up)  - iieaeoea naa?oo ii aa?oeeaee, aey iaa iaiiaeyai neeo acaeiiaaenoaey nieco

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



	for (j=0; j<N_d_choose; j+=3)		// coordinates are fixed at j=0
	for (i=0; i<13; i++) {
	if (flag_rand_c==1){
	for (int ii=0; ii<5; ii++) {		
			 if (get_norm_vals(ii, &rand_buf[2*ii], &rand_buf[2*ii+1])!=0) {printf("NaN error!!!!\n"); return -1;}
		}
		}
		
	for (int p=0; p<10; p++)	
		//fprintf(f_r,"%.3f\n",rand_buf[p]);	
		
		


			 temp_val1=rand_buf[0];
			 temp_val2=rand_buf[1];
			 temp_val3=rand_buf[2];


		f_x = lat_l_x[i][j] + lat_r_x[i][j] + long_u_x[i][j] + long_d_x[i][j];
		f_y = lat_l_y[i][j] + lat_r_y[i][j] + long_u_y[i][j] + long_d_y[i][j];
		f_t = lat_l_t[i][j] + lat_r_t[i][j] + long_u_t[i][j] + long_d_t[i][j];

		
		if (j>0){
			 if (flag_rand_c==1){
				 x[i][j] -= dt_viscPF * f_x + sqrt_PF_xy*temp_val1;
				 y[i][j] -= dt_viscPF * f_y + sqrt_PF_xy*temp_val2;
				 t[i][j] -= dt_viscPF_teta* f_t + sqrt_PF_teta*temp_val3;
			 } else{
				x[i][j] -= dt_viscPF * f_x ;
				y[i][j] -= dt_viscPF * f_y;
				t[i][j] -= dt_viscPF_teta* f_t;
				
			 }
		}

			 temp_val1=rand_buf[3];
			 temp_val2=rand_buf[4];
			 temp_val3=rand_buf[5];

		f_x = lat_l_x[i][j+1] + lat_r_x[i][j+1] + long_u_x[i][j+1] + long_d_x[i][j+1];
		f_y = lat_l_y[i][j+1] + lat_r_y[i][j+1] + long_u_y[i][j+1] + long_d_y[i][j+1];
		f_t = lat_l_t[i][j+1] + lat_r_t[i][j+1] + long_u_t[i][j+1] + long_d_t[i][j+1];

		 if (flag_rand_c==1){
			 x[i][j+1] -= dt_viscPF * f_x + sqrt_PF_xy*temp_val1;
			 y[i][j+1] -= dt_viscPF * f_y + sqrt_PF_xy*temp_val2;
			 t[i][j+1] -= dt_viscPF_teta* f_t + sqrt_PF_teta*temp_val3;
		 }
		 else{
			x[i][j+1] -= dt_viscPF * f_x ;
			y[i][j+1] -= dt_viscPF * f_y;
			t[i][j+1] -= dt_viscPF_teta* f_t;
			
		 }



			 temp_val1=rand_buf[6];
			 temp_val2=rand_buf[7];
			 temp_val3=rand_buf[8];

		f_x = lat_l_x[i][j+2] + lat_r_x[i][j+2] + long_u_x[i][j+2] + long_d_x[i][j+2];
		f_y = lat_l_y[i][j+2] + lat_r_y[i][j+2] + long_u_y[i][j+2] + long_d_y[i][j+2];
		f_t = lat_l_t[i][j+2] + lat_r_t[i][j+2] + long_u_t[i][j+2] + long_d_t[i][j+2];

		if (flag_rand_c==1){
			x[i][j+2] -= dt_viscPF * f_x + sqrt_PF_xy*temp_val1;
			y[i][j+2] -= dt_viscPF * f_y + sqrt_PF_xy*temp_val2;
			t[i][j+2] -= dt_viscPF_teta* f_t + sqrt_PF_teta*temp_val3;
		}
		else{
			x[i][j+2] -= dt_viscPF * f_x;
			y[i][j+2] -= dt_viscPF * f_y;
			t[i][j+2] -= dt_viscPF_teta* f_t;
			
		 }






	}


return 0;
}




void calc_grad_c(		unsigned int i1, 		// i index i?aaie iieaeoeu
						unsigned int j1,			// j index i?aaie iieaeoeu

						unsigned int i2,			// i index eaaie iieaeoeu

						bit type, 		// dimer type: 0 - 'D', 1 - 'T'
						bit pos,		// monomer position in dimer: 0 - bottom, 1 - top

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







