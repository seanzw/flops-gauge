#ifndef PEAK_FLOPS_KERNEL_AVX2_FLOAT_H
#define PEAK_FLOPS_KERNEL_AVX2_FLOAT_H

#include "immintrin.h"
#include <stdint.h>

// Each fmadd is 8 flop.
const uint64_t flop_per_iter = 14 * 4 * 2;

__attribute__((noinline)) __m256d impl(int64_t N, double a, double b,
                                       double c) {

  __m256d A = _mm256_set1_pd(a);
  __m256d B = _mm256_set1_pd(b);
  __m256d C = _mm256_set1_pd(c);

  __m256d s;

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < N; ++i) {
    __asm__("vfmadd231pd     %ymm15, %ymm14, %ymm0\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm1\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm2\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm3\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm4\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm5\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm6\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm7\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm8\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm9\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm10\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm11\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm12\n\t"
            "vfmadd231pd     %ymm15, %ymm14, %ymm13\n\t");
  }

  return s;
}

void kernel(int64_t N) { impl(N, 0.4, 1.5, 1.0); }

#endif