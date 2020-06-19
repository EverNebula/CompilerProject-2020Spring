#include "../run2.h"
void grad_case10(float (& dA)[8][8],float (& dB)[10][8]){
float temp1[8][8];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      dA[i][j] = dA[i][j] + ((dB[i][j] + dB[i + 1][j] + dB[i + 2][j]) * (3)) / ((3) * (3));
    }
  }


}
