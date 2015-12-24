#include <stdio.h>
#include <cuda_runtime.h>

#include "mt_cpu.h"

#include "Timer.h"

namespace boom {

#define N_d 12


#define viscPF       (4.3e-7f*2f)	//(1.1e-6  )		// (kcal/mol)*(s/nm^2)
#define viscPF_teta  (2.3e-6f*2f)	//(1.7e-5  )		// (kcal/mol)*s

#define B_Koeff 174.0f //kcal/mol   #define B_Koeff 91.25  //kcal/mol

#define dt          (2e-10f) // s

#define dt_viscPF_teta		4.34782596e-5f				//			dt_viscPF = dt/viscPF;
#define dt_viscPF			0.000232558144f				//			dt_viscPF_teta = dt/viscPF_teta;



#define R_MT 8.128f


#define A_Koeff 53.0f
#define b_lat   12.9f

#define A_long_D 90.0f
#define b_long_D 7.9f

#define A_long_T 90.0f
#define b_long_T 7.9f

#define ro0       0.12f
#define ro0_long  0.12f

#define inv_ro0_long  8.3333f	// 1/ro0_long

#define c_lat  0.10f
#define d_lat  0.25f

#define C_Koeff 300.0f
#define Rad       2.0f

#define inv_ro0 			8.3333f 	// = 1 / ro0;
#define clat_dlat_ro0		6.6666f		// 		//clat_dlat_ro0   = 2*c_lat / (d_lat* ro0);

#define clong_dlong_ro0		6.6666f				//clong_dlong_ro0 = 2*c_lat / (d_lat* ro0);

#define 	d_lat_ro0			33.3333f		// 			d_lat_ro0 = 1 / (d_lat*ro0);


#define		d_long_ro0			33.3333f		//			d_long_ro0 = 1 / (d_lat*ro0_long);

#define pi 3.141592653f


#define fi_r 		1.3291395f
#define psi_r 		1.801785f

#define fi_l  		1.0856f
#define psi_l  		-1.339725f
#define rad_mon 	 2.0f

#define teta0_D 0.2f //rad
#define teta0_T 0.0f

#define bit char

__constant__  float Ax_1[13] = {-0.165214628f, 0.0561592989f, 0.264667839f, 0.412544012f, 0.465911359f, 0.412544012f, 0.264667839f, 0.0561594106f, -0.165214419f, -0.348739684f, -0.452372819f, -0.452372819f, -0.348739684f};

__constant__  float Ax_2[13] = {1.76747036f, 1.87652779f, 1.5556947f, 0.878470898f, 0.0f, -0.878470898f, -1.5556947f, -1.87652767f, -1.76747072f, -1.25350749f, -0.452380866f, 0.452380747f, 1.25350738f};


__constant__  float Ax_3[13] = {0.162366703f, -0.0551912338f, -0.26010555f, -0.405432671f, -0.45788008f, -0.405432671f, -0.26010555f, -0.0551913455f, 0.162366495f, 0.342728168f, 0.444574922f, 0.444574922f, 0.342728198f};


__constant__  float A_Bx_4[13] = {0.0f, 0.46472317f, 0.822983861f, 0.992708862f, 0.935016215f, 0.663122654f, 0.239315659f, -0.239315659f, -0.663122654f, -0.935016215f, -0.992708862f, -0.822983861f, -0.46472317f};


__constant__  float Ay_1[13] = {0.435634613f, 0.462514341f, 0.383437514f, 0.216519803f, 0.0f, -0.216519803f, -0.383437514f, -0.462514341f, -0.435634702f, -0.308956355f, -0.111499891f, 0.111499861f, 0.308956355f};


__constant__  float Ay_2[13] = {-0.428125232f, -0.454541624f, -0.376827925f, -0.212787479f, -0.0f, 0.212787479f, 0.376827925f, 0.454541624f, 0.428125322f, 0.30363065f, 0.109577879f, -0.10957785f, -0.30363062f};


__constant__  float Ay_3[13] = {-0.670314014f, 0.227851257f, 1.07381856f, 1.67378652f, 1.89031017f, 1.67378652f, 1.07381856f, 0.227851719f, -0.67031312f, -1.41491747f, -1.83538115f, -1.83538127f, -1.41491759f};


__constant__  float A_By_4[13] = {1.0f, 0.885456026f, 0.568064749f, 0.120536678f, -0.3546049f, -0.748510778f, -0.970941842f, -0.970941842f, -0.748510778f, -0.3546049f, 0.120536678f, 0.568064749f, 0.885456026f};


__constant__  float Az_1 = 0.465911359f;

__constant__  float Az_2 = -0.45788008f;


__constant__  float Bx_1[13] = {0.321971923f, -0.109443799f, -0.515787303f, -0.80396992f, -0.907972693f, -0.80396992f, -0.515787303f, -0.109444022f, 0.321971506f, 0.679627359f, 0.881588638f, 0.881588697f, 0.679627359f};


__constant__  float Bx_2[13] = {-1.61023343f, -1.70958889f, -1.41729772f, -0.800320745f, -0.0f, 0.800320745f, 1.41729772f, 1.70958877f, 1.61023378f, 1.14199352f, 0.412136346f, -0.412136227f, -1.1419934f};


__constant__  float Bx_3[13] = {-0.16242376f, 0.0552106313f, 0.260196954f, 0.405575156f, 0.458040982f, 0.405575156f, 0.260196954f, 0.0552107431f, -0.162423551f, -0.342848599f, -0.444731146f, -0.444731146f, -0.342848629f};


__constant__  float By_1[13] = {-0.848969102f, -0.901352584f, -0.747246861f, -0.421955943f, -0.0f, 0.421955943f, 0.747246861f, 0.901352525f, 0.848969221f, 0.602097273f, 0.2172921f, -0.217292041f, -0.602097213f};


__constant__  float By_2[13] = {0.428275675f, 0.454701364f, 0.376960337f, 0.212862253f, 0.0f, -0.212862253f, -0.376960337f, -0.454701334f, -0.428275764f, -0.303737342f, -0.109616384f, 0.109616362f, 0.303737313f};


__constant__  float By_3[13] = {0.610681832f, -0.207581252f, -0.978290021f, -1.52488387f, -1.7221452f, -1.52488387f, -0.978290021f, -0.207581669f, 0.610681057f, 1.28904426f, 1.67210281f, 1.67210281f, 1.28904426f};


__constant__  float Bz_1 = -0.907972693f;

__constant__  float Bz_2 = 0.458040982f;




__device__
__forceinline__
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






__global__ void
mt_cuda_kernel1(
const int          niters,                                                                   
float*    x_inout,                          
float*    y_inout,
float*    t_inout
)
{

 int i = blockDim.x * blockIdx.x + threadIdx.x;
 int j = blockDim.y * blockIdx.y + threadIdx.y;
 int s;

if(i==0 && j==0){
printf("niters %d\n", niters);
}


__shared__ float __x[13][N_d+3];
__shared__ float __y[13][N_d+3];
__shared__ float __t[13][N_d+3];

__shared__ float4 __lat_r[13][N_d+3];
__shared__ float4 __long_d[13][N_d+1];

//__lat_r[i][j]=0;
//__long_d[i][j]=0;
__x[i][j]=x_inout[i*(N_d+3) + j];     
__y[i][j]=y_inout[i*(N_d+3) + j];     
__t[i][j]=t_inout[i*(N_d+3) + j];



 float f_x, f_y, f_t;
 bit pos = 0; 
 bit type = 0;

float x_ij, y_ij, t_ij, x_i2j2,y_i2j2,t_i2j2,x_ij_1, y_ij_1, t_ij_1;

float lat_l_x, lat_l_y, lat_l_t,
lat_r_x, lat_r_y, lat_r_t,
long_u_x, long_u_y, long_u_t,
long_d_x, long_d_y, long_d_t;


int i2 = (i==12)? 0 : (i+1);
int j2 = (i==12)? (j+3) : j;
pos =(j % 2);

__syncthreads();

//#pragma unroll 2
for (s=0;s<niters;s++){

 x_ij=  __x[i][j];     y_ij=__y[i][j];     t_ij=__t[i][j];
 x_i2j2=__x[i2][j2]; y_i2j2=__y[i2][j2]; t_i2j2=__t[i2][j2];
 x_ij_1=__x[i][j+1]; y_ij_1=__y[i][j+1], t_ij_1=__t[i][j+1];

			calc_grad_c(i, j, i2, type,  pos,

				x_ij, y_ij, t_ij, 
				x_i2j2,y_i2j2,t_i2j2,
				x_ij_1, y_ij_1, t_ij_1,


				&lat_l_x,&lat_l_y,&lat_l_t,
				&lat_r_x,&lat_r_y,&lat_r_t,
				&long_u_x, &long_u_y, &long_u_t,
				&long_d_x, &long_d_y, &long_d_t);


__lat_r[i2][j2].x=lat_r_x;  __lat_r[i2][j2].y=lat_r_y;  __lat_r[i2][j2].z=lat_r_t;
__long_d[i][j+1].x=long_d_x;__long_d[i][j+1].y=long_d_y;__long_d[i][j+1].z=long_d_t;


__syncthreads();


if (j!=0){

 f_x = lat_l_x + __lat_r[i][j].x + long_u_x + __long_d[i][j].x;
 f_y = lat_l_y + __lat_r[i][j].y + long_u_y + __long_d[i][j].y;
 f_t = lat_l_t + __lat_r[i][j].z + long_u_t + __long_d[i][j].z;

 x_ij -= dt_viscPF * f_x;
 y_ij -= dt_viscPF * f_y;
 t_ij -= dt_viscPF_teta * f_t;

__x[i][j]=x_ij;     
__y[i][j]=y_ij;     
__t[i][j]=t_ij;

} // if j


__syncthreads();




} // for s



if (j!=0){
 x_inout[i*(N_d+3) + j] = __x[i][j];
 y_inout[i*(N_d+3) + j] = __y[i][j];
 t_inout[i*(N_d+3) + j] = __t[i][j];
}



}

/*
__global__ void
mt_cuda_kernel_root(
const int          niters,                                                                   
float*    x_inout,                          
float*    y_inout,
float*    t_inout
)
{
    dim3 block_size;
    block_size.x=13;
    block_size.y=N_d;

    dim3 grid_size;
    grid_size.x = 1;
    grid_size.y = 1;


    boom::mt_cuda_kernel1<<<grid_size, block_size>>>(niters, x_inout, y_inout, t_inout);
}
*/


} // ns boom



void init_coords(float x[][N_d], float y[][N_d], float t[][N_d]);
void rnd_coords(float x[][N_d], float y[][N_d], float t[][N_d]);
void cpy_coords2(float x[][N_d], float y[][N_d], float t[][N_d],float x2[][N_d+3], float y2[][N_d+3], float t2[][N_d+3]);
bool compare2(const float refData[][N_d], const float data[][N_d+3],float* err, float* delta, const float epsilon);


float x_1[13][N_d];
float y_1[13][N_d];
float t_1[13][N_d];


float x_3[13][N_d+3];
float y_3[13][N_d+3];
float t_3[13][N_d+3];


int
main(void)
{
    cudaError_t err = cudaSuccess;
    CPerfCounter t_cu;

    int use_cmp=0;

        float err1=0.0f;
        float delta1=0.0f;
        float epsilon = 1e-2f;

    int niters=100000;

    size_t size = 13*(N_d+3)*sizeof(float);


/*
    for (int i = 0; i < 13; ++i)
     for (int j = 0; j < N_d; ++j)
     {
        x_3[i][j] = 7;//rand()/(float)RAND_MAX;
        y_3[i][j] = 9;//rand()/(float)RAND_MAX;
        t_3[i][j] = 13;//rand()/(float)RAND_MAX;
     }
*/


	init_coords(x_1,y_1,t_1);
	rnd_coords(x_1,y_1,t_1);
        cpy_coords2(x_1,y_1,t_1,x_3,y_3,t_3);


    // Allocate the device vector
    float *d_x = NULL;
    err = cudaMalloc((void **)&d_x, size);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector x (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Allocate the device vector
    float *d_y = NULL;
    err = cudaMalloc((void **)&d_y, size);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector y (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Allocate the device vector
    float *d_t = NULL;
    err = cudaMalloc((void **)&d_t, size);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector t (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }



    t_cu.Reset();t_cu.Start();


    // Copy the host input vectors A and B in host memory to the device input vectors in
    // device memory
    printf("Copy input data from the host memory to the CUDA device\n");
    err = cudaMemcpy(d_x, x_3, size, cudaMemcpyHostToDevice);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector x from host to device (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMemcpy(d_y, y_3, size, cudaMemcpyHostToDevice);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector y from host to device (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }


    err = cudaMemcpy(d_t, t_3, size, cudaMemcpyHostToDevice);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector t from host to device (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Launch the Kernel
    dim3 block_size;
    block_size.x=13;
    block_size.y=N_d;

    dim3 grid_size;
    grid_size.x = 1;//num_elements_x / block_size.x;
    grid_size.y = 1;//num_elements_y / block_size.y;


    printf("CUDA kernel launch with %d,%d %d,%d\n", block_size.x, block_size.y, grid_size.x,grid_size.y);


    boom::mt_cuda_kernel1<<<grid_size, block_size>>>(niters, d_x, d_y, d_t);


    err = cudaGetLastError();

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to launch kernel (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy the device result vector in device memory to the host result vector
    // in host memory.
//    printf("Copy output data from the CUDA device to the host memory\n");
    err = cudaMemcpy(x_3, d_x, size, cudaMemcpyDeviceToHost);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector x from device to host (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMemcpy(y_3, d_y, size, cudaMemcpyDeviceToHost);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector y from device to host (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMemcpy(t_3, d_t, size, cudaMemcpyDeviceToHost);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector t from device to host (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }


    t_cu.Stop();

    printf("niters %d t_cu %lf\n",niters,t_cu.GetElapsedTime());


//    printf("%f,%f,%f\n",x_3[0][1],y_3[0][1],t_3[0][1]);
if(use_cmp){
	mt_cpu(niters,1,x_1,y_1,t_1,x_1,y_1,t_1);


	if(!compare2(x_1, x_3, &err1, &delta1, epsilon) || 
	   !compare2(y_1, y_3, &err1, &delta1, epsilon) ||
	   !compare2(t_1, t_3, &err1, &delta1, epsilon))
        {
		printf("Compare cu results failed (%f,%f)\n",err1,delta1);
        }
	else printf("Test OK(%f,%f)\n",err1,delta1);

}



    // Free device global memory
    err = cudaFree(d_x);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaFree(d_y);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector B (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaFree(d_t);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector C (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Free host memory
    //free(h_A);
    //free(h_B);
    //free(h_C);

    // Reset the device and exit
    // cudaDeviceReset causes the driver to clean up all state. While
    // not mandatory in normal operation, it is good practice.  It is also
    // needed to ensure correct operation when the application is being
    // profiled. Calling cudaDeviceReset causes all profile data to be
    // flushed before the application exits
    err = cudaDeviceReset();

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to deinitialize the device! error=%s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    printf("Done\n");
    return 0;
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


bool compare2(const float refData[][N_d], const float data[][N_d+3],
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
    if (::fabs((float) ref) < 1e-7f)
    {
        return false;
    }
    float normError = ::sqrtf((float) error);
    error = normError / normRef;
    if(error>*err)*err=error;
    return error < epsilon;
}

