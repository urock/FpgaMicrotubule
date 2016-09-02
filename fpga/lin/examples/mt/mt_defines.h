#ifndef MT_DEF_H
#define MT_DEF_H



#define VAL2TO31	0x80000000
#define VAL2TO31F	((float)(VAL2TO31))

#define  NUM_UNI_VALUES 32
#define NUM_SEEDS 10


#define viscPF_d       		(4.3e-7*2)	//(1.1e-6  )		// (kcal/mol)*(s/nm^2)
#define viscPF_teta_d  		(2.3e-6*2)	//(1.7e-5  )		// (kcal/mol)*s
#define B_Koeff_d 			174.0f //kcal/mol   #define B_Koeff 91.25  //kcal/mol
#define dt_d 				(2e-10) // s
#define dt_viscPF_teta_d	4.34782596e-5f				//			dt_viscPF = dt/viscPF;
#define dt_viscPF_d			0.000232558144f				//			dt_viscPF_teta = dt/viscPF_teta;
#define sqrt_PF_xy_d 		0.016669f
#define sqrt_PF_teta_d 		0.007207f
#define R_MT_d 				8.128f
#define A_Koeff_d 			53.0f
#define b_lat_d   			12.9f
#define A_long_D_d 			90.0f
#define b_long_D_d 			7.9f
#define A_long_T_d 			90.0f
#define b_long_T_d 			7.9f
#define ro0_d       		0.12f
#define ro0_long_d  		0.12f
#define inv_ro0_long_d  	8.3333f	// 1/ro0_long
#define c_lat_d  			0.10f
#define d_lat_d  			0.25f
#define C_Koeff_d 			300.0f
#define Rad_d       		2.0f
#define inv_ro0_d 			8.3333f 	// = 1 / ro0;
#define clat_dlat_ro0_d		6.6666f		// 		//clat_dlat_ro0   = 2*c_lat / (d_lat* ro0);
#define clong_dlong_ro0_d	6.6666f				//clong_dlong_ro0 = 2*c_lat / (d_lat* ro0);
#define d_lat_ro0_d			33.3333f		// 			d_lat_ro0 = 1 / (d_lat*ro0);
#define	d_long_ro0_d		33.3333f		//			d_long_ro0 = 1 / (d_lat*ro0_long);
#define fi_r_d 				1.3291395f
#define psi_r_d 			1.801785f
#define fi_l_d  			1.0856f
#define psi_l_d  			-1.339725f
#define rad_mon_d 	 		2.0f
#define teta0_D_d 			0.2f //rad
#define teta0_T_d 			0.0f

#define Nc					34				// number of coefficients

#define pi 3.141592653f

const float Ax_1[13] = {-0.165214628f, 0.0561592989f, 0.264667839f, 0.412544012f, 0.465911359f, 0.412544012f, 0.264667839f, 0.0561594106f, -0.165214419f, -0.348739684f, -0.452372819f, -0.452372819f, -0.348739684f};

const float Ax_2[13] = {1.76747036f, 1.87652779f, 1.5556947f, 0.878470898f, 0.0f, -0.878470898f, -1.5556947f, -1.87652767f, -1.76747072f, -1.25350749f, -0.452380866f, 0.452380747f, 1.25350738f};


const float Ax_3[13] = {0.162366703f, -0.0551912338f, -0.26010555f, -0.405432671f, -0.45788008f, -0.405432671f, -0.26010555f, -0.0551913455f, 0.162366495f, 0.342728168f, 0.444574922f, 0.444574922f, 0.342728198f};


const float A_Bx_4[13] = {0.0f, 0.46472317f, 0.822983861f, 0.992708862f, 0.935016215f, 0.663122654f, 0.239315659f, -0.239315659f, -0.663122654f, -0.935016215f, -0.992708862f, -0.822983861f, -0.46472317f};


const float Ay_1[13] = {0.435634613f, 0.462514341f, 0.383437514f, 0.216519803f, 0.0f, -0.216519803f, -0.383437514f, -0.462514341f, -0.435634702f, -0.308956355f, -0.111499891f, 0.111499861f, 0.308956355f};


const float Ay_2[13] = {-0.428125232f, -0.454541624f, -0.376827925f, -0.212787479f, -0.0f, 0.212787479f, 0.376827925f, 0.454541624f, 0.428125322f, 0.30363065f, 0.109577879f, -0.10957785f, -0.30363062f};


const float Ay_3[13] = {-0.670314014f, 0.227851257f, 1.07381856f, 1.67378652f, 1.89031017f, 1.67378652f, 1.07381856f, 0.227851719f, -0.67031312f, -1.41491747f, -1.83538115f, -1.83538127f, -1.41491759f};


const float A_By_4[13] = {1.0f, 0.885456026f, 0.568064749f, 0.120536678f, -0.3546049f, -0.748510778f, -0.970941842f, -0.970941842f, -0.748510778f, -0.3546049f, 0.120536678f, 0.568064749f, 0.885456026f};


const float Az_1 = 0.465911359f;

const float Az_2 = -0.45788008f;


const float Bx_1[13] = {0.321971923f, -0.109443799f, -0.515787303f, -0.80396992f, -0.907972693f, -0.80396992f, -0.515787303f, -0.109444022f, 0.321971506f, 0.679627359f, 0.881588638f, 0.881588697f, 0.679627359f};


const float Bx_2[13] = {-1.61023343f, -1.70958889f, -1.41729772f, -0.800320745f, -0.0f, 0.800320745f, 1.41729772f, 1.70958877f, 1.61023378f, 1.14199352f, 0.412136346f, -0.412136227f, -1.1419934f};


const float Bx_3[13] = {-0.16242376f, 0.0552106313f, 0.260196954f, 0.405575156f, 0.458040982f, 0.405575156f, 0.260196954f, 0.0552107431f, -0.162423551f, -0.342848599f, -0.444731146f, -0.444731146f, -0.342848629f};


const float By_1[13] = {-0.848969102f, -0.901352584f, -0.747246861f, -0.421955943f, -0.0f, 0.421955943f, 0.747246861f, 0.901352525f, 0.848969221f, 0.602097273f, 0.2172921f, -0.217292041f, -0.602097213f};


const float By_2[13] = {0.428275675f, 0.454701364f, 0.376960337f, 0.212862253f, 0.0f, -0.212862253f, -0.376960337f, -0.454701334f, -0.428275764f, -0.303737342f, -0.109616384f, 0.109616362f, 0.303737313f};


const float By_3[13] = {0.610681832f, -0.207581252f, -0.978290021f, -1.52488387f, -1.7221452f, -1.52488387f, -0.978290021f, -0.207581669f, 0.610681057f, 1.28904426f, 1.67210281f, 1.67210281f, 1.28904426f};


const float Bz_1 = -0.907972693f;

const float Bz_2 = 0.458040982f;

#define kinetic_step_hydrolysis  (0.001f) / (2e-10)  //time step for kinetic algorithm, hydrolysis
#define K_hydrolysis 9.0f //rate of GTP hydrolysis, s^-1
#define kinetic_step_attachment  (0.013f) / (2e-10)  //time step for kinetic algorithm, polymerization
#define K_on 8.3f //on-rate constant for tubulin addition per MT, (uM*s)^-1
#define concentration 10.0f //soluble tubulin concentration, uM 


// namespace microtubule {

struct mt_coords_t {
    vector<vector<float> >  x;
    vector<vector<float> >  y;
    vector<vector<float> >  t;
};



// class 

// }

#endif //MT_DEF_H
