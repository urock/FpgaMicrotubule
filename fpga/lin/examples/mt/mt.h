#pragma once
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <new>
#include <fstream>
#include <string>  
#include <vector>
#include "fpga.h"
#include "rand.h"

using namespace std;

#include "mt_defines.h"

#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"

#define STEPS_TO_WRITE     500         // steps in fpga. was 1000 ; 5000000*100 to almost destroy  //500000

namespace microtubule {

struct mt_coords_t {
    vector<vector<float> >  x;
    vector<vector<float> >  y;
    vector<vector<float> >  t;
};  


class mt {

public:

  mt(string json_name, unsigned int N_d_in);
  mt(FpgaDev *Fpga_, int board, int chip, string json_name, unsigned int N_d_in);
  ~mt();
  int calc_dynamics();
  void calc_kinetics(); 

  void print_coords_type();
  void print_coords(); 
  void print_length();
  mt_coords_t* get_coords(); 

private:

  void init(string json_name, unsigned int N_d_in); 
  bool use_json_coeffs(float *cooefs_buf_out); 
  void init_coords_and_type(void); 
  void addDimer (int i);
  int calc_dynamics_cpu(unsigned int n_layers);
  void shift_coords(bool up, int shift);
  void removeDimer (int i);
  void choose_to_shift_coords();
  void polimerization_algorithm();  

  bool use_coeffs_from_json;
  bool brownian_en;
  bool kinetics_en;
  bool const_seeds; 

  unsigned int N_d;
  // TODO: work on n_layers in calc dynamics function!!!
  unsigned int n_layers;      //length of MT with counting shifts/ should be a multiple of 3
  unsigned int NStop[13];     //numbers of last monomers;
  unsigned int NStart[13];    //numbers of first monomers;

  mt_coords_t coords;
  vector<vector<int> > type_mol;

  FpgaDev  *Fpga;
  int       dev;  // fpga device handle
  bool      use_fpga; 

  ifstream icf;    // input coords file
  ofstream ocf;    // output coords file
  ofstream olf;    // output length file
  ofstream otf;    // output type file

  unsigned int seeds[NUM_SEEDS];  // for random number generator used in dyncamics

  UniRandom *KineticRand;
};





} // microtubule