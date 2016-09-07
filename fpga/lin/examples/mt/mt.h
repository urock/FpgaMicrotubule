#pragma once
#include <stdlib.h>
#include <iostream>
#include <new>
#include <fstream>
#include <string>  
#include <vector>
#include "fpga.h"

using namespace std;

#include "mt_defines.h"

#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"

namespace microtubule {


class mt {

public:

  mt(string json_name, unsigned int N_d_in);
  mt(FpgaDev *Fpga, string json_name, unsigned int N_d_in);

  bool UseJsonCoeefs(float *cooefs_buf_out); 

private:

  void init(string json_name, unsigned int N_d_in); 
  bool use_coeffs_from_json;
  bool brownian_en;
  bool kinetics_en;
  bool const_seeds; 

  // coefficients
  float viscPF;   
  float viscPF_teta;   
  float B_Koeff; 
  float dt; 
  float dt_viscPF_teta; 
  float dt_viscPF; 
  float sqrt_PF_xy; 
  float sqrt_PF_teta;   
  float R_MT; 
  float A_Koeff; 
  float b_lat; 
  float A_long_D; 
  float b_long_D; 
  float A_long_T; 
  float b_long_T; 
  float ro0; 
  float ro0_long; 
  float inv_ro0_long; 
  float c_lat; 
  float d_lat; 
  float C_Koeff; 
  float Rad; 
  float inv_ro0; 
  float clat_dlat_ro0;   
  float clong_dlong_ro0; 
  float d_lat_ro0; 
  float d_long_ro0; 
  float fi_r; 
  float psi_r; 
  float fi_l; 
  float psi_l; 
  float rad_mon; 
  float teta0_D; 
  float teta0_T; 


  double attachment_probability;    //Probability per protofilament
  double hydrolysis_probability;

  unsigned int N_d;
  unsigned int n_layers;      //length of MT with counting shifts/ should be a multiple of 3
  unsigned int NStop[13];     //numbers of last monomers;
  unsigned int NStart[13];    //numbers of first monomers;

  mt_coords_t coords;
  vector<vector<int> > type_mol;

  FpgaDev  *Fpga;

  FILE *icf;    // input coords file
  FILE *ocf;    // output coords file
  FILE *olf;    // output length file
  FILE *otf;    // output type file



};





} // microtubule