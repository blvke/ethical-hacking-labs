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
	uint8_t lpad[1000];
	uint8_t dat;
	uint8_t rpad[1000];
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

	// TODO: Copy your code here

}

int reloadSideChannel() {
	int secret = -1;

	// TODO: Copy your code here

	return secret;
}

/**
 * Abuse the spectre principle to steal a secret. After calling this
 * function, the cache should be setup in such a way that we can extract the
 * secret value, which is saved at the normally inaccessible location buffer[larger_x]
 */
void spectreAttack(size_t larger_x) {
	// TODO: Implement the attack
}

int main() {
	flushSideChannel();
	size_t index_beyond = (size_t) ((size_t) secret - (size_t) buffer);
	fprintf(stderr, "Targeting address %p at offset %zu", secret, index_beyond);
	spectreAttack(index_beyond);
	int secret_byte = reloadSideChannel();

	printf("The secret is %d (%c).\n", secret_byte, secret_byte);

	return 0;
}
