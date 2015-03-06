# Default values for some variables. Please be aware
#  that some of them might be overriden by being passed in
#  to the 'make' program (see the 'sat.sh' script, in the
#  'sh' directory, inside the sources directory)

SRC_DIR=./src
BIN_DIR=./bin
BIN_NAME=sat
CC=g++
CFLAGS=-c -O3 -Wall

all: sat

sat: sat.o
	$(CC) sat.o -o $(BIN_DIR)/$(BIN_NAME)
	rm -f *.o

sat.o: $(SRC_DIR)/sat.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/sat.cpp

clean:
	rm -f *.o $(BIN_DIR)/sat
