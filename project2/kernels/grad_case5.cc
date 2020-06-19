#include "../run2.h"
void grad_case5(float (& dA)[16][32],float (& dB)[16][32][4]){
float temp1[16][32];
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      for (int k = 0; k < 32; ++k) {
        for (int l = 0; l < 4; ++l) {
          temp1[i][j] = temp1[i][j] + ((B[i][k][l]) * (C[k][j])) * (D[l][j]);
        }
      }
    }
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      A[i][j] = temp1[i][j];
    }
  }

}
