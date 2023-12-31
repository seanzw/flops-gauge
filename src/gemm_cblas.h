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

#define max_repeats 2048
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

#ifdef USE_OPENBLAS
void gemm_bf16bf16f32_init(int M, int N, int K, void **a, void **b, void **c) {
  *a = aligned_alloc(4096, sizeof(uint16_t) * M * K);
  *b = aligned_alloc(4096, sizeof(uint16_t) * K * N);
  *c = aligned_alloc(4096, sizeof(float) * M * N);
}

uint64_t gemm_bf16bf16f32_wrap(int M, int N, int K, void *a, void *b, void *c) {
  int lda = K;
  int ldb = N;
  int ldc = N;
  float alpha = 1.0;
  float beta = 0.0;
  cblas_sbgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha,
               (bfloat16 *)a, lda, (bfloat16 *)b, ldb, beta, (float *)c, ldc);
}
#endif

#ifdef USE_MKL
// Not sure if these exists in AOCL so limit to MKL.
void gemm_bf16bf16f32_init(int M, int N, int K, void **a, void **b, void **c) {
  *a = aligned_alloc(4096, sizeof(uint16_t) * M * K);
  *b = aligned_alloc(4096, sizeof(uint16_t) * K * N);
  *c = aligned_alloc(4096, sizeof(float) * M * N);
}

uint64_t gemm_bf16bf16f32_wrap(int M, int N, int K, void *a, void *b, void *c) {
  int lda = K;
  int ldb = N;
  int ldc = N;
  float alpha = 1.0;
  float beta = 0.0;
  cblas_gemm_bf16bf16f32(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K,
                         alpha, (MKL_BF16 *)a, lda, (MKL_BF16 *)b, ldb, beta,
                         (float *)c, ldc);
}

void gemm_s8u8s32_init(int M, int N, int K, void **a, void **b, void **c) {
  *a = aligned_alloc(4096, sizeof(MKL_INT8) * M * K);
  *b = aligned_alloc(4096, sizeof(MKL_INT8) * K * N);
  *c = aligned_alloc(4096, sizeof(int32_t) * M * N);
}

uint64_t gemm_s8u8s32_wrap(int M, int N, int K, void *a, void *b, void *c) {
  int lda = K;
  int ldb = N;
  int ldc = N;
  MKL_INT8 a_offset = 0;
  MKL_INT8 b_offset = 0;
  int32_t c_offset = 0;
  float alpha = 1.0;
  float beta = 0.0;
  cblas_gemm_s8u8s32(CblasRowMajor, CblasNoTrans, CblasNoTrans, CblasFixOffset,
                     M, N, K, alpha, (MKL_INT8 *)a, lda, a_offset,
                     (MKL_INT8 *)b, ldb, b_offset, beta, (int32_t *)c, ldc,
                     &c_offset);
}

#endif

struct DataTypeConfig {
  const char *type;
  wrap_gemm gemm;
  init_gemm init;
  int a_size;
  int b_size;
  int c_size;
};

struct DataTypeConfig data_type_configs[] = {
#ifdef PROFILE

#ifdef USE_OPENBLAS
    {
        .type = "bf16bf16f32",
        .gemm = gemm_bf16bf16f32_wrap,
        .init = gemm_bf16bf16f32_init,
        .a_size = sizeof(bfloat16),
        .b_size = sizeof(bfloat16),
        .c_size = sizeof(float),
    },
#endif

#ifdef USE_MKL
    {
        .type = "s8u8s32",
        .gemm = gemm_s8u8s32_wrap,
        .init = gemm_s8u8s32_init,
        .a_size = sizeof(MKL_INT8),
        .b_size = sizeof(MKL_INT8),
        .c_size = sizeof(int32_t),
    },
#endif

#else

// {
//     .type = "float",
//     .gemm = sgemm_wrap,
//     .init = sgemm_init,
//     .a_size = sizeof(float),
//     .b_size = sizeof(float),
//     .c_size = sizeof(float),
// },
// {
//     .type = "double",
//     .gemm = dgemm_wrap,
//     .init = dgemm_init,
//     .a_size = sizeof(double),
//     .b_size = sizeof(double),
//     .c_size = sizeof(double),
// },

#ifdef USE_OPENBLAS
    {
        .type = "bf16bf16f32",
        .gemm = gemm_bf16bf16f32_wrap,
        .init = gemm_bf16bf16f32_init,
        .a_size = sizeof(bfloat16),
        .b_size = sizeof(bfloat16),
        .c_size = sizeof(float),
    },
#endif

#ifdef USE_MKL
    {
        .type = "bf16bf16f32",
        .gemm = gemm_bf16bf16f32_wrap,
        .init = gemm_bf16bf16f32_init,
        .a_size = sizeof(MKL_BF16),
        .b_size = sizeof(MKL_BF16),
        .c_size = sizeof(float),
    },
    {
        .type = "s8u8s32",
        .gemm = gemm_s8u8s32_wrap,
        .init = gemm_s8u8s32_init,
        .a_size = sizeof(MKL_INT8),
        .b_size = sizeof(MKL_INT8),
        .c_size = sizeof(int32_t),
    },
#endif
#endif

};

#endif
