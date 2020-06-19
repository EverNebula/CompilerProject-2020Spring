#include "../run2.h"
#include <cstring>
void grad_case7(float (& dB)[16][32],float (& dA)[32][16]){
float temp1[16][32];
  memset(dA, 0, sizeof dA);
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      dA[j][i] += dB[i][j];
    }
  }


}
