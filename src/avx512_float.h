#ifndef PEAK_FLOPS_KERNEL_AVX512_FLOAT_H
#define PEAK_FLOPS_KERNEL_AVX512_FLOAT_H

#include "immintrin.h"
#include <stdint.h>

// Each fmadd is 32 flop.
const uint64_t flop_per_iter = 30 * 16 * 2;

__attribute__((noinline)) __m512 impl(int64_t N, float a, float b, float c) {

  __m512 A = _mm512_set1_ps(a);
  __m512 B = _mm512_set1_ps(b);
  __m512 C = _mm512_set1_ps(c);

  __m512 s;

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < N; ++i) {
    __asm__("vfmadd231ps     %zmm31, %zmm30, %zmm0\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm1\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm2\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm3\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm4\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm5\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm6\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm7\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm8\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm9\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm10\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm11\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm12\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm13\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm14\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm15\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm16\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm17\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm18\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm19\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm20\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm21\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm22\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm23\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm24\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm25\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm26\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm27\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm28\n\t"
            "vfmadd231ps     %zmm31, %zmm30, %zmm29\n\t");
  }

  return s;
}

void kernel(int64_t N) { impl(N, 0.4f, 1.5f, 1.0f); }

#endif