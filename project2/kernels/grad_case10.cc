#include "../run2.h"
#include <cstring>
void grad_case10(float (& dA)[8][8],float (& dB)[10][8]){
float temp1[8][8];
  memset(dB, 0, sizeof dB);
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      dB[i][j] += ((dA[i][j]) * (3)) / ((3) * (3));
      dB[i + 2][j] += ((dA[i][j]) * (3)) / ((3) * (3));
      dB[i + 1][j] += ((dA[i][j]) * (3)) / ((3) * (3));
    }
  }


}
