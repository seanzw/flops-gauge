#ifndef PEAK_FLOPS_KERNEL_AVX2_FLOAT_H
#define PEAK_FLOPS_KERNEL_AVX2_FLOAT_H

#include "immintrin.h"
#include <stdint.h>

// Each fmadd is 16 flop.
const uint64_t flop_per_iter = 14 * 2 * 8 * 2;

__attribute__((noinline)) __m256 impl(int64_t N, float a, float b, float c) {

  __m256 A = _mm256_set1_ps(a);
  __m256 B = _mm256_set1_ps(b);
  __m256 C = _mm256_set1_ps(c);

  __m256 s;

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < N; ++i) {
    __asm__("vfmadd231ps     %ymm15, %ymm14, %ymm0\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm1\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm2\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm3\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm4\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm5\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm6\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm7\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm8\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm9\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm10\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm11\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm12\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm13\n\t");
    __asm__("vfmadd231ps     %ymm15, %ymm14, %ymm0\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm1\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm2\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm3\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm4\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm5\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm6\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm7\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm8\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm9\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm10\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm11\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm12\n\t"
            "vfmadd231ps     %ymm15, %ymm14, %ymm13\n\t");
  }

  return s;
}

void kernel(int64_t N) { impl(N, 0.4f, 1.5f, 1.0f); }

#endif
