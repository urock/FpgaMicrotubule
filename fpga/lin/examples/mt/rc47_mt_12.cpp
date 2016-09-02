/********************************************
* rc47_mt.c
*
*
*  to work with rc_mt_2000_12 project
*
*******************************************/


#include "RostaPCIeAPI.h"
#include "rc47-api.h"

#include "rc_47.h"
#include "rand.h"
#include "fpga.h"
#include "mt.h"
// #include "mt_defines.h"


// #include "jsoncpp/json/json-forwards.h"
// #include "jsoncpp/json/json.h"

#include <fstream>
#include <iostream>
#include <string>  

using namespace std;
using namespace microtubule;

void init_coords(mt_coords_t  &mt_coords);




unsigned int n_layers; //length of MT with counting shifts/ should be a multiple of 3
unsigned int NStop[13]; //numbers of last monomers;
unsigned int NStart[13];   //numbers of first monomers;

int flag_seed = 1;


// coefficients
float viscPF            = viscPF_d;             
float viscPF_teta       = viscPF_teta_d;        
float B_Koeff        = B_Koeff_d;         
float dt             = dt_d;           
float dt_viscPF_teta = dt_viscPF_teta_d;  
float dt_viscPF         = dt_viscPF_d;       
float sqrt_PF_xy     = sqrt_PF_xy_d;      
float sqrt_PF_teta      = sqrt_PF_teta_d;       
float R_MT           = R_MT_d;            
float A_Koeff        = A_Koeff_d;         
float b_lat          = b_lat_d;           
float A_long_D          = A_long_D_d;        
float b_long_D          = b_long_D_d;        
float A_long_T          = A_long_T_d;        
float b_long_T          = b_long_T_d;        
float ro0            = ro0_d;             
float ro0_long       = ro0_long_d;        
float inv_ro0_long   = inv_ro0_long_d;    
float c_lat          = c_lat_d;           
float d_lat          = d_lat_d;           
float C_Koeff        = C_Koeff_d;         
float Rad            = Rad_d;             
float inv_ro0        = inv_ro0_d;         
float clat_dlat_ro0     = clat_dlat_ro0_d;      
float clong_dlong_ro0   = clong_dlong_ro0_d; 
float d_lat_ro0         = d_lat_ro0_d;       
float d_long_ro0     = d_long_ro0_d;      
float fi_r           = fi_r_d;            
float psi_r          = psi_r_d;        
float fi_l           = fi_l_d;         
float psi_l          = psi_l_d;           
float rad_mon        = rad_mon_d;         
float teta0_D        = teta0_D_d;         
float teta0_T        = teta0_T_d;   

bool use_coeffs_from_json;


double attachment_probability = concentration * K_on * kinetic_step_attachment * dt / 13; //Probability per protofilament
double hydrolysis_probability = K_hydrolysis * kinetic_step_hydrolysis * dt;



mt_coords_t mt_coords_f;
mt_coords_t mt_coords_c;


vector<vector<int> > type_mol;   //types

unsigned int seeds[NUM_SEEDS];

//#define TEST_SEEDS 

const unsigned int test_seeds[NUM_SEEDS] = { 0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 
                           0x60000000, 0x70000000, 0x80000000, 0x90000000, 0xA0000000  };


#define STEPS_TO_WRITE     500000         // steps in fpga. was 1000 ; 5000000*100 to almost destroy  //500000

#define N   80          // steps to write   number of time steps was 100?

#define TOTAL_STEPS        (STEPS_TO_WRITE*N)      

#define absTol    0.001f
#define relTol    0.001f

#define cut_off 1.0     //for deattach

#define nullHigh 10        //indent from top at the beginning

/////////////////////////////////////////////////




int fpga_board = -1;
int fpga_chip = -1; 

bool flag_compare = false;
bool flag_rand = false;
bool flag_file = false; 
char *out_file;

string coord_out_file; 
 
unsigned int N_d;

void print_usage(char *argv[])
{
   cerr << "\nUsage:\n";
   cerr << "sudo " << argv[0] << " -j json_file -b board -v chip_select -N N_d -c=flag_compare -r=flar_rand -f output file name]\n";   
   cerr << "json_file - input file with coefficients\n";
   cerr << "board: 0-1\n";
   cerr << "chip_select: 0-3\n";
   cerr << "N_d: number of molecules in one filament\n" << endl; 
}




int main(int argc, char *argv[])
{
  int dev;

  int opt;

  string json_file; 

  bool nd_set = false; 

  while ((opt = getopt(argc, argv, "j:b:v:N:f:cr")) != -1) {
    switch (opt) {
      case 'j': json_file = string(optarg); break;
      case 'b': fpga_board = stoi(optarg); break;
      case 'v': fpga_chip = stoi(optarg); break;
      case 'N': N_d = stoi(optarg); nd_set =  true; break;
      case 'f': coord_out_file = string(optarg); flag_file = true; break;
      case 'c': flag_compare = true; break;
      case 'r': flag_rand = true; break;
      default: print_usage(argv); break;
    }
  }

  if (!nd_set) {
    cerr << "N_d not set!" << endl;
    print_usage(argv); 
    return -1; 
  }

  FpgaDev Fpga;
  mt      MtFpga(&Fpga, json_file, N_d);
  mt      MtCpu(json_file, N_d);

  if (Fpga.FindDevices() < 0) {
    cerr << "Error in FindDevices\n";
    return -1;
  }


  dev = Fpga.open(fpga_board, fpga_chip);

  if (dev == -1) {
    cerr << "Error in Fpga Open Device\n";
    return -1;      
  }







  float coeff_buf[Nc];

  if (MtFpga.UseJsonCoeefs(coeff_buf)) {

    for (int i = 0; i < Nc; ++i) {
      cout << coeff_buf[i] << endl;
    }

    Fpga.LoadCoeffs(dev, Nc, coeff_buf);       

  }  
  
  if (Fpga.close(dev) < 0) {
    cerr << "Error in Fpga Close Device\n";
    return -1;         
  }


  return 0;


   double dt_c[N];
   double dt_f[N];
   printf("N_d is %d\n",N_d);
   
   
   printf("size is %d\n",(int)SIZE_DWORD);
   
   try{
      
      type_mol.resize(13);
      mt_coords_f.x.resize(13);
      mt_coords_f.y.resize(13);
      mt_coords_f.t.resize(13);
      mt_coords_c.x.resize(13);
      mt_coords_c.y.resize(13);
      mt_coords_c.t.resize(13);
      for (int i = 0; i < 13; i++){
         type_mol[i].resize(N_d);
         mt_coords_f.x[i].resize(N_d);
         mt_coords_f.y[i].resize(N_d);
         mt_coords_f.t[i].resize(N_d);
         mt_coords_c.x[i].resize(N_d);
         mt_coords_c.y[i].resize(N_d);
         mt_coords_c.t[i].resize(N_d);
      }
      
   } catch (const  std::bad_alloc& ba){
      std::cout << "bad_alloc caught: " << ba.what() << std::endl;
   }  
   FILE  *f_p, *f_p1, *f_length, *f_type;

   int error = 0;
   if (flag_compare==1){
      f_p = fopen ("MT_coords_CPU.txt","w");
      if (f_p==NULL) {

         printf("Error opening file!\n");
         return -1;
      }
      
   }
   
   if (flag_file)    f_p1 = fopen (out_file,"w");
   else           f_p1 = fopen ("MT_coords_FPGA.txt","w");
   
   f_length = fopen ("len_MT_coords_FPGA.txt","w");
   f_type = fopen ("type_MT_coords_FPGA.txt","w");
   
   if (f_p1==NULL) {

      printf("Error opening file!\n");
      return -1;
   }
   
   


   printf("TOTAL_STEPS = %d\nSTEPS_TO_WRITE = %d\n", TOTAL_STEPS, STEPS_TO_WRITE);

   for(int i = 0; i < 13; i++) {
      NStop[i] = N_d - nullHigh;
   }
   for(int i = 0; i < 13; i++) {
      NStart[i] = 0;
   }
   n_layers = NStop[0] - NStart[0];

   // get golden results
   init_coords(mt_coords_f);
   init_coords(mt_coords_c);
   


   error = 0; 

   
   printf("\nFlag rand is SET, %d\n\n",flag_rand);
   
   if (flag_rand==1)      {
      
      srand (time(NULL));
      
      // set seed vals
      for (int i =0; i < NUM_SEEDS; ++i){
#ifdef TEST_SEEDS
            seeds[i] = test_seeds[i];
#else
            seeds[i]=(unsigned int)rand();
#endif
      }   

      Fpga.StartRandomGenerator(dev,NUM_SEEDS, seeds); 
      
      printf("\nFlag rand is SET\n\n");
      
   }
   
   
   printf("\n\nhereerereerere\n\n");

   for(int k=0; k<N; k++) {

      int err;
      struct timeval tt1, tt2;
      if (flag_compare==1) {
         get_time(&tt1);
         if (mt_cpu(STEPS_TO_WRITE, flag_rand, flag_seed, seeds,  mt_coords_c,  N_d)<0) { 
            printf("Nan Error in cpu. Step is %d. Exitting....\n",k); 
            break;
         }

         get_time(&tt2);
         calc_dt(&tt1,&tt2, &dt_c[k]);
      }
      
      get_time(&tt1);

      // TODO: work on n_layers var
      if (Fpga.CalcDynamics(dev,STEPS_TO_WRITE, n_layers, mt_coords_f, type_mol) < 0) {
        cerr << "Error int Fpga.CalcDynamics" << endl;
        return -1; 
      }

      get_time(&tt2);
      calc_dt(&tt1,&tt2, &dt_f[k]);

      flag_seed = 0; 
      
      printf("Step %d\n\t CPU Time = %f\n\t FPGA Time = %f\n",k,dt_c[k],dt_f[k] );

      if (flag_compare==1){
         // uncomment u1
         // err = compare_results(x_1,y_1,t_1,x_2,y_2,t_2);
         if (err) {
            error += err;
            printf("Compare results failed at step = %d, errors = %d\n", k, error);
         }
      }


//-------------------------------------- add reattach, kinetics---------------------------------------------------------------------------------------------------
      /*if (temp == 12) {
         temp = 0;
      }
      */
      
      /*
   for(i = 0; i < 13; i++) {
      gen_rand = Rnd_pol.genrand_real2();
      temp++;*/
      //for (temp=0; temp<24; temp++)
      //printf("y_2[0][] = %f\n", y_2[0][temp]);
      //removeDimer(temp, x_2,y_2,t_2);
      //printf("N[5] = %i, max = %i, min = %i, y = %fy = %fy = %fy = %fy = %f\n", NStop[5], max_N(NStop), min_N(NStop), y_2[1][0], y_2[1][1], y_2[1][2], y_2[1][3], y_2[1][4], y_2[1][5]);
      unsigned int i = 10, j = 34, j1;
      
      
      /*
      printf("y_2[11][11] = %f\n", y_2[11][11]);
      printf("y_2[11][10] = %f\n", y_2[11][10]);
      printf("y_2[11][9] = %f\n", y_2[11][9]);
      printf("y_2[10][11] = %f\n", y_2[10][11]);
      printf("y_2[12][11] = %f\n", y_2[12][11]);*/
      /*for (i=0; i<13; i++) {
         for (j=0; j<N_d; j++) {
            if (y_2[i][j] == -100) printf("NONONONO");
         }
      }*/
      //fixed -> check?, fix 250!
      //printf("r = %f\n", getDistance(x_2[1][j+1], y_2[i][j+1], t_2[i][j+1], x_2[i][j], y_2[i][j], t_2[i][j]));
      

      for (i=0; i<13; i++) {
         for (j=NStart[i]; j<NStop[i]-1; j++) {
            if (getDistance(mt_coords_f.x[i][j+1], mt_coords_f.y[i][j+1], mt_coords_f.t[i][j+1], 
                            mt_coords_f.x[i][j],   mt_coords_f.y[i][j],   mt_coords_f.t[i][j] ) >= cut_off) { //molecule deattached
               //printf("deattached :%i %i r = %f\n", i, j,getDistance(x_2[i][j+1], y_2[i][j+1], t_2[i][j+1], x_2[i][j], y_2[i][j], t_2[i][j]));
               for (j1=j; j1<NStop[i]; j1++) {
                  mt_coords_f.y[i][j1] = -100;
                  type_mol[i][j1] = -1;   //'-'
                  printf("deattached :%i %i\n", i, j1);
                  //todo type change
               } 
               NStop[i] = j;
            }
         }
      }
      
      polimerization_algorithm(mt_coords_f, (float) tt1.tv_usec);
      
      choose_to_shift_coords(mt_coords_f);
      
      
//-------------------------------------- add reattach, kinetics---------------------------------------------------------------------------------------------------
      

      if (flag_compare==1) print_coords(f_p, mt_coords_c); 
      print_coords(f_p1, mt_coords_f); 
      print_coords_type(f_type, type_mol); 
      float y_avg = 0;;
      for (i = 0; i < 13; i++) {
         y_avg += mt_coords_f.y[i][NStop[i]-1];
      }
      printf("avg_y %f \n", y_avg);
      
      fprintf(f_length,"%.3f \n", y_avg/13);


   }
   if (flag_compare==1){
      if (!error)
      printf("Test OK!\n");
   }
   
   

  if (flag_compare==1) fclose(f_p);
  fclose(f_p1);
  fclose(f_length);
  fclose(f_type);


  Fpga.StopRandomGenerator(dev); 


  Fpga.close(dev);
  return 0; 

}






   








void print_coords(FILE *f_p, mt_coords_t  &mt_coords)
{
   unsigned int i,j;

   for (i=0; i<13; i++) {
      for (j=0; j<N_d; j++)
      fprintf(f_p,"%.3f\t  ", mt_coords.x[i][j]);

      for (j=0; j<N_d; j++)
      fprintf(f_p,"%.3f\t  ", mt_coords.y[i][j]);

      for (j=0; j<N_d; j++)
      fprintf(f_p,"%.3f\t  ", mt_coords.t[i][j]);
      
   }

   fprintf(f_p,"\n");

}

void print_coords_type(FILE *f_p, vector<vector<int> > &type_mol)
{
   unsigned int i,j;

   for (i=0; i<13; i++) {
      
      for (j=0; j<N_d; j++)
      fprintf(f_p,"%i\t  ", type_mol[i][j]);
   }

   fprintf(f_p,"\n");

}




// void init_coords(
//    vector<vector<float> >  &  x, 
//    vector<vector<float> >  &  y, 
//    vector<vector<float> >  &  t)
void init_coords(mt_coords_t  &mt_coords)
{
   unsigned int i,j;


   // задание y координаты для нижней спирали
   for (i=0; i<13; i++)
   mt_coords.y[i][0] = 2.0f*6/13*(i+1);


   // задание y координат для остальных молекул до половины высоты трубочки
   for (j=1; j<N_d; j++)//-4
   for (i=0; i<13; i++)
   mt_coords.y[i][j] = mt_coords.y[i][j-1] + 2.0f*Rad;


   // задание x и teta координат так чтобы был цилиндр до половины высоты трубочки
   for (j=0; j<N_d; j++)//-5
   for (i=0; i<13; i++)  {

      mt_coords.x[i][j] = 0.0;
      mt_coords.t[i][j] = 0.0;
   }

/*
   //
   for (i=0; i<13; i++)  {

      x[i][N_d-5] = 0.6;
      t[i][N_d-5] = 0.2;

   }


   for (j=N_d-4; j<N_d; j++)
   for (i=0; i<13; i++)  {

      x[i][j] = x[i][j-1] + 2*Rad*sinf(t[i][j-1]);
      y[i][j] = y[i][j-1] + 2*Rad*cosf(t[i][j-1]);
      t[i][j] = t[i][j-1];

   }
*/
   for (i=0; i<13; i++){
      for (j=NStart[i]; j<NStop[i]; j++){ 
         type_mol[i][j] = 1;  // T
      }
      /*for(j = NStart[i]-1; j >= 0; j-- ){
      type_mol[i][j] = 0;  *///  'D'
      
   for(j = NStop[i]; j < N_d; j++){
      mt_coords.x[i][j] = 0;      
      mt_coords.y[i][j] = -100.0;
      mt_coords.t[i][j] = 0;
      type_mol[i][j] = -1; // '-'   
   }
   }
}





int compare_results(vector<vector<float> >  & x_1, vector<vector<float> >  & y_1, vector<vector<float> >  & t_1,
vector<vector<float> >  &  x_2, vector<vector<float> >  & y_2, vector<vector<float> >  & t_2 )
{
   unsigned int i,j;
   int error = 0;
   for(i = 0; i<13; i++)
   for(j = 0; j<N_d; j++) {

      if ((x_1[i][j]!=x_1[i][j])||(x_2[i][j]!=x_2[i][j])||(!equal(x_1[i][j], x_2[i][j])))
      error++;
      if ((y_1[i][j]!=y_1[i][j])||(y_2[i][j]!=y_2[i][j])||(!equal(y_1[i][j], y_2[i][j])))
      error++;
      if ((t_1[i][j]!=t_1[i][j])||(t_2[i][j]!=t_2[i][j])||(!equal(t_1[i][j], t_2[i][j])))
      error++;

   }


   return error;
}



void calc_dt(struct timeval *tt1, struct timeval *tt2, double *tsec)
{
   double ttv1 = ((double) tt1->tv_usec) / 1000000 + tt1->tv_sec;
   double ttv2 = ((double) tt2->tv_usec) / 1000000 + tt2->tv_sec;
   *tsec = ttv2 - ttv1;
}

float max(float A, float B){
   if(A>=B) return A;
   else return B;
}

int equal(float A, float B)
{
   return (fabs(A - B) <= max(absTol, relTol * max(fabs(A), fabs(B))));

}

float getDistance(float x1, float y1, float t1, float x2, float y2, float t2) {
   float r_long_x = (x1 - x2) - Rad*(sinf(t1) + sinf(t2));
   float r_long_y = (y1 - y2) - Rad*(cosf(t1) + cosf(t2));
   float r_long = sqrtf( r_long_x*r_long_x + r_long_y*r_long_y);
   return r_long;
}

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

void choose_to_shift_coords(mt_coords_t &mt_coords) {
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
      shift_coords(mt_coords, false, 4);
   if (NStopMin <= 4){
      //printf("%iUOP:  \n",NStop[1]);
      //for (int i = 0; i < 10; i++)
      // printf("%iUOP: %f \n",i, y_2[1][i]);
      shift_coords(mt_coords, true, 4);
      //for (int i = 0; i < 10; i++)
      // printf("%iUOP: %f \n",i, y_2[1][i]);
      //printf("%iUOP:  \n",NStop[1]);
      //printf("UP!");
      }
}

void shift_coords(mt_coords_t &mt_coords, bool up, const int shift)
{
   unsigned int i,j;
   
   if (up) {
      for (i=0; i<13; i++){
      for (j=NStart[i]; j<NStop[i]; j++){
         
        mt_coords.x[i][j + shift] = mt_coords.x[i][j];
        mt_coords.y[i][j + shift] = mt_coords.y[i][j];
        mt_coords.t[i][j + shift] = mt_coords.t[i][j];
        type_mol[i][j + shift] = type_mol[i][j];
      
      }
      NStop[i] += shift;
      }
      printf("UP!%f", mt_coords.y[1][4+8]);
      //NStart[i] += shift;
      n_layers += shift;
      
      for (i=0; i<13; i++)
      for (j = shift-1; j >= 0; j-- ){
        mt_coords.x[i][j] = mt_coords.x[i][j + 1];     //?need to test
        mt_coords.y[i][j] = mt_coords.y[i][j + 1] - 2*Rad;
        mt_coords.t[i][j] = 0;
        type_mol[i][j] = 0;  //todo  was 'D'
      }
      
   } else {
      for (i=0; i<13; i++){
      for (j=shift; j<NStop[i]; j++){
         
         mt_coords.x[i][j - shift] = mt_coords.x[i][j];
         mt_coords.y[i][j - shift] = mt_coords.y[i][j];
         mt_coords.t[i][j - shift] = mt_coords.t[i][j];
         type_mol[i][j - shift] = type_mol[i][j];
      
      }
      NStop[i] -= shift;
      }
      //NStart[i] -= shift;
      n_layers -= shift;
   }
   
   for (i=0; i<13; i++)
   for (j = NStop[i]; j < N_d; j++ ){
      mt_coords.x[i][j] = 0;      
      mt_coords.y[i][j] = -100.0;
      mt_coords.t[i][j] = 0;
      type_mol[i][j] = -1; //todo was '-' 
   }
   
}

void addDimer (int i, mt_coords_t &mt_coords) {
   if (NStop[i] > N_d-1) return;
   //todo angle
   mt_coords.x[i][NStop[i]] = mt_coords.x[i][NStop[i] - 1] + 2*Rad*sinf(mt_coords.t[i][NStop[i] - 1]);      
   mt_coords.y[i][NStop[i]] = mt_coords.y[i][NStop[i] - 1] + 2*Rad*cosf(mt_coords.t[i][NStop[i] - 1]);
   mt_coords.t[i][NStop[i]] = mt_coords.t[i][NStop[i]-1];
   type_mol[i][NStop[i]] = 1; //T
   NStop[i]++;
   mt_coords.x[i][NStop[i]] = mt_coords.x[i][NStop[i] - 1] + 2*Rad*sinf(mt_coords.t[i][NStop[i] - 1]);      
   mt_coords.y[i][NStop[i]] = mt_coords.y[i][NStop[i] - 1] + 2*Rad*cosf(mt_coords.t[i][NStop[i] - 1]);
   mt_coords.t[i][NStop[i]] = mt_coords.t[i][NStop[i]-1];
   type_mol[i][NStop[i]] = 1; //T
   NStop[i]++;
   //type_mol[i][NStop[i]] = 0;  //todo  was 'D'      ??1045
}



// not used function??
void removeDimer (int i, mt_coords_t &mt_coords) {
// void removeDimer (int i, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t) {
   mt_coords.y[i][NStop[i] - 1] = -100;
   type_mol[i][NStop[i] - 1] = -1;
   mt_coords.y[i][NStop[i] - 2] = -100;
   type_mol[i][NStop[i] - 2] = -1;
   NStop[i] -= 2;
}
   //t1 = time elapsed, print to console
void polimerization_algorithm(mt_coords_t &mt_coords, float t1) {
   srand((unsigned) time(NULL));
   unsigned long InitRnd = (t1-(int)t1)*1e+10 + rand();  
   Random Rnd_pol(InitRnd*1.7 + 0.35*rand());
   unsigned int i, j;
   float gen_rand;
   for(i = 0; i < 13; i++) {
      gen_rand = Rnd_pol.genrand_real2();
   
      if(gen_rand <= attachment_probability){      
            addDimer(i, mt_coords);
      }
      
      int dim_calc = 1;
      for (j=NStart[i]; j<NStop[i]; j++){
         //gen_rand = Rnd_pol.genrand_real2();
         if(dim_calc == 0) {
            if( type_mol[i][j] == 1 && type_mol[i][j-1] == 1){          
                  if(Rnd_pol.genrand_real2() <= hydrolysis_probability){
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


