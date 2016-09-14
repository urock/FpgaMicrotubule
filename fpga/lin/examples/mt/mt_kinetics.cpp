#include "mt.h"

using namespace std;

namespace microtubule {

  extern float Rad; 

  extern float attachment_probability;    //Probability per protofilament
  extern float hydrolysis_probability;  


  unsigned int max_N(unsigned int N_arr[13]) {
     int i;
     unsigned int NMax = 0;
     for (i = 0; i<13; i++)
        if (NMax < N_arr[i]) 
           NMax = N_arr[i];
     return NMax;
  }

  unsigned int min_N(unsigned int N_arr[13]) {
     int i;
     unsigned int NMin = N_arr[0];
     for (i = 0; i < 13; i++)
        if (NMin > N_arr[i]) 
           NMin = N_arr[i];
     return NMin;
  }

  float getDistance(float x1, float y1, float t1, float x2, float y2, float t2) {
     float r_long_x = (x1 - x2) - Rad*(sinf(t1) + sinf(t2));
     float r_long_y = (y1 - y2) - Rad*(cosf(t1) + cosf(t2));
     float r_long = sqrtf( r_long_x*r_long_x + r_long_y*r_long_y);
     return r_long;
  }



  void mt::addDimer (int i) {
     if (NStop[i] > N_d-1) return;
     //todo angle
     coords.x[i][NStop[i]] = coords.x[i][NStop[i] - 1] + 2*Rad*sinf(coords.t[i][NStop[i] - 1]);      
     coords.y[i][NStop[i]] = coords.y[i][NStop[i] - 1] + 2*Rad*cosf(coords.t[i][NStop[i] - 1]);
     coords.t[i][NStop[i]] = coords.t[i][NStop[i]-1];
     type_mol[i][NStop[i]] = 1; //T
     NStop[i]++;
     coords.x[i][NStop[i]] = coords.x[i][NStop[i] - 1] + 2*Rad*sinf(coords.t[i][NStop[i] - 1]);      
     coords.y[i][NStop[i]] = coords.y[i][NStop[i] - 1] + 2*Rad*cosf(coords.t[i][NStop[i] - 1]);
     coords.t[i][NStop[i]] = coords.t[i][NStop[i]-1];
     type_mol[i][NStop[i]] = 1; //T
     NStop[i]++;
     //type_mol[i][NStop[i]] = 0;  //todo  was 'D'      ??1045
  }



  // not used function??
  void mt::removeDimer (int i) {
  // void removeDimer (int i, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t) {
     coords.y[i][NStop[i] - 1] = -100;
     type_mol[i][NStop[i] - 1] = -1;
     coords.y[i][NStop[i] - 2] = -100;
     type_mol[i][NStop[i] - 2] = -1;
     NStop[i] -= 2;
  }



  void mt::shift_coords(bool up, int shift)
  {
     unsigned int i,j;
     
     if (up) {
        for (i=0; i<13; i++){
        for (j=NStart[i]; j<NStop[i]; j++){
           
          coords.x[i][j + shift] = coords.x[i][j];
          coords.y[i][j + shift] = coords.y[i][j];
          coords.t[i][j + shift] = coords.t[i][j];
          type_mol[i][j + shift] = type_mol[i][j];
        
        }
        NStop[i] += shift;
        }
        printf("UP!%f", coords.y[1][4+8]);
        //NStart[i] += shift;
        n_layers += shift;
        
        for (i=0; i<13; i++)
        for (j = shift-1; j >= 0; j-- ){
          coords.x[i][j] = coords.x[i][j + 1];     //?need to test
          coords.y[i][j] = coords.y[i][j + 1] - 2*Rad;
          coords.t[i][j] = 0;
          type_mol[i][j] = 0;  //todo  was 'D'
        }
        
     } else {                     // down
        for (i=0; i<13; i++){
        for (j=shift; j<NStop[i]; j++){
           
           coords.x[i][j - shift] = coords.x[i][j];
           coords.y[i][j - shift] = coords.y[i][j];
           coords.t[i][j - shift] = coords.t[i][j];
           type_mol[i][j - shift] = type_mol[i][j];
        
        }
        NStop[i] -= shift;
        }
        //NStart[i] -= shift;
        n_layers -= shift;
     }
     
     for (i=0; i<13; i++)
     for (j = NStop[i]; j < N_d; j++ ){
        coords.x[i][j] = 0;      
        coords.y[i][j] = -100.0;
        coords.t[i][j] = 0;
        type_mol[i][j] = -1; //todo was '-' 
     }
     
  }



  void mt::choose_to_shift_coords() {

     unsigned int NStopMax = max_N(NStop), NStopMin = min_N(NStop);
     /*
     if (NStopMax >= N_d - 5 && NStopMin <= 16) { //Error
        printf("Error!!! NStopMax >= N_d - 5 && NStopMin <= 16 N_d = %i \n", N_d);
     }
     if (NStopMax >= N_d - 8 && NStopMin <= 3) {  //Error
        printf("NStopMax >= N_d - 8 && NStopMin <= 3N_d = %i \n", N_d);
     }
     if (NStopMax >= N_d - 5)
        shift_coords(x, y, t, false, 16);
     if (NStopMin <= 4){
        //printf("%iUOP:  \n",NStop[1]);
        //for (int i = 0; i < 10; i++)
        // printf("%iUOP: %f \n",i, y_2[1][i]);
        shift_coords(x, y, t, true, 8);
        //for (int i = 0; i < 10; i++)
        // printf("%iUOP: %f \n",i, y_2[1][i]);
        //printf("%iUOP:  \n",NStop[1]);
        //printf("UP!");
        }*/
        
     if (NStopMax >= N_d - 5 && NStopMin <= 7) {  //Error
        printf("Error!!! NStopMax >= N_d - 5 && NStopMin <= 16 N_d = %i \n", N_d);
     }
     if (NStopMax >= N_d - 6 && NStopMin <= 3) {  //Error
        printf("NStopMax >= N_d - 8 && NStopMin <= 3N_d = %i \n", N_d);
     }
     if (NStopMax >= N_d - 5)
        shift_coords(false, 4);

     if (NStopMin <= 4) {
        //printf("%iUOP:  \n",NStop[1]);
        //for (int i = 0; i < 10; i++)
        // printf("%iUOP: %f \n",i, y_2[1][i]);
        shift_coords(true, 4);
        //for (int i = 0; i < 10; i++)
        // printf("%iUOP: %f \n",i, y_2[1][i]);
        //printf("%iUOP:  \n",NStop[1]);
        //printf("UP!");
        }
  }




  void mt::polimerization_algorithm() {
     
      
     unsigned int i, j;
     float gen_rand;
     for(i = 0; i < 13; i++) {

        gen_rand = KineticRand->get_uni_num();
     
        if(gen_rand <= attachment_probability){      
            addDimer(i);
        }
        
        int dim_calc = 1;
        for (j=NStart[i]; j<NStop[i]; j++){
           //gen_rand = Rnd_pol.genrand_real2();
           if(dim_calc == 0) {
              if( type_mol[i][j] == 1 && type_mol[i][j-1] == 1){          
                    if(KineticRand->get_uni_num() <= hydrolysis_probability){
                       //printf("%i %iHYDRO:  \n",i, j);
                       type_mol[i][j] = 0;  
                       type_mol[i][j-1] = 0;
                    }
                 }
           }
           dim_calc++;
           if(dim_calc == 2) dim_calc = 0;
        }
     }
  }


  void mt::calc_kinetics() {
  
    unsigned int i,j,j1; 
      
    for (i=0; i<13; i++) {
      for (j=NStart[i]; j<NStop[i]-1; j++) {
          if (getDistance(coords.x[i][j+1], coords.y[i][j+1], coords.t[i][j+1], 
                          coords.x[i][j],   coords.y[i][j],   coords.t[i][j] ) >= cut_off) { //molecule deattached
             //printf("deattached :%i %i r = %f\n", i, j,getDistance(x_2[i][j+1], y_2[i][j+1], t_2[i][j+1], x_2[i][j], y_2[i][j], t_2[i][j]));
             for (j1=j; j1<NStop[i]; j1++) {
                coords.y[i][j1] = -100;
                type_mol[i][j1] = -1;   //'-'
                printf("deattached :%i %i\n", i, j1);
                //todo type change
             } 
             NStop[i] = j;
          }
      }
    }
    
    polimerization_algorithm();
    
    choose_to_shift_coords();    
  }



} // microtubule
