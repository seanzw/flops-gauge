
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#define INCF(F) INCF_(F)
#define INCF_(F) #F

#ifdef kernel_config
#define kernel_str INCF_(kernel_config)
#define kernel_header kernel_config.h
#include INCF(kernel_header)
#endif

#ifndef kernel_str
#error "Please select kernel"
#endif

void nop_kernel_init() {}

#ifndef kernel_init
#define kernel_init nop_kernel_init
#endif

#define BILLION 1000000000L

int main(int argc, char **argv) {
  uint64_t diff;
  struct timespec start, end;

  kernel_init();
  int64_t N = 8000000000;
  int64_t numThreads = 1;

#ifdef USE_OPENMP

  // Get number of threads.
  if (argc > 1) {
    numThreads = atoi(argv[1]);
  }
  omp_set_num_threads(numThreads);

  // Init threads.
  {
    int x;
    int *px = &x;
#pragma omp parallel for schedule(static)
    for (int i = 0; i < numThreads; ++i) {
      volatile int x = *px;
    }
  }


  // Multi thread.
  clock_gettime(CLOCK_MONOTONIC, &start);
#pragma omp parallel
  { kernel(N); }
  clock_gettime(CLOCK_MONOTONIC, &end);

#else

  // Single thread.
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  kernel(N);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

#endif

  diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

  int64_t flop = N * numThreads * flop_per_iter;
  double gflops = ((double)flop) / ((double)diff);

  printf("[%s] CPU Threads %4ld time = %10llu ns %.4f GFlops/Core.\n",
         kernel_str, numThreads, (long long unsigned int)diff, gflops);

  return 0;
}
