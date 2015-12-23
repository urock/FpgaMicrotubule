

#include "rand_fpga.h"
#include <stdlib.h>
#include <time.h>

#define	HIST_SIZE	2000
#define MARGIN 0.0001f
#define NUM_STEPS			100000
#define RAND_BUF_SIZE		(2*NUM_STEPS)

#define RAND_BUF_SIZE_F		(float)RAND_BUF_SIZE



#define dx		0.01


void print_coords(float * array,int  n_step, bool val);




int main() {



	int i;
	 
	float *rand_buf_cpu = (float*)malloc(RAND_BUF_SIZE*4);
	float *rand_buf_fpga = (float*)malloc(RAND_BUF_SIZE*4);


	srand (time(NULL));

	int s0 = rand();
	int s1 = rand();


	unsigned int tmp_val;
	
	printf("Start Gen random numbers\n");

	printf("RAND_BUF_SIZE = %d\n",RAND_BUF_SIZE);



  	int t_cpu = rand_cpu(NUM_STEPS, s0, s1 , rand_buf_cpu);


  	int t_fpga = rand_top(NUM_STEPS, s0, s1 ,rand_buf_fpga);

  	printf("CPU generated %d numbers out of %d max size\n",2*t_cpu, RAND_BUF_SIZE);
  	printf("FPGA generated %d numbers out of %d max size\n",2*t_fpga, RAND_BUF_SIZE);

  	if (t_cpu != t_fpga) {
  		printf("Error!!\n");
  		return -2;
  	}



	print_coords(rand_buf_fpga, 2*t_fpga,1);
	print_coords(rand_buf_cpu, 2*t_cpu,0);


	for(i=0; i<2*t_cpu; i++) {
	if ((rand_buf_cpu[i]!=rand_buf_cpu[i])|(rand_buf_fpga[i]!=rand_buf_fpga[i])|(fabs(rand_buf_cpu[i] - rand_buf_fpga[i]) > MARGIN)){
			printf("Error comparing results!\n");
			free(rand_buf_cpu);
			free(rand_buf_fpga);
			return -1;
		}

	}


	printf("Congratulations! Results from FPGA and CPU match!\n");

	free(rand_buf_cpu);
	free(rand_buf_fpga);
	return 0;
}





void print_coords(float * array,int  n_step, bool val){
	FILE *f_valx, *f_valy, * f_valt;
	f_valx = (val) ? fopen ("rand_fpga.txt","w") : fopen ("rand_cpu.txt","w");

	for(int t=0; t<n_step; t++)
		fprintf(f_valx,"%f \n",array[t]);




	fclose(f_valx);

}
