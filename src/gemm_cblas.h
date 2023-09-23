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

typedef uint64_t (*wrap_gemm)(int, int, int, void *, void *, void *);
typedef void (*init_gemm)(int, int, int, void **, void **, void **);

uint64_t run_gemm(int M, int N, int K, void *a, void *b, void *c,
                  wrap_gemm gemm, int repeats) {
  struct timespec start, end;

  // Do some warm up first.
  int warm_ups = 5;
  for (int i = 0; i < warm_ups; ++i) {
    gemm(M, N, K, a, b, c);
  }

  // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < repeats; ++i) {
    gemm(M, N, K, a, b, c);
  }

  // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  clock_gettime(CLOCK_MONOTONIC, &end);

  uint64_t diff =
      BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  return diff;
}

void sgemm_init(int M, int N, int K, void **a, void **b, void **c) {
  *a = aligned_alloc(4096, sizeof(float) * M * K);
  *b = aligned_alloc(4096, sizeof(float) * K * N);
  *c = aligned_alloc(4096, sizeof(float) * M * N);
}

uint64_t sgemm_wrap(int M, int N, int K, void *a, void *b, void *c) {
  int lda = K;
  int ldb = N;
  int ldc = N;
  float alpha = 1.0;
  float beta = 0.0;
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha,
              (float *)a, lda, (float *)b, ldb, beta, (float *)c, ldc);
}

void dgemm_init(int M, int N, int K, void **a, void **b, void **c) {
  *a = aligned_alloc(4096, sizeof(double) * M * K);
  *b = aligned_alloc(4096, sizeof(double) * K * N);
  *c = aligned_alloc(4096, sizeof(double) * M * N);
}

uint64_t dgemm_wrap(int M, int N, int K, void *a, void *b, void *c) {
  int lda = K;
  int ldb = N;
  int ldc = N;
  double alpha = 1.0;
  double beta = 0.0;
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha,
              (double *)a, lda, (double *)b, ldb, beta, (double *)c, ldc);
}


#endif