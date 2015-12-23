#ifndef MT_CPU_H
#define MT_CPU_H


#include <cmath>
#include <stdio.h>
#include <string.h>
#include "mt_defines.h"





void mt_cpu(	int		n_step,				// полное количество шагов по времени
				int 	load_coords,		//

				float 	x_in[][N_d],
				float 	y_in[][N_d],
				float 	t_in[][N_d],

				float 	x_out[][N_d],
				float 	y_out[][N_d],
				float 	t_out[][N_d],
				int             flag_seed_c,
                int seeds[],
				bool ompflag
				
				//box_mull & rg1,
				//box_mull & rg2
			);


#define N_period 624
#define M_period 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */			



#endif //MT_CPU_H
