SRC_DIR=./cpp
BIN_DIR=./bin
CC=g++
CFLAGS=-c -O3

sat: sat.o
	$(CC) sat.o -o $(BIN_DIR)/sat
	rm -f *.o

sat.o: $(SRC_DIR)/sat.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/sat.cpp 

clean:
	rm -f *.o $(BIN_DIR)/sat