#include "mt_fpga.h"

unsigned int N_d_choose = 0;

float viscPF       		= viscPF_d;       		
float viscPF_teta  		= viscPF_teta_d;  		
float B_Koeff 			= B_Koeff_d; 			
float dt 				= dt_d; 				
float dt_viscPF_teta	= dt_viscPF_teta_d;	
float dt_viscPF			= dt_viscPF_d;			
float sqrt_PF_xy 		= sqrt_PF_xy_d; 		
float sqrt_PF_teta 		= sqrt_PF_teta_d; 		
float R_MT 				= R_MT_d; 				
float A_Koeff 			= A_Koeff_d; 			
float b_lat   			= b_lat_d;   			
float A_long_D 			= A_long_D_d; 			
float b_long_D 			= b_long_D_d; 			
float A_long_T 			= A_long_T_d; 			
float b_long_T 			= b_long_T_d; 			
float ro0       		= ro0_d;       		
float ro0_long  		= ro0_long_d;  		
float inv_ro0_long  	= inv_ro0_long_d;  	
float c_lat  			= c_lat_d;  			
float d_lat  			= d_lat_d;  			
float C_Koeff 			= C_Koeff_d; 			
float Rad       		= Rad_d;       		
float inv_ro0 			= inv_ro0_d; 			
float clat_dlat_ro0		= clat_dlat_ro0_d;		
float clong_dlong_ro0	= clong_dlong_ro0_d;	
float d_lat_ro0			= d_lat_ro0_d;			
float d_long_ro0		= d_long_ro0_d;		
float fi_r 				= fi_r_d; 				
float psi_r 			= psi_r_d; 			
float fi_l  			= fi_l_d;  			
float psi_l  			= psi_l_d;  			
float rad_mon 	 		= rad_mon_d; 	 		
float teta0_D 			= teta0_D_d; 			
float teta0_T 			= teta0_T_d; 			

// hardware function
int mt_top(
		uint32_t InputLength,	
		uint32_t p1,
		uint32_t p2,						// N_d (should be multiple of 3) max is N_d = 216
		uint32_t offset_i,					// flag to load coeffs from ddr
		uint32_t offset_o,		
		uint32_t cp_size,		
		volatile two_floats *bus_ptr_i,
		
		volatile uint64_t 	*stream1,
		volatile uint64_t 	*stream2,
		volatile uint64_t 	*stream3,
		volatile uint64_t 	*stream4,
		volatile uint64_t 	*stream5
		)
{

#pragma HLS interface ap_stable register port=InputLength
#pragma HLS interface ap_stable register port=offset_i
#pragma HLS interface ap_stable register port=offset_o
#pragma HLS interface ap_stable register port=cp_size
#pragma HLS interface ap_stable register port=p1
#pragma HLS interface ap_stable register port=p2

#pragma HLS INTERFACE m_axi port=bus_ptr_i offset=direct
#pragma HLS DATA_PACK variable=bus_ptr_i struct_level

#pragma HLS DATA_PACK variable=stream1 struct_level
#pragma HLS DATA_PACK variable=stream2 struct_level
#pragma HLS DATA_PACK variable=stream3 struct_level
#pragma HLS DATA_PACK variable=stream4 struct_level
#pragma HLS DATA_PACK variable=stream5 struct_level
#pragma HLS INTERFACE ap_ack port=stream1
#pragma HLS INTERFACE ap_ack port=stream2
#pragma HLS INTERFACE ap_ack port=stream3
#pragma HLS INTERFACE ap_ack port=stream4
#pragma HLS INTERFACE ap_ack port=stream5
	
	N_d_choose = p2;


	
	two_floats tmp_buf[2*N_d];

	uint32_t 	n_step 	= p1;
	//bit 	 	heat_on 	= p2 & 0x00000001;

	int k = 0; // ddr buffer index


	// fist array indice is protofiment index (horizontal index)
	// second array indice is molecule index inside protofilament (vertical index)

	bit type[13][N_d];		// for now define all molecules type as D

	// second indice range is > N_d not to get outside array when calculating upper logtitudal component for the highest row
	// and right lateral component for the last three rows

	float_3d m1[13][N_d+3];
	float_3d m2[13][N_d+3];
#pragma HLS data_pack variable=m1
#pragma HLS ARRAY_PARTITION variable=m1 cyclic factor=4 dim=2
#pragma HLS RESOURCE variable=m1 core=RAM_2P_BRAM latency=5

#pragma HLS data_pack variable=m2
#pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=4 dim=2
#pragma HLS RESOURCE variable=m2 core=RAM_2P_BRAM latency=5

	float_3d mc_n0, mc_n1, mc_n2;
	
	if (offset_i) {	// load coeffs from ddr
	
		memcpy(tmp_buf,(const two_floats*)(bus_ptr_i + coeffs_ddr_offset),Nc*sizeof(two_floats)/2);
		
		viscPF       		= tmp_buf[0].d0;
		viscPF_teta  		= tmp_buf[0].d1;
		B_Koeff 			= tmp_buf[1].d0;
		dt 					= tmp_buf[1].d1;
		dt_viscPF_teta		= tmp_buf[2].d0;
		dt_viscPF			= tmp_buf[2].d1;
		sqrt_PF_xy 			= tmp_buf[3].d0;
		sqrt_PF_teta 		= tmp_buf[3].d1;
		R_MT 				= tmp_buf[4].d0;
		A_Koeff 			= tmp_buf[4].d1;
		b_lat   			= tmp_buf[5].d0;
		A_long_D 			= tmp_buf[5].d1;
		b_long_D 			= tmp_buf[6].d0;
		A_long_T 			= tmp_buf[6].d1;
		b_long_T 			= tmp_buf[7].d0;
		ro0       			= tmp_buf[7].d1;
		ro0_long  			= tmp_buf[8].d0;
		inv_ro0_long  		= tmp_buf[8].d1;
		c_lat  				= tmp_buf[9].d0;
		d_lat  				= tmp_buf[9].d1;
		C_Koeff 			= tmp_buf[10].d0;
		Rad       			= tmp_buf[10].d1;
		inv_ro0 			= tmp_buf[11].d0;
		clat_dlat_ro0		= tmp_buf[11].d1;
		clong_dlong_ro0		= tmp_buf[12].d0;
		d_lat_ro0			= tmp_buf[12].d1;
		d_long_ro0			= tmp_buf[13].d0;
		fi_r 				= tmp_buf[13].d1;
		psi_r 				= tmp_buf[14].d0;
		fi_l  				= tmp_buf[14].d1;
		psi_l  				= tmp_buf[15].d0;
		rad_mon 	 		= tmp_buf[15].d1;
		teta0_D 			= tmp_buf[16].d0;
		teta0_T 			= tmp_buf[16].d1;
		
	
	}



	// monomer position in dimer: 0 - lower, 1 - upper. longitudal (vertical) force depends on that
	bit pos = 0;

	loop_in_1: for (int i=0; i<13; i++) {


		memcpy(tmp_buf,(const two_floats*)(bus_ptr_i + i*N_d_choose*2),N_d_choose*2*sizeof(two_floats));

		loop_in_2: for (int j=0; j<N_d_choose; j++) {

			//type[i][j] = 0;		// all dimer are type D for now

			m1[i][j].x = tmp_buf[2*j].d0;
			m1[i][j].y = tmp_buf[2*j].d1;
			m1[i][j].t = tmp_buf[2*j + 1].d0;
			
			type[i][j] = (bit)tmp_buf[2*j + 1].d1;
		}
		
		m2[i][0].x = m1[i][0].x;
		m2[i][0].y = m1[i][0].y;
		m2[i][0].t = m1[i][0].t;
		
	}


	// time loop
	time_loop: for (int step=1; step <= (p1>>1); step++) {

		l11: for (int j=0; j<N_d_choose; j+=3){

			l21: for (int i=0; i<13; i++) {
	#pragma HLS PIPELINE

				// (i,j) - current molecule
				// (i2, j2) - left neighbor of current molecule
				// each row is spiral (going up and left). last molecule in row is above first molecule in row by 3 rows
				// so that last mol in (12,0) will have as left neighbor (0,3) mol; (12,1) - (0,4) and so on
				int i2 = (i==12)? 0 : (i+1);
				int j2 = (i==12)? (j+3) : j;

				calc_grad_update_coord(i, j, i2, pos, 0, 0, 0,

								m1[i][j],   m1[i][j+1],   m1[i][j+2],			// central molecules
								m1[i2][j2], m1[i2][j2+1], m1[i2][j2+2],			// left molecules
								m1[i][j+3],										// upper molecule

								&mc_n0, &mc_n1, &mc_n2,				// new coords of central molecules
								stream1, stream2, stream3, stream4, stream5
				);


				// store new coords of current mol
				if (j==0) { m2[i][j+1] = mc_n1; m2[i][j+2] = mc_n2; }
				else { m2[i][j] = mc_n0; m2[i][j+1] = mc_n1; m2[i][j+2] = mc_n2;  }

			}


			pos = (pos==0)? 1 : 0;
		}


		// reverse order

		l12: for (int j=0; j<N_d_choose; j+=3){

			l22: for (int i=0; i<13; i++) {
	#pragma HLS PIPELINE

				// (i,j) - current molecule
				// (i2, j2) - left neighbor of current molecule
				// each row is spiral (going up and left). last molecule in row is above first molecule in row by 3 rows
				// so that last mol in (12,0) will have as left neighbor (0,3) mol; (12,1) - (0,4) and so on
				int i2 = (i==12)? 0 : (i+1);
				int j2 = (i==12)? (j+3) : j;

				calc_grad_update_coord(i, j, i2, pos, type[i][j], type[i][j+1], type[i][j+2],

								m2[i][j],   m2[i][j+1],   m2[i][j+2],			// central molecules
								m2[i2][j2], m2[i2][j2+1], m2[i2][j2+2],			// left molecules
								m2[i][j+3],										// upper molecule

								&mc_n0, &mc_n1, &mc_n2,				// new coords of central molecules
								stream1, stream2, stream3, stream4, stream5

				);


				// store new coords of current mol
				if (j==0) { m1[i][j+1] = mc_n1; m1[i][j+2] = mc_n2; }
				else { m1[i][j] = mc_n0; m1[i][j+1] = mc_n1; m1[i][j+2] = mc_n2;  }

			}

			pos = (pos==0)? 1 : 0;

		}


	}



	k = 0;
	loop_out_1: for (int i=0; i<13; i++) {
		loop_out_2: for (int j=0; j<N_d_choose; j++) 	{

			tmp_buf[2*j].d0 = m1[i][j].x;
			tmp_buf[2*j].d1 = m1[i][j].y;

			tmp_buf[2*j + 1].d0 = m1[i][j].t;
			tmp_buf[2*j + 1].d1 = 0;
		}


		memcpy((two_floats *)(bus_ptr_i + i*N_d_choose*2), tmp_buf,N_d_choose*2*sizeof(two_floats));
	}

	return 0;

}


// central mol forces
float_3d c_lat_l[3] = { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};		// lateral left
float_3d c_lat_r[3] = { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};		// lateral right

float_3d c_long_u[3] = { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};		// longitudal up
float_3d c_long_d[3] = { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}}; 	// longitudal down

// left mol forces
float_3d left_lat_r[3] = { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};		// lateral right

// up mol forces
float_3d up_long_d =  {0.0f, 0.0f, 0.0f}; 		// longitudal down

// up mol forces are calculated and should be saved for future use
float_3d c_long_d_save[13]={{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
							{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
							{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
							{0.0f, 0.0f, 0.0f}};



const float_3d zero_3d = {0.0f, 0.0f, 0.0f};




void calc_grad_update_coord( 

					int i1, int j1,			// current molecule on the right

					int i2,			// i index of molecule on the left

					bit pos,		// lowest monomer position in dimer: 0 - bottom, 1 - top

					bit type0,		bit type1, 		bit type2,			// molecule type: 0 - 'D', 1 - 'T'
					float_3d mc0, 	float_3d mc1, 	float_3d mc2,  		// central molecules

					float_3d ml0, float_3d ml1, float_3d ml2,		// left molecules

					float_3d mu,									// upper molecule
					
					float_3d *mc_n0, float_3d *mc_n1, float_3d *mc_n2,		// current molecule	mol1 NEW coords
					volatile uint64_t	 *stream1,
					volatile uint64_t	 *stream2,
					volatile uint64_t	 *stream3,
					volatile uint64_t	 *stream4,
					volatile uint64_t	 *stream5	
				)
									

{

#pragma HLS ARRAY_PARTITION variable=c_lat_l complete
#pragma HLS ARRAY_PARTITION variable=c_lat_r complete
#pragma HLS ARRAY_PARTITION variable=c_long_u complete
#pragma HLS ARRAY_PARTITION variable=c_long_d complete

#pragma HLS ARRAY_PARTITION variable=left_lat_r complete

#pragma HLS ARRAY_PARTITION variable=c_long_d_save complete



#pragma HLS EXPRESSION_BALANCE
#pragma HLS PIPELINE

	two_floats rand_buf2d[5];
#pragma HLS ARRAY_PARTITION variable=rand_buf2d complete

	
	bit pos2 = (pos==0)? 1 : 0;


	// this function will calculate left and up force components of current molecule
	// right force component as loaded on previous call of this function
	
	// load down component
	if (j1==0) {
		c_long_d[0] = zero_3d;
	} else {
		c_long_d[0] = c_long_d_save[i1];
	}



	// calc lateral forces
	float_3d lat_r_tmp[3], lat_l_tmp[3];

	calc_lateral_gradients(i1, i2, mc0, ml0, &lat_r_tmp[0], &lat_l_tmp[0]);
	calc_lateral_gradients(i1, i2, mc1, ml1, &lat_r_tmp[1], &lat_l_tmp[1]);
	calc_lateral_gradients(i1, i2, mc2, ml2, &lat_r_tmp[2], &lat_l_tmp[2]);

	for (int k=0; k<3; k++) {
#pragma HLS UNROLL
		if ((i1==12)&&(j1==(N_d_choose-3))) {
			left_lat_r[k] = zero_3d;
			c_lat_l[k]	= zero_3d;
		} else {
			left_lat_r[k] = lat_r_tmp[k];
			c_lat_l[k] = lat_l_tmp[k];
		}
	}



	// calc longitudal forces
	float_3d long_u_tmp[3], long_d_tmp[3];

	calc_longitudal_gradiets(type0, pos,  mc0, mc1, &long_u_tmp[0], &long_d_tmp[0]);
	calc_longitudal_gradiets(type1, pos2, mc1, mc2, &long_u_tmp[1], &long_d_tmp[1]);
	calc_longitudal_gradiets(type2, pos,  mc2, mu,  &long_u_tmp[2], &long_d_tmp[2]);

	c_long_u[0] = long_u_tmp[0];
	c_long_d[1] = long_d_tmp[0];
	c_long_u[1] = long_u_tmp[1];
	c_long_d[2] = long_d_tmp[1];


	if (j1 == (N_d_choose-3)) {


		c_long_u[2] = zero_3d;
		up_long_d = zero_3d;


	} else {

		c_long_u[2] = long_u_tmp[2];
		up_long_d 	= long_d_tmp[2];
	}
	


	// finish calculating forces
	// update coordinates

	float fx0 = c_lat_l[0].x + c_lat_r[0].x + c_long_u[0].x + c_long_d[0].x;
	float fy0 = c_lat_l[0].y + c_lat_r[0].y + c_long_u[0].y + c_long_d[0].y;
	float ft0 = c_lat_l[0].t + c_lat_r[0].t + c_long_u[0].t + c_long_d[0].t;

	float fx1 = c_lat_l[1].x + c_lat_r[1].x + c_long_u[1].x + c_long_d[1].x;
	float fy1 = c_lat_l[1].y + c_lat_r[1].y + c_long_u[1].y + c_long_d[1].y;
	float ft1 = c_lat_l[1].t + c_lat_r[1].t + c_long_u[1].t + c_long_d[1].t;

	float fx2 = c_lat_l[2].x + c_lat_r[2].x + c_long_u[2].x + c_long_d[2].x;
	float fy2 = c_lat_l[2].y + c_lat_r[2].y + c_long_u[2].y + c_long_d[2].y;
	float ft2 = c_lat_l[2].t + c_lat_r[2].t + c_long_u[2].t + c_long_d[2].t;

	float dx0 = mc0.x - dt_viscPF * fx0;
	float dy0 = mc0.y - dt_viscPF * fy0;
	float dt0 = mc0.t - dt_viscPF_teta * ft0;
	
	float dx1 = mc1.x - dt_viscPF * fx1;
	float dy1 = mc1.y - dt_viscPF * fy1;
	float dt1 = mc1.t - dt_viscPF_teta * ft1;

	float dx2 = mc2.x - dt_viscPF * fx2;
	float dy2 = mc2.y - dt_viscPF * fy2;
	float dt2 = mc2.t - dt_viscPF_teta * ft2;

	long_float temp[5];
#pragma HLS ARRAY_PARTITION variable=temp complete

		temp[0].A=*stream1;
		rand_buf2d[0] = temp[0].B;

		temp[1].A=*stream2;
		rand_buf2d[1] = temp[1].B;

		temp[2].A=*stream3;
		rand_buf2d[2] = temp[2].B;

		temp[3].A=*stream4;
		rand_buf2d[3] = temp[3].B;

		temp[4].A=*stream5;
		rand_buf2d[4] = temp[4].B;
		
		if (mc0.y != -100) {															//changed. In case reattached molecule
			mc_n0->x = dx0 - rand_buf2d[0].d0*sqrt_PF_xy;
			mc_n0->y = dy0 - rand_buf2d[0].d1*sqrt_PF_xy;
			mc_n0->t = dt0 - rand_buf2d[1].d0*sqrt_PF_teta;
		} else {
			mc_n0->x = dx0;
			mc_n0->y = dy0;
			mc_n0->t = dt0;
		}
		
		if (mc1.y != -100) {	
			mc_n1->x = dx1 - rand_buf2d[1].d1*sqrt_PF_xy;
			mc_n1->y = dy1 - rand_buf2d[2].d0*sqrt_PF_xy;
			mc_n1->t = dt1 - rand_buf2d[2].d1*sqrt_PF_teta;
		} else {
			mc_n1->x = dx1;
			mc_n1->y = dy1;
			mc_n1->t = dt1;
		}
		
		if (mc2.y != -100) {
			mc_n2->x = dx2 - rand_buf2d[3].d0*sqrt_PF_xy;
			mc_n2->y = dy2 - rand_buf2d[3].d1*sqrt_PF_xy;
			mc_n2->t = dt2 - rand_buf2d[4].d0*sqrt_PF_teta;
		} else {
			mc_n2->x = dx2;
			mc_n2->y = dy2;
			mc_n2->t = dt2;
		}
		
	
	// now need to save left component of right mol and down component of up mol for future use

	// right component
	c_lat_r[0] = left_lat_r[0];
	c_lat_r[1] = left_lat_r[1];
	c_lat_r[2] = left_lat_r[2];


	// save down component
	c_long_d_save[i1] = up_long_d;



}


void calc_longitudal_gradiets(

		bit type, 		// dimer type: 0 - 'D', 1 - 'T'
		bit pos,		// monomer position in dimer: 0 - bottom, 1 - top

		float_3d m1, 	// current molecule
		float_3d m3, 	// upper molecule

		float_3d *c_long_u_ret,
		float_3d *up_long_d_ret
)
{
#pragma HLS PIPELINE
	//	[nd] 	- 	mol3
	//	[nd-1] 	- 	mol1

	

	float x_1 = m1.x, y_1 = m1.y, teta_1 = m1.t;

	float x_3 = m3.x, y_3 = m3.y, teta_3 = m3.t;

	if (y_1 == -100 || y_3 == -100) {	//1 or 3 disconnected, f = 0, don't count it
		c_long_u_ret->x 	= 0.0f;
		c_long_u_ret->y 	= 0.0f;
		c_long_u_ret->t		= 0.0f;

		up_long_d_ret->x	= 0.0f;
		up_long_d_ret->y	= 0.0f;
		up_long_d_ret->t	= 0.0f;
		return;
	}
	

	float cos_t_1 = cosf(teta_1);
	float sin_t_1 = sinf(teta_1);

	float cos_t_3 = cosf(teta_3);
	float sin_t_3 = sinf(teta_3);

	float r_long_x_a1; r_long_x_a1 = x_3 - x_1;
	float r_long_x_a2; r_long_x_a2 = sin_t_1 + sin_t_3;
	float r_long_x_m1; r_long_x_m1 = r_long_x_a2 *Rad;
	float r_long_x; r_long_x = r_long_x_a1 - r_long_x_m1;
//	float r_long_x = (x_3 - x_1) - Rad*(sin_t_1 + sin_t_3);

	float r_long_y_a1; r_long_y_a1 = y_3 - y_1;
	float r_long_y_a2; r_long_y_a2 =  cos_t_1 + cos_t_3;
	float r_long_y_m1; r_long_y_m1 = r_long_y_a2 * Rad;
	float r_long_y; r_long_y =  r_long_y_a1 - r_long_y_m1;
	//float r_long_y = (y_3 - y_1) - Rad*(cos_t_1 + cos_t_3);

	float r_long_x2; r_long_x2 = r_long_x * r_long_x;
	float r_long_y2; r_long_y2 = r_long_y * r_long_y;
	float r_long_a1; r_long_a1 = r_long_x2 + r_long_y2;
	float r_long; r_long = sqrtf(r_long_a1);
//	float r_long = sqrtf( r_long_x*r_long_x + r_long_y*r_long_y);

	float r_long1;
	if (r_long <=1e-15 ){
		r_long1 = 1e-7;
	} else
		r_long1 = r_long;


	float drdx_long = - r_long_x/r_long1;
	float drdy_long = - r_long_y/r_long1;


	float tmp1_m1; tmp1_m1 = -r_long1*inv_ro0_long;
	float tmp1_a1; tmp1_a1 = 2 + tmp1_m1;
	float tmp1_exp1; tmp1_exp1 = expf(tmp1_m1);
	float tmp1_m2; tmp1_m2 =  tmp1_a1 * tmp1_exp1;
	float tmp1; tmp1 = tmp1_m2 * r_long1;
//	float tmp1 = r_long1 *  expf(-r_long1*inv_ro0_long)*(2 - r_long1*inv_ro0_long);

	float tmp2_m1; tmp2_m1 = -r_long1*r_long1;
	float tmp2_m2; tmp2_m2 = tmp2_m1*d_long_ro0;
	float tmp2_m3; tmp2_m3 = r_long1 * clong_dlong_ro0;
	float tmp2_exp1; tmp2_exp1 = expf(tmp2_m2);
	float tmp2; tmp2 = tmp2_m3 * tmp2_exp1;
//	float tmp2	= r_long1 * clong_dlong_ro0 * expf(-(r_long1*r_long1) * d_long_ro0 );


	float dUdr_C_1; dUdr_C_1 = C_Koeff*r_long1;

	float dUdr_C_2_m1; dUdr_C_2_m1 = tmp1*b_long_D;
	float dUdr_C_2_a1; dUdr_C_2_a1 =  dUdr_C_2_m1 + tmp2;
	float dUdr_C_2; dUdr_C_2 = dUdr_C_2_a1 *  A_long_D;
	//float dUdr_C_2 = (tmp1*b_long_D + tmp2) * A_long_D;

	float dUdr_C_3_m1; dUdr_C_3_m1 = tmp1*b_long_T;
	float dUdr_C_3_a1; dUdr_C_3_a1 = dUdr_C_3_m1 + tmp2;
	float dUdr_C_3; dUdr_C_3 = dUdr_C_3_a1 * A_long_T;
	//float dUdr_C_3 = (tmp1*b_long_T + tmp2) * A_long_T;


	float dUdr_C;

	if (pos==0) {		// bottom monomer (interaction inside dimer)
		dUdr_C = dUdr_C_1;
	} else {			// top monomer (interaction with upper dimer)

		if (type==0)	// dimer type 'D'
			dUdr_C = dUdr_C_2;
		else 			// dimer type 'T'
			dUdr_C = dUdr_C_3;
	}



	float Grad_tmp_x; Grad_tmp_x = drdx_long * dUdr_C;
	float Grad_tmp_y; Grad_tmp_y = drdy_long * dUdr_C;

	float GradU_C_teta_1_m1; GradU_C_teta_1_m1 = -Rad*cos_t_1;
	float GradU_C_teta_1_m2; GradU_C_teta_1_m2 = drdx_long * GradU_C_teta_1_m1;
	float GradU_C_teta_1_m3; GradU_C_teta_1_m3 = Rad*sin_t_1;
	float GradU_C_teta_1_m4; GradU_C_teta_1_m4 = drdy_long*GradU_C_teta_1_m3;
	float GradU_C_teta_1_a1; GradU_C_teta_1_a1 = -GradU_C_teta_1_m2 - GradU_C_teta_1_m4;
	float GradU_C_teta_1; GradU_C_teta_1 = dUdr_C * GradU_C_teta_1_a1;
	//float GradU_C_teta_1 = -dUdr_C*( drdx_long*(-Rad*cos_t_1) + drdy_long*(Rad*sin_t_1));

	float GradU_C_teta_3_m1; GradU_C_teta_3_m1 = Rad*cos_t_3;
	float GradU_C_teta_3_m2; GradU_C_teta_3_m2 = GradU_C_teta_3_m1*drdx_long;
	float GradU_C_teta_3_m3; GradU_C_teta_3_m3 = Rad*sin_t_3;
	float GradU_C_teta_3_m4; GradU_C_teta_3_m4 = GradU_C_teta_3_m3*drdy_long;
	float GradU_C_teta_3_a1; GradU_C_teta_3_a1 = GradU_C_teta_3_m2 - GradU_C_teta_3_m4;
	float GradU_C_teta_3; GradU_C_teta_3 = dUdr_C*GradU_C_teta_3_a1;
	//float GradU_C_teta_3 =  dUdr_C*(-drdx_long*(-Rad*cos_t_3) - drdy_long*(Rad*sin_t_3));

	float Grad_tmp_a1; Grad_tmp_a1 = teta_3 - teta_1;
	float Grad_tmp_a2;
	float Grad_tmp;
	float teta0_DT;
	if (type==0)		// dimer type 'D'
		teta0_DT = teta0_D;
	else				// dimer type 'T'
		teta0_DT = teta0_T;

	Grad_tmp_a2 = Grad_tmp_a1 - teta0_DT;
	Grad_tmp = Grad_tmp_a2 * B_Koeff;


	float GradU_B_teta_1 = - Grad_tmp;
	float GradU_B_teta_3 = + Grad_tmp;


	c_long_u_ret->x 	= Grad_tmp_x;
	c_long_u_ret->y 	= Grad_tmp_y;
	c_long_u_ret->t		= GradU_C_teta_1 + GradU_B_teta_1;

	up_long_d_ret->x	= - Grad_tmp_x;
	up_long_d_ret->y	= - Grad_tmp_y;
	up_long_d_ret->t	= GradU_C_teta_3 + GradU_B_teta_3;



}





void calc_lateral_gradients(
					int i1, 			// current molecule on the right
					int i2,			// i index of molecule on the left
					
					float_3d m1,		// current
					float_3d m2, 		// left

					float_3d *left_lat_r_ret,
					float_3d *c_lat_l_ret

)

{
#pragma HLS PIPELINE
	// PE_left in Pavel code is i2 index, PF_right - i1 index

	float x_1 = m1.x, y_1 = m1.y, teta_1 = m1.t;

	float x_2 = m2.x, y_2 = m2.y, teta_2 = m2.t;

	
	if (y_1 == -100 || y_2 == -100) {	//1 or 2 disconnected, f = 0, don't count it
		left_lat_r_ret->x 	= 0.0f;
		left_lat_r_ret->y 	= 0.0f;
		left_lat_r_ret->t	= 0.0f;

		c_lat_l_ret->x	= 0.0f; 
		c_lat_l_ret->y	= 0.0f;
		c_lat_l_ret->t	= 0.0f;
		return;
	}

	float cos_t_A = cosf(teta_2);
	float sin_t_A = sinf(teta_2);
	float cos_t_B = cosf(teta_1);
	float sin_t_B = sinf(teta_1);


	float cos_t_1 = cos_t_B;
	float sin_t_1 = sin_t_B;


	// swap i1 <=> i2


	float Ax_left_m1;	Ax_left_m1 = Ax_1[i2] * cos_t_A;
	float Ax_left_m2; 	Ax_left_m2 = Ax_3[i2]*sin_t_A;
	float Ax_left_add1;	Ax_left_add1 = x_2 + R_MT;
	float Ax_left_m3;	Ax_left_m3 =  A_Bx_4[i2] * Ax_left_add1;
	float Ax_left_add2;	Ax_left_add2 = Ax_left_m1 + Ax_left_m2;
	float Ax_left_add3;	Ax_left_add3 = Ax_left_m3 + Ax_left_add2;
	float Ax_left;		Ax_left = Ax_left_add3 - Ax_2[i2];


//	float Ax_left = Ax_1[i2]*cos_t_A + Ax_3[i2]*sin_t_A - Ax_2[i2] +
//				(x_2 + R_MT) * A_Bx_4[i2];


	float Ay_left_m1; Ay_left_m1 = Ay_1[i2] * cos_t_A;
	float Ay_left_m2; Ay_left_m2 = Ay_2[i2]*sin_t_A;
	float Ay_left_m3; Ay_left_m3 =  A_By_4[i2] * Ax_left_add1;
	float Ay_left_add2; Ay_left_add2 = Ay_left_m1 + Ay_left_m2;
	float Ay_left_add3; Ay_left_add3 = Ay_left_m3 + Ay_left_add2;
	float Ay_left; Ay_left = Ay_left_add3 + Ay_3[i2];


//	float Ay_left = Ay_1[i2]*cos_t_A + Ay_2[i2]*sin_t_A + Ay_3[i2] +
//				(x_2 + R_MT) * A_By_4[i2];

	float Az_left_m1; Az_left_m1 = Az_1*sin_t_A;
	float Az_left_m2; Az_left_m2 = Az_2*cos_t_A;
	float Az_left_add1; Az_left_add1 = Az_left_m2 + y_2;
	float Az_left; 	Az_left = Az_left_add1 - Az_left_m1;

//	float Az_left = -Az_1*sin_t_A + Az_2*cos_t_A + y_2;

	float Bx_right_m1; Bx_right_m1 = Bx_1[i1]*cos_t_B;
	float Bx_right_m2; Bx_right_m2 = Bx_3[i1]*sin_t_B;
	float Bx_right_add1; Bx_right_add1 = Bx_right_m1 + Bx_right_m2;
	float Bx_right_add2; Bx_right_add2 = x_1 + R_MT;
	float Bx_right_m3; Bx_right_m3 = A_Bx_4[i1] * Bx_right_add2;
	float Bx_right_add3; Bx_right_add3 = Bx_right_add1 + Bx_right_m3;
	float Bx_right; Bx_right = Bx_right_add3 - Bx_2[i1];

//	float Bx_right = Bx_1[i1]*cos_t_B + Bx_3[i1]*sin_t_B - Bx_2[i1] +
//				(x_1 + R_MT) * A_Bx_4[i1];

	float By_right_m1; By_right_m1 = By_1[i1]*cos_t_B;
	float By_right_m2; By_right_m2 = By_2[i1]*sin_t_B;
	float By_right_add1;By_right_add1 = By_right_m1 + By_right_m2;
	float By_right_m3;	By_right_m3 = Bx_right_add2 *  A_By_4[i1];
	float By_right_add3; By_right_add3 = By_right_add1 + By_right_m3;
	float By_right;
	By_right = By_right_add3 + By_3[i1];

//	float By_right = By_1[i1]*cos_t_B + By_2[i1]*sin_t_B + By_3[i1] +
//				(x_1 + R_MT) * A_By_4[i1];


	float Bz_right_m1;	Bz_right_m1 = Bz_1*sin_t_B;
	float Bz_right_m2;	Bz_right_m2 = Bz_2*cos_t_B;
	float Bz_right_add1;	Bz_right_add1 = Bz_right_m2 + y_1;
	float Bz_right;	Bz_right = Bz_right_add1 - Bz_right_m1;

	//float Bz_right = -Bz_1*sin_t_B + Bz_2*cos_t_B + y_1;

	float Dx; Dx = Ax_left - Bx_right;
	float Dx2; Dx2 = Dx * Dx;
	float Dy; Dy = Ay_left - By_right;
	float Dy2; Dy2 = Dy * Dy;
	float Dz; Dz = Az_left - Bz_right;
	float Dz2; Dz2 = Dz * Dz;
	float Dd_add1; Dd_add1 = Dx2 + Dy2;
	float Dd_add2; Dd_add2 = Dz2 + Dd_add1;

	float dist; dist = sqrtf(Dd_add2);
	//float dist = sqrtf(( Dx2 + Dy2 + Dz2 ));
	//float dist = sqrtf(( pow(Dx, 2) + pow(Dy, 2) + pow(Dz, 2) ));

	float dist1;
	if (dist <=1e-7 ){
		dist1 = 1e-5;
	} else {
		dist1 = dist;
	}

	float inv_dist = 1/dist1;

	float drdAx; drdAx = Dx * inv_dist;
	float drdAy; drdAy = Dy * inv_dist;
	float drdAz; drdAz = Dz * inv_dist;
	float drdBx = -drdAx;
	float drdBy = -drdAy;
	float drdBz = -drdAz;

	float dA_X_dteta_m1; dA_X_dteta_m1 = sin_t_A*Ax_1[i2];
	float dA_X_dteta_m2; dA_X_dteta_m2 = cos_t_A*Ax_3[i2];
	float dA_X_dteta; dA_X_dteta = dA_X_dteta_m2 - dA_X_dteta_m1;
	//float dA_X_dteta = -sin_t_A*Ax_1[i2] + cos_t_A*Ax_3[i2];

	float dA_Y_dteta_m1; dA_Y_dteta_m1 = sin_t_A*Ay_1[i2];
	float dA_Y_dteta_m2; dA_Y_dteta_m2 = cos_t_A*Ay_2[i2];
	float dA_Y_dteta; dA_Y_dteta = dA_Y_dteta_m2 - dA_Y_dteta_m1;
	//float dA_Y_dteta = -sin_t_A*Ay_1[i2] + cos_t_A*Ay_2[i2];

	float dA_Z_dteta_m1; dA_Z_dteta_m1 = cos_t_A*Az_1;
	float dA_Z_dteta_m2; dA_Z_dteta_m2 = sin_t_A*Az_2;
	float dA_Z_dteta; dA_Z_dteta = -dA_Z_dteta_m1 - dA_Z_dteta_m2;
	//float dA_Z_dteta = -cos_t_A*Az_1 - sin_t_A*Az_2;

	float drdx_A_m1; drdx_A_m1 = drdAx*A_Bx_4[i2];
	float drdx_A_m2; drdx_A_m2 = drdAy*A_By_4[i2];
	float drdx_A; drdx_A = drdx_A_m1 + drdx_A_m2;
	//float drdx_A = drdAx*A_Bx_4[i2] + drdAy*A_By_4[i2];
	float drdy_A = drdAz;

	float drdteta_A_m1; drdteta_A_m1 = drdAx*dA_X_dteta;
	float drdteta_A_m2; drdteta_A_m2 = drdAy*dA_Y_dteta;
	float drdteta_A_m3; drdteta_A_m3 = drdAz*dA_Z_dteta;
	float drdteta_A_add1; drdteta_A_add1 = drdteta_A_m1 + drdteta_A_m2;
	float drdteta_A; drdteta_A = drdteta_A_m3 + drdteta_A_add1;
	//float drdteta_A = drdAx*dA_X_dteta + drdAy*dA_Y_dteta + drdAz*dA_Z_dteta;

	//================================================
	float dB_X_dteta_m1; dB_X_dteta_m1 = sin_t_B*Bx_1[i1];
	float dB_X_dteta_m2; dB_X_dteta_m2 = cos_t_B*Bx_3[i1];
	float dB_X_dteta; dB_X_dteta = dB_X_dteta_m2 - dB_X_dteta_m1;
	//float dB_X_dteta = -sin_t_B*Bx_1[i1] + cos_t_B*Bx_3[i1];

	float dB_Y_dteta_m1; dB_Y_dteta_m1 = sin_t_B*By_1[i1];
	float dB_Y_dteta_m2; dB_Y_dteta_m2 = cos_t_B*By_2[i1];
	float dB_Y_dteta; dB_Y_dteta = dB_Y_dteta_m2 - dB_Y_dteta_m1;
	//float dB_Y_dteta = -sin_t_B*By_1[i1] + cos_t_B*By_2[i1];

	float dB_Z_dteta_m1;	dB_Z_dteta_m1 = cos_t_B*Bz_1;

	float dB_Z_dteta_m2;
	dB_Z_dteta_m2 = sin_t_B*Bz_2;
	float dB_Z_dteta; dB_Z_dteta = -dB_Z_dteta_m1 -dB_Z_dteta_m2;
	//float dB_Z_dteta = -cos_t_B*Bz_1 - sin_t_B*Bz_2;

	float drdx_B_m1; drdx_B_m1 = drdBx*A_Bx_4[i1];
	float drdx_B_m2; drdx_B_m2 = drdBy*A_By_4[i1];
	float drdx_B; drdx_B = drdx_B_m1 + drdx_B_m2;
	//float drdx_B = drdBx*A_Bx_4[i1] + drdBy*A_By_4[i1];

	float drdy_B = drdBz;

	float drdteta_B_m1; drdteta_B_m1 = drdBx*dB_X_dteta;
	float drdteta_B_m2; drdteta_B_m2 = drdBy*dB_Y_dteta;
	float drdteta_B_m3; drdteta_B_m3 = drdBz*dB_Z_dteta;
	float drdteta_B_add1; drdteta_B_add1 = drdteta_B_m1 + drdteta_B_m2;
	float drdteta_B; drdteta_B = drdteta_B_add1 + drdteta_B_m3;
	//float drdteta_B = drdBx*dB_X_dteta + drdBy*dB_Y_dteta + drdBz*dB_Z_dteta;


	float gu_m1; gu_m1 = -dist1*inv_ro0;
	float gu_m2; gu_m2 = A_Koeff * dist1;
	float gu_m3; gu_m3 = dist1*dist1;
	float gu_m4; gu_m4 =  -gu_m3 * d_lat_ro0;
	float gu_a1; gu_a1 = 2.0f + gu_m1;
	float gu_exp1; gu_exp1 = expf(gu_m1);
	float gu_exp2; gu_exp2 = expf(gu_m4);
	float gu_m5; gu_m5 = b_lat*gu_exp1;
	float gu_m6; gu_m6 = gu_m5*gu_a1;
	float gu_m7; gu_m7 = clat_dlat_ro0*gu_exp2;
	float gu_a2; gu_a2 = gu_m6 +	 gu_m7;

	float Grad_U_tmp; Grad_U_tmp = gu_a2 * gu_m2;

//	float Grad_U_tmp = (b_lat*expf(-dist1*inv_ro0)*(2.0f - dist1*inv_ro0) +
//						clat_dlat_ro0 * expf( - (dist1*dist1) * d_lat_ro0 )  ) * A_Koeff * dist1;

	left_lat_r_ret->x = Grad_U_tmp * drdx_A;
	left_lat_r_ret->y = Grad_U_tmp * drdy_A;
	left_lat_r_ret->t = Grad_U_tmp * drdteta_A;

	c_lat_l_ret->x = Grad_U_tmp * drdx_B;
	c_lat_l_ret->y= Grad_U_tmp * drdy_B;
	c_lat_l_ret->t = Grad_U_tmp * drdteta_B;

	

}
