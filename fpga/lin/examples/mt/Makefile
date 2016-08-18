CC=g++
CFLAGS = -g -Wall  -O3 -fopenmp -Wno-uninitialized
#CFLAGS = -g -Wall -Wimplicit-function-declaration -O3


RINC = $(ROSTA_SDK_DIR)/lin/ApiLib/include
RLIB = $(ROSTA_SDK_DIR)/lin/ApiLib



FLIB = -L$(RLIB)  -lRostaPCIeAPI -lRC47 



ILIB = -I$(RINC)



TEST2=rc47_mt_12


all:  $(TEST2)

clean:
	-rm -f $(TEST2)
	
$(TEST2): $(TEST2).cpp mt_cpu.cpp
	$(CC) $(ILIB) $(CFLAGS) -o $@ $(TEST2).cpp mt_cpu.cpp jsoncpp/jsoncpp.cpp $(FLIB)	 


