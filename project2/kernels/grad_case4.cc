#include "../run2.h"
void grad_case4(float (& dA)[16][32],float (& dB)[16][32],float (& dC)[32][32]){
float temp1[16][32];
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        temp1[i][j] = 0 + (1) * (C[k][j]) + (B[i][k]) * (0);
      }
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      A[i][j] = 0;
    }
  }

  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      temp1[i][j] = 0;
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      for (int k = 0; k < 0; ++k) {
        temp1[i][j] = 0 + (0) * (C[k][j]) + (B[i][k]) * (1);
      }
    }
  }
  for (int i = 0; i < 0; ++i) {
    for (int j = 0; j < 0; ++j) {
      A[i][j] = 0;
    }
  }


}
