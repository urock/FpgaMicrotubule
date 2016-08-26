CC=gcc
CFLAGS=-g -Wall -std=c99 -fPIC -shared -D_XOPEN_SOURCE=500 -D_BSD_SOURCE 
# ifdef PLX_SDK_DIR 
# CFLAGS += -DUSE_PLX
# endif


RINC = $(ROSTA_SDK_DIR)/lin/ApiLib/include
RLIB = $(ROSTA_SDK_DIR)/lin/ApiLib

PINC = $(PLX_SDK_DIR)/Include

all: libRC47.a

libRC47.a: rc47-api.c rc47-stm-misc.c
	$(CC) -I$(RINC) -I$(PINC) $(CFLAGS) -o $@ rc47-api.c rc47-fpga-lib.c rc47-stm-misc.c misc-utils.c sd_file.c ini.c -L$(RLIB) -lRostaPCIeAPI 
	cp $@ ..
	rm -f $@
	
# $@ - name of current task
# $< - name of current file, output file is dependent on

#%.o: %.c
#	$(CC) -I$(RINC) $(CFLAGS) -c $<
	
clean: 	
	-rm -f libRC47.a
	
	
	




