
#include "gemm_cblas.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define BILLION 1000000000L

double measured_gflops[max_repeats];

int measure_gemm(int M, int N, int K, void *a, void *b, void *c,
                 wrap_gemm gemm) {

#ifdef SINGLE_THREAD
  int repeats = 5;
#elif defined(PROFILE)
  int repeats = 64;
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

    uint64_t diff = run_gemm(M, N, K, a, b, c, gemm, 1 /* repeats*/);

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

  return best_gflops;
}

double measure_gemm_multi(int M, int N, int K, wrap_gemm gemm, init_gemm init) {
  int less_count = 0;
  double best_gflops = 0;

#ifdef SINGLE_THREAD
  int less_count_threshold = 2;
#else
  int less_count_threshold = 5;
  if (M <= 512) {
    less_count_threshold *= 2;
  }
  if (N <= 512) {
    less_count_threshold *= 2;
  }
  if (K <= 512) {
    less_count_threshold *= 2;
  }
#endif

  void *a;
  void *b;
  void *c;
  init(M, N, K, &a, &b, &c);

  while (1) {
    double gflops = measure_gemm(M, N, K, a, b, c, gemm);
    if (gflops > best_gflops * 1.01) {
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

  free(a);
  free(b);
  free(c);

  return best_gflops;
}

const int dense_mm_MNK[][3] = {

#ifdef PROFILE
    {2048, 2048, 2048},
#else
    // Some simple regular square marix size.
    {512, 512, 512},
    {1024 * 1, 1024 * 1, 1024 * 1},
    {1024 * 2, 1024 * 2, 1024 * 2},
    {1024 * 4, 1024 * 4, 1024 * 4},
    // {1024 * 8, 1024 * 8, 1024 * 8},
    // {1024 * 16, 1024 * 16, 1024 * 16},
    // {1024 * 32, 1024 * 32, 1024 * 32},
    // Matrix size from DeepBench
    {5124, 700, 2048},
    {35, 700, 2048},
    {5124, 700, 2560},
    {35, 700, 2560},
    {5124, 1500, 2048},
    {35, 1500, 2048},
    {5124, 1500, 2560},
    {35, 1500, 2560},
    {7680, 1, 2560},
    {7680, 2, 2560},
    {7680, 4, 2560},
    {3072, 1, 1024},
    {3072, 2, 1024},
    {3072, 4, 1024},
    {64, 1, 1216},
    {512, 1, 500000},
    {1024, 1, 500000},
    {512, 2, 500000},
    {1024, 2, 500000},
    {512, 4, 500000},
    {1024, 4, 500000},
    {1024, 700, 512},
    {7680, 1500, 2560},
    {6144, 1500, 2048},
    {4608, 1500, 1536},
    {8448, 1500, 2816},
    {3072, 1500, 1024},
    {7680, 3000, 2560},
    {6144, 3000, 2048},
    {4608, 3000, 1536},
    {8448, 3000, 2816},
    {3072, 3000, 1024},
    {7680, 6000, 2560},
    {6144, 6000, 2048},
    {4608, 6000, 1536},
    {8448, 6000, 2816},
    {3072, 6000, 1024},
    {6144, 1, 2048},
    {4608, 1, 1536},
    {8448, 1, 2816},
    {6144, 2, 2048},
    {4608, 2, 1536},
    {8448, 2, 2816},
    {6144, 4, 2048},
    {4608, 4, 1536},
    {8448, 4, 2816},
    {128, 1500, 1280},
    {3072, 1500, 128},
    {128, 1, 1024},
    {3072, 1, 128},
    {176, 1500, 1408},
    {4224, 1500, 176},
    {128, 1, 1408},
    {4224, 1, 128},
    {512, 1500, 2816},
    {512, 1500, 2048},
    {512, 1500, 2560},
    {512, 1500, 1530},
    {1024, 1500, 2816},
    {1024, 1500, 2048},
    {1024, 1500, 2560},
    {1024, 1500, 1530},
    {512, 1, 512},
    {1024, 1, 512},
    {512, 3000, 2816},
    {512, 3000, 2048},
    {512, 3000, 2560},
    {512, 3000, 1530},
    {1024, 3000, 2816},
    {1024, 3000, 2048},
    {1024, 3000, 2560},
    {1024, 3000, 1530},
    {512, 2, 512},
    {1024, 2, 512},
    {512, 6000, 2816},
    {512, 6000, 2048},
    {512, 6000, 2560},
    {512, 6000, 1530},
    {1024, 6000, 2816},
    {1024, 6000, 2048},
    {1024, 6000, 2560},
    {1024, 6000, 1530},
    {512, 4, 512},
    {1024, 4, 512},
#endif
};

int main(int argc, char *argv[]) {

  int benchmark_count = sizeof(dense_mm_MNK) / sizeof(dense_mm_MNK[0]);

  FILE *csv = fopen("dense_mm.csv", "w");

  // Dump all the data type information.
  fprintf(csv, ",,,");
  int data_type_config_count =
      sizeof(data_type_configs) / sizeof(data_type_configs[0]);
  for (int i = 0; i < data_type_config_count; ++i) {
    fprintf(csv, "%s,,,,,", data_type_configs[i].type);
  }
  fprintf(csv, "\n");

  fprintf(csv, "M,N,K,");
  for (int i = 0; i < data_type_config_count; ++i) {
    fprintf(csv, "Bytes,Time (ns),Flop,Compute Intensity "
                 "(flop/byte),Compute Throughput (GFlops),");
  }

  fprintf(csv, "\n");

  if (argc >= 2) {
    int threads = atoi(argv[1]);
    omp_set_num_threads(threads);
    printf("NumThreads %d.\n", threads);
  }

  for (int i = 0; i < benchmark_count; ++i) {
    int M = dense_mm_MNK[i][0];
    int N = dense_mm_MNK[i][1];
    int K = dense_mm_MNK[i][2];

    printf("%d %d %d %d %d %d.\n", M, N, K, 4 * M * K, K * N, M * N);

    fprintf(csv, "%d,%d,%d,", M, N, K);

    for (int j = 0; j < data_type_config_count; ++j) {
      struct DataTypeConfig *config = data_type_configs + j;

      printf("DataType %s\n", config->type);

      double gflops = measure_gemm_multi(M, N, K, config->gemm, config->init);

      double flop = ((double)M) * N * K * 2;
      double ns = flop / gflops;
      double bytes = ((double)M) * N * config->c_size +
                     ((double)M) * K * config->a_size +
                     ((double)K) * N * config->b_size;

      double compute_intensity = flop / bytes;

      fprintf(csv, "%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,", bytes, ns, flop,
              compute_intensity, gflops);
    }

    fprintf(csv, "\n");
  }

  fclose(csv);
  return 0;
}
