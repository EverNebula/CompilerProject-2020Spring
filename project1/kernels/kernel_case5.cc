#include "../run.h"
void kernel_case5(float (&B)[16][32],float (&C)[32][32],float (&D)[16][32],float (&alpha),float (&beta),float (&A)[16][32]) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = 0;
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = temp[i][j] + A[i][j];
      for (int k = 0; k < 32; ++k) {
        temp[i][j] = temp[i][j] + (alpha) * ((B[i][k]) * (C[k][j]));
      }
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      A[i][j] = temp[i][j];
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = 0;
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = temp[i][j] + A[i][j];
      temp[i][j] = temp[i][j] + (beta) * (D[i][j]);
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      A[i][j] = temp[i][j];
    }
  }
}

