CC=g++
CFLAGS=-c -Wall -O3  -fopenmp -ftree-loop-vectorize -mfpmath=sse -mavx
LDFLAGS= -fopenmp -O3 -Wall
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