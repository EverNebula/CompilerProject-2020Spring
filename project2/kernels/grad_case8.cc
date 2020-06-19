#include "../run2.h"
#include <cstring>
void grad_case8(float (& dB)[32],float (& dA)[2][16]){
float temp1[32];
  memset(dA, 0, sizeof dA);
  for (int i = 0; i < 32; ++i) {
    dA[(i) / (16)][(i) % (16)] += dB[i];
  }


}
