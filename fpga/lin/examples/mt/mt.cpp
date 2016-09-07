#include "mt.h"

using namespace std;

namespace microtubule {

  const unsigned int test_seeds[NUM_SEEDS] = { 0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 
                           0x60000000, 0x70000000, 0x80000000, 0x90000000, 0xA0000000  };


  void mt::init(string json_name, unsigned int N_d_in) {

    N_d = N_d_in;


    Json::Value root;

    std::ifstream config_doc(json_name, std::ifstream::binary);
    config_doc >> root;  


    use_coeffs_from_json = false;
    use_coeffs_from_json = root.get("use_json", false).asBool();


    brownian_en = root.get("brownian_en", false).asBool(); 
    kinetics_en = root.get("kinetics_en", false).asBool(); 
    const_seeds = root.get("const_seeds", false).asBool();



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

      viscPF          = viscPF_d;         
      viscPF_teta     = viscPF_teta_d;    
      B_Koeff         = B_Koeff_d;         
      dt              = dt_d;           
      dt_viscPF_teta  = dt_viscPF_teta_d;  
      dt_viscPF       = dt_viscPF_d;      
      sqrt_PF_xy      = sqrt_PF_xy_d;      
      sqrt_PF_teta    = sqrt_PF_teta_d;   
      R_MT            = R_MT_d;            
      A_Koeff         = A_Koeff_d;         
      b_lat           = b_lat_d;           
      A_long_D        = A_long_D_d;       
      b_long_D        = b_long_D_d;       
      A_long_T        = A_long_T_d;       
      b_long_T        = b_long_T_d;       
      ro0             = ro0_d;             
      ro0_long        = ro0_long_d;        
      inv_ro0_long    = inv_ro0_long_d;    
      c_lat           = c_lat_d;           
      d_lat           = d_lat_d;           
      C_Koeff         = C_Koeff_d;         
      Rad             = Rad_d;             
      inv_ro0         = inv_ro0_d;         
      clat_dlat_ro0   = clat_dlat_ro0_d;  
      clong_dlong_ro0 = clong_dlong_ro0_d;
      d_lat_ro0       = d_lat_ro0_d;      
      d_long_ro0      = d_long_ro0_d;      
      fi_r            = fi_r_d;            
      psi_r           = psi_r_d;        
      fi_l            = fi_l_d;         
      psi_l           = psi_l_d;           
      rad_mon         = rad_mon_d;         
      teta0_D         = teta0_D_d;         
      teta0_T         = teta0_T_d;       


    }


    attachment_probability = concentration * K_on * kinetic_step_attachment * dt / 13; //Probability per protofi lament
    hydrolysis_probability = K_hydrolysis * kinetic_step_hydrolysis * dt;


    try{
       
       type_mol.resize(13);
       coords.x.resize(13);
       coords.y.resize(13);
       coords.t.resize(13);
       for (int i = 0; i < 13; i++){
          type_mol[i].resize(N_d);
          coords.x[i].resize(N_d);
          coords.y[i].resize(N_d);
          coords.t[i].resize(N_d);
       }
       
    } catch (const  std::bad_alloc& ba){
       std::cerr << "bad_alloc caught: " << ba.what() << std::endl;
       throw;
    }     

    string icf_name; 
    string ocf_name; 
    string olf_name; 
    string otf_name;

    icf_name = root.get("icf", "").asString();

    if (Fpga) {

      ocf_name = root.get("ocf_fpga", "").asString();
      olf_name = root.get("olf_fpga", "").asString();
      otf_name = root.get("otf_fpga", "").asString();

    } else {

      ocf_name = root.get("ocf_cpu", "").asString();
      olf_name = root.get("olf_fpga", "").asString();
      otf_name = root.get("otf_fpga", "").asString();
    }

    //icf = fopen(icf_name, "r");
    ocf = fopen(ocf_name, "w");
    olf = fopen(olf_name, "w");
    otf = fopen(otf_name, "w");

    if ((ocf == NULL) || (olf == NULL) || (otf == NULL))
      throw; 

    for(int i = 0; i < 13; ++i) {
      NStop[i] = N_d - 1;
      NStart[i] = NStop[i] - nullHigh;
    }
    
    n_layers = NStop[0] - NStart[0];

    if (brownian_en) {
      // set seed vals
      for (int i =0; i < NUM_SEEDS; ++i){
        if (const_seeds)
          seeds[i] = test_seeds[i];
        else
          seeds[i]=(unsigned int)rand();
      }

    }    

  }


  mt::mt(string json_name, unsigned int N_d_in) : Fpga (NULL) {

    std::cout << "MT constructor CPU" << std::endl;

    // if (Fpga == NULL)
    //   std::cout << "Fpga = NULL" << std::endl;
    
    init(json_name, N_d_in); 

  }


  mt::mt(FpgaDev *Fpga_, string json_name, unsigned int N_d_in) : Fpga (Fpga_)

  {

    std::cout << "MT constructor FPGA" << std::endl;

    // if (Fpga->FindDevices() < 0) {
    //   std::cerr << "Error in FindDevices\n";
    // }


    init(json_name, N_d_in); 

  }



  bool mt::UseJsonCoeefs(float *cooefs_buf_out) {

  if (use_coeffs_from_json) {

      cooefs_buf_out[0]  = viscPF;         
      cooefs_buf_out[1]  = viscPF_teta;    
      cooefs_buf_out[2]  = B_Koeff;        
      cooefs_buf_out[3]  = dt;
      cooefs_buf_out[4]  = dt_viscPF_teta; 
      cooefs_buf_out[5]  = dt_viscPF;      
      cooefs_buf_out[6]  = sqrt_PF_xy;     
      cooefs_buf_out[7]  = sqrt_PF_teta;   
      cooefs_buf_out[8]  = R_MT;           
      cooefs_buf_out[9]  = A_Koeff;        
      cooefs_buf_out[10] = b_lat;          
      cooefs_buf_out[11] = A_long_D;       
      cooefs_buf_out[12] = b_long_D;       
      cooefs_buf_out[13] = A_long_T;       
      cooefs_buf_out[14] = b_long_T;       
      cooefs_buf_out[15] = ro0;            
      cooefs_buf_out[16] = ro0_long;       
      cooefs_buf_out[17] = inv_ro0_long;   
      cooefs_buf_out[18] = c_lat;          
      cooefs_buf_out[19] = d_lat;          
      cooefs_buf_out[20] = C_Koeff;        
      cooefs_buf_out[21] = Rad;            
      cooefs_buf_out[22] = inv_ro0;        
      cooefs_buf_out[23] = clat_dlat_ro0;  
      cooefs_buf_out[24] = clong_dlong_ro0;
      cooefs_buf_out[25] = d_lat_ro0;      
      cooefs_buf_out[26] = d_long_ro0;     
      cooefs_buf_out[27] = fi_r;           
      cooefs_buf_out[28] = psi_r;          
      cooefs_buf_out[29] = fi_l;           
      cooefs_buf_out[30] = psi_l;          
      cooefs_buf_out[31] = rad_mon;        
      cooefs_buf_out[32] = teta0_D;        
      cooefs_buf_out[33] = teta0_T;     


    }

    return use_coeffs_from_json; 
  }




} // microtubule
