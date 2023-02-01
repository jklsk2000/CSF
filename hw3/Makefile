# Makefile for CSF Fall 2020 - HW3: Cache Simulator
# Danny Lee & Simone Bliss
# jlee692@jhu.edu, sbliss5@jhu.edu

# Some variable definitions to save typing later on
CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)


# Target to create the csim executable
csim: csim.o simfuncs.o
	$(CC) csim.o simfuncs.o -o csim

# Compiles csim.cpp into an object file
csim.o: csim.cpp simfuncs.h
	$(CC) $(CFLAGS) -c csim.cpp

# Compiles simfuncs.cpp into an object file
simfuncs.o: simfuncs.cpp simfuncs.h
	$(CC) $(CFLAGS) -c simfuncs.cpp

# Use to make gradescope submission
.PHONY: solution.zip
solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile *.h *.cpp *.md

# 'make clean' to remove intermediate & executable files
clean:
	rm -f *.o csim solution.zip
