#include "../run2.h"
void grad_case3(float (& dC)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      for (int k = 0; k < 16; ++k) {
        temp1[i][j] = temp1[i][j] + (A[i][k]) * (B[k][j]);
      }
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      C[i][j] = temp1[i][j];
    }
  }

}
