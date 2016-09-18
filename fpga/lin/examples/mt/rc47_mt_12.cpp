/********************************************
* rc47_mt.c
*
*
*  to work with rc_mt_2000_12 project
*
*******************************************/

#include <fstream>
#include <iostream>
#include <string>  
#include <vector>

#include "mt.h"


using namespace std;
using namespace microtubule;


void calc_dt(struct timeval *tt1, struct timeval *tt2, double *tsec); 
int compare_results(mt_coords_t *c1, mt_coords_t *c2);


int fpga_board = -1;
int fpga_chip = -1; 

bool use_cpu = false;
bool use_fpga = false; 


 
unsigned int N_d;
unsigned int k_steps = 100; 

void print_usage(char *argv[])
{
  cerr << "\nUsage:\n";
  cerr << "sudo " << argv[0] << " -j json_file -b board -v cs -N N_d -c -f -i it\n";   
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
  int opt;

  string json_file; 

  bool nd_set = false; 

  mt  *mt_fpga;
  mt  *mt_cpu;
  FpgaDev *Fpga;

  while ((opt = getopt(argc, argv, "j:b:v:N:cfi:")) != -1) {
    switch (opt) {
      case 'j': json_file = string(optarg); break;
      case 'b': fpga_board = stoi(optarg); break;
      case 'v': fpga_chip = stoi(optarg); break;
      case 'N': N_d = stoi(optarg); nd_set =  true; break;
      case 'c': use_cpu = true; break;
      case 'f': use_fpga = true; break;      
      case 'i': k_steps = stoi(optarg); break; 
      default: print_usage(argv); break;
    }
  }

  if ((!use_fpga) && (!use_cpu)) {
    cerr << "No platform selected" << endl;
    return -1;
  }

  if (!nd_set) {
    cerr << "N_d not set!" << endl;
    print_usage(argv); 
    return -1; 
  }

  if ((N_d % 3) != 0) {
    cerr << "N_d should be a multiple of 3. Exitting" << endl;
    return -2; 
  }

  if (use_fpga) {
    Fpga = new FpgaDev();
    mt_fpga = new mt(Fpga, fpga_board, fpga_chip, json_file, N_d);
  }
  
  if (use_cpu) {
    mt_cpu = new mt(json_file, N_d);
  }


   double dt_c;
   double dt_f;
   cout << "N_d -> " << N_d << endl;
   
   cout << "Dynamic steps -> " << STEPS_TO_WRITE << endl;
   cout << "Start kintetic steps -> " << k_steps << endl;


   int total_error = 0; 

   
   for(unsigned int k=0; k < k_steps; ++k) {

      int err = 0;
      struct timeval tt1, tt2;

      cout << "Step -> " << k << endl; 

      if (use_cpu) {
        // get_time(&tt1);
        gettimeofday(&tt1, 0);
        if (mt_cpu->calc_dynamics() < 0) { 
           cerr << "Nan Error in mt_cpu->calc_dynamics. Step is " << k << endl; 
           break;
        }

        // get_time(&tt2);
        gettimeofday(&tt2, 0);
        calc_dt(&tt1,&tt2, &dt_c);

        cout << "CPU Time = " << dt_c << endl;

        mt_cpu->calc_kinetics();
        mt_cpu->print_coords(); 
        mt_cpu->print_coords_type(); 
        mt_cpu->print_length();         
      }
      
      if (use_fpga) {
        // get_time(&tt1);
        gettimeofday(&tt1, 0);
        // TODO: work on n_layers var
        if (mt_fpga->calc_dynamics() < 0) {
          cerr << "Error in Fpga.CalcDynamics. Step is " << k << endl;
          return -1; 
        }

        // get_time(&tt2);
        gettimeofday(&tt2, 0);
        calc_dt(&tt1,&tt2, &dt_f);

        cout << "FPGA Time = " << dt_f << endl;

        mt_fpga->calc_kinetics();
        mt_fpga->print_coords(); 
        mt_fpga->print_coords_type(); 
        mt_fpga->print_length();
      }
      

      if (use_cpu && use_fpga){
         // uncomment u1
         err = compare_results(mt_cpu->get_coords(), mt_fpga->get_coords());
         if (err) {
            total_error += err;
            cerr << "Compare results failed, errors = " << total_error << endl;
         }
      }
     

   } // k_steps


   if (use_cpu && use_fpga){
      if (!total_error)
      printf("Compare OK!\n");
   }
   
   

  return 0; 

}



float max(float A, float B){
  return (A>=B) ? A : B;
}

int equal(float A, float B) {
  return (fabs(A - B) <= max(absTol, relTol * max(fabs(A), fabs(B))));
}

int compare_results(mt_coords_t *c1, mt_coords_t *c2) {
  unsigned int i,j;
  int error = 0;
  for(i = 0; i<13; i++) {
    for(j = 0; j<N_d; j++) {
       if ( (c1->x[i][j]!=c1->x[i][j]) || (c2->x[i][j]!=c2->x[i][j]) || (!equal(c1->x[i][j], c2->x[i][j])) )
         error++;
       if ( (c1->y[i][j]!=c1->y[i][j]) || (c2->y[i][j]!=c2->y[i][j]) || (!equal(c1->y[i][j], c2->y[i][j])) )
         error++;
       if ( (c1->t[i][j]!=c1->t[i][j]) || (c2->t[i][j]!=c2->t[i][j]) || (!equal(c1->t[i][j], c2->t[i][j])))
         error++;
    }
  }
  return error;
}



void calc_dt(struct timeval *tt1, struct timeval *tt2, double *tsec) {
  double ttv1 = ((double) tt1->tv_usec) / 1000000 + tt1->tv_sec;
  double ttv2 = ((double) tt2->tv_usec) / 1000000 + tt2->tv_sec;
  *tsec = ttv2 - ttv1;
}











