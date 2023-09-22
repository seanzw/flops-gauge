#ifndef GEMM_CLBAS_H
#define GEMM_CLBAS_H

#ifdef USE_MKL
// MKL
#include "mkl.h"
#else
// AOCL
#include "cblas.h"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L

#define max_repeats 256
double measured_gflops[max_repeats];

uint64_t run_gemm(int M, int N, int K, float *a, float *b, float *c,
                  int repeats) {
  int I, J;
  int lda = K;
  int ldb = N;
  int ldc = N;
  float alpha = 1.0;
  float beta = 0.0;

  struct timespec start, end;

  // Do some warm up first.
  int warm_ups = 5;
  for (int i = 0; i < warm_ups; ++i) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha, a,
                lda, b, ldb, beta, c, ldc);
  }

  // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < repeats; ++i) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha, a,
                lda, b, ldb, beta, c, ldc);
  }

  // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  clock_gettime(CLOCK_MONOTONIC, &end);

  uint64_t diff =
      BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  return diff;
}

#endif