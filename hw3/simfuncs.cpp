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

/*
 * Purpose: simulate a cache according to the command line arguments specified by the user (in main)
 * Parameters: 
 * 		numSets - (long) number of sets in the cache
 *      numbytes
 *      sAllocationStrategy_ - string containing the allocation strategy (no-write-allocate or write-allocate)
 *      sWriteStrategy_ - string containing the write strategy (write-through or write-back)
 *      sEvictionPolicy_ - string containing the eviction policy ()
 *      
 * Returns:
 * 		0
 */
int cache(long numSets, long numBlocks, long numBytes, string sAllocationStrategy_, string sWriteStrategy_, string sEvictionPolicy_) {
    
    
    map<unsigned, deque<pair<unsigned, unsigned>>> cacheMap;

    // Set offset, index, and tag size
    unsigned offsetSize = setSize(numBytes);
    unsigned indexSize = setSize(numSets);

    char memreq;        // load or store memory request
    unsigned memaddr;   // 32-bit memory address
    unsigned useless;   // third argument (trivial)

    policy pol(sAllocationStrategy_, sWriteStrategy_, sEvictionPolicy_);
    
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

/*
 * Purpose: set size of the offset or the index
 * Parameters: 
 * 		num - an unsigned int, either the offset or the index
 * Returns:
 * 		the log2 of num
 */
unsigned setSize(unsigned num) {
    return log2 (num);
}

/*
 * Purpose: sets the allocation_strategy, write_strategy, and eviction_policy fields of the policy
 *      struct with coded values (0 or 1) according to the command line arguments for easy 
 *      comparison later in the program
 * Parameters: 
 * 		&pol - the address of a policy struct
 * Returns:
 * 		void
 */
void decodePolicy(policy &pol) {
    if ((pol.sAllocationStrategy).compare("no-write-allocate") == 0) {
        pol.allocation_strategy = pol.NO_WRITE_ALLOCATE; // 0
    } else if ((pol.sAllocationStrategy).compare("write-allocate") == 0) {
        pol.allocation_strategy = pol.WRITE_ALLOCATE; // 1
    }

    // hit = 0: write-through; hit = 1; write-back
    if ((pol.sWriteStrategy).compare("write-through") == 0) {
        pol.write_strategy = pol.WRITE_THROUGH; // 0 
    } else if ((pol.sWriteStrategy).compare("write-back") == 0) {
        pol.write_strategy = pol.WRITE_BACK; // 1           
    }

    // evict = 0: lru; evict = 1; fifo
    if ((pol.sEvictionPolicy).compare("lru") == 0) {
        pol.eviction_policy = pol.LRU; // 0
    } else if ((pol.sEvictionPolicy).compare("fifo") == 0) {
        pol.eviction_policy = pol.FIFO; // 1
    }
}

/*
 * Purpose: perform operations necessary in the event of a load
 * Parameters: 
 *      &cacheMap - a map address of the cache map (unsigned as key (index), deque of tag and dirty status pair as value)
 *      numBlocks - (long) number of blocks in the each set
 *      numBytes - (long) number of bytes per block     
 *      tag - (unsigned) tag section of the address
 *      index - (unsigned) index section of the address
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void load(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap, long numBlocks, 
    long numBytes, unsigned tag, unsigned index, policy pol, result &res) {
    
    auto it = cacheMap.find(index);

    if (it == cacheMap.end()) {                                     // load miss: valid bit = 0
        invalidLoadMiss(cacheMap, numBytes, tag, index, res);                               
    }  else {                                                       // check tag: valid bit = 1
        auto deqit = find_if((it->second).begin(), (it->second).end(), 
            [&tag](const pair<unsigned, unsigned>& element) {
            return element.first == tag;
        });
        if (deqit == (it->second).end()) {                          // load miss: no tag matches
            validLoadMiss(it->second, numBlocks, numBytes, tag, pol, res);
        } else {                                                    // load hit: tag matches
            loadHit(it->second, deqit, pol, res);            
        }
    }

    res.loads++;                                                    // increment load count
}

/*
 * Purpose: perform operations necessary in the event of a load miss in which the index does not match
 * Parameters: 
 *      &cacheMap - a map address of the cache map (unsigned as key (index), deque of tag and dirty status pair as value)
 *      numBytes - (long) number of bytes per block
 *      tag - (unsigned) tag section of the address
 *      index - (unsigned) index section of the address
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void invalidLoadMiss(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap, 
    long numBytes, unsigned tag, unsigned index, result &res) {

    deque<pair<unsigned, unsigned>> deq;
    deq.push_front({tag, 0});
    cacheMap.insert({index, deq});
    res.cycles += (100 * (numBytes / 4));                           // read data from main memory to cache
    res.cycles++;                                                   // return data from cache
    res.lMisses++;                                                  // increment load misses
}


/*
 * Purpose: perform operations necessary in the event of a load miss in which the index matches 
 *          but the tag does not
 * Parameters: 
 * 		&deq - address of the deque of block-dirty(indicator) pairs 
 *      numBlocks - long number of blocks in the each set
 *      tag - (unsigned) tag section of the address
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void validLoadMiss(deque<pair<unsigned, unsigned>> &deq, long numBlocks, long numBytes, 
    unsigned tag, policy pol, result &res) {

    if (deq.size() >= (unsigned) numBlocks) {                       // eviction needed
        if (pol.write_strategy == pol.WRITE_BACK && deq.back().second == pol.DIRTY) {// write-back: check for dirty bit (1)
            res.cycles += 100 * (numBytes / 4);                     // write data with dirty bit to main memory
        }
        deq.pop_back();                                             // evict back block (works for both lru/fifo)
    }
    deq.push_front({tag, 0});
    res.cycles += (100 * (numBytes / 4));                           // read data from main memory to cache
    res.cycles++;                                                   // retrun data from cache
    res.lMisses++;                                                  // increment load misses
}


/*
 * Purpose: perform operations necessary in the event of a load hit
 * Parameters: 
 * 		&deq - address of the deque of block-dirty(indicator) pairs
 *      deqit - deque iterator for deq
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void loadHit(deque<pair<unsigned, unsigned>> &deq, deque<pair<unsigned, unsigned>>::iterator deqit, 
    policy pol, result &res) {

    if (pol.eviction_policy == pol.LRU) {                           // lru: cache hit means most recently used
        unsigned tag = deqit->first;
        unsigned dirty = deqit->second;
        deq.erase(deqit);                                           // erase from original position
        deq.push_front({tag, dirty});                               // place the block in the front
    }                                                               // fifo: deque ds takes care of fifo
    res.cycles++;                                                   // load data from cache block
    res.lHits++;                                                    // increment load hits
}

/*
 * Purpose: perform operations necessary in the event of a store
 * Parameters: 
 *      &cacheMap - a map address of the cache map (unsigned as key (index), deque of tag and dirty status pair as value)
 *      numBlocks - (long) number of blocks in the each set
 *      numBytes - (long) number of bytes per block     
 *      tag - (unsigned) tag section of the address
 *      index - (unsigned) index section of the address
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void store(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap, long numBlocks, 
    long numBytes, unsigned tag, unsigned index, policy pol, result &res) {

    auto it = cacheMap.find(index);

    if (it == cacheMap.end()) {                                     // store miss: valid bit = 0
        invalidStoreMiss(cacheMap, numBytes, tag, index, pol, res);
    } else {                                                        // check tag: valid bit = 1
        auto deqit = find_if((it->second).begin(), (it->second).end(), 
            [&tag](const pair<unsigned, unsigned>& element) {
            return element.first == tag;
        });

        if (deqit == (it->second).end()) {                          // store miss: no tag matches
            validStoreMiss(it->second, numBlocks, numBytes, tag, pol, res);
        } else {                                                    // store hit: tag matches
            storeHit(it->second, deqit, pol, res);
        }
    }
    
    res.stores++;                                                   // increment store count
}

/*
 * Purpose: perform operations necessary in the event of a store miss in which the index does not match
 * Parameters: 
 *      &cacheMap - a map address of the cache map (unsigned as key (index), deque of tag and dirty status pair as value)
 *      numBytes - (long) number of bytes per block
 *      tag - (unsigned) tag section of the address
 *      index - (unsigned) index section of the address
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void invalidStoreMiss(map<unsigned, deque<pair<unsigned, unsigned>>> &cacheMap,
    long numBytes, unsigned tag, unsigned index, policy pol, result &res) {

    if (pol.allocation_strategy == pol.NO_WRITE_ALLOCATE) {         // no-write-allocate
        res.cycles += 100;                                          // store data to main memory
    } else {                                                        // write-allocate
        deque<pair<unsigned, unsigned>> deq;
        deq.push_front({tag, 1});                                       
        cacheMap.insert({index, deq});                                  
        res.cycles += (100 * (numBytes / 4));                       // load data from main memory & store data to cache block
        if (pol.write_strategy == pol.WRITE_THROUGH) {
            res.cycles += 100;         
        } else {
            res.cycles++;                                           // store new data to cache block
        }            
    }
    res.sMisses++;                                                  // increment store misses
}

/*
 * Purpose: perform operations necessary in the event of a store miss in which the index matches 
 *          but the tag does not
 * Parameters: 
 * 		&deq - address of the deque of block-dirty(indicator) pairs
 *      numBlocks - long number of blocks in the each set
 *      tag - (unsigned) tag section of the address
 *      numBytes - (long) number of bytes per block
 *      &deqit - deque iterator for deq
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void validStoreMiss(deque<pair<unsigned, unsigned>> &deq, long numBlocks, long numBytes, 
    unsigned tag, policy pol, result &res) {

    if (pol.allocation_strategy == pol.NO_WRITE_ALLOCATE) {         // no-write-allocate
        res.cycles += 100;                                          // store data to main memory
    } else {                                                        // write-allocate
        if (deq.size() >= (unsigned) numBlocks) {                   // eviction needed
            if (pol.write_strategy == pol.WRITE_BACK && deq.back().second == pol.DIRTY) { // write-back: check for dirty bit
                res.cycles += 100 * (numBytes / 4);                 // write data with dirty bit to main memory
            }
            deq.pop_back();                                         // evict back block (works for both lru/fifo)
        }
        deq.push_front({tag, 1});                                   // push dirty cache block into deque
        res.cycles += (100 * (numBytes / 4));                       // load data from main memory & store data to cache block
        
        if (pol.write_strategy == pol.WRITE_THROUGH) {
            res.cycles += 100;         
        } else {
            res.cycles++;                                           // store new data to cache block
        }    
    }
    res.sMisses++;                                                  // increment store misses
}

/**
 * Purpose: perform operations necessary in the event of a store hit
 * Parameters: 
 * 		&deq - address of the deque of block-dirty(indicator) pairs
 *      &deqit - deque iterator for deq
 *      pol - cache policy struct to indicate to the functions the operations to implement 
 *          (based on command line arguments originally entered)
 *      &res - address of the result struct to be updated   
 * Returns:
 * 		void
 */
void storeHit(deque<pair<unsigned, unsigned>> &deq, deque<pair<unsigned, unsigned>>::iterator &deqit, 
    policy pol, result &res) {

    if (pol.write_strategy == pol.WRITE_THROUGH) {                  // write-through
        res.cycles += 100;                                          // store data to main memory & cache block
    } else {                                                        // write-back
        res.cycles++;                                               // store new data to cache block
        deqit->second = 1;                                          // mark cache block as dirty
    }

    if (pol.eviction_policy == pol.LRU) {                           // lru
        unsigned tag = deqit->first;
        unsigned dirty = deqit->second;
        deq.erase(deqit);                                      
        deq.push_front({tag, dirty});
    }

    res.sHits++;                                                    // increment store hits
}

/*
 * Purpose: format and output the results of a given cache simulation
 * Parameters: 
 * 		&res - the address of a result struct
 * Returns:
 * 		void
 */
void outputFormat(result &res) {
    cout << "Total loads: " << res.loads << endl;
    cout << "Total stores: " << res.stores << endl;
    cout << "Load hits: " << res.lHits << endl;
    cout << "Load misses: " << res.lMisses << endl;
    cout << "Store hits: " << res.sHits << endl;
    cout << "Store misses: " << res.sMisses << endl;
    cout << "Total cycles: " << res.cycles << endl;
}