#include "../run2.h"
#include <cstring>
void grad_case6(float (& C)[8][16][3][3],float (& dA)[2][8][5][5],float (& dB)[2][16][7][7]){
  memset(dB, 0, sizeof dB);
  for (int k = 0; k < 8; ++k) {
    for (int n = 0; n < 2; ++n) {
      for (int p = 0; p < 5; ++p) {
        for (int q = 0; q < 5; ++q) {
          for (int c = 0; c < 16; ++c) {
            for (int dB2 = 0; dB2 < 7; ++dB2) {
              for (int dB3 = 0; dB3 < 7; ++dB3) {
                if (((dB2 - p >= 0) && (dB2 - p < 3)) && ((dB3 - q >= 0) && (dB3 - q < 3))) {
                  dB[n][c][dB2][dB3] += (dA[n][k][p][q]) * (C[k][c][dB2 - p][dB3 - q]);
                }
              }
            }
          }
        }
      }
    }
  }


}
