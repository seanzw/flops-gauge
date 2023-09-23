#ifndef PEAK_FLOPS_KERNEL_AMX_COMMON_H
#define PEAK_FLOPS_KERNEL_AMX_COMMON_H

#include "immintrin.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MAX 1024
#define STRIDE 64
#define ARCH_GET_XCOMP_PERM 0x1022
#define ARCH_REQ_XCOMP_PERM 0x1023
#define XFEATURE_XTILECFG 17
#define XFEATURE_XTILEDATA 18

// Define tile config data structure
typedef struct __tile_config {
  uint8_t palette_id;
  uint8_t start_row;
  uint8_t reserved_0[14];
  uint16_t colsb[16];
  uint8_t rows[16];
} __tilecfg;

/* Initialize tile config */
static void init_tile_config(__tilecfg *tileinfo, uint8_t row, uint16_t cols) {
  tileinfo->palette_id = 1;
  tileinfo->start_row = 0;

  for (int i = 0; i < 8; ++i) {
    tileinfo->colsb[i] = cols;
    tileinfo->rows[i] = row;
  }

  _tile_loadconfig(tileinfo);
}

/* Set_tiledata_use() - Invoke syscall to set ARCH_SET_STATE_USE */
static bool set_tiledata_use() {
  if (syscall(SYS_arch_prctl, ARCH_REQ_XCOMP_PERM, XFEATURE_XTILEDATA)) {
    printf("\n Fail to do XFEATURE_XTILEDATA \n\n");
    return false;
  } else {
    printf("\n TILE DATA USE SET - OK \n\n");
    return true;
  }

  return true;
}

/* Initialize int8_t buffer */
static void init_buffer(int8_t *buf, int8_t value, int rows, int colsb) {

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < colsb; j++) {
      buf[i * colsb + j] = value;
    }
}

/* Initialize int32_t buffer */
static void init_buffer32(int32_t *buf, int8_t value, int rows, int colsb) {
  int colsb2 = colsb / 4;
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < (colsb2); j++) {
      buf[i * colsb2 + j] = value;
    }
}

/* Print int8_t buffer */
static void print_buffer(int8_t *buf, int32_t rows, int32_t colsb) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < (colsb); j++) {
      printf("%d ", buf[i * colsb + j]);
    }
    printf("\n");
  }
  printf("\n");
}

/* Print int32_t buffer */
static void print_buffer32(int32_t *buf, int32_t rows, int32_t colsb) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < (colsb); j++) {
      printf("%d ", buf[i * colsb + j]);
    }
    printf("\n");
  }
  printf("\n");
}

#endif
