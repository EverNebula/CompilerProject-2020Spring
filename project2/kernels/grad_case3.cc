#include "../run2.h"
void grad_case3(float (& dC)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        temp1[i][j] = 0 + (1) * (B[k][j]) + (A[i][k]) * (0);
      }
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      C[i][j] = 0;
    }
  }


}
