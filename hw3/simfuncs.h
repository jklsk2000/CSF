/*
* CSF Assignment 3
* Cache Simulator
* Function prototypes for cache simulator functions（headers）
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/

#ifndef SIMFUNCS_H
#define SIMFUNCS_H
#include <map>
#include <deque>
#include <string>

typedef struct policy {
    std::string sAllocationStrategy;
    std::string sWriteStrategy;
    std::string sEvictionPolicy;
    unsigned write_strategy; 
    unsigned allocation_strategy;
    unsigned eviction_policy;

    //constants for semantic meaning/clarity of comparisons
    const unsigned NO_WRITE_ALLOCATE = 0;
    const unsigned WRITE_ALLOCATE = 1;
    const unsigned WRITE_THROUGH = 0;
    const unsigned WRITE_BACK = 1;
    const unsigned LRU = 0;
    const unsigned FIFO = 1;
    const unsigned DIRTY = 1;
    
    policy(std::string sAllocationStrategy_, std::string sWriteStrategy_, std::string sEvictionPolicy_) {
        sAllocationStrategy = sAllocationStrategy_;
        sWriteStrategy = sWriteStrategy_;
        sEvictionPolicy = sEvictionPolicy_;
    }

} policy;

typedef struct {
    unsigned loads;
    unsigned stores;
    unsigned lHits;
    unsigned lMisses;
    unsigned sHits;
    unsigned sMisses;
    unsigned cycles;
} result;

int cache(long numSets, long numBlocks, long numBytes, 
    std::string sMissPol, std::string sHitPol, std::string evictPol);

//helper functions
unsigned setSize(unsigned num);

void decodePolicy(policy &pol);

void load(std::map<unsigned, std::deque<std::pair<unsigned, unsigned>>> &cacheMap, 
    long numBlocks, long numBytes, unsigned tag, unsigned index, policy pol, result &res);

void invalidLoadMiss(std::map<unsigned, std::deque<std::pair<unsigned, unsigned>>> &cacheMap, 
    long numBytes, unsigned tag, unsigned index, result &res);

void validLoadMiss(std::deque<std::pair<unsigned, unsigned>> &deq, 
    long numBlocks, long numBytes, unsigned tag, policy pol, result &res);

void loadHit(std::deque<std::pair<unsigned, unsigned>> &deq, 
    std::deque<std::pair<unsigned, unsigned>>::iterator deqit, policy pol, result &res);

void store(std::map<unsigned, std::deque<std::pair<unsigned, unsigned>>> &cacheMap, 
    long numBlocks, long numBytes, unsigned tag, unsigned index, policy pol, result &res);

void invalidStoreMiss(std::map<unsigned, std::deque<std::pair<unsigned, unsigned>>> &cacheMap,
    long numBytes, unsigned tag, unsigned index, policy pol, result &res);

void validStoreMiss(std::deque<std::pair<unsigned, unsigned>> &deq, 
    long numBlocks, long numBytes, unsigned tag, policy pol, result &res);

void storeHit(std::deque<std::pair<unsigned, unsigned>> &deq, 
    std::deque<std::pair<unsigned, unsigned>>::iterator &deqit, policy pol, result &res);

void outputFormat(result &res);

#endif