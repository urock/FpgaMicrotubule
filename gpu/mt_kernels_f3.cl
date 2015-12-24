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

__constant float Ax_1[13] = {-0.165214628f, 0.0561592989f, 0.264667839f, 0.412544012f, 0.465911359f, 0.412544012f, 0.264667839f, 0.0561594106f, -0.165214419f, -0.348739684f, -0.452372819f, -0.452372819f, -0.348739684f};

__constant float Ax_2[13] = {1.76747036f, 1.87652779f, 1.5556947f, 0.878470898f, 0.0f, -0.878470898f, -1.5556947f, -1.87652767f, -1.76747072f, -1.25350749f, -0.452380866f, 0.452380747f, 1.25350738f};


__constant float Ax_3[13] = {0.162366703f, -0.0551912338f, -0.26010555f, -0.405432671f, -0.45788008f, -0.405432671f, -0.26010555f, -0.0551913455f, 0.162366495f, 0.342728168f, 0.444574922f, 0.444574922f, 0.342728198f};


__constant float A_Bx_4[13] = {0.0f, 0.46472317f, 0.822983861f, 0.992708862f, 0.935016215f, 0.663122654f, 0.239315659f, -0.239315659f, -0.663122654f, -0.935016215f, -0.992708862f, -0.822983861f, -0.46472317f};


__constant float Ay_1[13] = {0.435634613f, 0.462514341f, 0.383437514f, 0.216519803f, 0.0f, -0.216519803f, -0.383437514f, -0.462514341f, -0.435634702f, -0.308956355f, -0.111499891f, 0.111499861f, 0.308956355f};


__constant float Ay_2[13] = {-0.428125232f, -0.454541624f, -0.376827925f, -0.212787479f, -0.0f, 0.212787479f, 0.376827925f, 0.454541624f, 0.428125322f, 0.30363065f, 0.109577879f, -0.10957785f, -0.30363062f};


__constant float Ay_3[13] = {-0.670314014f, 0.227851257f, 1.07381856f, 1.67378652f, 1.89031017f, 1.67378652f, 1.07381856f, 0.227851719f, -0.67031312f, -1.41491747f, -1.83538115f, -1.83538127f, -1.41491759f};


__constant float A_By_4[13] = {1.0f, 0.885456026f, 0.568064749f, 0.120536678f, -0.3546049f, -0.748510778f, -0.970941842f, -0.970941842f, -0.748510778f, -0.3546049f, 0.120536678f, 0.568064749f, 0.885456026f};


__constant float Az_1 = 0.465911359f;

__constant float Az_2 = -0.45788008f;


__constant float Bx_1[13] = {0.321971923f, -0.109443799f, -0.515787303f, -0.80396992f, -0.907972693f, -0.80396992f, -0.515787303f, -0.109444022f, 0.321971506f, 0.679627359f, 0.881588638f, 0.881588697f, 0.679627359f};


__constant float Bx_2[13] = {-1.61023343f, -1.70958889f, -1.41729772f, -0.800320745f, -0.0f, 0.800320745f, 1.41729772f, 1.70958877f, 1.61023378f, 1.14199352f, 0.412136346f, -0.412136227f, -1.1419934f};


__constant float Bx_3[13] = {-0.16242376f, 0.0552106313f, 0.260196954f, 0.405575156f, 0.458040982f, 0.405575156f, 0.260196954f, 0.0552107431f, -0.162423551f, -0.342848599f, -0.444731146f, -0.444731146f, -0.342848629f};


__constant float By_1[13] = {-0.848969102f, -0.901352584f, -0.747246861f, -0.421955943f, -0.0f, 0.421955943f, 0.747246861f, 0.901352525f, 0.848969221f, 0.602097273f, 0.2172921f, -0.217292041f, -0.602097213f};


__constant float By_2[13] = {0.428275675f, 0.454701364f, 0.376960337f, 0.212862253f, 0.0f, -0.212862253f, -0.376960337f, -0.454701334f, -0.428275764f, -0.303737342f, -0.109616384f, 0.109616362f, 0.303737313f};


__constant float By_3[13] = {0.610681832f, -0.207581252f, -0.978290021f, -1.52488387f, -1.7221452f, -1.52488387f, -0.978290021f, -0.207581669f, 0.610681057f, 1.28904426f, 1.67210281f, 1.67210281f, 1.28904426f};


__constant float Bz_1 = -0.907972693f;

__constant float Bz_2 = 0.458040982f;


#define cosf cos
#define sinf sin
#define sqrtf sqrt
#define expf exp

#define printf


inline void calc_grad_c_f3(		int i1, 		// i index правой молекулы
					int j1,			// j index правой молекулы

					int i2,			// i index левой молекулы

					bit type, 		// dimer type: 0 - 'D', 1 - 'T'
					bit pos,		// monomer position in dimer: 0 - bottom, 1 - top

					float3 _1,		// правая молекула		mol1
					//float y_1,
					//float teta_1,

					float3 _2,		// левая молекула		mol2
					//float y_2,
					//float teta_2,

					float3 _3,		// верхняя молекула 	mol3
					//float y_3,
					//float teta_3,

					float3 *grad_lat_1,			// left component of mol1
					//float *grad_lat_y_1,
					//float *grad_lat_teta_1,

					float3 *grad_lat_2,			// right component of mol2
					//float *grad_lat_y_2,
					//float *grad_lat_teta_2,

					float3 *grad_long_1,			// up component of mol1
					//float *grad_long_y_1,
					//float *grad_long_teta_1,

					float3 *grad_long_3			// down component of mol3
					//float *grad_long_y_3,
					//float *grad_long_teta_3, 

							)


{


	// теперь PE_left - это индекс i2, а PF_right - индекс i1

	float cos_t_A = cosf(_2.z);
	float sin_t_A = sinf(_2.z);
	float cos_t_B = cosf(_1.z);
	float sin_t_B = sinf(_1.z);


	float cos_t_1 = cos_t_B;
	float sin_t_1 = sin_t_B;

	float cos_t_3 = cosf(_3.z);
	float sin_t_3 = sinf(_3.z);

	// swap i1 <=> i2


        float3 A_left;


	A_left.x = Ax_1[i2]*cos_t_A + Ax_3[i2]*sin_t_A - Ax_2[i2] +
				(_2.x + R_MT) * A_Bx_4[i2];

	A_left.y = Ay_1[i2]*cos_t_A + Ay_2[i2]*sin_t_A + Ay_3[i2] +
				(_2.x + R_MT) * A_By_4[i2];

	A_left.z = -Az_1*sin_t_A + Az_2*cos_t_A + _2.y;

//	float Ax_left = Ax_1[i2]*cos_t_A + Ax_3[i2]*sin_t_A - Ax_2[i2] +
//				(_2.x + R_MT) * A_Bx_4[i2];
//
//	float Ay_left = Ay_1[i2]*cos_t_A + Ay_2[i2]*sin_t_A + Ay_3[i2] +
//				(_2.x + R_MT) * A_By_4[i2];
//
//	float Az_left = -Az_1*sin_t_A + Az_2*cos_t_A + _2.y;

        float3 B_right;

	B_right.x = Bx_1[i1]*cos_t_B + Bx_3[i1]*sin_t_B - Bx_2[i1] +
				(_1.x + R_MT) * A_Bx_4[i1];

	B_right.y = By_1[i1]*cos_t_B + By_2[i1]*sin_t_B + By_3[i1] +
				(_1.x + R_MT) * A_By_4[i1];

	B_right.z = -Bz_1*sin_t_B + Bz_2*cos_t_B + _1.y;

//	float Bx_right = Bx_1[i1]*cos_t_B + Bx_3[i1]*sin_t_B - Bx_2[i1] +
//				(_1.x + R_MT) * A_Bx_4[i1];
//
//	float By_right = By_1[i1]*cos_t_B + By_2[i1]*sin_t_B + By_3[i1] +
//				(_1.x + R_MT) * A_By_4[i1];
//
//	float Bz_right = -Bz_1*sin_t_B + Bz_2*cos_t_B + _1.y;

	float3 D = A_left - B_right;

//	float Dx = Ax_left - Bx_right;
//	float Dy = Ay_left - By_right;
//	float Dz = Az_left - Bz_right;

	float dist = sqrtf(( pow(D.x, 2) + pow(D.y, 2) + pow(D.z, 2) ));

//	float dist = sqrtf(( pow(Dx, 2) + pow(Dy, 2) + pow(Dz, 2) ));


	if (dist <=1e-7 ){
		dist = 1e-5;
	}

	float inv_dist = 1/dist;

	float3 drdA = D * inv_dist;

//	float drdAx = D.x * inv_dist;
//	float drdAy = D.y * inv_dist;
//	float drdAz = D.z * inv_dist;

	float3 drdB = -drdA;

//	float drdBx = -drdAx;
//	float drdBy = -drdAy;
//	float drdBz = -drdAz;
        float3 dA_dteta;
	dA_dteta.x = -sin_t_A*Ax_1[i2] + cos_t_A*Ax_3[i2];
	dA_dteta.y = -sin_t_A*Ay_1[i2] + cos_t_A*Ay_2[i2];
	dA_dteta.z = -cos_t_A*Az_1 - sin_t_A*Az_2;

//	float dA_X_dteta = -sin_t_A*Ax_1[i2] + cos_t_A*Ax_3[i2];
//	float dA_Y_dteta = -sin_t_A*Ay_1[i2] + cos_t_A*Ay_2[i2];
//	float dA_Z_dteta = -cos_t_A*Az_1 - sin_t_A*Az_2;

	float drdx_A = drdA.x*A_Bx_4[i2] + drdA.y*A_By_4[i2];
	float drdy_A = drdA.z;
//	float drdteta_A = drdA.x*dA_dteta.x + drdA.y*dA_dteta.y + drdA.z*dA_dteta.z;
        float drdteta_A = dot(drdA,dA_dteta);

	//================================================
        float3 dB_dteta;
	dB_dteta.x = -sin_t_B*Bx_1[i1] + cos_t_B*Bx_3[i1];
	dB_dteta.y = -sin_t_B*By_1[i1] + cos_t_B*By_2[i1];
	dB_dteta.z = -cos_t_B*Bz_1 - sin_t_B*Bz_2;

//	float dB_X_dteta = -sin_t_B*Bx_1[i1] + cos_t_B*Bx_3[i1];
//	float dB_Y_dteta = -sin_t_B*By_1[i1] + cos_t_B*By_2[i1];
//	float dB_Z_dteta = -cos_t_B*Bz_1 - sin_t_B*Bz_2;

	float drdx_B = drdB.x*A_Bx_4[i1] + drdB.y*A_By_4[i1];
	float drdy_B = drdB.z;
//	float drdteta_B = drdB.x*dB_dteta.x + drdB.y*dB_dteta.y + drdB.z*dB_dteta.z;
        float drdteta_B = dot(drdB,dB_dteta);


	float Grad_U_tmp = (b_lat* dist *expf(-dist*inv_ro0)*(2.0f - dist*inv_ro0) +
				dist* clat_dlat_ro0 * expf( - (dist*dist) * d_lat_ro0 )  ) * A_Koeff;



	if ((i1==12)&&(j1>=(N_d-3))) {

//		grad_lat_2->x = 0.0f;
//		grad_lat_2->y = 0.0f;
//		grad_lat_2->z = 0.0f;
		*grad_lat_2 = (float3)(0.0f, 0.0f, 0.0f);

//		grad_lat_1->x = 0.0f;
//		grad_lat_1->y = 0.0f;
//		grad_lat_1->z = 0.0f;
		*grad_lat_1 = (float3)(0.0f, 0.0f, 0.0f);

	} else {

		(*grad_lat_2).x = Grad_U_tmp * drdx_A;
		(*grad_lat_2).y = Grad_U_tmp * drdy_A;
		(*grad_lat_2).z = Grad_U_tmp * drdteta_A;

		(*grad_lat_1).x = Grad_U_tmp * drdx_B;
		(*grad_lat_1).y = Grad_U_tmp * drdy_B;
		(*grad_lat_1).z = Grad_U_tmp * drdteta_B;

	}



	//	[nd] 	- 	mol3
	//	[nd-1] 	- 	mol1


	// longitudinal gradient

	float r_long_x = (_3.x - _1.x) - Rad*(sin_t_1 + sin_t_3);
	float r_long_y = (_3.y - _1.y) - Rad*(cos_t_1 + cos_t_3);
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
		Grad_tmp = B_Koeff*(_3.z - _1.z - teta0_D);
	else				// dimer type 'T'
		Grad_tmp = B_Koeff*(_3.z - _1.z - teta0_T);

	// поменял тут знак - все заработало!
	float GradU_B_teta_1 = - Grad_tmp;
	float GradU_B_teta_3 = + Grad_tmp;


	if (j1 == (N_d-1)) {

//		grad_long_1->x 		= 0.0f;
//		grad_long_1->y 		= 0.0f;
//		grad_long_1->z		= 0.0f;
		(*grad_long_1) = (float3)(0.0f, 0.0f, 0.0f);

//		grad_long_3->x 		= 0.0f;
//		grad_long_3->y 		= 0.0f;
//		grad_long_3->z		= 0.0f;
		(*grad_long_3) = (float3)(0.0f, 0.0f, 0.0f);

	} else {

		(*grad_long_1).x 		= Grad_tmp_x;
		(*grad_long_1).y 		= Grad_tmp_y;
		(*grad_long_1).z		= GradU_C_teta_1 + GradU_B_teta_1;

		(*grad_long_3).x 		= - Grad_tmp_x;
		(*grad_long_3).y 		= - Grad_tmp_y;
		(*grad_long_3).z		= GradU_C_teta_3 + GradU_B_teta_3;


	}


}





__kernel void mt_kernel2d_1g( // one group 2d kernel
const int          niters,                                                                   
__global float    x_inout[13][N_d+3],                          
__global float    y_inout[13][N_d+3],
__global float    t_inout[13][N_d+3]
)
{
 int i = get_global_id(0);
 int j = get_global_id(1);
 int s;


__local float3 __d[13][N_d+3];


__local float3 __lat_r[13][N_d+3];
__local float3 __long_d[13][N_d+1];


if ( i<13 && j<N_d) {

__lat_r[i][j]=(float3)(0.0f, 0.0f, 0.0f);
__long_d[i][j]=(float3)(0.0f, 0.0f, 0.0f);


__d[i][j].x=x_inout[i][j];     
__d[i][j].y=y_inout[i][j];     
__d[i][j].z=t_inout[i][j];

}

barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

 bit pos = 0; 
 bit type = 0;

float3 _f;
float3 _ij;

float3 lat_l;
float3 lat_r;
float3 long_u;
float3 long_d;

int i2 = (i==12)? 0 : (i+1);
int j2 = (i==12)? (j+3) : j;
pos =(j % 2);


for (s=0;s<niters;s++){

if ( i<13 && j<N_d) {



   _ij=__d[i][j];

			calc_grad_c_f3(i, j, i2, type,  pos,

				_ij,
				__d[i2][j2],
				__d[i][j+1],

				&lat_l,
				&lat_r,
				&long_u,
				&long_d    );


__lat_r[i2][j2] = lat_r;
__long_d[i][j+1]=long_d;

} //if i && j

barrier(CLK_LOCAL_MEM_FENCE);

if ( i<13 && j<N_d) {

if (j!=0){

_f = lat_l + __lat_r[i][j] + long_u + __long_d[i][j];

// _ij.x -= dt_viscPF * _f.x;
// _ij.y -= dt_viscPF * _f.y;
 _ij.xy -= dt_viscPF * _f.xy;

 _ij.z -= dt_viscPF_teta * _f.z;


__d[i][j]=_ij;     

} // if j

} // if i && j

barrier(CLK_LOCAL_MEM_FENCE);

} // for s


if ( i<13 && j<N_d) {

if (j!=0){
 x_inout[i][j] = __d[i][j].x;
 y_inout[i][j] = __d[i][j].y;
 t_inout[i][j] = __d[i][j].z;
}

} // if i && j

}


//volatile global  float3  __glat_r[13*(N_d+3)];//13*(N_d+3)
//__global volatile float3* __restrict __long_u, //13*(N_d+1)
//volatile global  float3  __glong_d[13*(N_d+3)]; //13*(N_d+1)
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

void global_synch(int s,volatile global int*  aBuf)
{
  if(get_local_id(0)==0 && get_local_id(1)==0){
   int gn = get_num_groups(0) * get_num_groups(1);
//   int gid = get_group_id(1)*get_num_groups(0)+get_group_id(0);
   //if(s==0){
//    printf("%d,%d, %d,%d = %d - %d\n",get_group_id(0),get_group_id(1),get_num_groups(0),get_num_groups(1),gid,gn);
   //}
   if(gn>1){
//    atomic_fetch_add_explicit ((atomic_int *)atomicBuf, 1, memory_order_seq_cst, memory_scope_device);
//    while (atomic_load_explicit (&atomicBuf[0], memory_order_seq_cst, memory_scope_device) % gn !=0);

	atomic_inc(&aBuf[0]);
        while (aBuf[0] % gn !=0);
//      atomic_inc(&atomicBuf[0]);
//      while (atomicBuf[0] % gn !=0);
/*
   int v=(aBuf[gid]+1)%2; 
   aBuf[gid]=v;
   while(1){
    int flag=0;
    for(int i=0;i<gn;i++)if(aBuf[i]!=v){flag=1;break;} 
    if(!flag)break;
    printf("%d,%d:",gid,v);
    for(int i=0;i<gn;i++)printf("%d",aBuf[i]);
    printf("\n"); 
   }
*/    

   }
  }
}

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

__kernel void mt_kernel2d_mg( // many groups 2d kernel
const int          niters,                                                                   
__global  float x_inout[13][N_d+3],                          
__global  float y_inout[13][N_d+3],
__global  float t_inout[13][N_d+3],
__global  float3  __lat_r[13][N_d+3],//13*(N_d+3)
__global  float3  __long_d[13][N_d+1], //13*(N_d+1)
__global volatile   int* flags

)
{


 int i = get_global_id(0);
 int j = get_global_id(1);
 int s;

if ( i<13 && j<N_d) {

__lat_r[i][j]=(float3)0.0f;
__long_d[i][j]=(float3)0.0f;


}

barrier(CLK_GLOBAL_MEM_FENCE);

bit pos = 0; 
bit type = 0;
float3 _f;
float3 _ij;
float3 _i2j2;
float3 _ij_1;
float3 lat_l;
float3 lat_r;
float3 long_u;
float3 long_d;

int i2 = (i==12)? 0 : (i+1);
int j2 = (i==12)? (j+3) : j;
pos =(j % 2);

for (s=0;s<niters;s++){

if ( i<13 && j<N_d) {



  _ij.x=x_inout[i][j];     _ij.y=y_inout[i][j];     _ij.z=t_inout[i][j];
_i2j2.x=x_inout[i2][j2]; _i2j2.y=y_inout[i2][j2]; _i2j2.z=t_inout[i2][j2];
_ij_1.x=x_inout[i][j+1]; _ij_1.y=y_inout[i][j+1]; _ij_1.z=t_inout[i][j+1];


			calc_grad_c_f3(i, j, i2, type,  pos,
				_ij,
				_i2j2,
				_ij_1,
				&lat_l,
				&lat_r,
				&long_u,
				&long_d    );


__lat_r[i2][j2] = lat_r;
__long_d[i][j+1]=long_d;

} //if i && j

barrier(CLK_GLOBAL_MEM_FENCE);

if ( i<13 && j<N_d) {

if (j!=0){

_f = lat_l + __lat_r[i][j] + long_u + __long_d[i][j];

 _ij.xy -= dt_viscPF * _f.xy;
// _ij.y -= dt_viscPF * _f.y;
 _ij.z -= dt_viscPF_teta * _f.z;


x_inout[i][j]=_ij.x;     
y_inout[i][j]=_ij.y;     
t_inout[i][j]=_ij.z;


} // if j

} // if i && j

barrier(CLK_GLOBAL_MEM_FENCE);

} // for s


}



