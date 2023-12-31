#ifndef PEAK_FLOPS_KERNEL_AVX512_FLOAT_H
#define PEAK_FLOPS_KERNEL_AVX512_FLOAT_H

#include "immintrin.h"
#include <stdint.h>

// Each load is 64B. We have 16 loads.
const uint64_t flop_per_iter = 64 * 16;

__attribute__((aligned(64))) __m512 data[16];

__attribute__((noinline)) __m512 impl(int64_t N) {

  __m512 s;

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < N; ++i) {
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
            : "m"(data[0]), "m"(data[1]), "m"(data[2]), "m"(data[3]),
              "m"(data[4]), "m"(data[5]), "m"(data[6]), "m"(data[7]),
              "m"(data[8]), "m"(data[9]), "m"(data[10]), "m"(data[11]),
              "m"(data[12]), "m"(data[13]), "m"(data[14]), "m"(data[15])
            :);
  }

  return s;
}

void kernel(int64_t N) { impl(N); }

#endif