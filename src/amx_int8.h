#ifndef PEAK_FLOPS_KERNEL_AMX_INT8_H
#define PEAK_FLOPS_KERNEL_AMX_INT8_H

#include "amx_common.h"

#define TILE_M 16
#define TILE_K 64
#define TILE_N 16
#define SRC_ELEM_SIZE 1
#define DST_ELEM_SIZE 4
#define NUM_DST_TILE 6

#define MAX 1024
#define MAX_ROWS 16
#define MAX_COLS 64
#define STRIDE 64

const uint64_t flop_per_iter = NUM_DST_TILE * TILE_M * TILE_N * TILE_K * 2;

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
  tile_data.palette_id = 1;
  tile_data.start_row = 0;

  for (int i = 0; i < 8; ++i) {
    if (i < NUM_DST_TILE) {
      // Dest.
      tile_data.colsb[i] = TILE_N * DST_ELEM_SIZE;
      tile_data.rows[i] = TILE_M;
    } else if (i == NUM_DST_TILE) {
      // Src1.
      tile_data.colsb[i] = TILE_K * SRC_ELEM_SIZE;
      tile_data.rows[i] = TILE_M;
    } else {
      // Src2.
      tile_data.colsb[i] = TILE_K * SRC_ELEM_SIZE;
      tile_data.rows[i] = TILE_N;
    }
  }

  _tile_loadconfig(&tile_data);

  // Init src matrix buffers with data
  init_buffer(src1, 2, MAX_ROWS, MAX_COLS);
  print_buffer(src1, rows, colsb);

  init_buffer(src2, 2, MAX_ROWS, MAX_COLS);
  print_buffer(src2, rows, colsb);

  // Init dst matrix buffers with data
  init_buffer32(res, 0, MAX_ROWS, MAX_COLS);

  // Load tile rows from memory
  _tile_loadd(6, src1, TILE_K * SRC_ELEM_SIZE);
  _tile_loadd(7, src2, TILE_N * SRC_ELEM_SIZE);

  _tile_loadd(0, res, TILE_N * DST_ELEM_SIZE);
  _tile_loadd(1, res, TILE_N * DST_ELEM_SIZE);
  _tile_loadd(2, res, TILE_N * DST_ELEM_SIZE);
  _tile_loadd(3, res, TILE_N * DST_ELEM_SIZE);
  _tile_loadd(4, res, TILE_N * DST_ELEM_SIZE);
  _tile_loadd(5, res, TILE_N * DST_ELEM_SIZE);
}

__attribute__((noinline)) void impl(int64_t N, float a, float b, float c) {

  printf("SHit\n");

#pragma clang loop unroll(disable)
  for (int64_t i = 0; i < N; ++i) {
    _tile_dpbssd(0, 6, 7);
    _tile_dpbssd(1, 6, 7);
    _tile_dpbssd(2, 6, 7);
    _tile_dpbssd(3, 6, 7);
    _tile_dpbssd(4, 6, 7);
    _tile_dpbssd(5, 6, 7);
  }

  return;
}

#define kernel_init setup_tile

void kernel(int64_t N) { impl(N, 0.4f, 1.5f, 1.0f); }

#endif
