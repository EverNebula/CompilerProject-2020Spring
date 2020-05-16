#include "../run.h"
void kernel_example(float (&A)[32][16]){
float temp1[32][16];
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp1[i][j] = temp1[i][j] + A[i][j];
      temp1[i][j] = temp1[i][j] + ((float)(1)) / (3);
    }
  }
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 16; ++j) {
      A[i][j] = temp1[i][j];
    }
  }

}
