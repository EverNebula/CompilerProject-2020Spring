#include "../run2.h"
void grad_case2(float (& dB)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp1[i][j] = temp1[i][j] + (A[i][j]) * (A[i][j]);
      temp1[i][j] = temp1[i][j] + 1;
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      B[i][j] = temp1[i][j];
    }
  }

}
