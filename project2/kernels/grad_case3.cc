#include "../run2.h"
#include <cstring>
void grad_case3(float (& B)[16][16],float (& dC)[4][16],float (& dA)[4][16]){
  memset(dA, 0, sizeof dA);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      for (int k = 0; k < 16; ++k) {
        dA[i][k] += (dC[i][j]) * (B[k][j]);
      }
    }
  }


}
