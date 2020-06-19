#include "../run2.h"
void grad_case8(float (& dB)[32],float (& dA)[2][16]){
float temp1[32];
  for (int i = 0; i < 32; ++i) {
    temp1[i] = 0;
  }
  for (int i = 0; i < 32; ++i) {
    temp1[i] = temp1[i] + A[(i) / (16)][(i) % (16)];
  }
  for (int i = 0; i < 32; ++i) {
    B[i] = temp1[i];
  }

}
