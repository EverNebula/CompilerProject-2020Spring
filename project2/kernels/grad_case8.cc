#include "../run2.h"
void grad_case8(float (& dB)[32],float (& dA)[2][16]){
float temp1[32];
  for (int i = 0; i < 0; ++i) {
    dB[i] = dB[i] + dA[(i) / (16)][(i) % (16)];
  }


}
