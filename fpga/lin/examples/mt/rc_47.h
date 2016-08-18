#ifndef RC_47_H
#define RC_47_H
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <new>

#include "rc_47_defines.h"
#include "mt_cpu.h"


struct cmd_params_t {
	int		board;
	int		v7;
};

typedef struct {
	float d0;
	float d1; 
} two_floats; 






int equal(float A, float B);
void print_usage(char *argv[]);
int fpga_init(int argc, char *argv[], int *dev);
int process_cmd_line(int argc, char *argv[], struct cmd_params_t *cmd_p);
void init_coords(vector<vector<float> >  & x, vector<vector<float> >  & y, vector<vector<float> >  & t);
int compare_results(vector<vector<float> >  & x_1,vector<vector<float> >  & y_1, vector<vector<float> >  & t_1,
vector<vector<float> >  & x_2, vector<vector<float> >  & y_2, vector<vector<float> >  & t_2);

void calc_dt(struct timeval *tt1, struct timeval *tt2, double *tsec);						

void print_coords(FILE *f_p, vector<vector<float> >  & x, vector<vector<float> >  & y, vector<vector<float> >  & t);
void print_coords_type(FILE *f_p, vector<vector<int> > &type_mol);


int mt_fpga(	int 	dev,
int		n_step,				// полное количество шагов по времени
int 	load_coords,		//
int seeds[],

vector<vector<float> >  & x_in,
vector<vector<float> >  & y_in,
vector<vector<float> >  & t_in,

vector<vector<float> >  & x_out,
vector<vector<float> >  & y_out,
vector<vector<float> >  & t_out
) ; 

int mt_fpga1(	int 	dev,
int		n_step,				// полное количество шагов по времени
int 	load_coords,		//
//int seeds[],

vector<vector<float> >  & x_in,
vector<vector<float> >  & y_in,
vector<vector<float> >  & t_in,

vector<vector<float> >  & x_out,
vector<vector<float> >  & y_out,
vector<vector<float> >  & t_out,
vector<vector<int> > &type_mol
) ; 



int mt_fpga(	int 	dev,
int		n_step,				// полное количество шагов по времени
int 	load_coords,		//
//int seeds[],

vector<vector<float> >  & x_in,
vector<vector<float> >  & y_in,
vector<vector<float> >  & t_in,

vector<vector<float> >  & x_out,
vector<vector<float> >  & y_out,
vector<vector<float> >  & t_out
) ; 

float getDistance(float x1, float y1, float t1, float x2, float y2, float t2);
void shift_coords(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t, bool up, const int shift);
void choose_to_shift_coords(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t);
int max_N(int N[13]);
int min_N(int N[13]);
void addDimer (int i, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t);
void removeDimer (int i, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t);
void polimerization_algorithm(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t, float t1);
#endif