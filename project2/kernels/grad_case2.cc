#include "../run2.h"
void grad_case2(float (& dB)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      temp1[i][j] = (1) * (A[i][j]) + (A[i][j]) * (1);
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      B[i][j] = 0;
    }
  }


}
