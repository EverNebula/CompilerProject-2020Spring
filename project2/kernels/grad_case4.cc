#include "../run2.h"
void grad_case4(float (& dA)[16][32],float (& dB)[16][32],float (& dC)[32][32]){
float temp1[16][32];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        dA[i][j] = dA[i][j] + (dB[i][k]) * (C[k][j]);
      }
    }
  }

  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        dA[i][j] = dA[i][j] + (B[i][k]) * (dC[k][j]);
      }
    }
  }


}
