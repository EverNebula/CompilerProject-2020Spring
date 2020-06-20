#include "../run2.h"
#include <cstring>
void grad_case2(float (& A)[4][16],float (& dB)[4][16],float (& dA)[4][16]){
  memset(dA, 0, sizeof dA);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 16; ++j) {
      dA[i][j] += (dB[i][j]) * (A[i][j]) + (A[i][j]) * (dB[i][j]);
    }
  }


}
