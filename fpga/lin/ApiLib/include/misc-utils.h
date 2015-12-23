#ifndef MISC_UTILS_H_
#define MISC_UTILS_H_

#include "sys-include.h"


struct work_progress_str
{
	int	bar_width;
	int number_of_updates;
	int total_rounds;
	int rounds_done;

};


void work_progress_init(struct work_progress_str *wprogress, 
						int bar_width, int number_of_updates, int total_rounds, int rounds_done);


void work_progress_print(struct work_progress_str *wprogress);


#endif // MISC_UTILS_H_
