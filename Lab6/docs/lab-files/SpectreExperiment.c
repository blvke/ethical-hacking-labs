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

int size = 10;
DataBlock array[256];

void flushSideChannel() {

	// TODO: Copy your code here

}

int reloadSideChannel() {
	int secret = -1;

	// TODO: Copy your code here

	return secret;
}

void victim(size_t x) {
	if (x < size) {
		uint8_t temp = array[x].dat;
	}
}

int main() {
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
