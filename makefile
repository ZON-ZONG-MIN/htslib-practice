CC = g++


test: test
		$(CC) main.cpp -o test -L/path/to/htslib/dir -I/path/to/htslib/dir -lhts

clean:
		rm test
