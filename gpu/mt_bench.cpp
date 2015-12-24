#include "mt_cpu.h"
//#include "mt_cpu_mt.h"
//#include "mt_fpga.h"
#include "mt_ocl.h"
#include <time.h>
#include <unistd.h>

#include "Timer.h"

void calculate_constants();
void init_coords(float x[][N_d], float y[][N_d], float t[][N_d]);
void rnd_coords(float x[][N_d], float y[][N_d], float t[][N_d]);
void cpy_coords(float x[][N_d], float y[][N_d], float t[][N_d],float x2[][N_d], float y2[][N_d], float t2[][N_d]);
void cpy_coords2(float x[][N_d], float y[][N_d], float t[][N_d],float x2[][N_d+3], float y2[][N_d+3], float t2[][N_d+3]);
void init_coords2(float x[][N_d], float y[][N_d], float t[][N_d]);
int compare_results(float x_1[][N_d], float y_1[][N_d], float t_1[][N_d],
						float x_2[][N_d], float y_2[][N_d], float t_2[][N_d] );
int compare_results2(float x_1[][N_d],   float y_1[][N_d],   float t_1[][N_d],
		     float x_2[][N_d+3], float y_2[][N_d+3], float t_2[][N_d+3], float* delta);


//bool compare(const float *refData, const float *data,
//             const int length, float* delta, float* err, const float epsilon);
bool compare1(const float refData[][N_d], const float data[][N_d],
             float* err, float* delta, const float epsilon);
bool compare2(const float refData[][N_d], const float data[][N_d+3],
              float* err, float* delta, const float epsilon);
void minmax(const float data[][N_d+3],float* minVal, float* maxVal);

void print_coords(FILE *f_p, float x[][N_d], float y[][N_d], float t[][N_d]);
void print_coords3(FILE *f_p, float x[][N_d], float y[][N_d], float t[][N_d]);
void print_coords2(FILE *f_p, float* x, float* y, float* t);
void print_coords3a(FILE *f_p, float x[][N_d+3], float y[][N_d+3], float t[][N_d+3]);

#define BFSZ 13*(N_d+3)
// во время дебага TOTAL_STEPS можно делать меньше
//#define TOTAL_STEPS		1		// полное количество шагов по времени
//#define STEPS_TO_WRITE		1			// через это значение шагов сравниваем координаты и выводим в файл


void usage()
{
            std::cout << "Usage options:\n";
            std::cout << "-cpu 1/0            use cpu 1 thread\n" ;
            std::cout << "-cpu2 1/0            use cpu2 multi threads\n" ;
            std::cout << "-ocl <algo>            use ocl algo: 0-off,1-1g,2-mg,3-k1+k2,\n" ;
            std::cout << "\n" ;
            std::cout << "-pl <i>            use ocl platform index\n" ;
            std::cout << "-dev <i>            use ocl device index\n" ;
            std::cout << "-buf 1/0          use unqueue read/write buf IO\n";
            std::cout << "-cmp 1/0            use compare results with ref cpu\n" ;
            std::cout << "\n" ;
            std::cout << "-st <n>            total steps\n" ;
            std::cout << "-stw <n>           steps to write\n";
            std::cout << "-gszx <n>          global size x\n";
            std::cout << "-gszy <n>          global size y\n";
            std::cout << "-lszx <n>          local size x\n";
            std::cout << "-lszy <n>          local size y\n";
            std::cout << "\n";
            std::cout << "\n";

            exit(1);
}

int use_pl = 0; // platform index
int use_dev = 0; // device index
int use_cpu = 1; // reference cpu
int use_cpu2 = 0; // pthred
int use_ocl = 1; // 1 - 1g, 2 - mg, 3 - k1+k2
int use_cmp = 1; // compare with reference cpu
int use_rw_buffer = 1; // use clEnqueueWrite/ReadBuffer

int total_steps = 1;
int steps_to_write = 1;
int n_steps = 1;

// NDRange:
int gszx = 13;
int gszy = N_d;
int lszx = gszx;
int lszy = gszy;
int gn=1;

void parseOptions(int argc, char * argv[])
{
    while(--argc) 
    {
        if( strcmp(argv[argc], "-st") == 0 )
            total_steps = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-stw") == 0 )
            steps_to_write = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-gszx") == 0 )
            gszx = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-gszy") == 0 )
            gszy = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-lszx") == 0 )
            lszx = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-lszy") == 0 )
            lszy = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-cpu") == 0 )
            use_cpu = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-cpu2") == 0 )
            use_cpu2 = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-ocl") == 0 )
            use_ocl = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-dev") == 0 )
            use_dev = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-cmp") == 0 )
            use_cmp = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-buf") == 0 )
            use_rw_buffer = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-pl") == 0 )
            use_pl = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-h") == 0 )
           usage();
    }

}


#define N_g 16

// cpu
float x_1[13][N_d];
float y_1[13][N_d];
float t_1[13][N_d];
// cpu2
float x_2[13][N_d];
float y_2[13][N_d];
float t_2[13][N_d];
// ocl
float x_3[N_g][13][N_d+3];
float y_3[N_g][13][N_d+3];
float t_3[N_g][13][N_d+3];


int main(int argc, char * argv[]) {

	//calculate_constants();

         parseOptions(argc,argv);


	 FILE *f_cpu, *f_cpu2,*f_ocl;
	 int i,j;

    

        int cmp_status=0;
        int di=0,dj=0;
        float err=0.0f;
        float delta=0.0f;
        float epsilon = 1e-2f;

        float minVal=1e+10;
        float maxVal=1e-10;

	 CPerfCounter t_cpu;
	 CPerfCounter t_cpu2;
	 CPerfCounter t_ocl;



if(use_cmp && !use_cpu){
		 printf("use_cmp && !use_cpu.\n");
		 return -1;
}


if(use_cpu){
	 f_cpu = fopen ("MT_coords_CPU.txt","w");
	 if (f_cpu==NULL) {

		 printf("Error opening MT_coords_CPU.txt!\n");
		 return -1;
	 }
}
if(use_cpu2){
	 f_cpu2 = fopen ("MT_coords_CPU2.txt","w");
	 if (f_cpu2==NULL) {

		 printf("Error opening MT_coords_CPU2.txt!\n");
		 return -1;
	 }

//         mt::mt_cpu_mt_init(gszx,gszy,lszx,lszy);
}
if(use_ocl){
	 f_ocl = fopen ("MT_coords_ocl.txt","w");
	 if (f_ocl==NULL) {

		 printf("Error opening MT_coords_ocl.txt!\n");
		 return -1;
	 }
}
        n_steps=total_steps/steps_to_write;
	gn=(gszx/lszx)*(gszy/lszy);
	printf("Use cpu %d, cpu2 %d, ocl %d, buf %d, cmp %d, pl %d, dev %d\n",
		use_cpu,use_cpu2,use_ocl,use_rw_buffer, use_cmp,use_pl, use_dev );
	printf("Run total_steps %d, steps_to_write %d, n_steps %d, gsz (%d,%d) lsz(%d,%d), gn %d\n",
		total_steps,steps_to_write,n_steps,gszx,gszy,lszx,lszy, gn);


	// get golden results
	init_coords(x_1,y_1,t_1);
	rnd_coords(x_1,y_1,t_1);
        cpy_coords(x_1,y_1,t_1,x_2,y_2,t_2);

        for(int g=0;g<gn;g++)
        cpy_coords2(x_1,y_1,t_1,x_3[g],y_3[g],t_3[g]);

if(use_cpu){
	print_coords3(f_cpu, x_1, y_1, t_1);
}
if(use_cpu2){
	print_coords3(f_cpu2, x_2, y_2, t_2);
}

if(use_ocl){
        if(use_ocl==2){
         for(int g=0;g<gn;g++)
 	  print_coords3a(f_ocl, x_3[g], y_3[g], t_3[g]);
 
        }
        else
	print_coords3a(f_ocl, x_3[0], y_3[0], t_3[0]);

}

        mtOcl mt_ocl;


if(use_ocl){
	mt_ocl.dev_idx=use_dev;
        mt_ocl.pl_idx=use_pl;
        mt_ocl.use_rw_buffer=use_rw_buffer;
        mt_ocl.bufSz = BFSZ;
        mt_ocl.bufSzNg = BFSZ*gn;
        mt_ocl.gszX = gszx;
	mt_ocl.gszY = gszy;
	mt_ocl.lszX = lszx;
	mt_ocl.lszY = lszy;
	mt_ocl.numIter=steps_to_write;


//    // Initialize Host application 
//    if(mt_ocl.initializeHost() != SDK_SUCCESS){
//	printf("mt_ocl.initializeHost() failure\n");
//        return SDK_FAILURE;
//    }	

    // Initialize OpenCL resources
    if(mt_ocl.initializeCL() != SDK_SUCCESS){
	printf("mt_ocl.initializeCL() failure\n");
        return SDK_FAILURE;
    }

}


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

	for(int k=0,fl=0; k<n_steps; k++) {
        if(k==0)fl=1;
        else fl=0; 

if(use_cpu){
    	t_cpu.Start();
		mt_cpu(steps_to_write,fl,x_1,y_1,t_1,x_1,y_1, t_1);
    	t_cpu.Stop();
	print_coords3(f_cpu, x_1, y_1, t_1);

} // cpu


if(use_cpu2){
    	t_cpu2.Start();
//		mt::mt_cpu_mt(steps_to_write,fl,x_2,y_2,t_2,x_2,y_2, t_2);
    	t_cpu2.Stop();
	print_coords3(f_cpu2, x_2, y_2, t_2);

 if(use_cmp){
	if(!compare1(x_1, x_2, &err, &delta, epsilon) || 
	   !compare1(y_1, y_2, &err, &delta, epsilon) ||
	   !compare1(t_1, t_2, &err, &delta, epsilon))
        {
		printf("Compare cpu2 results failed at step %d (%f,%f)\n", k, err,delta);
		goto L_END;
        }
 } // cmp

} // cpu2

if(use_ocl){ 
    	t_ocl.Start();

              if(use_ocl==2){
		if(mt_ocl.run2(steps_to_write,fl,(float*)x_3,(float*)y_3,(float*)t_3)!= SDK_SUCCESS){
	 		printf("mt_ocl.run2 failure\n");
			return SDK_FAILURE;
		}

              } else {
		if(mt_ocl.run(use_ocl,steps_to_write,fl,(float*)x_3[0],(float*)y_3[0],(float*)t_3[0])!= SDK_SUCCESS){
	 		printf("mt_ocl.run failure\n");
			return SDK_FAILURE;
		}
              }
    	t_ocl.Stop();

              if(use_ocl==2){
        for(int g=0;g<gn;g++){
	print_coords3a(f_ocl, x_3[g], y_3[g], t_3[g]);

   minmax(x_3[g], &minVal, &maxVal);
   minmax(y_3[g], &minVal, &maxVal);
   minmax(t_3[g], &minVal, &maxVal);
        }


 if(use_cmp){
        for(int g=0;g<gn;g++)
	if(!compare2(x_1, x_3[g], &err, &delta, epsilon) || 
	   !compare2(y_1, y_3[g], &err, &delta, epsilon) ||
	   !compare2(t_1, t_3[g], &err, &delta, epsilon))
        {
		printf("Compare ocl results failed at step %d (%f,%f) g=%d\n",k,err,delta,g);
		goto L_END;
        }

 } // cmp


              } else {

	print_coords3a(f_ocl, x_3[0], y_3[0], t_3[0]);

   minmax(x_3[0], &minVal, &maxVal);
   minmax(y_3[0], &minVal, &maxVal);
   minmax(t_3[0], &minVal, &maxVal);

 if(use_cmp){
	if(!compare2(x_1, x_3[0], &err, &delta, epsilon) || 
	   !compare2(y_1, y_3[0], &err, &delta, epsilon) ||
	   !compare2(t_1, t_3[0], &err, &delta, epsilon))
        {
		printf("Compare ocl results failed at step %d (%f,%f)\n",k,err,delta);
		goto L_END;
        }

 } // cmp

              }


 
} // ocl


} // for k


if(use_ocl){
mt_ocl.postRun(use_ocl);
mt_ocl.cleanupCL();
}


cmp_status=1;

L_END:

  printf("minmax (%f,%f)\n", minVal,maxVal);


if(use_cpu){
	printf("t_cpu %lf (%lf)\n",t_cpu.GetElapsedTime()/(double)n_steps,t_cpu.GetElapsedTime());
}
if(use_cpu2){
	printf("t_cpu2 %lf (%lf)\n",t_cpu2.GetElapsedTime()/(double)n_steps,t_cpu2.GetElapsedTime());
}
if(use_ocl){
	printf("t_ocl %lf (%lf) prof time %lf\n",t_ocl.GetElapsedTime()/(double)n_steps,t_ocl.GetElapsedTime(),mt_ocl.prof_time);
}

if(cmp_status && use_cmp)
	printf("Test OK (%f,%f)!\n",err,delta);

if(use_cpu){
	fclose(f_cpu);
}
if(use_cpu2){
//        mt::mt_cpu_mt_finit();
	fclose(f_cpu2);
}
if(use_ocl){
	fclose(f_ocl);
}
	return 0;
}




void print_coords(FILE *f_p, float x[][N_d], float y[][N_d], float t[][N_d])
{
	int i,j;

	for (i=0; i<13; i++) {
		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.16f\t  ", x[i][j]);
		fprintf(f_p,"\n");

		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.16f\t  ", y[i][j]);
		fprintf(f_p,"\n");

		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.16f\t  ", t[i][j]);
		fprintf(f_p,"\n");
	}

	fprintf(f_p,"\n\n");

}

void print_coords2(FILE *f_p, float* x, float* y, float* t)
{
	int i,j;

	for (i=0; i<13; i++) {
		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.2f\t  ", x[i*N_d+j]);

		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.2f\t  ", y[i*N_d+j]);

		for (j=0; j<N_d; j++)
			fprintf(f_p,"%.2f\t  ", t[i*N_d+j]);
	}

	fprintf(f_p,"\n\n");

}

void print_coords3(FILE *f_p, float x[][N_d], float y[][N_d], float t[][N_d])
{
	int i,j;

	for (i=0; i<13; i++) {
		for (j=0; j<N_d; j++){
			fprintf(f_p,"%.2d,%.2d:", i,j);
			fprintf(f_p,"%.16f ", x[i][j]);
			fprintf(f_p,"%.16f ", y[i][j]);
			fprintf(f_p,"%.16f\n", t[i][j]);
		}
	}

	fprintf(f_p,"\n\n");

}

void print_coords3a(FILE *f_p, float x[][N_d+3], float y[][N_d+3], float t[][N_d+3])
{
	int i,j;

	for (i=0; i<13; i++) {
		for (j=0; j<N_d; j++){
			fprintf(f_p,"%.2d,%.2d:", i,j);
			fprintf(f_p,"%.16f ", x[i][j]);
			fprintf(f_p,"%.16f ", y[i][j]);
			fprintf(f_p,"%.16f\n", t[i][j]);
		}
	}

	fprintf(f_p,"\n\n");

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

void rnd_coords(float x[][N_d], float y[][N_d], float t[][N_d])
{
	int i,j;

        srand (time(NULL));


	for (i=0; i<13; i++){
	for (j=0; j<N_d; j++){
		x[i][j]+= (rand()%100)*1e-5; 
		y[i][j]-= (rand()%100)*1e-5;
		t[i][j]+= (rand()%100)*1e-5; 
	}
	}
}

void cpy_coords(float x[][N_d], float y[][N_d], float t[][N_d],float x2[][N_d], float y2[][N_d], float t2[][N_d])
{
	int i,j;


	for (i=0; i<13; i++){
	for (j=0; j<N_d; j++){
		x2[i][j]= x[i][j];
		y2[i][j]= y[i][j];
		t2[i][j]= t[i][j];
	}
	}
}

void cpy_coords2(float x[][N_d], float y[][N_d], float t[][N_d],float x2[][N_d+3], float y2[][N_d+3], float t2[][N_d+3])
{
	int i,j;


	for (i=0; i<13; i++){
	for (j=0; j<N_d; j++){
		x2[i][j]= x[i][j];
		y2[i][j]= y[i][j];
		t2[i][j]= t[i][j];
	}
	}
}


void init_coords2(float x[][N_d], float y[][N_d], float t[][N_d])
{
	int i,j;


	// задание y координаты для нижней спирали
	for (i=0; i<13; i++){
	for (j=0; j<N_d; j++){
		x[i][j] = i*N_d+j; 
		y[i][j] = i*N_d+j; 
		t[i][j] = i*N_d+j; 
	}
	}
}

#define DEBUG_PRINT_COORDS

int compare_results(float x_1[][N_d], float y_1[][N_d], float t_1[][N_d],
						float x_2[][N_d], float y_2[][N_d], float t_2[][N_d] )
{

	int error = 0;
	for(int i = 0; i<13; i++)
		for(int j = 0; j<N_d; j++) {
	const float epsilon = 0.01f;

#ifndef DEBUG_PRINT_COORDS
			if (fabs(x_1[i][j] - x_2[i][j]) > epsilon)
				error++;
			if (fabs(y_1[i][j] - y_2[i][j]) > epsilon)
				error++;
			if (fabs(t_1[i][j] - t_2[i][j]) > epsilon)
				error++;
#else
			if (fabs(x_1[i][j] - x_2[i][j]) > epsilon) {
				printf("x(%d)(%d) error: x1=%f, x2=%f\n",i,j,x_1[i][j],x_2[i][j]);
				error++;
			}
			if (fabs(y_1[i][j] - y_2[i][j]) > epsilon) {
				printf("y(%d)(%d) error: y1=%f, y2=%f\n",i,j,y_1[i][j],y_2[i][j]);
				error++;
			}
			if (fabs(t_1[i][j] - t_2[i][j]) > epsilon) {
				printf("t(%d)(%d) error: t1=%f, t2=%f\n",i,j,t_1[i][j],t_2[i][j]);
				error++;
			}

#endif
		}


	return error;
}


int compare_results2(float x_1[][N_d],   float y_1[][N_d],   float t_1[][N_d],
		     float x_2[][N_d+3], float y_2[][N_d+3], float t_2[][N_d+3], float* delta)
{
	const float epsilon = 0.01f;

	int error = 0; float d=0.0f;
	for(int i = 0; i<13; i++)
		for(int j = 0; j<N_d; j++) {

			if ((d=fabs(x_1[i][j] - x_2[i][j])) > epsilon){error++;printf("x(%d)(%d) error: x1=%f, x2=%f\n",i,j,x_1[i][j],x_2[i][j]);break;}
                        if(*delta<d){*delta=d;}
			if ((d=fabs(y_1[i][j] - y_2[i][j])) > epsilon){error++;printf("y(%d)(%d) error: y1=%f, y2=%f\n",i,j,y_1[i][j],y_2[i][j]);break;}
                        if(*delta<d){*delta=d;}
			if ((d=fabs(t_1[i][j] - t_2[i][j])) > epsilon){error++;printf("t(%d)(%d) error: t1=%f, t2=%f\n",i,j,t_1[i][j],t_2[i][j]);break;}
                        if(*delta<d){*delta=d;}
		}


	return error;
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

bool compare(const float *refData, const float *data,
             const int length, float* err, float* delta, const float epsilon = 1e-6f)
{
    float error = 0.0f;
    float ref = 0.0f;
    for(int i = 0; i < length; ++i)
    {
        float diff = refData[i] - data[i];
        if(fabs(diff)>*delta)*delta=fabs(diff);
        error += diff * diff;
        ref += refData[i] * refData[i];
    }
    float normRef =::sqrtf((float) ref);
    //if (::fabs((float) ref) < 1e-7f)
    //{
    //    return false;
    //}
    float normError = ::sqrtf((float) error);
    error = normError / normRef;
    if(error>*err)*err=error;
    return error < epsilon;
}


bool compare1(const float refData[][N_d], const float data[][N_d],
             float* err, float* delta, const float epsilon = 1e-6f)
{
    float error = 0.0f;
    float ref = 0.0f;
	for(int i = 0; i<13; i++)
		for(int j = 0; j<N_d; j++) {
        float diff = refData[i][j] - data[i][j];
        if(fabs(diff)>*delta){*delta=fabs(diff);}
        error += diff * diff;
        ref += refData[i][j] * refData[i][j];
    }
    float normRef =::sqrtf((float) ref);
    //if (::fabs((float) ref) < 1e-7f)
    //{
    //    return false;
    //}
    float normError = ::sqrtf((float) error);
    error = normError / normRef;
    if(error>*err)*err=error;
    return error < epsilon;
}

bool compare2(const float refData[][N_d], const float data[][N_d+3],
             float* err, float* delta, const float epsilon = 1e-6f)
{
    float error = 0.0f;
    float ref = 0.0f;
    float del = 0.0f;
	for(int i = 0; i<13; i++)
		for(int j = 0; j<N_d; j++) {
        float diff = refData[i][j] - data[i][j];
        if(fabs(diff)>del){del=fabs(diff);}
        error += diff * diff;
        ref += refData[i][j] * refData[i][j];
    }
    float normRef =::sqrtf((float) ref);
    //if (::fabs((float) ref) < 1e-7f)
    //{
    //    return false;
    //}
    float normError = ::sqrtf((float) error);
    error = normError / normRef;
    if(error>*err)*err=error;
    if(del>*delta)*delta=del;
    return error < epsilon && del < epsilon;
}

void minmax(const float data[][N_d+3],
             float* minVal, float* maxVal)
{
	for(int i = 0; i<13; i++)
		for(int j = 0; j<N_d; j++) {
        if(data[i][j]>*maxVal){*maxVal=data[i][j];}
        if(data[i][j]<*minVal){*minVal=data[i][j];}
    }
}

