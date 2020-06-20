#include "../run2.h"
#include <cstring>
void grad_case10(float (& dA)[8][8],float (& dB)[10][8]){
  memset(dB, 0, sizeof dB);
  for (int dB0 = 0; dB0 < 10; ++dB0) {
    for (int j = 0; j < 8; ++j) {
      if ((dB0 - 2 >= 0) && (dB0 - 2 < 8)) {
        dB[dB0][j] += ((dA[dB0 - 2][j]) * (3)) / ((3) * (3));
      }
      if ((dB0 - 1 >= 0) && (dB0 - 1 < 8)) {
        dB[dB0][j] += ((dA[dB0 - 1][j]) * (3)) / ((3) * (3));
      }
      if ((dB0 >= 0) && (dB0 < 8)) {
        dB[dB0][j] += ((dA[dB0][j]) * (3)) / ((3) * (3));
      }
    }
  }


}
