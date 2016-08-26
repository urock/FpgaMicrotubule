CC=g++
CFLAGS = -g -Wall -O1 -std=c++11 -Wno-uninitialized


RINC = $(ROSTA_SDK_DIR)/lin/ApiLib/include
RLIB = $(ROSTA_SDK_DIR)/lin/ApiLib


FLIB = -L$(RLIB)  -lRostaPCIeAPI -lRC47 
ILIB = -I$(RINC)



SOURCES=rc47_mt_12.cpp mt_cpu.cpp rand.cpp fpga.cpp
JSON_SRC=jsoncpp/jsoncpp.cpp
OBJECTS=$(SOURCES:.cpp=.o)
JSON_OBJ=jsoncpp.o
EXECUTABLE=rc47_mt_12


all: $(SOURCES) $(EXECUTABLE) $(JSON_OBJ)

.cpp.o:
	$(CC) $(ILIB) $(CFLAGS) -c $< 

$(JSON_OBJ): $(JSON_SRC)
	$(CC) $(CFLAGS) -c $< 

$(EXECUTABLE): $(OBJECTS) $(JSON_OBJ)
	$(CC) $(LDFLAGS) $(OBJECTS) $(JSON_OBJ) -o $@ $(FLIB)



clean:
	-rm -f $(EXECUTABLE)
	-rm -f *.o



# CC=g++
# CFLAGS = -g -Wall  -O3 -fopenmp -Wno-uninitialized
# #CFLAGS = -g -Wall -Wimplicit-function-declaration -O3


# RINC = $(ROSTA_SDK_DIR)/lin/ApiLib/include
# RLIB = $(ROSTA_SDK_DIR)/lin/ApiLib



# FLIB = -L$(RLIB)  -lRostaPCIeAPI -lRC47 



# ILIB = -I$(RINC)



# TEST2=rc47_mt_12


# all:  $(TEST2)

# clean:
# 	-rm -f $(TEST2)
	
# $(TEST2): $(TEST2).cpp mt_cpu.cpp
# 	$(CC) $(ILIB) $(CFLAGS) -o $@ $(TEST2).cpp mt_cpu.cpp jsoncpp/jsoncpp.cpp $(FLIB)	 



