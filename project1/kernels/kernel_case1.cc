#include "../run.h"
void kernel_case1() {
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp[i][j] = 0;
    }
  }
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp[i][j] = temp[i][j] + 2;
    }
  }
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 16; ++j) {
      A[i][j] = temp[i][j];
    }
  }
}

