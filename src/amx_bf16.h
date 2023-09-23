#ifndef PEAK_FLOPS_KERNEL_AMX_BF16_H
#define PEAK_FLOPS_KERNEL_AMX_BF16_H

#include "amx_common.h"

// Each amx int8 is 16*16*32*2=16k flop.
const uint64_t flop_per_iter = 6 * 16 * 16 * 32 * 2;

#define MAX 1024
#define MAX_ROWS 16
#define MAX_COLS 64
#define STRIDE 64

void setup_tile() {

  __tilecfg tile_data = {0};
  int8_t src1[MAX];
  int8_t src2[MAX];
  int32_t res[MAX / 4];
  int rows = MAX_ROWS;
  int colsb = MAX_COLS;

  // Request permission to linux kernel to run AMX
  if (!set_tiledata_use())
    exit(-1);

  // Load tile configuration
  init_tile_config(&tile_data, MAX_ROWS, MAX_COLS);

  // Init src matrix buffers with data
  init_buffer(src1, 2, MAX_ROWS, MAX_COLS);
  print_buffer(src1, rows, colsb);

  init_buffer(src2, 2, MAX_ROWS, MAX_COLS);
  print_buffer(src2, rows, colsb);

  // Init dst matrix buffers with data
  init_buffer32(res, 0, MAX_ROWS, MAX_COLS);

  // Load tile rows from memory
  _tile_loadd(6, src1, STRIDE);
  _tile_loadd(7, src2, STRIDE);

  _tile_loadd(0, res, STRIDE);
  _tile_loadd(1, res, STRIDE);
  _tile_loadd(2, res, STRIDE);
  _tile_loadd(3, res, STRIDE);
  _tile_loadd(4, res, STRIDE);
  _tile_loadd(5, res, STRIDE);
}

__attribute__((noinline)) void impl(int64_t N, float a, float b, float c) {

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < N; ++i) {
    _tile_dpbf16ps(0, 6, 7);
    _tile_dpbf16ps(1, 6, 7);
    _tile_dpbf16ps(2, 6, 7);
    _tile_dpbf16ps(3, 6, 7);
    _tile_dpbf16ps(4, 6, 7);
    _tile_dpbf16ps(5, 6, 7);
  }

  return;
}

#define kernel_init setup_tile

void kernel(int64_t N) { impl(N, 0.4f, 1.5f, 1.0f); }

#endif
