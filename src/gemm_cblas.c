
#include "gemm_cblas.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L

#define max_repeats 256
double measured_gflops[max_repeats];

int measure_gemm(int M, int N, int K) {
  int I, J;
  int lda = K;
  int ldb = N;
  int ldc = N;
  float alpha = 1.0;
  float beta = 0.0;

  float *a = aligned_alloc(4096, sizeof(float) * M * K);
  float *b = aligned_alloc(4096, sizeof(float) * K * N);
  float *c = aligned_alloc(4096, sizeof(float) * M * N);

  for (I = 0; I < M; I++) {
    for (J = 0; J < K; J++) {
      a[I * K + J] = (float)rand() / (float)(RAND_MAX / 2.0);
    }
  }
  for (I = 0; I < K; I++) {
    for (J = 0; J < N; J++) {
      b[I * N + J] = (float)rand() / (float)(RAND_MAX / 2.0);
    }
  }

#ifdef SINGLE_THREAD
  int repeats = 5;
#else
  int repeats = 32;
  if (M <= 1024) {
    repeats *= 2;
  }
  if (N <= 1024) {
    repeats *= 2;
  }
  if (K <= 1024) {
    repeats *= 2;
  }
#endif

  // Try to measure the best performance.
  double flop = ((double)M) * N * K * 2;
  for (int i = 0; i < repeats; ++i) {

    uint64_t diff = run_gemm(M, N, K, a, b, c, 1 /* repeats*/);

    double gflops = ((double)flop) / ((double)diff);
    measured_gflops[i] = gflops;
  }

  double best_gflops = 0;
  for (int i = 0; i < repeats; ++i) {
    double gflops = measured_gflops[i];
    if (gflops > best_gflops) {
      best_gflops = gflops;
    }
  }

  printf("M %5d N %5d K %5d Best %.lf GFlops.\n", M, N, K, best_gflops);

  free(a);
  free(b);
  free(c);

  return best_gflops;
}

double measure_gemm_multi(int M, int N, int K) {
  int less_count = 0;
  double best_gflops = 0;

#ifdef SINGLE_THREAD
  int less_count_threshold = 2;
#else
  int less_count_threshold = 5;
  if (M <= 1024) {
    less_count_threshold *= 2;
  }
  if (N <= 1024) {
    less_count_threshold *= 2;
  }
  if (K <= 1024) {
    less_count_threshold *= 2;
  }
#endif

  while (1) {
    double gflops = measure_gemm(M, N, K);
    if (gflops > best_gflops) {
      best_gflops = gflops;
      less_count = 0;
    } else {
      less_count++;
      if (less_count == less_count_threshold) {
        printf("Measured M %5d N %5d K %5d Best %.lf GFlops.\n", M, N, K,
               best_gflops);
        break;
      }
    }
  }

  return best_gflops;
}

const int dense_mm_MNK[][3] = {

#ifdef PROFILE
    {2048, 2048, 2048},
#else
    {5124, 700, 2048},  {35, 700, 2048},    {5124, 700, 2560},
    {35, 700, 2560},    {5124, 1500, 2048}, {35, 1500, 2048},
    {5124, 1500, 2560}, {35, 1500, 2560},   {7680, 1, 2560},
    {7680, 2, 2560},    {7680, 4, 2560},    {3072, 1, 1024},
    {3072, 2, 1024},    {3072, 4, 1024},    {64, 1, 1216},
    {512, 1, 500000},   {1024, 1, 500000},  {512, 2, 500000},
    {1024, 2, 500000},  {512, 4, 500000},   {1024, 4, 500000},
    {1024, 700, 512},   {7680, 1500, 2560}, {6144, 1500, 2048},
    {4608, 1500, 1536}, {8448, 1500, 2816}, {3072, 1500, 1024},
    {7680, 3000, 2560}, {6144, 3000, 2048}, {4608, 3000, 1536},
    {8448, 3000, 2816}, {3072, 3000, 1024}, {7680, 6000, 2560},
    {6144, 6000, 2048}, {4608, 6000, 1536}, {8448, 6000, 2816},
    {3072, 6000, 1024}, {6144, 1, 2048},    {4608, 1, 1536},
    {8448, 1, 2816},    {6144, 2, 2048},    {4608, 2, 1536},
    {8448, 2, 2816},    {6144, 4, 2048},    {4608, 4, 1536},
    {8448, 4, 2816},    {128, 1500, 1280},  {3072, 1500, 128},
    {128, 1, 1024},     {3072, 1, 128},     {176, 1500, 1408},
    {4224, 1500, 176},  {128, 1, 1408},     {4224, 1, 128},
    {512, 1500, 2816},  {512, 1500, 2048},  {512, 1500, 2560},
    {512, 1500, 1530},  {1024, 1500, 2816}, {1024, 1500, 2048},
    {1024, 1500, 2560}, {1024, 1500, 1530}, {512, 1, 512},
    {1024, 1, 512},     {512, 3000, 2816},  {512, 3000, 2048},
    {512, 3000, 2560},  {512, 3000, 1530},  {1024, 3000, 2816},
    {1024, 3000, 2048}, {1024, 3000, 2560}, {1024, 3000, 1530},
    {512, 2, 512},      {1024, 2, 512},     {512, 6000, 2816},
    {512, 6000, 2048},  {512, 6000, 2560},  {512, 6000, 1530},
    {1024, 6000, 2816}, {1024, 6000, 2048}, {1024, 6000, 2560},
    {1024, 6000, 1530}, {512, 4, 512},      {1024, 4, 512},
#endif
};

int main() {

  int benchmark_count = sizeof(dense_mm_MNK) / sizeof(dense_mm_MNK[0]);

  FILE *csv = fopen("dense_mm.csv", "w");
  fprintf(csv, "M,N,K,Bytes,Time (ns),Flop,Compute Intensity "
               "(flop/byte),Compute Throughput (GFlops)\n");

  for (int i = 0; i < benchmark_count; ++i) {
    int M = dense_mm_MNK[i][0];
    int N = dense_mm_MNK[i][1];
    int K = dense_mm_MNK[i][2];

    printf("%d %d %d %d %d %d.\n", M, N, K, 4 * M * K, K * N, M * N);

    double gflops = measure_gemm_multi(M, N, K);

    double flop = ((double)M) * N * K * 2;
    double ns = flop / gflops;
    double bytes =
        (((double)M) * N + ((double)M) * K + ((double)K) * N) * sizeof(float);

    double compute_intensity = flop / bytes;

    fprintf(csv, "%d,%d,%d,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf\n", M, N, K, bytes, ns,
            flop, compute_intensity, gflops);
  }

  fclose(csv);
  return 0;
}
