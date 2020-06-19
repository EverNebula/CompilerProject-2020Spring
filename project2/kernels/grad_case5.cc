#include "../run2.h"
void grad_case5(float (& dA)[16][32],float (& dB)[16][32][4]){
float temp1[16][32];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        for (int l = 0; l < 0; ++l) {
          dA[i][j] = dA[i][j] + ((dB[i][k][l]) * (C[k][j])) * (D[l][j]);
        }
      }
    }
  }


}
