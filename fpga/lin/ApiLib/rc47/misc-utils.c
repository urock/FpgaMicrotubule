#include "misc-utils.h"


void work_progress_init(struct work_progress_str *wprogress, 
						int bar_width, int number_of_updates, int total_rounds, int rounds_done)
{

	wprogress->bar_width 			= bar_width;
	wprogress->number_of_updates 	= number_of_updates;
	wprogress->total_rounds 		= total_rounds;
	wprogress->rounds_done 			= rounds_done;

	if (total_rounds < number_of_updates) {
		wprogress->number_of_updates 	= total_rounds;
		wprogress->bar_width 			= number_of_updates;		
	}

}



void work_progress_print(struct work_progress_str *wprogress)
{
	
	float ratio;
	int   c;

	int x;

	if (wprogress->rounds_done % (wprogress->total_rounds/wprogress->number_of_updates) == 0) {

		// Calculuate the ratio of complete-to-incomplete.
		ratio = (float)(wprogress->rounds_done)/(float)(wprogress->total_rounds);
		c     = ratio * wprogress->bar_width;
 
		// Show the percentage complete.
		printf("%3d%% [", (int)(ratio*100) );
 
		// Show the load bar.
		for (x=0; x<c; x++)
		   printf("=");
 
		for (x=c; x<wprogress->bar_width; x++)
		   printf(" ");
		// ANSI Control codes to go back to the
		// previous line and clear it.
		// printf("]\n33[F33[J");
		printf("]\r"); // Move to the first column
		fflush(stdout);					
		
	}

	wprogress->rounds_done++; 
}


