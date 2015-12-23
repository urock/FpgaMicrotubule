CC=gcc
CFLAGS=-g -Wall -std=c99 -fPIC -shared -D_XOPEN_SOURCE=500 -D_BSD_SOURCE 
ifdef PLX_SDK_DIR 
CFLAGS += -DUSE_PLX
endif


RINC = $(ROSTA_SDK_DIR)/lin/ApiLib/include
PINC = $(PLX_SDK_DIR)/Include

all: libRostaPCIeAPI.a

libRostaPCIeAPI.a: driver-api.c device.c locate.c power.c inter-fpga.c
	$(CC) -I$(RINC) -I$(PINC) $(CFLAGS) -o $@ driver-api.c device.c locate.c power.c inter-fpga.c -lpthread
	cp $@ ..
	rm -f $@
	
# $@ - name of current task
# $< - name of current file, output file is dependent on

%.o: %.c
	$(CC) -I$(RINC) $(CFLAGS) -c $<
	
clean: 	
	-rm -f RostaPCIeAPI.a
	
	
	




