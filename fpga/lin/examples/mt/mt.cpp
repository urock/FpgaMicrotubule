#include "mt.h"

using namespace std;

namespace microtubule {


  mt::mt(string json_name) {


  Json::Value root;

  std::ifstream config_doc(json_name, std::ifstream::binary);
  config_doc >> root;  


  bool use_coeffs_from_json = root.get("use_coeffs_from_json", false).asBool();

  if (use_coeffs_from_json) {
    printf("Using coeeficients from json\n");

    viscPF            = root.get("viscPF", 0).asFloat();
    viscPF_teta       = root.get("viscPF_teta", 0).asFloat();
    B_Koeff           = root.get("B_Koeff", 0).asFloat();
    dt                = root.get("dt", 0).asFloat();
    dt_viscPF_teta    = root.get("dt_viscPF_teta", 0).asFloat();
    dt_viscPF         = root.get("dt_viscPF", 0).asFloat();
    sqrt_PF_xy        = root.get("sqrt_PF_xy", 0).asFloat();
    sqrt_PF_teta      = root.get("sqrt_PF_teta", 0).asFloat();
    R_MT              = root.get("R_MT", 0).asFloat();
    A_Koeff           = root.get("A_Koeff", 0).asFloat();
    b_lat             = root.get("b_lat", 0).asFloat();
    A_long_D          = root.get("A_long_D", 0).asFloat();
    b_long_D          = root.get("b_long_D", 0).asFloat();
    A_long_T          = root.get("A_long_T", 0).asFloat();
    b_long_T          = root.get("b_long_T", 0).asFloat();
    ro0               = root.get("ro0", 0).asFloat();
    ro0_long          = root.get("ro0_long", 0).asFloat();
    inv_ro0_long      = root.get("inv_ro0_long", 0).asFloat();
    c_lat             = root.get("c_lat", 0).asFloat();
    d_lat             = root.get("d_lat", 0).asFloat();
    C_Koeff           = root.get("C_Koeff", 0).asFloat();
    Rad               = root.get("Rad", 0).asFloat();
    inv_ro0           = root.get("inv_ro0", 0).asFloat();
    clat_dlat_ro0     = root.get("clat_dlat_ro0", 0).asFloat();
    clong_dlong_ro0   = root.get("clong_dlong_ro0", 0).asFloat();
    d_lat_ro0         = root.get("d_lat_ro0", 0).asFloat();
    d_long_ro0        = root.get("d_long_ro0", 0).asFloat();
    fi_r              = root.get("fi_r", 0).asFloat();
    psi_r             = root.get("psi_r", 0).asFloat();
    fi_l              = root.get("fi_l", 0).asFloat();
    psi_l             = root.get("psi_l", 0).asFloat();
    rad_mon           = root.get("rad_mon", 0).asFloat();
    teta0_D           = root.get("teta0_D", 0).asFloat();
    teta0_T           = root.get("teta0_T", 0).asFloat();  

  } else {
    printf("Using default coeeficients\n");

    viscPF            = viscPF_d;         
    viscPF_teta       = viscPF_teta_d;    
    B_Koeff        = B_Koeff_d;         
    dt             = dt_d;           
    dt_viscPF_teta = dt_viscPF_teta_d;  
    dt_viscPF         = dt_viscPF_d;      
    sqrt_PF_xy     = sqrt_PF_xy_d;      
    sqrt_PF_teta      = sqrt_PF_teta_d;   
    R_MT           = R_MT_d;            
    A_Koeff        = A_Koeff_d;         
    b_lat          = b_lat_d;           
    A_long_D          = A_long_D_d;       
    b_long_D          = b_long_D_d;       
    A_long_T          = A_long_T_d;       
    b_long_T          = b_long_T_d;       
    ro0            = ro0_d;             
    ro0_long       = ro0_long_d;        
    inv_ro0_long   = inv_ro0_long_d;    
    c_lat          = c_lat_d;           
    d_lat          = d_lat_d;           
    C_Koeff        = C_Koeff_d;         
    Rad            = Rad_d;             
    inv_ro0        = inv_ro0_d;         
    clat_dlat_ro0     = clat_dlat_ro0_d;  
    clong_dlong_ro0   = clong_dlong_ro0_d;
    d_lat_ro0         = d_lat_ro0_d;      
    d_long_ro0     = d_long_ro0_d;      
    fi_r           = fi_r_d;            
    psi_r          = psi_r_d;        
    fi_l           = fi_l_d;         
    psi_l          = psi_l_d;           
    rad_mon        = rad_mon_d;         
    teta0_D        = teta0_D_d;         
    teta0_T        = teta0_T_d;       


  }


  attachment_probability = concentration * K_on * kinetic_step_attachment * dt / 13; //Probability per protofi lament
  hydrolysis_probability = K_hydrolysis * kinetic_step_hydrolysis * dt;

}



} // microtubule
