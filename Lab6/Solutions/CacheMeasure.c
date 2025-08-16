#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h>  // for __rdtscp, _mm_clflush, fences

// How many samples to take for each case
#define SAMPLES 100

// Fenced timing helper to get clean cycle counts
static inline uint64_t timed_read(volatile uint8_t *addr) {
    unsigned aux;
    uint64_t t0 = __rdtscp(&aux);
    

    (void)*addr;  // the memory access we’re timing

   
    uint64_t t1 = __rdtscp(&aux);
  
    return t1 - t0;
}

int main(void) {
        uint8_t data = 1;
    // Allocate one cache‐aligned byte
    volatile uint8_t *buf =&data; 
          *buf = 123;  // initialize and fault page in

    // Storage for our results
    uint64_t hit_times[SAMPLES];
    uint64_t miss_times[SAMPLES];

    for (int i = 0; i < SAMPLES; i++) {
        // --- Cached read ---
        (void)*buf;               // ensure it’s in L1/L2
        hit_times[i] = timed_read(buf);

        // --- Uncached read ---
        _mm_clflush((void*)buf);  // evict from all caches
        miss_times[i] = timed_read(buf);
    }

    // Output CSV to file
    FILE *f = fopen("results.csv", "w");
    if (!f) {
        perror("fopen results.csv");
        return 1;
    }
    fprintf(f, "iter,hit,miss\n");
    for (int i = 0; i < SAMPLES; i++) {
        fprintf(f, "%d,%llu,%llu\n",
                i,
                (unsigned long long)hit_times[i],
                (unsigned long long)miss_times[i]);
    }
    fclose(f);

    printf("Done: %s written with %d samples each.\n", "results.csv", SAMPLES);
    return 0;
}
