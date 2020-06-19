#include "../run2.h"
void grad_case6(float (& dA)[2][8][5][5],float (& dB)[2][16][7][7]){
float temp1[2][8][5][5];
  for (int k = 0; k < 0; ++k) {
    for (int n = 0; n < 0; ++n) {
      for (int p = 0; p < 0; ++p) {
        for (int q = 0; q < 0; ++q) {
          temp1[n][k][p][q] = 0;
        }
      }
    }
  }
  for (int k = 0; k < 0; ++k) {
    for (int n = 0; n < 0; ++n) {
      for (int p = 0; p < 0; ++p) {
        for (int q = 0; q < 0; ++q) {
          for (int c = 0; c < 0; ++c) {
            for (int r = 0; r < 0; ++r) {
              for (int s = 0; s < 0; ++s) {
                temp1[n][k][p][q] = 0 + (1) * (C[k][c][r][s]) + (B[n][c][p + r][q + s]) * (0);
              }
            }
          }
        }
      }
    }
  }
  for (int k = 0; k < 0; ++k) {
    for (int n = 0; n < 0; ++n) {
      for (int p = 0; p < 0; ++p) {
        for (int q = 0; q < 0; ++q) {
          A[n][k][p][q] = 0;
        }
      }
    }
  }


}
