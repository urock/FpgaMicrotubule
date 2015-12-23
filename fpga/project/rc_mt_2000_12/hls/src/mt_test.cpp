
#include "mt_cpu.h"
#include "mt_fpga.h"
#include <stdlib.h>
#include <time.h>

int equal(float A, float B);
void calculate_constants();
void init_coords(float x[][N_d], float y[][N_d], float t[][N_d]);
int compare_results(float x_1[][N_d], float y_1[][N_d], float t_1[][N_d],
						float x_2[][N_d], float y_2[][N_d], float t_2[][N_d] );

void print_coords(FILE *f_p, float x[][N_d], float y[][N_d], float t[][N_d]);

float x_1[13][N_d];
float y_1[13][N_d];
float t_1[13][N_d];

float x_2[13][N_d];
float y_2[13][N_d];
float t_2[13][N_d];
#define NUM_SEEDS 10
int seeds[NUM_SEEDS];
#define FLAG_RND 1



#define TOTAL_STEPS             5000                 // полное количество шагов по времени
#define STEPS_TO_WRITE          100            // через это значение шагов сравниваем координаты и выводим в файл


#define absTol		0.01f
#define relTol		0.01f

int main() {

	//calculate_constants();


	 FILE *f_u, *f_p, *f_p1, *f_valx, *f_valy, *f_valt;
	 int i,j;

	 int error = 0;


	 f_p = fopen ("MT_coords_CPU.txt","w");
	 if (f_p==NULL) {

		 printf("Error opening file!\n");
		 return -1;
	 }
	 f_p1 = fopen ("MT_coords_FPGA.txt","w");
	 if (f_p1==NULL) {

		 printf("Error opening file!\n");
		 return -1;
	 }


	printf("Hello3\n");






	// get golden results
	init_coords(x_1,y_1,t_1);
	init_coords(x_2,y_2,t_2);

	//mt_cpu(STEPS_TO_WRITE,1,x_2,y_2,t_2,x_2,y_2, t_2);
	//print_coords(f_p, x_2, y_2, t_2);


	/*
	 * в этом цикле проводим вычисления и сравниваем результаты
	 *
	 * в данный момент чтобы проверить сравнение на каждом шаге вызывается функция mt_cpu
	 * с параметром load_coords = 1 (иначе состояния глобальных массивов координат будет все время меняться)
	 *
	 * Когда вместо mt_cpu будет реализация на OpenCL, то надо делать по-другому:
	 *  перед циклом один раз вызываем mt_cpu (load_coords = 1), в цикле уменьшаем количество итераций на 1 и
	 *  вызываем mt_cpu (load_coords = 0)
	 *
	 */

	srand (time(NULL));

	for(int k=0; k<(TOTAL_STEPS/STEPS_TO_WRITE); k++) {

		for (int i =0; i< NUM_SEEDS; i++){
			seeds[i]=i;//rand();
		//printf("%d\n", seeds[i]);
		}


		mt_cpu(FLAG_RND,STEPS_TO_WRITE,1,seeds,x_1,y_1,t_1,x_1,y_1, t_1);


		mt_fpga(FLAG_RND,STEPS_TO_WRITE,1,seeds,x_2,y_2,t_2,x_2,y_2, t_2);


		error += compare_results(x_1,y_1,t_1,x_2,y_2,t_2);
		if (error)
			printf("Compare results failed at step = %d, errors = %d\n", k, error);


		print_coords(f_p, x_2, y_2, t_2);
		print_coords(f_p1, x_1, y_1, t_1);

	}

	if (!error)
		printf("Test OK!\n");

	fclose(f_p);
	fclose(f_p1);


	return 0;
}


int mt_fpga(	bool flag,
				int		n_step,				// полное количество шагов по времени
				int 	load_coords,		//

				int seeds[],

				float 	x_in[][N_d],
				float 	y_in[][N_d],
				float 	t_in[][N_d],

				float 	x_out[][N_d],
				float 	y_out[][N_d],
				float 	t_out[][N_d]
			)
{
	
	two_floats buf[(3*13*N_d/4)*4+4+NUM_SEEDS];
	
	two_floats	tmp; 
	
	two_floats w0, w1;
	
	int i,j;
	
	int k = 0; // ddr buffer index
		
	// на каждую молекулу отводим 16 байт в памЯти - это 2 64-битных слова
	for (i=0; i<13; i++)
		for (j=0; j<N_d; j++) {
			tmp.d0 = x_in[i][j];
			tmp.d1 = y_in[i][j];
			buf[k++] = tmp;
			tmp.d0 = t_in[i][j];
			tmp.d1 = 0;			
			buf[k++] = tmp; 
		}

	/*seed_loop_1:for(int l=0; l<3; l++){
	  seed_loop_2: for( int j = 0; j<3; j++ ){
		seed_loop_3:for(int m =0; m< NUM_UNI_VALUES/2; m++){
		tmp.d0=(float)(seeds[(l * 3 + j)*NUM_UNI_VALUES + 2*m]);
		tmp.d1=(float)(seeds[(l * 3 + j)*NUM_UNI_VALUES + 2*m+1]);
		buf[k++]=tmp;
		}
	  }
	}*/

float *fseeds=(float *)seeds;

	for (int l=0; l<NUM_SEEDS; l+=2){
		tmp.d0=fseeds[l];
		tmp.d1=fseeds[l+1];
		buf[k++]=tmp;
	}
		
	// run hardware function
	mt_top(0,n_step,(int)flag,0,0,0, buf);
	
	
	k = 0;
	for (i=0; i<13; i++)
		for (j=0; j<N_d; j++) {
			
			w0 = buf[k++];
			w1 = buf[k++]; 
			
			x_out[i][j] = w0.d0; 
			y_out[i][j] = w0.d1; 
			t_out[i][j] = w1.d0; 
		}
	
	return 0; 
}
			



void print_coords(FILE *f_p, float x[][N_d], float y[][N_d], float t[][N_d])
{
	int i,j;

	for (i=0; i<13; i++) {
		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.2f\t  ", x[i][j]);

		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.2f\t  ", y[i][j]);

		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.2f\t  ", t[i][j]);
	}

	fprintf(f_p,"\n");

}




void init_coords(float x[][N_d], float y[][N_d], float t[][N_d])
{
	int i,j;


	// задание y координаты для нижней спирали
	for (i=0; i<13; i++)
		y[i][0] = 2.0f*6/13*(i+1);


	// задание y координат для остальных молекул до половины высоты трубочки
	for (j=1; j<N_d-4; j++)
		for (i=0; i<13; i++)
			y[i][j] = y[i][j-1] + 2.0f*Rad;


	// задание x и teta координат так чтобы был цилиндр до половины высоты трубочки
	for (j=0; j<N_d-5; j++)
		for (i=0; i<13; i++)  {

			x[i][j] = 0.0;
			t[i][j] = 0.0;
		}


	//
	for (i=0; i<13; i++)  {

		x[i][N_d-5] = 0.6;
		t[i][N_d-5] = 0.2;

	}


	for (j=N_d-4; j<N_d; j++)
		for (i=0; i<13; i++)  {

			x[i][j] = x[i][j-1] + 2*Rad*sinf(t[i][j-1]);
			y[i][j] = y[i][j-1] + 2*Rad*cosf(t[i][j-1]);
			t[i][j] = t[i][j-1];

		}



}


#define DEBUG_PRINT_COORDS

int compare_results(float x_1[][N_d], float y_1[][N_d], float t_1[][N_d],
						float x_2[][N_d], float y_2[][N_d], float t_2[][N_d] )
{

	int error = 0;
	for(int i = 0; i<13; i++)
		for(int j = 0; j<N_d; j++) {

			if ((x_1[i][j]!=x_1[i][j])||(x_2[i][j]!=x_2[i][j])||(!equal(x_1[i][j], x_2[i][j])))
				error++;
			if ((y_1[i][j]!=y_1[i][j])||(y_2[i][j]!=y_2[i][j])||(!equal(y_1[i][j], y_2[i][j])))
				error++;
			if ((t_1[i][j]!=t_1[i][j])||(t_2[i][j]!=t_2[i][j])||(!equal(t_1[i][j], t_2[i][j])))
				error++;

		}


	return error;
}



float max(float A, float B){
if(A>=B) return A;
else return B;
}

int equal(float A, float B)
{
	 return (abs(A - B) <= max(absTol, relTol * max(abs(A), abs(B))));

}





// функция, которая использовалась для вычисления массивов констант в mt_defines.h
void calculate_constants()
{

	int num_PF, i;
	float gamma = 0;

	float Ax_1[13];
	float Ax_2[13];
	float Ax_3[13];
	float A_Bx_4[13];
	float Ay_1[13];
	float Ay_2[13];
	float Ay_3[13];

	float A_By_4[13];
	float Az_1, Az_2;

	float Bx_1[13];
	float Bx_2[13];
	float Bx_3[13];
	float By_1[13];
	float By_2[13];
	float By_3[13];
	float Bz_1, Bz_2;


	float C_1_r = rad_mon*sin(psi_r)*cos(fi_r);
	float C_2_r = rad_mon*sin(psi_r)*sin(fi_r);
	float C_3_r = rad_mon*cos(psi_r);


	float C_1_L = rad_mon*sin(psi_l)*cos(fi_l);
	float C_2_L = rad_mon*sin(psi_l)*sin(fi_l);
	float C_3_L = rad_mon*cos(psi_l);


	Az_1 = C_1_r;
	Az_2 = C_3_r;

	Bz_1 = C_1_L;
	Bz_2 = C_3_L;


	for (num_PF=1; num_PF<=13; num_PF++) {
		if( num_PF == 1) gamma = (2*pi/13*(num_PF-23));
		else if ( num_PF == 2)  gamma = (2*pi/13*(num_PF-25));
		else if ( num_PF == 3)  gamma = (2*pi/13*(num_PF-1));
		else if ( num_PF == 4)  gamma = (2*pi/13*(num_PF-3));
		else if ( num_PF == 5)  gamma = (2*pi/13*(num_PF-5));
		else if ( num_PF == 6)  gamma = (2*pi/13*(num_PF-7));
		else if ( num_PF == 7)  gamma = (2*pi/13*(num_PF-9));
		else if ( num_PF == 8)  gamma = (2*pi/13*(num_PF-11));
		else if ( num_PF == 9)  gamma = (2*pi/13*(num_PF-13));
		else if ( num_PF == 10) gamma = (2*pi/13*(num_PF-15));
		else if ( num_PF == 11) gamma = (2*pi/13*(num_PF-17));
		else if ( num_PF == 12) gamma = (2*pi/13*(num_PF-19));
		else if ( num_PF == 13) gamma = (2*pi/13*(num_PF-21));


		i = num_PF - 1;

		Ax_1[i] = C_1_r*cos(gamma);
		Ax_2[i] = C_2_r*sin(gamma);
		Ax_3[i] = C_3_r*cos(gamma);
		A_Bx_4[i] = sin(2*pi/13*(num_PF-1));

		Ay_1[i] = C_1_r*sin(gamma);
		Ay_2[i] = C_3_r*sin(gamma);
		Ay_3[i] = C_2_r*cos(gamma);
		A_By_4[i] = cos(2*pi/13*(num_PF-1));

		Bx_1[i] = C_1_L*cos(gamma);
		Bx_2[i] = C_2_L*sin(gamma);
		Bx_3[i] = C_3_L*cos(gamma);

		By_1[i] = C_1_L*sin(gamma);
		By_2[i] = C_3_L*sin(gamma);
		By_3[i] = C_2_L*cos(gamma);


	}


	printf("done!\n");






}
