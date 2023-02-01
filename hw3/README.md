# CSF Fall 2020 - HW3: Cache Simulator
**Authors:** Simone Bliss (sbliss5) & Danny Lee (jlee692)

## Experiments
For our experiments we ran the following arguments using various strategies with the full results at the bottom of the file.
1. ./csim 256 4 16 - **WINNER:** write-allocate, write-back, lru
2. ./csim 1 1 4 - **WINNER:** write-allocate, write-back, lru (tie w/fifo because direct mapped)
3. ./csim 128 32 64 - **WINNER:** write-allocate, write-back, lru

We ran these particular experiments because we thought they covered a wide range of scenarios (sampled from the autograder), and got lots of cycles using gcc.trace. This way, we would to be able to most clearly see changes because it has so many requests. 

We got these particular results by updating the cycle and hit/miss counts as follows:
- 100 cycles for write-through (store)
- 100 cycles for no-write-allocate (store)
- (100 * (numBytes / 4) for reading/writing data to/from the main memory
- 1 cycle for reading/writing to/from the cache

## Best Cache Configuration
We think that the write-allocate write-back lru configuration has the best overall effectiveness because even though it ties oftenwith write-allocate write-through in hits and misses, it uses fewer processor cycles than write-through because the only time we write to memory is when a block is getting evicted. Though this means that the data in main memory is not always up to date, we get the big advantage of using the speed of the cache as well as mitigating some of the cost of a miss. 

We also saw that the lru configuration also uses fewer cycles than the fifo configuration in every configuration (except in the direct mapped case, in which case it's irrelevant) and has an advantage over fifo when data certain is being accessed repeatedly. Lastly, though no-write-allocate write-through was faster than write-allocate write-through (and write back in the direct mapped case), we saw that even though misses were less costly, they were much more frequent which we thought did not outweigh the benefit. This may need to be reconsidered if the block size were too large (in which case the misses would get more and more expensive). In addition, when the number of blocks per set is higher, write back becomes much faster relative to both of them, so this initial speed advantage goes away. Also, the better temporal and spacial locality of the trace can make write back even faster, so if the user is accessing the same data frequently or data stored near each other, write back becomes an even better choice.

Thus, though write-back may be more complicated to implement, we think it's speed and accuracy make it worth it. We found this pattern to be consistent across all cache sizes.

## Contributions
**Danny:**
- outline
- testing & debugging
- implementation structure
- error handling
- main function
- store-related functions
- cache function

**Simone:**
- README
- Makefile
- outline
- testing & debugging
- load-related functions
- output format function
- Valgrind + code cleanup

## Runs and Results

Total loads: 318197
Total stores: 197486

### #1
**no-write-allocate, write-through, lru**
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 22700397

**write-allocate, write-through, lru**
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 25120797

**write-allocate, write-back, lru**
Load hits: 314798
Load misses: 3399
Store hits: 188250
Store misses: 9236
Total cycles: 9344483  

**no-write-allocate, write-through, fifo**
Load hits: 311017
Load misses: 7180
Store hits: 163705
Store misses: 33781
Total cycles: 22938797

**write-allocate, write-through, fifo**
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 25452797

**write-allocate, write-back, fifo**
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9845283 

### #2
**no-write-allocate, write-through, lru**
Load hits: 10396
Load misses: 307801
Store hits: 16247
Store misses: 181239
Total cycles: 50846897

**write-allocate, write-through, lru**
Load hits: 13415
Load misses: 304782
Store hits: 13556
Store misses: 183930
Total cycles: 68937997

**write-allocate, write-back, lru** 
Load hits: 13415
Load misses: 304782
Store hits: 13556
Store misses: 183930
Total cycles: 68772983

**no-write-allocate, write-through, fifo**
Load hits: 10396
Load misses: 307801
Store hits: 16247
Store misses: 181239
Total cycles: 50846897

**write-allocate, write-through, fifo** 
Load hits: 13415
Load misses: 304782
Store hits: 13556
Store misses: 183930
Total cycles: 68937997

**write-allocate, write-back, fifo**
Load hits: 13415
Load misses: 304782
Store hits: 13556
Store misses: 183930
Total cycles: 68772983

### #3
**no-write-allocate, write-through, lru**
Load hits: 315894
Load misses: 2303
Store hits: 173709
Store misses: 23777
Total cycles: 23751597

**write-allocate, write-through, lru**
Load hits: 317200
Load misses: 997
Store hits: 195111
Store misses: 2375
Total cycles: 25461997

**write-allocate, write-back, lru**
Load hits: 317200
Load misses: 997
Store hits: 195111
Store misses: 2375
Total cycles: 5986083

**no-write-allocate, write-through, fifo**
Load hits: 315890
Load misses: 2307
Store hits: 173709
Store misses: 23777
Total cycles: 23757997

**write-allocate, write-through, fifo**
Load hits: 317188
Load misses: 1009
Store hits: 195110
Store misses: 2376
Total cycles: 25482797

**write-allocate, write-back, fifo**
Load hits: 317188
Load misses: 1009
Store hits: 195110
Store misses: 2376
Total cycles: 6019683 


