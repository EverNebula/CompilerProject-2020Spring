#include "../run2.h"
#include <cstring>
void grad_case1(float (& B)[4][16],float (& dC)[4][16],float (& dA)[4][16]){
float temp1[4][16];
  memset(dA, 0, sizeof dA);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      dA[i][j] += (dC[i][j]) * (B[i][j]);
      dA[i][j] += 0;
    }
  }


}
