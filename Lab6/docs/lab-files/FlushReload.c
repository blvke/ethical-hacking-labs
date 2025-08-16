#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// TODO: Define this threshold based on your previous experiments
// Use it for determining the secret
#define CACHE_HIT_THRESHOLD 1000

typedef struct datablock {
	uint8_t lpad[508];
	uint8_t dat;
	uint8_t rpad[508];
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
	// TODO: Perform the flushing step
}

uint8_t reloadSideChannel() {
	int secret = -1;

	// TODO: Perform the reload attack to gain the secret

	return secret;
}


int main(int argc, const char** argv) {
	flushSideChannel();
	victim();
	uint8_t secret = reloadSideChannel();

	printf("The secret is %d\n", secret);
}
