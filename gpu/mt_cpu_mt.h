#ifndef MT_CPU_MT_H
#define MT_CPU_MT_H

#include <cmath>
#include <stdio.h>
#include <string.h>

#include "mt_defines.h"

namespace mt{
void mt_cpu_mt(	int		n_step,
				int 	load_coords,

				float 	x_in[][N_d],
				float 	y_in[][N_d],
				float 	t_in[][N_d],

				float 	x_out[][N_d],
				float 	y_out[][N_d],
				float 	t_out[][N_d]
			);

void mt_cpu_mt_init(int _gszx,int _gszy,int _lszx,int _lszy);
void mt_cpu_mt_finit();
};

#endif

