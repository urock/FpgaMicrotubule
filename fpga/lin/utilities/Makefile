CC=gcc

PLX_DIR=$(ROSTA_SDK_DIR)/lin/ApiLib

RINC = $(ROSTA_SDK_DIR)/lin/ApiLib/include
RLIB = $(ROSTA_SDK_DIR)/lin/ApiLib

PINC = $(PLX_SDK_DIR)/Include
PLIB = $(PLX_SDK_DIR)/PlxApi/Library

FLIB = -L$(RLIB)  -lRostaPCIeAPI -lRC47 
ifdef PLX_SDK_DIR 
FLIB += -L$(PLIB)  -l:PlxApi.a -lm -ldl -lrt
endif

ILIB = -I$(RINC)
ifdef PLX_SDK_DIR 
ILIB += -I$(PINC)
endif

CFLAGS = -DGCC -O2 -Wall -pipe -DPLX_LINUX -I$(RINC) -I$(PINC)
ifdef PLX_SDK_DIR 
CFLAGS += -DUSE_PLX
endif

TEST1=rc47_find_devices
TEST2=rc47_config
TEST3=rc47_reset_v7
TEST4=rc47_monitor
TEST5=rc47_load_sd
TEST6=rc47_build_sd

all: $(TEST1) $(TEST2) $(TEST3) $(TEST4) $(TEST5) $(TEST6)

clean:
	-rm -f $(TEST1)
	-rm -f $(TEST2)
	-rm -f $(TEST3)
	-rm -f $(TEST4)
	-rm -f $(TEST5)
	-rm -f $(TEST6)


$(TEST1): $(TEST1).c 
	@echo
	@echo Compiling $@....
	@echo
	$(CC)  $(CFLAGS) -o $@ $(TEST1).c  $(FLIB) 

$(TEST2): $(TEST2).c 
	@echo
	@echo Compiling $@....
	@echo
	$(CC)  $(CFLAGS) -o $@ $(TEST2).c  $(FLIB)
		

$(TEST3): $(TEST3).c 
	@echo
	@echo Compiling $@....
	@echo
	$(CC)  $(CFLAGS) -o $@ $(TEST3).c  $(FLIB)

$(TEST4): $(TEST4).c 
	@echo
	@echo Compiling $@....
	@echo
	$(CC)  $(CFLAGS) -o $@ $(TEST4).c  $(FLIB)

$(TEST5): $(TEST5).c 
	@echo
	@echo Compiling $@....
	@echo
	$(CC)  $(CFLAGS) -o $@ $(TEST5).c  $(FLIB)

$(TEST6): $(TEST6).c 
	@echo
	@echo Compiling $@....
	@echo
	$(CC)  $(CFLAGS) -o $@ $(TEST6).c  $(FLIB)
	
	

