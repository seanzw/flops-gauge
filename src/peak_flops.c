
#include <stdint.h>
#include <stdio.h>
#include <time.h>

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

#define BILLION 1000000000L

int main(int argc, char **argv) {
  uint64_t diff;
  struct timespec start, end;
  int i;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  int N = 10000000;
  kernel(N);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

  diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

  int64_t flop = N * flop_per_iter;
  double gflops = ((double)flop) / ((double)diff);

  printf("[%s] CPU time = %llu ns %.4f GFlops/Core.\n", kernel_str,
         (long long unsigned int)diff, gflops);

  return 0;
}
