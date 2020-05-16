#include "../run.h"
void kernel_case7(float (&A)[32][16]) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = 0;
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = temp[i][j] + A[j][i];
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      B[i][j] = temp[i][j];
    }
  }
}

