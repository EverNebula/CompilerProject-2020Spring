#include "../run2.h"
#include <cstring>
void grad_case9(float (& dB)[4][6],float (& dA)[4]){
  memset(dA, 0, sizeof dA);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 6; ++j) {
      dA[i] += dB[i][j];
    }
  }


}
