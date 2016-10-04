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

int flag_seed = 1;



char *out_file;
int flag_file = 0; 



vector<vector<float> > x_1;
vector<vector<float> > y_1;
vector<vector<float> > t_1;

vector<vector<float> > x_2;
vector<vector<float> > y_2;
vector<vector<float> > t_2;


int seeds[NUM_SEEDS];

//#define TEST_SEEDS	

const int test_seeds[NUM_SEEDS] = {	0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 
									0x60000000, 0x70000000, 0x80000000, 0x90000000, 0xA0000000  };


#define STEPS_TO_WRITE		1000			// через это значение шагов сравниваем координаты и выводим в файл

#define N	10				// количчество  запусков функций mt_cpu и mt_fpga

#define TOTAL_STEPS			(STEPS_TO_WRITE*N)		// полное количество шагов по времени

#define absTol		0.001f
#define relTol		0.001f


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
		

		x_1.resize(13);
		y_1.resize(13);
		t_1.resize(13);
		x_2.resize(13);
		y_2.resize(13);
		t_2.resize(13);
		for (int i = 0; i < 13; i++){
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
	FILE  *f_p, *f_p1;

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
	
	if (f_p1==NULL) {

		printf("Error opening file!\n");
		return -1;
	}
	
	


	printf("TOTAL_STEPS = %d\nSTEPS_TO_WRITE = %d\n", TOTAL_STEPS, STEPS_TO_WRITE);



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
	
	struct timeval time; 
     gettimeofday(&time,NULL);

     // microsecond has 1 000 000
     // Assuming you did not need quite that accuracy
     // Also do not assume the system clock has that accuracy.
     srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

     // The trouble here is that the seed will repeat every
     // 24 days or so.

     // If you use 100 (rather than 1000) the seed repeats every 248 days.

     // Do not make the MISTAKE of using just the tv_usec
     // This will mean your seed repeats every second.
	 

	
	printf("\nFlag rand is SET, %d\n\n",flag_rand);
	
	if (flag_rand==1) 	  {
		
		//srand (time(NULL));
		
		// set seed vals
		for (int i =0; i < NUM_SEEDS; i++){
#ifdef TEST_SEEDS
				seeds[i] = test_seeds[i];
#else
				seeds[i]=rand();
#endif
			unsigned int addr = SEED_REG + 4*i;
			RD_WriteDeviceReg32m(dev, CNTRL_BAR, addr, seeds[i]);	
			
			printf("seed %x \n",seeds[i]); 
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
	
	

	for(int k=0; k<N; k++) {


		int err;
		struct timeval tt1, tt2;
		if (flag_compare==1){
			get_time(&tt1);
			if (mt_cpu(STEPS_TO_WRITE,1, flag_rand, flag_seed, seeds,  x_1,y_1,t_1,x_1,y_1, t_1)<0) { printf("Nan Error in cpu. Step is %d. Exitting....\n",k); break;}

			get_time(&tt2);
			calc_dt(&tt1,&tt2, &dt_c[k]);
		}
		
		get_time(&tt1);
		mt_fpga(dev,STEPS_TO_WRITE,1,x_2,y_2,t_2,x_2,y_2, t_2);
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


		if (flag_compare==1)	print_coords(f_p, x_1, y_1, t_1);
		print_coords(f_p1, x_2, y_2, t_2);
		

	}
	if (flag_compare==1){
		if (!error)
		printf("Test OK!\n");
	}
	
	

	if (flag_compare==1) fclose(f_p);
	fclose(f_p1);


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

	two_floats tmp; 
	two_floats w0, w1;
	
	unsigned int reg_val; 
	
	
	int i,j, cnt, hls_done;
	
	int k = 0; // ddr buffer index
	
	two_floats * buf_in = (two_floats *)wr_buf;
	
	// на каждую молекулу отводим 16 байт в памЯти - это 2 64-битных слова
	for (i=0; i<13; i++)
	for (j=0; j<N_d; j++) {
		tmp.d0 = x_in[i][j];
		tmp.d1 = y_in[i][j];
		buf_in[k++] = tmp;
		tmp.d0 = t_in[i][j];
		tmp.d1 = 0;			
		buf_in[k++] = tmp; 
	}


	// deassert mt hls reset
	RD_ReadDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);
	reg_val |= (1<<6);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, COMMAND_REG, reg_val);


	

	
	//////////////////////////////////////////////////////////////////////////////
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_A, n_step);
	RD_WriteDeviceReg32m(dev, CNTRL_BAR, HLS_B, N_d);


	
	
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
		fprintf(f_p,"%.3f  ", x[i][j]);

		for (j=0; j<N_d; j++)
		fprintf(f_p,"%.3f  ", y[i][j]);

		for (j=0; j<N_d; j++)
		fprintf(f_p,"%.3f  ", t[i][j]);
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
	for (j=1; j<N_d-4; j++)
	for (i=0; i<13; i++)
	y[i][j] = y[i][j-1] + 2.0f*Rad;


	// задание x и teta координат так чтобы был цилиндр до половины высоты трубочки
	for (j=0; j<N_d-5; j++)
	for (i=0; i<13; i++)  {

		x[i][j] = 0.0;
		t[i][j] = 0.0;
	}


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


	//	x[0][0] = 1;
	//	y[0][0] = 2;
	//	t[0][0] = 3;
	//
	//	x[0][1] = 4;
	//	y[0][1] = 5;
	//	t[0][1] = 6;
	//
	//
	//	x[0][2] = 7;
	//	y[0][2] = 8;
	//	t[0][2] = 9;
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
