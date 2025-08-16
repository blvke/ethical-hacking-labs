#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// TODO: Define this threshold based on your previous experiments
// Use it for determining the secret
#define CACHE_HIT_THRESHOLD 357

typedef struct datablock {
        uint8_t lpad[2048];
        uint8_t dat;
        uint8_t rpad[2047];
} DataBlock;

DataBlock array[256];


void victim() {
        uint8_t secret = 94;

        // It is helpful to give the victim time for a couple accesses to the data - this way, we
        // are less likely to be affected by cache-displacement strategies, which we really
        // don't want for this simple experiment.
        for (int i = 0; i < 20; i++) {
                uint8_t temp = array[secret].dat;
                array[secret].dat = 0;
                temp = array[secret].dat;
        }
}


void flushSideChannel() {
        for (int i = 0; i < 256; i++) {
                
                _mm_clflush(&array[i].dat);
    }

}


uint8_t reloadSideChannel() {
         int aux = 0;
        uint64_t timeElapse;
        int best_index = 0;
        uint64_t best_time = UINT64_MAX;

    for (int i = 255; i > -1; i--) {
        volatile uint8_t *add = &array[i].dat;

    uint64_t start = __rdtscp(&aux);
    aux = *add; // just to access the value of the data, to measure what is time needed to access it.
    uint64_t end = __rdtscp(&aux);
        timeElapse = end - start;
        if (timeElapse < best_time) {
           best_time = timeElapse;
           best_index = i;
        }
    }

    if (best_time <= CACHE_HIT_THRESHOLD) {
        return (uint8_t)best_index;
    } else {
        return -1;
    }




}


int main(int argc, const char** argv) {
        for ( int i = 0; i < 256; i++){ 
                array[i].dat = 1; 
}        

        flushSideChannel();
        victim();
        uint8_t secret = reloadSideChannel();
        printf("The secret is %d\n", secret);
}
