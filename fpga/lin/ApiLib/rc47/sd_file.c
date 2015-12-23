
#include "sd_file.h"






unsigned char *bitfile_mem_buffer;


// return 	number v7 records 
//			<0 on error; 


// return file length in bytes or <0 on error
int get_biffile_length(char *file_name)
{

	int fd, fileLength; 

	fd = open((const char *)file_name,O_RDWR);

	if (fd < 0) {
		printf("Error opening file\n"); 
		return -1;
	}

	fileLength = lseek(fd,0,SEEK_END); 

//	printf("file length = %d\n",fileLength);
	lseek(fd,0,0); 

	close(fd);

	return fileLength;


}




// return file length in bytes or <0 on error
int load_file_to_mem(char *file_name)
{

	int fd, fileLength; 
	int bytes_read; 



	int i; 



/*********** Reading bit file *****************/	
	//printf("load_bitstream: bit_file_path = %s\n",file_name); 

	fd = open((const char *)file_name,O_RDWR);

	if (fd < 0) {
		printf("Error opening file\n"); 
		return -1;
	}

	fileLength = lseek(fd,0,SEEK_END); 

//	printf("file length = %d\n",fileLength);
	lseek(fd,0,0); 


	bitfile_mem_buffer = (unsigned char *) malloc(fileLength); 
	if (!bitfile_mem_buffer) {
		printf("Error allocting memory for mem_buffer\n");
		close(fd);
		return -2;
	}



	for (i=0; i<fileLength; i++) 
		bitfile_mem_buffer[i] = 0; 


	bytes_read = read(fd,(void *)bitfile_mem_buffer,fileLength); 
	if (bytes_read != fileLength) {
		printf("Error reading file! bytes_read = %d\n", bytes_read);
		free(bitfile_mem_buffer);
		close(fd);
		return -4;		
	}
	close(fd);

	// bitswap
	unsigned char val8, out8, cnt, bit; 

	for (i=0; i<fileLength; i++) {
		val8 = bitfile_mem_buffer[i];
		out8 = 0;
		for (cnt=1;cnt<=8;cnt++) {
			bit = val8 & 0x01;
			val8 = val8 >> 1;
			out8 = out8 << 1;
			if (bit==1) out8++;
		}
		bitfile_mem_buffer[i] = out8;
	}


	return fileLength;

}

int build_sd(char* sp6_file_name_ptr, char* v7_file_name_ptr,unsigned int* v7_description, char* sd_card_file ){

	int 	bytes_write, i;
	int 	record_length, sp6_bit_length; 
	int 	v7_bit_length[MAX_V7_BITFILES]; 

	unsigned char zero_buf[512]; 

	FILE* fd;

	msd_master_record_str	msd_master_record;
	msd_record_str			msd_record_1; 

	printf("sp: %s\nv7: %s\n",sp6_file_name_ptr,v7_file_name_ptr);
	printf("v7 description: %d\n",v7_description[0]); 


	sp6_bit_length = get_biffile_length(sp6_file_name_ptr);

	printf("sp6_bit_length = %d\n", sp6_bit_length);

	v7_bit_length[0] = get_biffile_length(v7_file_name_ptr); 

	printf("v7_bit_length = %d\n", v7_bit_length[0]);

fd = fopen((const char *)sd_card_file, "w+"); 
	if (!fd) {
		printf("Error opening SD image file\n"); 
		return -1;
	} 

	// skip first block - fill it with 0
	for (i=0; i < 512; i++)
		zero_buf[i] = 0;
	
	record_length = 512;

	bytes_write = fwrite((void *)zero_buf,1,record_length,fd); 
	if (bytes_write != record_length) {
		printf("Error writing zero buf to file! bytes_write = %d\n", bytes_write);
		return -2;
	}	



	// create master record and write it to start of the first block

	msd_master_record.valid_code		= MSD_VALID_CODE; 
	msd_master_record.card_type			= 0;
	msd_master_record.num_records		= 2;
	msd_master_record.next_record_addr	= 0x210;

	record_length = 0x10; 

	bytes_write = fwrite((void*)&msd_master_record,1,record_length,fd); 

	if (bytes_write != record_length) {
		printf("Error writing msd_master_record to file! bytes_write = %d\n", bytes_write);
		return -3;
	}	


	// create and write second record for SP6
	msd_record_1.record_type 		= BITFILE_SP6; 
	msd_record_1.record_description = LX45T; 
	msd_record_1.bit_file_addr		= 2; 			// bit_file address in blocks (512 bytes)
	msd_record_1.bit_file_length	= sp6_bit_length;		// 1 MB
	msd_record_1.next_record_addr	= 0x230; 
	msd_record_1.dummy_1			= 0; 
	msd_record_1.dummy_2			= 0; 
	msd_record_1.dummy_3			= 0; 


	record_length = 0x20; 

	bytes_write = fwrite((void*)&msd_record_1,1,record_length,fd); 

	if (bytes_write != record_length) {
		printf("Error writing msd_record_1 to file! bytes_write = %d\n", bytes_write);
		return -4;
	}

	
//	printf("((int)(sp6_bit_length>>9) + 3) = 0x%8x\n",((int)(sp6_bit_length>>9) + 3));



	// create and write third record for V7
	msd_record_1.record_type 		= BITFILE_V7; 
	msd_record_1.record_description = v7_description[0]; 
	msd_record_1.bit_file_addr		= ((int)(sp6_bit_length>>9) + 3) ; 			// bit_file address in blocks (512 bytes)
																			// here 3 = 2 first blocks + 1 block - ostatok
	msd_record_1.bit_file_length	= v7_bit_length[0];		
	msd_record_1.next_record_addr	= 0x0; 
	msd_record_1.dummy_1			= 0; 
	msd_record_1.dummy_2			= 0; 
	msd_record_1.dummy_3			= 0; 


	record_length = 0x20; 

	bytes_write = fwrite((void*)&msd_record_1,1,record_length,fd); 

	if (bytes_write != record_length) {
		printf("Error writing msd_record_2 to file! bytes_write = %d\n", bytes_write);
		return -5;
	}


// dummy writting to get to the begining of 2 block 
	for (i=0; i < 512; i++)
		zero_buf[i] = 0x11;

	record_length = 0x1B0;
	bytes_write = fwrite((void *)zero_buf,1,record_length,fd); 
	if (bytes_write != record_length) {
		printf("Error writing zero buf to file! bytes_write = %d\n", bytes_write);
		return -2;
	}	


/////////////////////////////////////////////////////
// write sp6 file to sd image


	printf("\nwrite sp6 file to sd image\n\n"); 

	// read data from sp6 file to memory
	sp6_bit_length = load_file_to_mem(sp6_file_name_ptr);
	if (sp6_bit_length < 0) {
		printf("Error loading sp6 bit file to memory\n");
		return 0;
	}

	bytes_write = fwrite((void *)bitfile_mem_buffer,1,sp6_bit_length,fd); 
	if (bytes_write != sp6_bit_length) 
		printf("Error writing sp6 to file! bytes_write = %d\n", bytes_write);

	free(bitfile_mem_buffer); 


// dummy writting to get to the begining of block to store v7 bitfile
	int x;

	x = ((int)(sp6_bit_length>>9) + 3)*512 - (1024 + sp6_bit_length); // difference between end of sp6_bitfile and start of v7_bitfile

	//printf("dummy x = %d\n", x);
	bytes_write = fwrite((void *)zero_buf,1,x,fd); 
	if (bytes_write != x) {
		printf("Error dummy writing to file! bytes_write = %d\n", bytes_write);
		return -2;
	}
	


/////////////////////////////////////////////////////
// write v7 file to sd image

	printf("\nwrite v7 file to sd image\n\n"); 

	// read data from sp6 file to memory
	v7_bit_length[0] = load_file_to_mem(v7_file_name_ptr);
	if (sp6_bit_length < 0) {
		printf("Error loading sp6 bit file to memory\n");
		return 0;
	}

	bytes_write = fwrite((void *)bitfile_mem_buffer,1,v7_bit_length[0],fd); 
	if (bytes_write != v7_bit_length[0]) 
		printf("Error writing v7 to file! bytes_write = %d\n", bytes_write);

	free(bitfile_mem_buffer); 
	


	fclose(fd); 

	return 0; 	



}
