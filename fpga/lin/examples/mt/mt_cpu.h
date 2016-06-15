#ifndef MT_CPU_H
#define MT_CPU_H

#include <cmath>
#include <stdio.h>
#include <string.h>
#include <vector>
using std::vector;


#include "mt_defines.h"



int mt_cpu(	int		n_step,				// iieiia eiee?anoai oaaia ii a?aiaie
			int 	load_coords,		//
			int		flag_rand_c,
			int		flag_seed_c,

			int seeds[],

			vector<vector<float> >  & x_in,		// aoiaiua ianneau eii?aeiao, eniieuco?ony i?e load_coords = 1
			vector<vector<float> >  & y_in,
			vector<vector<float> >  & t_in,

			vector<vector<float> >  & x_out,		// auoiaiua ianneau eii?aeiao
			vector<vector<float> >  & y_out,
			vector<vector<float> >  & t_out,
			int N_d_chooseInput
);


/* Period parameters for randomiser*/
#define N_period 624
#define M_period 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

#define pii 3.14159265359f
			

#endif //MT_CPU_H
