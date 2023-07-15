CC=g++
IDIR=-I/path/to/htslib/dir
CFLAGS=-std=c++11 -Wall -O3 $(IDIR)
LFLAGS=-lhts -lz #-lboost_iostreams

DEPS = main.cpp
OBJ = Util.o PhasingProcess.o Phasing.o  main.o 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ -c $< $(LFLAGS) -L/path/to/htslib/dir

longphase: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^  $(LFLAGS) -L/path/to/htslib/dir

.PHONY: clean

clean:
	rm -f *.o
