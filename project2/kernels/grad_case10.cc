#include "../run2.h"
void grad_case10(float (& dA)[8][8],float (& dB)[10][8]){
float temp1[8][8];
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      temp1[i][j] = temp1[i][j] + (B[i][j] + B[i + 1][j] + B[i + 2][j]) / (3);
    }
  }
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      A[i][j] = temp1[i][j];
    }
  }

}
