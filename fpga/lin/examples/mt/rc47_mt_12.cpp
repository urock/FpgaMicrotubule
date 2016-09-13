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



unsigned int n_layers; //length of MT with counting shifts/ should be a multiple of 3
unsigned int NStop[13]; //numbers of last monomers;
unsigned int NStart[13];   //numbers of first monomers;





mt_coords_t mt_coords_f;
mt_coords_t mt_coords_c;


vector<vector<int> > type_mol;   //types



////////////////////////////////////////////////




int fpga_board = -1;
int fpga_chip = -1; 

bool flag_cpu = false;
bool flag_fpga = false; 


bool flag_rand = false;
bool flag_file = false; 
char *out_file;

 
unsigned int N_d;
unsigned int k_steps = 100; 

void print_usage(char *argv[])
{
  cerr << "\nUsage:\n";
  cerr << "sudo " << argv[0] << " -j json_file -b board -v cs -N N_d -c -i it\n";   
  cerr << "-j json_file - input file with coefficients\n";
  cerr << "-b fpga board: [0-1]\n";
  cerr << "-v cs: fpga chip_select [0-3]\n";
  cerr << "-N N_d: number of molecules in one filament\n";
  cerr << "-c: set to use cpu \n";
  cerr << "-f: set to use fpga (-c and -f could be both set) \n";  
  cerr << "-i number of kinetic steps\n" << endl; 
}




int main(int argc, char *argv[])
{
  int dev;

  int opt;

  string json_file; 

  bool nd_set = false; 

  while ((opt = getopt(argc, argv, "j:b:v:N:c")) != -1) {
    switch (opt) {
      case 'j': json_file = string(optarg); break;
      case 'b': fpga_board = stoi(optarg); break;
      case 'v': fpga_chip = stoi(optarg); break;
      case 'N': N_d = stoi(optarg); nd_set =  true; break;
      case 'c': flag_cpu = true; break;
      case 'f': flag_fpga = true; break;      
      case 'i': k_steps = stoi(optarg); break; 
      default: print_usage(argv); break;
    }
  }

  if ((!flag_fpga) && (!flag_cpu)) {
    cerr << "No platform selected" << endl;
    return -1;
  }

  if (!nd_set) {
    cerr << "N_d not set!" << endl;
    print_usage(argv); 
    return -1; 
  }

  if ((N_d % 3) != ) {
    cerr << "N_d should be a multiple of 3. Exitting" << endl;
    return -2; 
  }

  if (flag_fpga) {
    FpgaDev Fpga;
    mt      MtFpga(&Fpga, fpga_board, fpga_chip, json_file, N_d);
  }
  
  if (flag_cpu) {
    mt MtCpu(json_file, N_d);
  }


   double dt_c[N];
   double dt_f[N];
   cout << "N_d -> " << N_d << endl;
   
   cout << "STEPS_TO_WRITE -> " << STEPS_TO_WRITE << endl;
   cout << "Kintetic steps -> " << k_steps << endl;



   error = 0; 

   
   for(int k=0; k<N; k++) {

      int err;
      struct timeval tt1, tt2;
      if (flag_cpu==1) {
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

      if (flag_cpu==1){
         // uncomment u1
         // err = compare_results(x_1,y_1,t_1,x_2,y_2,t_2);
         if (err) {
            error += err;
            printf("Compare results failed at step = %d, errors = %d\n", k, error);
         }
      }


//-------------------------------------- add reattach, kinetics---------------------------------------------------------------------------------------------------
      unsigned int i = 10, j = 34, j1;
      
      
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
      

      if (flag_cpu==1) print_coords(f_p, mt_coords_c); 
      print_coords(f_p1, mt_coords_f); 
      print_coords_type(f_type, type_mol); 
      float y_avg = 0;;
      for (i = 0; i < 13; i++) {
         y_avg += mt_coords_f.y[i][NStop[i]-1];
      }
      printf("avg_y %f \n", y_avg);
      
      fprintf(f_length,"%.3f \n", y_avg/13);


   }
   if (flag_cpu==1){
      if (!error)
      printf("Test OK!\n");
   }
   
   

  if (flag_cpu==1) fclose(f_p);
  fclose(f_p1);
  fclose(f_length);
  fclose(f_type);


  Fpga.StopRandomGenerator(dev); 

  if (Fpga.close(dev) < 0) {
    cerr << "Error in Fpga Close Device\n";
    return -1;         
  }

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


