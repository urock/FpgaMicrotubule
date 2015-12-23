#include "mt_cpu.h"

void run_step_c(bool flag);

// 32bits Linear Feedback Shift Register
// Generates uniformly distributed pseudo random numbers
class uni_num {
	unsigned int lfsr;
public:
	void set_seed (unsigned int);
	unsigned int get_uni_num();
};

void uni_num::set_seed (unsigned int seed) {
	lfsr = seed;
}

unsigned int uni_num::get_uni_num(void) {

	unsigned lsb = lfsr & 1;
	lfsr >>= 1;

	if (lsb == 1)
		lfsr ^= 0x80400002;

	return lfsr;
}

// create 3 arrays of uniform number generators (Linear Feedback Shift Registers)
// each array generates one normal value
// we need tree normal values for each molecule
uni_num	uni_num1[3][NUM_UNI_VALUES];
uni_num	uni_num2[3][NUM_UNI_VALUES];
uni_num	uni_num3[3][NUM_UNI_VALUES];



// generates 3 normal pseudo ranmod values based on central limit theorem (CLT)
// all uniform number generators (3*NUM_UNI_VALUES) produce new uni values in parallel each cycle
// after we need to sum them up and convert to N(0,1) value
// this can be pipelines and we end up getting 3 norm values each cycle
void generate_norm_numbers_cpu(float *n1, float *n2, float *n3)
{
	unsigned int norm_value1[3];
	unsigned int norm_value2[3];
	unsigned int norm_value3[3];

	unsigned long long tmp1[3]={0};
	unsigned long long tmp2[3]={0};
	unsigned long long tmp3[3]={0};
	unsigned int temp;

	for (int j=0; j < 3; j++) {

	for (int l=0; l < NUM_UNI_VALUES; l++) {
		temp=uni_num1[j][l].get_uni_num();
		tmp1[j] += (unsigned long long)temp;
		temp=uni_num2[j][l].get_uni_num();
		tmp2[j] += (unsigned long long)temp;
		temp=uni_num3[j][l].get_uni_num();
		tmp3[j] += (unsigned long long)temp;
	}

	norm_value1[j] = (unsigned int)(tmp1[j]/NUM_UNI_VALUES);
	n1[j] = sqrt(3.0f*(float)NUM_UNI_VALUES)*(float)((int)norm_value1[j] - (int)VAL2TO31)/VAL2TO31F;

	norm_value2[j] = (unsigned int)(tmp2[j]/NUM_UNI_VALUES);
	n2[j] = sqrt(3.0f*(float)NUM_UNI_VALUES)*(float)((int)norm_value2[j] - (int)VAL2TO31)/VAL2TO31F;

	norm_value3[j] = (unsigned int)(tmp3[j]/NUM_UNI_VALUES);
	n3[j] = sqrt(3.0f*(float)NUM_UNI_VALUES)*(float)((int)norm_value3[j] - (int)VAL2TO31)/VAL2TO31F;

	}


}


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

							);



							
// тут первый индекс определяет протофиламент (по горизонтали)
// а второй - номер молекулы внутри этого протофиламента (по вертикали)

bit type[13][N_d];		// тип молекул - пока все молекулы типа D

// тут размеры > N_d чтобы не вылетать за границы массива при расчете лонгитудальных (N_d + 1) взаимодействий верхнего ряда
// и латеральных справа (N_d + 3) для последних трех верхних рядов
float x[13][N_d+3];
float y[13][N_d+3];
float t[13][N_d+3];


// латеральные взаимодействия - по горизонтали (слева и справа)
// слева
float lat_l_x[13][N_d+1];
float lat_l_y[13][N_d+1];
float lat_l_t[13][N_d+1];
// справа
float lat_r_x[13][N_d+3];
float lat_r_y[13][N_d+3];
float lat_r_t[13][N_d+3];

// лонгитудальные взаимодействия - по вертикали (вверх и вниз)
// вверх
float long_u_x[13][N_d+1];
float long_u_y[13][N_d+1];
float long_u_t[13][N_d+1];
//вниз
float long_d_x[13][N_d+1];
float long_d_y[13][N_d+1];
float long_d_t[13][N_d+1];



// функиция вычисляющая координаты в течение заданного количества шагов по времени - n_step
// первый раз должна вызываться с параметром load_coords = 1, чтобы проинициализировать массивы координат
// последующие вызовы должны быть с load_coords = 0, чтобы сохранять координаты
void mt_cpu(	bool flag,
				int		n_step,				// полное количество шагов по времени
				int 	load_coords,		//

				int seeds[],

				float 	x_in[][N_d],		// входные массивы координат, используются при load_coords = 1
				float 	y_in[][N_d],
				float 	t_in[][N_d],

				float 	x_out[][N_d],		// выходные массивы координат
				float 	y_out[][N_d],
				float 	t_out[][N_d]
			)

{



	int i,j;

	if (load_coords) {

		for (j=0; j<N_d; j++)
			for (i=0; i<13; i++) {
				type[i][j] = 0;		// все димеры типа D


				x[i][j] = x_in[i][j];
				y[i][j] = y_in[i][j];
				t[i][j] = t_in[i][j];

			}

	}
	int temp1, temp2;

	seed_loop_1:for(int l=0; l<3; l++){
	  seed_loop_2: for( int j = 0; j<3; j++ ){
		seed_loop_3:for(int k =0; k< NUM_UNI_VALUES/2; k++){
			 temp1=(seeds[(l * 3 + j) *NUM_UNI_VALUES + 2*k]);
			 temp2= (seeds[(l * 3 + j) *NUM_UNI_VALUES + 2*k+1]);
			switch (l) {
			case 0 :
			uni_num1[j][2*k].set_seed( temp1);
			uni_num1[j][2*k+1].set_seed( temp2);
			break;
			case 1 :
			uni_num2[j][2*k].set_seed( temp1);
			uni_num2[j][2*k+1].set_seed( temp2);
			break;
			case 2 :
			uni_num3[j][2*k].set_seed( temp1);
			uni_num3[j][2*k+1].set_seed( temp2);
			break;

			}


		}
	}
}


	for (int step=1; step <= n_step; step++)
		run_step_c(flag);




	for (i=0; i<13; i++)
		for (j=0; j<N_d; j++) 	{
			x_out[i][j] = x[i][j];
			y_out[i][j] = y[i][j];
			t_out[i][j] = t[i][j];
		}

}





void run_step_c(bool flag)
{
#pragma HLS DATAFLOW

	int i, j;

	float f_x, f_y, f_t;

	// параметр, определяющий считаем ли лонгитудальное (по вертикали) взаимодействие между молекулами (мономерами) 
	// внутри одной гантели (димера) (pos = 0), или между мономерами из разных димеров (pos = 1)
	bit pos = 0; 

// calculate gradients
	l1: for (i=0; i<13; i++){

		pos = 0;

		l2: for (j=0; j<N_d; j++) {
#pragma HLS PIPELINE

			// выбор соседа слева для латерального взаимодействия 
			// всего профиламентов (ПФ) 13 штук: от 0 до 12
			// нижний ряд молекул из протофиламентов образует не кольцо, а спираль. y координата 12 ПФ для одного ряда всегда больше y координаты 0 ПФ
			// спираль закручивается налево вверх
			// поэтому мономер 12 ПФ слева взаимодействует с мономером не из своего ряда, а из другого - со сдвигом на 3 ряда.
			// т.е. молекула (0,12) слева взаимодействует с (3,0); (1,12) - (4,0) и тд
			int i2 = (i==12)? 0 : (i+1);
			int j2 = (i==12)? (j+3) : j;

			calc_grad_c(i, j, i2, type[i][j],  pos,

							 x[i ][j],  y[i][j], t[i][j],											// mol1 (right) - правая молекула, для нее обновляем силы для взаимодействия слева и вверху

							 x[i2][j2], y[i2][j2], t[i2][j2],										// mol2 (left) - молекуля слева по горизонтали, для нее обновляем силу взаимодейтствия справа

							 x[i][j+1],  y[i][j+1], t[i][j+1],										// mol3 (up)  - молекула сверху по вертикали, для нее обновляем силу взаимодействия снизу

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



	l3: for (j=0; j<N_d; j+=3)		// coordinates are fixed at j=0
		l4: for (i=0; i<13; i++) {
#pragma HLS PIPELINE




			f_x = lat_l_x[i][j] + lat_r_x[i][j] + long_u_x[i][j] + long_d_x[i][j];
			f_y = lat_l_y[i][j] + lat_r_y[i][j] + long_u_y[i][j] + long_d_y[i][j];
			f_t = lat_l_t[i][j] + lat_r_t[i][j] + long_u_t[i][j] + long_d_t[i][j];


			if (j>0){
			x[i][j] -= dt_viscPF * f_x;
			y[i][j] -= dt_viscPF * f_y;
			t[i][j] -= dt_viscPF_teta* f_t;

				}



			f_x = lat_l_x[i][j+1] + lat_r_x[i][j+1] + long_u_x[i][j+1] + long_d_x[i][j+1];
			f_y = lat_l_y[i][j+1] + lat_r_y[i][j+1] + long_u_y[i][j+1] + long_d_y[i][j+1];
			f_t = lat_l_t[i][j+1] + lat_r_t[i][j+1] + long_u_t[i][j+1] + long_d_t[i][j+1];


			x[i][j+1] -= dt_viscPF * f_x;
			y[i][j+1] -= dt_viscPF * f_y;
			t[i][j+1] -= dt_viscPF_teta* f_t;





			f_x = lat_l_x[i][j+2] + lat_r_x[i][j+2] + long_u_x[i][j+2] + long_d_x[i][j+2];
			f_y = lat_l_y[i][j+2] + lat_r_y[i][j+2] + long_u_y[i][j+2] + long_d_y[i][j+2];
			f_t = lat_l_t[i][j+2] + lat_r_t[i][j+2] + long_u_t[i][j+2] + long_d_t[i][j+2];


			x[i][j+2] -= dt_viscPF * f_x;
			y[i][j+2] -= dt_viscPF * f_y;
			t[i][j+2] -= dt_viscPF_teta* f_t;


if(flag){
	float norm_val1[3], norm_val2[3], norm_val3[3];
	float temp_val1, temp_val2, temp_val3;

	generate_norm_numbers_cpu(norm_val1, norm_val2, norm_val3);
	temp_val1=norm_val1[0];
	temp_val2=norm_val1[1];
	temp_val3=norm_val1[2];


	if (j>0){
	x[i][j] -=sqrt_PF_xy*temp_val1;
	y[i][j] -=sqrt_PF_xy*temp_val2;
	t[i][j] -=sqrt_PF_teta*temp_val3;
		}

	temp_val1=  norm_val2[0];
	temp_val2=  norm_val2[1];
	temp_val3=  norm_val2[2];

	x[i][j+1] -=sqrt_PF_xy*temp_val1;
	y[i][j+1] -=sqrt_PF_xy*temp_val2;
	t[i][j+1] -=sqrt_PF_teta*temp_val3;


	temp_val1=  norm_val3[0];
	temp_val2=  norm_val3[1];
	temp_val3=  norm_val3[2];

	x[i][j+2] -=sqrt_PF_xy*temp_val1;
	y[i][j+2] -=sqrt_PF_xy*temp_val2;
	t[i][j+2] -=sqrt_PF_teta*temp_val3;

}



			}



}




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
#pragma HLS EXPRESSION_BALANCE
#pragma HLS PIPELINE


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







