#include "../run2.h"
void grad_case9(float (& dB)[4][6],float (& dA)[4]){
float temp1[4][6];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 6; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 6; ++j) {
      temp1[i][j] = temp1[i][j] + A[i];
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 6; ++j) {
      B[i][j] = temp1[i][j];
    }
  }

}
