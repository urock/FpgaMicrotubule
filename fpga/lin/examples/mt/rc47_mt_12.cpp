/********************************************
* rc47_mt.c
*
*
*	to work with rc_mt_2000_12 project
*
*******************************************/


#include "RostaPCIeAPI.h"
#include "rc47-api.h"

#include "rc_47.h"


struct pci_device pd[MAX_PCIE_DEVICES];
struct rc47_board_t rc47[MAX_RC47_BOARDS];
struct pci_device *pd_ptr;

unsigned int 	*wr_buf, *wr_buf_free;
unsigned int 	*rd_buf, *rd_buf_free;

bool flag_compare, flag_rand;
unsigned int N_d;

unsigned int length;	//length of MT with counting shifts
int NStop[13];	//numbers of last monomers;
int NStart[13];	//numbers of first monomers;

int flag_seed = 1;



char *out_file;
int flag_file = 0; 



vector<vector<float> > x_1;
vector<vector<float> > y_1;
vector<vector<float> > t_1;

vector<vector<float> > x_2;
vector<vector<float> > y_2;
vector<vector<float> > t_2;

vector<vector<float> > type_mol;	//types

int seeds[NUM_SEEDS];

//#define TEST_SEEDS	

const int test_seeds[NUM_SEEDS] = {	0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 
									0x60000000, 0x70000000, 0x80000000, 0x90000000, 0xA0000000  };


#define STEPS_TO_WRITE		500000			// steps in fpga. was 1000 ; 5000000*100 to almost destroy	//500000

#define N	80				// steps to write   number of time steps was 100?

#define TOTAL_STEPS			(STEPS_TO_WRITE*N)		// полное количество шагов по времени

#define absTol		0.001f
#define relTol		0.001f

#define cut_off 1.0 		//for deattach

#define nullHigh 10			//indent from top at the beginning

/* Period parameters for randomiser*/ //////////////////////////////////// 
//#include <cmath>
#define N_period 624
#define M_period 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
class Random
{		
	public:
		Random(unsigned long s) {
			mti=N_period+1;
			init_genrand(s);
			r1 = 0; r2 = 0; s = 0; rho = 0;
			valid = false;
		};
		~Random(){};		
		float Normal_dist(void);
		double genrand_real2(void);

	private:
		float r1, r2, s, rho;
		bool valid;
		void init_genrand(unsigned long s);
		unsigned long mt[N_period]; /* the array for the state vector  */
		int mti;					/* mti==N_period+1 means mt[N_period] is not initialized */
};
float Random :: Normal_dist(void)
{
	if (!valid)
	{
		do {
		r1 = 2.0*genrand_real2() - 1;
		r2 = 2.0*genrand_real2() - 1;
		s = r1 * r1 + r2 * r2;
		} 
		while (s > 1);
		rho = sqrtf((float)-2.0 * logf(s)/s);
		valid = true;
	} else {
	valid = false;
	}	
	return rho * (valid ? r1 : r2); //return rho * (valid ? r1 : r2) * m_sigma + m_mean;
};
double Random :: genrand_real2(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N_period) { /* generate N_period words at one time */
        int kk;
        if (mti == N_period+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N_period-M_period;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M_period] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N_period-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M_period-N_period)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N_period-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N_period-1] = mt[M_period-1] ^ (y >> 1) ^ mag01[y & 0x1UL];
        mti = 0;
    }  
    y = mt[mti++];
    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    return y*(1.0/4294967296.0); 
	//return y*2.328306436538696e-10;
};
void Random :: init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N_period; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
};
/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	int dev, ret_val;
	unsigned int reg_val;

	// find and init FPGA device
	ret_val = fpga_init(argc, argv, &dev);
	if (ret_val<0) return -1;
	
	double dt_c[N];
	double dt_f[N];
	printf("N_d is %d\n",N_d);
	
	printf("N_d is %d\n",N_d);
	
	printf("size is %d\n",(int)SIZE_DWORD);
	
	try{
		
		type_mol.resize(13);
		x_1.resize(13);
		y_1.resize(13);
		t_1.resize(13);
		x_2.resize(13);
		y_2.resize(13);
		t_2.resize(13);
		for (int i = 0; i < 13; i++){
			type_mol[i].resize(N_d);
			x_1[i].resize(N_d);
			y_1[i].resize(N_d);
			t_1[i].resize(N_d);
			x_2[i].resize(N_d);
			y_2[i].resize(N_d);
			t_2[i].resize(N_d);
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
	
	if (flag_file)		f_p1 = fopen (out_file,"w");
	else 				f_p1 = fopen ("MT_coords_FPGA.txt","w");
	
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
	length = NStop[0] - NStart[0];

	// get golden results
	init_coords(x_1,y_1,t_1);
	init_coords(x_2,y_2,t_2);
	



	/*
	* в этом цикле проводим вычисления и сравниваем результаты
	*
	* в данный момент чтобы проверить сравнение на каждом шаге вызывается функция mt_cpu
	* с параметром load_coords = 1 (иначе состояния глобальных массивов координат будет все время меняться)
	*
	* Когда вместо mt_cpu будет реализация на OpenCL, то надо делать по-другому:
	*  перед циклом один раз вызываем mt_cpu (load_coords = 1), в цикле уменьшаем количество итераций на 1 и
	*  вызываем mt_cpu (load_coords = 0)
	*
	*/
	error = 0; 

	
	printf("\nFlag rand is SET, %d\n\n",flag_rand);
	
	if (flag_rand==1) 	  {
		
		srand (time(NULL));
		
		// set seed vals
		for (int i =0; i < NUM_SEEDS; i++){
#ifdef TEST_SEEDS
				seeds[i] = test_seeds[i];
#else
				seeds[i]=rand();
#endif
			unsigned int addr = SEED_REG + 4*i;
			RD_WriteDeviceReg32m(dev, CNTRL_BAR, addr, seeds[i]);	
		}		
		
		printf("\nFlag rand is SET\n\n");
		
		
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	
		// deassert rand reset
		reg_val |= (1<<4);
		RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
		
		// set rand_enable flag
		reg_val |= (1<<7);
		RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);

		// start rand core
		reg_val |= (1<<5);	
		RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);		
		

		
		
	}
	
	
	printf("\n\nhereerereerere\n\n");

	int temp = 0;
	for(int k=0; k<N; k++) {

		int err;
		struct timeval tt1, tt2;
		if (flag_compare==1){
			get_time(&tt1);
			if (mt_cpu(STEPS_TO_WRITE,1, flag_rand, flag_seed, seeds,  x_1,y_1,t_1,x_1,y_1, t_1, N_d)<0) { printf("Nan Error in cpu. Step is %d. Exitting....\n",k); break;}

			get_time(&tt2);
			calc_dt(&tt1,&tt2, &dt_c[k]);
		}
		
		get_time(&tt1);
		mt_fpga1(dev,STEPS_TO_WRITE,1,x_2,y_2,t_2,x_2,y_2, t_2, type_mol);
		get_time(&tt2);
		calc_dt(&tt1,&tt2, &dt_f[k]);

		flag_seed = 0; 
		
		printf("Step %d\n\t CPU Time = %f\n\t FPGA Time = %f\n",k,dt_c[k],dt_f[k] );

		if (flag_compare==1){
			err = compare_results(x_1,y_1,t_1,x_2,y_2,t_2);
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
		int i = 10, j = 34, j1;
		
		
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
				if (getDistance(x_2[i][j+1], y_2[i][j+1], t_2[i][j+1], x_2[i][j], y_2[i][j], t_2[i][j]) >= cut_off) {	//molecule deattached
					//printf("deattached :%i %i r = %f\n", i, j,getDistance(x_2[i][j+1], y_2[i][j+1], t_2[i][j+1], x_2[i][j], y_2[i][j], t_2[i][j]));
					for (j1=j; j1<NStop[i]; j1++) {
						y_2[i][j1] = -100;
						type_mol[i][j1] = -1;	//'-'
						printf("deattached :%i %i\n", i, j1);
						//todo type change
					} 
					NStop[i] = j;
				}
			}
		}
		
		polimerization_algorithm(x_2, y_2, t_2, (float) tt1.tv_usec);
		
		choose_to_shift_coords(x_2, y_2, t_2);
		
		
//-------------------------------------- add reattach, kinetics---------------------------------------------------------------------------------------------------
		
		if (flag_compare==1)	print_coords(f_p, x_1, y_1, t_1);
		print_coords(f_p1, x_2, y_2, t_2);
		print_coords_type(f_type, type_mol);
		float x_avg, y_avg = 0;;
		for (i = 0; i < 13; i++) {
			y_avg += y_2[i][NStop[i]-1];
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

	RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	
	// assert rand reset
	reg_val &= ~(1<<4);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
		
	// reset rand_enable flag
	reg_val &= ~(1<<7);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);	
	


	free(wr_buf_free);
	free(rd_buf_free);


	RD_CloseDevice(pd_ptr);
	return 0; 

}



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
) 
{
return -1;	
}


int mt_fpga1(	int 	dev,
int		n_step,				//time steps to run
int 	load_coords,		//
//int seeds[],

vector<vector<float> >  & x_in,
vector<vector<float> >  & y_in,
vector<vector<float> >  & t_in,

vector<vector<float> >  & x_out,
vector<vector<float> >  & y_out,
vector<vector<float> >  & t_out,
vector<vector<float> > & type_mol
) 
{	

	two_floats tmp; 
	two_floats w0, w1;
	
	unsigned int reg_val; 
	
	
	int i,j, cnt, hls_done;
	
	int k = 0; // ddr buffer index
	
	two_floats * buf_in = (two_floats *)wr_buf;
	
	// 16 bytes for each molecule 
			
	
	for (i=0; i<13; i++)
	for (j=0; j<N_d; j++) {
		
		tmp.d0 = x_in[i][j];
		tmp.d1 = y_in[i][j];		
		
		
		buf_in[k++] = tmp;
		tmp.d0 = t_in[i][j];
		if (type_mol[i][j] == -1) {
			tmp.d1 = 1;
		} else {
			tmp.d1 = type_mol[i][j];
		}		
		buf_in[k++] = tmp; 
	}


	// deassert mt hls reset
	RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	reg_val |= (1<<6);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);


	

	
	//////////////////////////////////////////////////////////////////////////////
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_A, n_step);
	
	int p2 = 0;
	switch(N_d) {
		case 12:
			p2 = 0;
			break;
		case 24:
			p2 = 1;
			break;
		case 36:
			p2 = 2;
			break;
		case 48:
			p2 = 3;
			break;
		case 60:
			p2 = 4;
			break;
		case 72:
			p2 = 5;
			break;
		case 84:
			p2 = 6;
			break;
		case 108:
			p2 = 7;
			break;
		case 156:
			p2 = 8;
			break;
		case 216:
			p2 = 9;
			break;
		default:
			p2 = 2;	
	}
	
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_B, p2);
	
	if (fpga_write_to_axi(dev, wr_buf, SIZE_DWORD*sizeof(two_floats), 0x20000000) < 0){
		fprintf (stderr,"Error in fpga_write_to_axi\n");
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, reg_val);
		fprintf (stderr,"MasterMemRdTotalCnt 0x%x\n", reg_val);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, reg_val);
		fprintf (stderr,"FifoMemRdTotalCnt 0x%x\n", reg_val);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, reg_val);
		fprintf (stderr,"FifoMemWrTotalCnt 0x%x\n", reg_val);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, reg_val);
		fprintf (stderr,"MasterMemWrTotalCnt 0x%x\n", reg_val);
		
		return -2;
	}
	
	

	//////////////////////////////////start hls///////////////////////////////////
	RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	reg_val |= (1<<2);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	///////////////////////////////////////////////////////////////////////////////
	

	//////////////////////////wait hls to finish///////////////////////////////////
	cnt =0;
	do{
		usleep(1000);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
		cnt++;
		hls_done = (reg_val&(1<<3)) ? 1 : 0; 
		
	}while ((!hls_done)&&(cnt!=1000000));
	//printf("Command reg is 0x%x\n", reg_val);
	if (cnt ==1000000) {fprintf(stderr,"Timeout error\n"); return -1;}
	///////////////////////////////////////////////////////////////////////////////

RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);

printf("hls done cnt = %d, reg_val = 0x%x\n",cnt, reg_val);	
	
	///////////////////////read from ddr data(should be x)////////////////////////////////
	if (fpga_read_from_axi(dev, 0x20000000,  SIZE_DWORD*sizeof(two_floats), rd_buf) < 0){
		fprintf (stderr,"Error in fpga_read_from_axi\n");
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, reg_val);
		fprintf (stderr,"MasterMemRdTotalCnt 0x%x\n", reg_val);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, reg_val);
		fprintf (stderr,"FifoMemRdTotalCnt 0x%x\n", reg_val);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, reg_val);
		fprintf (stderr,"FifoMemWrTotalCnt 0x%x\n", reg_val);
		RD_ReadDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, reg_val);
		fprintf (stderr,"MasterMemWrTotalCnt 0x%x\n", reg_val);
		return -3; 
	}
	

	
	// assert rand reset
	//reg_val &= ~(1<<4);
	// assert mt reset
	//reg_val &= ~(1<<6);
	// reset rand_enable flag
	//reg_val &= ~(1<<7);
	//RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);	
	

	two_floats * buf_out = (two_floats *)rd_buf;
	

	k = 0;
	for (i=0; i<13; i++)
	for (j=0; j<N_d; j++) {
		
		w0 = buf_out[k++];
		w1 = buf_out[k++]; 
		
		x_out[i][j] = w0.d0; 
		y_out[i][j] = w0.d1; 
		t_out[i][j] = w1.d0; 
	}		

	return 0; 				

}


	
//##########################command line parser#####################################################
int process_cmd_line(int argc, char *argv[], struct cmd_params_t *cmd_p)
{
	int i;
	unsigned int board;

	int got_brd=0, got_v7=0, got_N_d=0;
	for (i=0;i<argc; i++) {
		
		if ((strlen(argv[i])>=6) && !strncmp("--help",argv[i],6) ){
			print_usage(argv);
			return -2;
		}
		
		if ((strlen(argv[i])>=5) && !strncmp("-file",argv[i],5) ){
			flag_file=1;
			out_file = argv[i+1];
		}	

		if ((strlen(argv[i])>=8) && !strncmp("-compare",argv[i],8) ){
			flag_compare=1;
		}	
		
		if ((strlen(argv[i])>=4) && !strncmp("-rand",argv[i],5) ){
			flag_rand=1;
			
			printf("=====================================================================================\n");
		}

		if ((strlen(argv[i])>=2) && !strncmp("-N",argv[i],5) ){
			sscanf(argv[i+1],"%d",&N_d);
			got_N_d=1;
		}		
		
		if ((strlen(argv[i])>=2)&& !strncmp("-b",argv[i],2)) {
			if (i!=(argc-1) && sscanf(argv[i+1],"%d",&board)>0){
				cmd_p->board = board;
				got_brd = 1;
			}
		}
		
		if ((strlen(argv[i])>=2)&& !strncmp("-v",argv[i],2)) {
			if (i <argc && (strlen(argv[i+1])>=2)&& !strncmp("C0",argv[i+1],2)) {
				cmd_p->v7 = CS_C0; got_v7 = 1; 
			} else if (i <argc  && (strlen(argv[i+1])>=2)&&  !strncmp("C1",argv[i+1],2)) {
				cmd_p->v7 = CS_C1;  got_v7 = 1;
			} else if (i <argc  && (strlen(argv[i+1])>=2)&&  !strncmp("C2",argv[i+1],2)) {
				cmd_p->v7 = CS_C2; got_v7 = 1;
			} else if (i <argc && (strlen(argv[i+1])>=2)&&  !strncmp("C3",argv[i+1],2)) {
				cmd_p->v7 = CS_C3;  got_v7 = 1; 
				
			}

		}
		
	}

	/////////////////////////////////////////////////////		
	if (got_brd && got_v7&&got_N_d){
		return 0;
	}
	else{
		printf("\n %s: missing or invalid operand\n Try 'sudo %s --help' for more information\n\n", argv[0], argv[0]);
		return -1;
	}

}


int fpga_init(int argc, char *argv[], int *dev_out) {
	
	int k, r;
	struct cmd_params_t	cmd_p;
	
	unsigned int reg_val;
	
	flag_compare=0;
	flag_rand=0;
	flag_file = 0; 

	if (process_cmd_line(argc, argv, &cmd_p) < 0) {
		return -1;
	}

	printf("\nSearching for RC47 boards\n");

	k = rc47_search_for_boards(pd,rc47);

	if (k<0) {
		fprintf (stderr,"No boards in system!\n\n");
		return 0;
	} else 
	if (k==1)		
	printf("Found %d board\n",k);
	else
	printf("Found %d boards\n",k);


	rc47_print_device_info(k,rc47);


	if (cmd_p.board >= k) {
		fprintf (stderr,"Invalid selected board number\n");
		return -2;
	} else {
		printf("\nSelected:\n");
		printf("Board number\t:::\t%d\n", cmd_p.board);
	}



	if ((cmd_p.v7 == CS_C0) && (rc47[cmd_p.board].LD_pd != NULL)) {
		pd_ptr = rc47[cmd_p.board].LD_pd;
		printf("Virtex Chip\t:::\t%s\n", "C0");
	} else if ((cmd_p.v7 == CS_C1) && (rc47[cmd_p.board].LU_pd != NULL)) {
		pd_ptr = rc47[cmd_p.board].LU_pd;
		printf("Virtex Chip\t:::\t%s\n", "C1");
	} else if ((cmd_p.v7 == CS_C2) && (rc47[cmd_p.board].RD_pd != NULL)) {
		pd_ptr = rc47[cmd_p.board].RD_pd;
		printf("Virtex Chip\t:::\t%s\n", "C2");
	} else if ((cmd_p.v7 == CS_C3) && (rc47[cmd_p.board].RU_pd != NULL)) {
		pd_ptr = rc47[cmd_p.board].RU_pd;
		printf("Virtex Chip\t:::\t%s\n", "C3");
	} else {
		fprintf (stderr,"Invalid selected virtex chip\n");
		return -2;
	}




	// open pcie device

	do_and_test(RD_OpenDevice,(pd_ptr));		

	int dev = pd_ptr->intfd;


	// reset user part
	if (fpga_user_reset(dev) < 0)
	return -1;
	
	if (fpga_release_user_reset(dev) < 0) 
	return -1; 		

	if (fpga_wait_for_ddr_init(dev) < 0)
	return -1;
	else
	printf("Init DDR OK!\n"); 
	
	
	// set interconnection 
	RD_ReadDeviceReg32m(dev, CNTRL_BAR, 0x4, reg_val);
	
	reg_val |= (1<<5);
	
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, 0x4, reg_val);
	



	/////////////////////////////////////set all counters to zero///////////////////
	
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, MasterMemRdTotalCnt, 0);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, FifoMemRdTotalCnt, 0);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, FifoMemWrTotalCnt, 0);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, MasterMemWrTotalCnt, 0);
	
	///////////////////////////////////////////////////////////////////////////////
	
	*dev_out = dev; 
	

	if (create_page_alligned_buffer(SIZE_BYTE, &wr_buf, &wr_buf_free)) {
		fprintf (stderr,"Error memory allocation for wr_buf\n");
		return -1;
	}

	if (create_page_alligned_buffer(SIZE_BYTE, &rd_buf, &rd_buf_free)) {
		fprintf (stderr,"Error memory allocation for rd_buf\n");
		return -1;
	}	

	return 0;
}


void print_usage(char *argv[])
{
	printf("Usage:\n");
	printf("\n--------------------------------------------------------------------------\n");
	printf("To run HLS TEST:\nsudo %s -b [board] -v [chip_select] -N [N_d] [-compare/ ] [-rand/ ] [-file output file name]\n", argv[0]);	
	printf("chip_select: C0 or C1 or C2 or C3\n");
	printf("N_d: number of molecules in one filament\n");
	printf("-compare to compare CPU and FPGA results\n");
	printf("-rand to run CPU test with randomness\n");
	printf("-file: if set uses this file to output\n");
	printf("\n--------------------------------------------------------------------------\n");
}






void print_coords(FILE *f_p, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t)
{
unsigned	int i,j;

	for (i=0; i<13; i++) {
		for (j=0; j<N_d; j++)
		fprintf(f_p,"%.3f\t  ", x[i][j]);

		for (j=0; j<N_d; j++)
		fprintf(f_p,"%.3f\t  ", y[i][j]);

		for (j=0; j<N_d; j++)
		fprintf(f_p,"%.3f\t  ", t[i][j]);
		
	}

	fprintf(f_p,"\n");

}

void print_coords_type(FILE *f_p, vector<vector<float> > &type_mol)
{
unsigned	int i,j;

	for (i=0; i<13; i++) {
		
		for (j=0; j<N_d; j++)
		fprintf(f_p,"%i\f  ", type_mol[i][j]);
	}

	fprintf(f_p,"\n");

}




void init_coords(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t)
{
	int i,j;


	// задание y координаты для нижней спирали
	for (i=0; i<13; i++)
	y[i][0] = 2.0f*6/13*(i+1);


	// задание y координат для остальных молекул до половины высоты трубочки
	for (j=1; j<N_d; j++)//-4
	for (i=0; i<13; i++)
	y[i][j] = y[i][j-1] + 2.0f*Rad;


	// задание x и teta координат так чтобы был цилиндр до половины высоты трубочки
	for (j=0; j<N_d; j++)//-5
	for (i=0; i<13; i++)  {

		x[i][j] = 0.0;
		t[i][j] = 0.0;
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
			type_mol[i][j] = 1;	//	T
		}
		/*for(j = NStart[i]-1; j >= 0; j-- ){
		type_mol[i][j] = 0;	*/// 	'D'
		
	for(j = NStop[i]; j < N_d; j++){
		x[i][j] = 0;		
		y[i][j] = -100.0;
		t[i][j] = 0;
		type_mol[i][j] = -1;	//	'-'	
	}
	}
}





int compare_results(vector<vector<float> >  & x_1, vector<vector<float> >  & y_1, vector<vector<float> >  & t_1,
vector<vector<float> >  &  x_2, vector<vector<float> >  & y_2, vector<vector<float> >  & t_2 )
{

	int error = 0;
	for(int i = 0; i<13; i++)
	for(int j = 0; j<N_d; j++) {

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

int max_N(int N_arr[13]) {
	int i;
	int NMax = 0;
	for (i = 0; i<13; i++)
		if (NMax < N_arr[i]) 
			NMax = N_arr[i];
	return NMax;
}

int min_N(int N_arr[13]) {
	int i;
	int NMin = N_arr[0];
	for (i = 0; i < 13; i++)
		if (NMin > N_arr[i]) 
			NMin = N_arr[i];
	return NMin;
}

void choose_to_shift_coords(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t) {
	int NStopMax = max_N(NStop), NStopMin = min_N(NStop);
	/*
	if (NStopMax >= N_d - 5 && NStopMin <= 16) {	//Error
		printf("Error!!! NStopMax >= N_d - 5 && NStopMin <= 16 N_d = %i \n", N_d);
	}
	if (NStopMax >= N_d - 8 && NStopMin <= 3) {	//Error
		printf("NStopMax >= N_d - 8 && NStopMin <= 3N_d = %i \n", N_d);
	}
	if (NStopMax >= N_d - 5)
		shift_coords(x, y, t, false, 16);
	if (NStopMin <= 4){
		//printf("%iUOP:  \n",NStop[1]);
		//for (int i = 0; i < 10; i++)
		//	printf("%iUOP: %f \n",i, y_2[1][i]);
		shift_coords(x, y, t, true, 8);
		//for (int i = 0; i < 10; i++)
		//	printf("%iUOP: %f \n",i, y_2[1][i]);
		//printf("%iUOP:  \n",NStop[1]);
		//printf("UP!");
		}*/
		
		if (NStopMax >= N_d - 5 && NStopMin <= 7) {	//Error
		printf("Error!!! NStopMax >= N_d - 5 && NStopMin <= 16 N_d = %i \n", N_d);
	}
	if (NStopMax >= N_d - 6 && NStopMin <= 3) {	//Error
		printf("NStopMax >= N_d - 8 && NStopMin <= 3N_d = %i \n", N_d);
	}
	if (NStopMax >= N_d - 5)
		shift_coords(x, y, t, false, 4);
	if (NStopMin <= 4){
		//printf("%iUOP:  \n",NStop[1]);
		//for (int i = 0; i < 10; i++)
		//	printf("%iUOP: %f \n",i, y_2[1][i]);
		shift_coords(x, y, t, true, 4);
		//for (int i = 0; i < 10; i++)
		//	printf("%iUOP: %f \n",i, y_2[1][i]);
		//printf("%iUOP:  \n",NStop[1]);
		//printf("UP!");
		}
}

void shift_coords(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t, bool up, const int shift)
{
	int i,j;
	
	if (up) {
		for (i=0; i<13; i++){
		for (j=NStart[i]; j<NStop[i]; j++){
			
			x[i][j + shift] = x[i][j];
			y[i][j + shift] = y[i][j];
			t[i][j + shift] = t[i][j];
			type_mol[i][j + shift] = type_mol[i][j];
		
		}
		NStop[i] += shift;
		}
		printf("UP!%f", y[1][4+8]);
		//NStart[i] += shift;
		length += shift;
		
		for (i=0; i<13; i++)
		for (j = shift-1; j >= 0; j-- ){
		x[i][j] = x[i][j + 1];		//?need to test
		y[i][j] = y[i][j + 1] - 2*Rad;
		t[i][j] = 0;
		type_mol[i][j] = 0;	//todo  was 'D'
		}
		
	} else {
		for (i=0; i<13; i++){
		for (j=shift; j<NStop[i]; j++){
			
			x[i][j - shift] = x[i][j];
			y[i][j - shift] = y[i][j];
			t[i][j - shift] = t[i][j];
			type_mol[i][j - shift] = type_mol[i][j];
		
		}
		NStop[i] -= shift;
		}
		//NStart[i] -= shift;
		length -= shift;
	}
	
	for (i=0; i<13; i++)
	for (j = NStop[i]; j < N_d; j++ ){
		x[i][j] = 0;		
		y[i][j] = -100.0;
		t[i][j] = 0;
		type_mol[i][j] = -1;	//todo was '-'	
	}
	
}

void addDimer (int i, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t) {
	if (NStop[i] > N_d-1) return;
	//todo angle
	x[i][NStop[i]] = x[i][NStop[i] - 1] + 2*Rad*sinf(t[i][NStop[i] - 1]);		
	y[i][NStop[i]] = y[i][NStop[i] - 1] + 2*Rad*cosf(t[i][NStop[i] - 1]);
	t[i][NStop[i]] = t[i][NStop[i]-1];
	type_mol[i][NStop[i]] = 1;	//T
	NStop[i]++;
	x[i][NStop[i]] = x[i][NStop[i] - 1] + 2*Rad*sinf(t[i][NStop[i] - 1]);		
	y[i][NStop[i]] = y[i][NStop[i] - 1] + 2*Rad*cosf(t[i][NStop[i] - 1]);
	t[i][NStop[i]] = t[i][NStop[i]-1];
	type_mol[i][NStop[i]] = 1;	//T
	NStop[i]++;
	//type_mol[i][NStop[i]] = 0;	//todo  was 'D'		??1045
}

void removeDimer (int i, vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t) {
	y_2[i][NStop[i] - 1] = -100;
	type_mol[i][NStop[i] - 1] = -1;
	y_2[i][NStop[i] - 2] = -100;
	type_mol[i][NStop[i] - 2] = -1;
	NStop[i] -= 2;
}
	//t1 = time elapsed, print to console
void polimerization_algorithm(vector<vector<float> >  &  x, vector<vector<float> >  &  y, vector<vector<float> >  &  t, float t1) {
	srand((unsigned) time(NULL));
	unsigned long InitRnd = (t1-(int)t1)*1e+10 + rand();	
	Random Rnd_pol(InitRnd*1.7 + 0.35*rand());
	int i, j;
	float gen_rand;
	for(i = 0; i < 13; i++) {
		gen_rand = Rnd_pol.genrand_real2();
	
		if(gen_rand <= attachment_probability){		
				addDimer(i, x, y, t);
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


