#include "../run2.h"
#include <cstring>
void grad_case8(float (& dB)[32],float (& dA)[2][16]){
  memset(dA, 0, sizeof dA);
  for (int dA1 = 0; dA1 < 16; ++dA1) {
    for (int dA0 = 0; dA0 < 2; ++dA0) {
      if ((((dA0) * (16) + dA1 >= 0) && ((dA0) * (16) + dA1 < 32)) && (((dA0) * (16) + dA1 >= 0) && ((dA0) * (16) + dA1 < 32))) {
        dA[dA0][dA1] += dB[(dA0) * (16) + dA1];
      }
    }
  }


}
