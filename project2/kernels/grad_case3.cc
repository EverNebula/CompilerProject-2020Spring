#include "../run2.h"
void grad_case3(float (& dC)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        dC[i][j] = dC[i][j] + (dA[i][k]) * (B[k][j]);
      }
    }
  }


}
