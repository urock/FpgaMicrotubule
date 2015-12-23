CC=g++
CFLAGS=-c -Wall -O2  -fopenmp 
LDFLAGS= -fopenmp -O2 -Wall
SOURCES=mt_cpu.cpp mt_app.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mt_app


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@	
	
	
clean:
	rm -rf *.o mt_app