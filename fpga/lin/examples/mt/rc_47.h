#ifndef RC_47_H
#define RC_47_H
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <new>

#include "rc_47_defines.h"
#include "mt_cpu.h"







int equal(float A, float B);
void print_usage(char *argv[]);


int compare_results(vector<vector<float> >  & x_1,vector<vector<float> >  & y_1, vector<vector<float> >  & t_1,
vector<vector<float> >  & x_2, vector<vector<float> >  & y_2, vector<vector<float> >  & t_2);

void calc_dt(struct timeval *tt1, struct timeval *tt2, double *tsec);           

void print_coords(FILE *f_p, mt_coords_t &mt_coords);
void print_coords_type(FILE *f_p, vector<vector<int> > &type_mol);





float getDistance(float x1, float y1, float t1, float x2, float y2, float t2);
void shift_coords(mt_coords_t &mt_coords, bool up, const int shift);
void choose_to_shift_coords(mt_coords_t &mt_coords);
int max_N(int N[13]);
int min_N(int N[13]);
void addDimer (int i, mt_coords_t &mt_coords);
void removeDimer (int i, mt_coords_t &mt_coords);
void polimerization_algorithm(mt_coords_t &mt_coords, float t1);
#endif