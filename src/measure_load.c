#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h>

#define ARRAY_SIZE 10000

// Function to flush a cache line
void flushCacheLine(volatile void* addr) {
    _mm_clflush(addr);
    _mm_mfence();  // Ensure the flush completes
}

// Function to write 0 to an array of size 10000
void writeToArray() {
    volatile uint64_t array[ARRAY_SIZE];
    
    // Initialize the array with 0
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = 0;
    }

    // Flush the entire array from the cache
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        flushCacheLine(&array[i]);
    }
}

// Function to measure the latency to load the first cache line
uint64_t measureLoadLatency(volatile void* addr) {
    uint64_t start, end;

    // Measure the start time
    start = __rdtsc();

    // Perform a load operation
    volatile uint64_t data = *(volatile uint64_t*)addr;

    // Measure the end time
    end = __rdtsc();

    // Calculate the elapsed cycles
    return end - start;
}

int main() {
    // Flush the cache line
    volatile uint64_t dummy;
    flushCacheLine(&dummy);

    // Write 0 to an array of size 10000
    writeToArray();

    // Measure the latency to load the first cache line
    uint64_t latency = measureLoadLatency(&dummy);

    // Print the result
    printf("Load Latency: %lu cycles\n", latency);

    return 0;
}
