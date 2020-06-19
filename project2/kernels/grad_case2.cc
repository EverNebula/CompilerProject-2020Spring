#include "../run2.h"
void grad_case2(float (& dB)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      dB[i][j] = dB[i][j] + (dA[i][j]) * (A[i][j]) + (A[i][j]) * (dA[i][j]);
      dB[i][j] = dB[i][j];
    }
  }


}
