#ifndef MT_FPGA_H
#define MT_FPGA_H


#include <cmath>
#include <stdio.h>
#include <string.h>
#include "hls_stream.h"
//#include <ap_int.h>

#include "mt_defines.h"

typedef unsigned long long uint64_t;
typedef unsigned   uint32_t;

typedef struct {
	float d0;
	float d1; 
} two_floats; 



typedef struct {
	float x;
	float y;
	float t;
} float_3d;

union long_float{
uint64_t A;
two_floats B;
};


// wrapper function
// int mt_fpga(	bool flag,
				// int		n_step,				// полное количество шагов по времени
				// int 	load_coords,		//

				// int seeds[],

				// float 	x_in[][N_d],
				// float 	y_in[][N_d],
				// float 	t_in[][N_d],

				// float 	x_out[][N_d],
				// float 	y_out[][N_d],
				// float 	t_out[][N_d]
			// );

			

// hardware function
int mt_top(
		uint32_t InputLength,	
		uint32_t p1,
		uint32_t p2,
		uint32_t offset_i,		
		uint32_t offset_o,		
		uint32_t cp_size,		
			
		volatile uint64_t *bus_ptr_i,
		volatile uint64_t *stream1,
		volatile uint64_t *stream2,
		volatile uint64_t *stream3,
		volatile uint64_t *stream4,
		volatile uint64_t *stream5		

);

//void run_step_f();




void calc_grad_update_coord(	

					int i1, int j1,			// current molecule on the right

					int i2,			// i index of molecule on the left

//					bit pos,		// lowest monomer position in dimer: 0 - bottom, 1 - top

					bit type0,		bit type1, 		bit type2,			// molecule type: 0 - 'D', 1 - 'T'
					float_3d mc0, 	float_3d mc1, 	float_3d mc2,  		// central molecules

					float_3d ml0, float_3d ml1, float_3d ml2,		// left molecules

					float_3d mu,									// upper molecule
					
					float_3d *mc_n0, float_3d *mc_n1, float_3d *mc_n2,		// current molecule	mol1 NEW coords
					volatile uint64_t *stream1,
					volatile uint64_t *stream2,
					volatile uint64_t *stream3,
					volatile uint64_t *stream4,
					volatile uint64_t *stream5	
					
				);

				
void calc_lateral_gradients(
					int i1, 			// current molecule on the right
					int i2,			// i index of molecule on the left

					float_3d m1,		// current
					float_3d m2, 		// left

					float_3d *left_lat_r_ret,
					float_3d *c_lat_l_ret

);


void calc_longitudal_gradiets(

		bit type, 		// dimer type: 0 - 'D', 1 - 'T'
		bit pos,		// monomer position in dimer: 0 - bottom, 1 - top

		float_3d m1, 	// current molecule
		float_3d m3, 	// upper molecule

		float_3d *c_long_u_ret,
		float_3d *up_long_d_ret
);



#endif //MT_FPGA_H
