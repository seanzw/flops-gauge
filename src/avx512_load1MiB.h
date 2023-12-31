#ifndef PEAK_FLOPS_KERNEL_AVX512_FLOAT_H
#define PEAK_FLOPS_KERNEL_AVX512_FLOAT_H

#include "immintrin.h"
#include <stdint.h>

// Each load is 64B. We have 16 loads per iteration, but from 1MB array.
const uint64_t flop_per_iter = 64 * 16;

__attribute__((aligned(64))) __m512 data[16384];

__attribute__((noinline)) __m512 impl(int64_t N) {

  __m512 s;

  int64_t NN = N / 1024;

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < NN; ++i) {
    for (int64_t j = 0; j < 1024; ++j) {
      __m512 *d = data + j * 16;
      __asm__("vmovdqa64 %0, %%zmm0\n\t"
              "vmovdqa64 %1, %%zmm1\n\t"
              "vmovdqa64 %2, %%zmm2\n\t"
              "vmovdqa64 %3, %%zmm3\n\t"
              "vmovdqa64 %4, %%zmm4\n\t"
              "vmovdqa64 %5, %%zmm5\n\t"
              "vmovdqa64 %6, %%zmm6\n\t"
              "vmovdqa64 %7, %%zmm7\n\t"
              "vmovdqa64 %8, %%zmm8\n\t"
              "vmovdqa64 %9, %%zmm9\n\t"
              "vmovdqa64 %10, %%zmm10\n\t"
              "vmovdqa64 %11, %%zmm11\n\t"
              "vmovdqa64 %12, %%zmm12\n\t"
              "vmovdqa64 %13, %%zmm13\n\t"
              "vmovdqa64 %14, %%zmm14\n\t"
              "vmovdqa64 %15, %%zmm15\n\t"
              :
              : "m"(d[0]), "m"(d[1]), "m"(d[2]), "m"(d[3]), "m"(d[4]),
                "m"(d[5]), "m"(d[6]), "m"(d[7]), "m"(d[8]), "m"(d[9]),
                "m"(d[10]), "m"(d[11]), "m"(d[12]), "m"(d[13]), "m"(d[14]),
                "m"(d[15])
              :);
    }
  }

  return s;
}

void kernel(int64_t N) { impl(N); }

#endif