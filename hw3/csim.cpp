/*
* CSF Assignment 3
* Cache Simulator
* Function prototypes for cache simulator functions
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/

#include <iostream>
#include <string> 
#include <cctype>
#include <map>
#include <fstream>
#include <vector>
#include "simfuncs.h"

using std::cout; using std::endl; 
using std::cerr; using std::cin;
using std::string;

int main(int argc, char *argv[]) {
    // Error handling: wrong number of command line arguments
    if (argc < 6) {
        cerr << "Error: There must be at least 6 command line arguments." << endl;
        return 1;
    }

    // Assign each argument to a local variable
    long numSets = atol(argv[1]);
    long numBlocks = atol(argv[2]);
    long numBytes = atol(argv[3]);
    string sMissPol = argv[4];
    string sHitPol = argv[5];
    string evictPol = argv[6];

    // Error handling: these arguments can not be combined
    if (sMissPol.compare("no-write-allocate") == 0 && sHitPol.compare("write-back") == 0) {
        cerr << "Error: Can't have both no-write-allocate and write-back." << endl;
        return 1;
    }

    // Error handling: numSets, numBlocks -> MUST be a positive power of 2
    if (numSets < 0 || (numSets & (numSets - 1)) != 0) {
        cerr << "Error: Number of sets in a cache is invalid." << endl;
        return 1;
    }
    if (numBlocks < 0 || (numBlocks & (numBlocks - 1)) != 0) {
        cerr << "Error: Number of blocks in each set is invalid." << endl;
        return 1;
    }

    // Error handling for numBytes -> MUST be a positive power of 2 and at least 4
    if (numBytes < 4 || (numBytes & (numBytes - 1)) != 0) {
        cerr << "Error: Number of bytes in each block is invalid." << endl;
        return 1;
    }

    cache(numSets, numBlocks, numBytes, sMissPol, sHitPol, evictPol);

    return 0;
}