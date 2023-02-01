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
#include <string>
#include <deque>
#include <algorithm>
#include <iterator>
#include <math.h>
#include "simfuncs.h"

using std::map; using std::iterator; using std::pair;
using std::cout; using std::endl; using std::string;
using std::cin; using std::string; using std::hex;
using std::deque; using std::find;

int cache(long numSets, long numBlocks, long numBytes, string sMissPol, string sHitPol, string evictPol) {
    
    map<unsigned, deque<pair<unsigned, unsigned>>> cacheMap;

    // Set offset, index, and tag size
    unsigned offsetSize = setSize(numBytes);
    unsigned indexSize = setSize(numSets);

    char memreq;                            // load or store memory request
    unsigned memaddr;                       // 32-bit memory address
    unsigned useless;                       // third argument (trivial)

    policy pol = {sMissPol, sHitPol, evictPol, 0, 0, 0};
    decodePolicy(pol);

    result res = {0, 0, 0, 0, 0, 0, 0};

    while (cin >> memreq >> hex >> memaddr >> useless) {
        // chop up memory address into index, and tag (offset does not matter)
        unsigned tag = memaddr >> (offsetSize + indexSize);
        memaddr -= tag << (offsetSize + indexSize);
        unsigned index = memaddr >> offsetSize;

        // load or store?
        if (memreq == 'l') {
            load(cacheMap, numBlocks, numBytes, tag, index, pol, res);
        } else if (memreq == 's') {
            store(cacheMap, numBlocks, numBytes, tag, index, pol, res);
        }
    }

    outputFormat(res);
    return 0;
}

// Helper functions

unsigned setSize(unsigned num) {
    return log2 (num);
}

void decodePolicy(policy &pol) {
    // miss = 0: no-write-allocate; miss = 1: write-allocate
    if ((pol.sMissPol).compare("no-write-allocate") == 0) {
        pol.miss = 0;
    } else if ((pol.sMissPol).compare("write-allocate") == 0) {
        pol.miss = 1;
    }

    // hit = 0: write-through; hit = 1; write-back
    if ((pol.sHitPol).compare("write-through") == 0) {
        pol.hit = 0;
    } else if ((pol.sHitPol).compare("write-back") == 0) {
        pol.hit = 1;            
    }

    // evict = 0: lru; evict = 1; fifo
    if ((pol.evictPol).compare("lru") == 0) {
        pol.evict = 0;
    } else if ((pol.evictPol).compare("fifo") == 0) {
        pol.evict = 1;
    }
}

void load(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap, long numBlocks, 
    long numBytes, unsigned tag, unsigned index, policy pol, result &res) {
    
    auto it = cacheMap.find(index);

    if (it == cacheMap.end()) {                                                 // load miss: valid bit = 0
        invalidLoadMiss(cacheMap, numBytes, tag, index, res);                               
    }  else {                                                                   // check tag: valid bit = 1
        auto deqit = find_if((it->second).begin(), (it->second).end(), 
            [&tag](const pair<unsigned, unsigned>& element) {
            return element.first == tag;
        });
        if (deqit == (it->second).end()) {                                      // load miss: no tag matches
            validLoadMiss(it->second, numBlocks, numBytes, tag, pol, res);
        } else {                                                                // load hit: tag matches
            loadHit(it->second, deqit, pol, res);            
        }
    }

    res.loads++;                                                                // increment load count
}

void invalidLoadMiss(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap, 
    long numBytes, unsigned tag, unsigned index, result &res) {

    deque<pair<unsigned, unsigned>> deq;
    deq.push_front({tag, 0});
    cacheMap.insert({index, deq});
    res.cycles += (100 * (numBytes / 4));                       // load data from main memory & store data to cache block
    res.lMisses++;                                              // increment load misses
}

void validLoadMiss(deque<pair<unsigned, unsigned>> &deq, long numBlocks, long numBytes, 
    unsigned tag, policy pol, result &res) {

    if (deq.size() >= (unsigned) numBlocks) {                   // eviction needed
        if (pol.hit == 1 && deq.back().second == 1) {           // write-back: check for dirty bit
            res.cycles += 100 * (numBytes / 4);                 // write data with dirty bit to main memory
        }
        deq.pop_back();                                         // evict back block (works for both lru/fifo)
    }
    deq.push_front({tag, 0});
    res.cycles += (100 * (numBytes / 4));                       // load data from main memory & store data to cache block
    res.lMisses++;                                              // increment load misses
}

void loadHit(deque<pair<unsigned, unsigned>> &deq, deque<pair<unsigned, unsigned>>::iterator deqit, 
    policy pol, result &res) {

    if (pol.evict == 0) {                                       // lru: cache hit means most recently used
        deq.erase(deqit);                                       // erase from original position
        deq.push_front({deqit->first, deqit->second});          // place the block in the front
    }                                                           // fifo: deque ds takes care of fifo
    res.cycles++;                                               // load data from cache block
    res.lHits++;                                                // increment load hits
}

void store(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap, long numBlocks, 
    long numBytes, unsigned tag, unsigned index, policy pol, result &res) {

    auto it = cacheMap.find(index);

    if (it == cacheMap.end()) {                                                 // store miss: valid bit = 0
        invalidStoreMiss(cacheMap, numBlocks, numBytes, tag, index, pol, res);
    } else {                                                                    // check tag: valid bit = 1
        auto deqit = find_if((it->second).begin(), (it->second).end(), 
            [&tag](const pair<unsigned, unsigned>& element) {
            return element.first == tag;
        });

        if (deqit == (it->second).end()) {                                      // store miss: no tag matches
            validStoreMiss(it->second, numBlocks, numBytes, tag, pol, res);
        } else {                                                                // store hit: tag matches
            storeHit(it->second, deqit, numBlocks, numBytes, pol, res);
        }
    }
    
    res.stores++;                                                               // increment store count
}

void invalidStoreMiss(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap,
    long numBlocks, long numBytes, unsigned tag, unsigned index, policy pol, result &res) {

    if (pol.miss == 0) {                                            // no-write-allocate
        res.cycles += 100 * (numBytes / 4) / numBlocks;             // store data to main memory
    } else {                                                        // write-allocate
        deque<pair<unsigned, unsigned>> deq;
        deq.push_front({tag, 1});                                       
        cacheMap.insert({index, deq});                                  
        res.cycles += (100 * (numBytes / 4));                       // load data from main memory & store data to cache block
        res.cycles++;                                               // store new data to cache block
    }
    res.sMisses++;                                                  // increment store misses
}

void validStoreMiss(deque<pair<unsigned, unsigned>> &deq, long numBlocks, long numBytes, 
    unsigned tag, policy pol, result &res) {

    if (pol.miss == 0) {                                            // no-write-allocate
        res.cycles += 100 * (numBytes / 4) / numBlocks;             // store data to main memory
    } else {                                                        // write-allocate
        if (deq.size() >= (unsigned) numBlocks) {                   // eviction needed
            if (pol.hit == 1 && deq.back().second == 1) {           // write-back: check for dirty bit
                res.cycles += 100 * (numBytes / 4);                 // write data with dirty bit to main memory
            }
            deq.pop_back();                                         // evict back block (works for both lru/fifo)
        }
        deq.push_front({tag, 1});                                   // push dirty cache block into deque
        res.cycles += (100 * (numBytes / 4));                       // load data from main memory & store data to cache block
        res.cycles++;                                               // store new data to cache block
    }
    res.sMisses++;                                                  // increment store misses
}

void storeHit(deque<pair<unsigned, unsigned>> &deq, deque<pair<unsigned, unsigned>>::iterator &deqit, 
    long numBlocks, long numBytes, policy pol, result &res) {

    if (pol.hit == 0) {                                             // write-through
        res.cycles += 100 * (numBytes / 4) / numBlocks;             // store data to main memory & cache block
    } else {                                                        // write-back
        res.cycles++;                                               // store new data to cache block
        deqit->second = 1;                                          // mark cache block as dirty
    }

    if (pol.evict == 0) {                                           // lru
        deq.erase(deqit);
        deq.push_front({deqit->first, deqit->second});
    }

    res.sHits++;                                                    // increment store hits
}

void outputFormat(result &res) {
    cout << "Total loads: " << res.loads << endl;
    cout << "Total stores: " << res.stores << endl;
    cout << "Load hits: " << res.lHits << endl;
    cout << "Load misses: " << res.lMisses << endl;
    cout << "Store hits: " << res.sHits << endl;
    cout << "Store misses: " << res.sMisses << endl;
    cout << "Total cycles: " << res.cycles << endl;
}
