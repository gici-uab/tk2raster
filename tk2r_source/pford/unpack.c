////
// Copyright (c) 2012 Universidad de Concepción, Chile. 
//
// Author: Diego Caro
//
// @UDEC_LICENSE_HEADER_START@ 
//
// @UDEC_LICENSE_HEADER_END@ 

////
// Copyright (c) 2008, WEST, Polytechnic Institute of NYU
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of WEST, Polytechnic Institute of NYU nor the names
//     of its contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author(s): Torsten Suel, Jiangong Zhang, Jinru He
//
// If you have any questions or problems with our code, please contact:
// jhe@cis.poly.edu
//

#include <stdio.h>
#include <stdint.h>
#include "unpack.h"

#define MASK_31 0x0000001F

pf unpack[17] = {unpack0, unpack1, unpack2, unpack3,
                 unpack4, unpack5, unpack6, unpack7,
				 unpack8, unpack9, unpack10, unpack11,
				 unpack12, unpack13, unpack16, unpack20,
				 unpack32};
		 
pf2 unpack_pos[17] = {unpack_pos0, unpack_pos1, unpack_pos2, unpack_pos3,
                      unpack_pos4, unpack_pos5, unpack_pos6, unpack_pos7,
					  unpack_pos8, unpack_pos9, unpack_pos10, unpack_pos11, 
					  unpack_pos12, unpack_pos13, unpack_pos16, unpack_pos20,
					  unpack_pos32};

void unpack0(unsigned int* p, unsigned int* w, int BS) {
    //printf("unpack0\n");
  int i;
  for (i = 0; i < BS; i++) {
    p[i] = 0;
  }
}

void unpack1(unsigned int* p, unsigned int* w, int BS) {
  int i;
    //printf("unpack1\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 1) {
    p[0] = (w[0] >> 31);
    p[1] = (w[0] >> 30) & 1;
    p[2] = (w[0] >> 29) & 1;
    p[3] = (w[0] >> 28) & 1;
    p[4] = (w[0] >> 27) & 1;
    p[5] = (w[0] >> 26) & 1;
    p[6] = (w[0] >> 25) & 1;
    p[7] = (w[0] >> 24) & 1;
    p[8] = (w[0] >> 23) & 1;
    p[9] = (w[0] >> 22) & 1;
    p[10] = (w[0] >> 21) & 1;
    p[11] = (w[0] >> 20) & 1;
    p[12] = (w[0] >> 19) & 1;
    p[13] = (w[0] >> 18) & 1;
    p[14] = (w[0] >> 17) & 1;
    p[15] = (w[0] >> 16) & 1;
    p[16] = (w[0] >> 15) & 1;
    p[17] = (w[0] >> 14) & 1;
    p[18] = (w[0] >> 13) & 1;
    p[19] = (w[0] >> 12) & 1;
    p[20] = (w[0] >> 11) & 1;
    p[21] = (w[0] >> 10) & 1;
    p[22] = (w[0] >> 9) & 1;
    p[23] = (w[0] >> 8) & 1;
    p[24] = (w[0] >> 7) & 1;
    p[25] = (w[0] >> 6) & 1;
    p[26] = (w[0] >> 5) & 1;
    p[27] = (w[0] >> 4) & 1;
    p[28] = (w[0] >> 3) & 1;
    p[29] = (w[0] >> 2) & 1;
    p[30] = (w[0] >> 1) & 1;
    p[31] = (w[0]) & 1;
  }
}

void unpack2(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack2\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 2) {
    p[0] = (w[0] >> 30);
    p[1] = (w[0] >> 28) & 3;
    p[2] = (w[0] >> 26) & 3;
    p[3] = (w[0] >> 24) & 3;
    p[4] = (w[0] >> 22) & 3;
    p[5] = (w[0] >> 20) & 3;
    p[6] = (w[0] >> 18) & 3;
    p[7] = (w[0] >> 16) & 3;
    p[8] = (w[0] >> 14) & 3;
    p[9] = (w[0] >> 12) & 3;
    p[10] = (w[0] >> 10) & 3;
    p[11] = (w[0] >> 8) & 3;
    p[12] = (w[0] >> 6) & 3;
    p[13] = (w[0] >> 4) & 3;
    p[14] = (w[0] >> 2) & 3;
    p[15] = (w[0]) & 3;
    p[16] = (w[1] >> 30);
    p[17] = (w[1] >> 28) & 3;
    p[18] = (w[1] >> 26) & 3;
    p[19] = (w[1] >> 24) & 3;
    p[20] = (w[1] >> 22) & 3;
    p[21] = (w[1] >> 20) & 3;
    p[22] = (w[1] >> 18) & 3;
    p[23] = (w[1] >> 16) & 3;
    p[24] = (w[1] >> 14) & 3;
    p[25] = (w[1] >> 12) & 3;
    p[26] = (w[1] >> 10) & 3;
    p[27] = (w[1] >> 8) & 3;
    p[28] = (w[1] >> 6) & 3;
    p[29] = (w[1] >> 4) & 3;
    p[30] = (w[1] >> 2) & 3;
    p[31] = (w[1]) & 3;
  }
}

void unpack3(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack3\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 3) {
    p[0] = (w[0] >> 29);
    p[1] = (w[0] >> 26) & 7;
    p[2] = (w[0] >> 23) & 7;
    p[3] = (w[0] >> 20) & 7;
    p[4] = (w[0] >> 17) & 7;
    p[5] = (w[0] >> 14) & 7;
    p[6] = (w[0] >> 11) & 7;
    p[7] = (w[0] >> 8) & 7;
    p[8] = (w[0] >> 5) & 7;
    p[9] = (w[0] >> 2) & 7;
    p[10] = (w[0] << 1) & 7;
    p[10] |= (w[1] >> 31);
    p[11] = (w[1] >> 28) & 7;
    p[12] = (w[1] >> 25) & 7;
    p[13] = (w[1] >> 22) & 7;
    p[14] = (w[1] >> 19) & 7;
    p[15] = (w[1] >> 16) & 7;
    p[16] = (w[1] >> 13) & 7;
    p[17] = (w[1] >> 10) & 7;
    p[18] = (w[1] >> 7) & 7;
    p[19] = (w[1] >> 4) & 7;
    p[20] = (w[1] >> 1) & 7;
    p[21] = (w[1] << 2) & 7;
    p[21] |= (w[2] >> 30);
    p[22] = (w[2] >> 27) & 7;
    p[23] = (w[2] >> 24) & 7;
    p[24] = (w[2] >> 21) & 7;
    p[25] = (w[2] >> 18) & 7;
    p[26] = (w[2] >> 15) & 7;
    p[27] = (w[2] >> 12) & 7;
    p[28] = (w[2] >> 9) & 7;
    p[29] = (w[2] >> 6) & 7;
    p[30] = (w[2] >> 3) & 7;
    p[31] = (w[2]) & 7;
  }
}

void unpack4(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack4\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 4) {
    p[0] = (w[0] >> 28);
    p[1] = (w[0] >> 24) & 15;
    p[2] = (w[0] >> 20) & 15;
    p[3] = (w[0] >> 16) & 15;
    p[4] = (w[0] >> 12) & 15;
    p[5] = (w[0] >> 8) & 15;
    p[6] = (w[0] >> 4) & 15;
    p[7] = (w[0]) & 15;
    p[8] = (w[1] >> 28);
    p[9] = (w[1] >> 24) & 15;
    p[10] = (w[1] >> 20) & 15;
    p[11] = (w[1] >> 16) & 15;
    p[12] = (w[1] >> 12) & 15;
    p[13] = (w[1] >> 8) & 15;
    p[14] = (w[1] >> 4) & 15;
    p[15] = (w[1]) & 15;
    p[16] = (w[2] >> 28);
    p[17] = (w[2] >> 24) & 15;
    p[18] = (w[2] >> 20) & 15;
    p[19] = (w[2] >> 16) & 15;
    p[20] = (w[2] >> 12) & 15;
    p[21] = (w[2] >> 8) & 15;
    p[22] = (w[2] >> 4) & 15;
    p[23] = (w[2]) & 15;
    p[24] = (w[3] >> 28);
    p[25] = (w[3] >> 24) & 15;
    p[26] = (w[3] >> 20) & 15;
    p[27] = (w[3] >> 16) & 15;
    p[28] = (w[3] >> 12) & 15;
    p[29] = (w[3] >> 8) & 15;
    p[30] = (w[3] >> 4) & 15;
    p[31] = (w[3]) & 15;
  }
}

void unpack5(unsigned int* p, unsigned int* w, int BS) {
  int i;

  for (i = 0; i < BS; i += 32, p += 32, w += 5) {
	  //printf("Inside unpack5: %d\n", i);
    p[0] = (w[0] >> 27);
    p[1] = (w[0] >> 22) & 31;
    p[2] = (w[0] >> 17) & 31;
    p[3] = (w[0] >> 12) & 31;
    p[4] = (w[0] >> 7) & 31;
    p[5] = (w[0] >> 2) & 31;
    p[6] = (w[0] << 3) & 31;
    p[6] |= (w[1] >> 29);
    p[7] = (w[1] >> 24) & 31;
    p[8] = (w[1] >> 19) & 31;
    p[9] = (w[1] >> 14) & 31;
    p[10] = (w[1] >> 9) & 31;
    p[11] = (w[1] >> 4) & 31;
    p[12] = (w[1] << 1) & 31;
    p[12] |= (w[2] >> 31);
    p[13] = (w[2] >> 26) & 31;
    p[14] = (w[2] >> 21) & 31;
    p[15] = (w[2] >> 16) & 31;
    p[16] = (w[2] >> 11) & 31;
    p[17] = (w[2] >> 6) & 31;
    p[18] = (w[2] >> 1) & 31;
    p[19] = (w[2] << 4) & 31;
    p[19] |= (w[3] >> 28);
    p[20] = (w[3] >> 23) & 31;
    p[21] = (w[3] >> 18) & 31;
    p[22] = (w[3] >> 13) & 31;
    p[23] = (w[3] >> 8) & 31;
    p[24] = (w[3] >> 3) & 31;
    p[25] = (w[3] << 2) & 31;
    p[25] |= (w[4] >> 30);
    p[26] = (w[4] >> 25) & 31;
    p[27] = (w[4] >> 20) & 31;
    p[28] = (w[4] >> 15) & 31;
    p[29] = (w[4] >> 10) & 31;
    p[30] = (w[4] >> 5) & 31;
    p[31] = (w[4]) & 31;
  }
}

void unpack6(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack6\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 6) {
    p[0] = (w[0] >> 26);
    p[1] = (w[0] >> 20) & 63;
    p[2] = (w[0] >> 14) & 63;
    p[3] = (w[0] >> 8) & 63;
    p[4] = (w[0] >> 2) & 63;
    p[5] = (w[0] << 4) & 63;
    p[5] |= (w[1] >> 28);
    p[6] = (w[1] >> 22) & 63;
    p[7] = (w[1] >> 16) & 63;
    p[8] = (w[1] >> 10) & 63;
    p[9] = (w[1] >> 4) & 63;
    p[10] = (w[1] << 2) & 63;
    p[10] |= (w[2] >> 30);
    p[11] = (w[2] >> 24) & 63;
    p[12] = (w[2] >> 18) & 63;
    p[13] = (w[2] >> 12) & 63;
    p[14] = (w[2] >> 6) & 63;
    p[15] = (w[2]) & 63;
    p[16] = (w[3] >> 26);
    p[17] = (w[3] >> 20) & 63;
    p[18] = (w[3] >> 14) & 63;
    p[19] = (w[3] >> 8) & 63;
    p[20] = (w[3] >> 2) & 63;
    p[21] = (w[3] << 4) & 63;
    p[21] |= (w[4] >> 28);
    p[22] = (w[4] >> 22) & 63;
    p[23] = (w[4] >> 16) & 63;
    p[24] = (w[4] >> 10) & 63;
    p[25] = (w[4] >> 4) & 63;
    p[26] = (w[4] << 2) & 63;
    p[26] |= (w[5] >> 30);
    p[27] = (w[5] >> 24) & 63;
    p[28] = (w[5] >> 18) & 63;
    p[29] = (w[5] >> 12) & 63;
    p[30] = (w[5] >> 6) & 63;
    p[31] = (w[5]) & 63;
  }
}

void unpack7(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack7\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 7) {
    p[0] = (w[0] >> 25);
    p[1] = (w[0] >> 18) & 127;
    p[2] = (w[0] >> 11) & 127;
    p[3] = (w[0] >> 4) & 127;
    p[4] = (w[0] << 3) & 127;
    p[4] |= (w[1] >> 29);
    p[5] = (w[1] >> 22) & 127;
    p[6] = (w[1] >> 15) & 127;
    p[7] = (w[1] >> 8) & 127;
    p[8] = (w[1] >> 1) & 127;
    p[9] = (w[1] << 6) & 127;
    p[9] |= (w[2] >> 26);
    p[10] = (w[2] >> 19) & 127;
    p[11] = (w[2] >> 12) & 127;
    p[12] = (w[2] >> 5) & 127;
    p[13] = (w[2] << 2) & 127;
    p[13] |= (w[3] >> 30);
    p[14] = (w[3] >> 23) & 127;
    p[15] = (w[3] >> 16) & 127;
    p[16] = (w[3] >> 9) & 127;
    p[17] = (w[3] >> 2) & 127;
    p[18] = (w[3] << 5) & 127;
    p[18] |= (w[4] >> 27);
    p[19] = (w[4] >> 20) & 127;
    p[20] = (w[4] >> 13) & 127;
    p[21] = (w[4] >> 6) & 127;
    p[22] = (w[4] << 1) & 127;
    p[22] |= (w[5] >> 31);
    p[23] = (w[5] >> 24) & 127;
    p[24] = (w[5] >> 17) & 127;
    p[25] = (w[5] >> 10) & 127;
    p[26] = (w[5] >> 3) & 127;
    p[27] = (w[5] << 4) & 127;
    p[27] |= (w[6] >> 28);
    p[28] = (w[6] >> 21) & 127;
    p[29] = (w[6] >> 14) & 127;
    p[30] = (w[6] >> 7) & 127;
    p[31] = (w[6]) & 127;
  }
}

void unpack8(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack8\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 8) {
    p[0] = (w[0] >> 24);
    p[1] = (w[0] >> 16) & 255;
    p[2] = (w[0] >> 8) & 255;
    p[3] = (w[0]) & 255;
    p[4] = (w[1] >> 24);
    p[5] = (w[1] >> 16) & 255;
    p[6] = (w[1] >> 8) & 255;
    p[7] = (w[1]) & 255;
    p[8] = (w[2] >> 24);
    p[9] = (w[2] >> 16) & 255;
    p[10] = (w[2] >> 8) & 255;
    p[11] = (w[2]) & 255;
    p[12] = (w[3] >> 24);
    p[13] = (w[3] >> 16) & 255;
    p[14] = (w[3] >> 8) & 255;
    p[15] = (w[3]) & 255;
    p[16] = (w[4] >> 24);
    p[17] = (w[4] >> 16) & 255;
    p[18] = (w[4] >> 8) & 255;
    p[19] = (w[4]) & 255;
    p[20] = (w[5] >> 24);
    p[21] = (w[5] >> 16) & 255;
    p[22] = (w[5] >> 8) & 255;
    p[23] = (w[5]) & 255;
    p[24] = (w[6] >> 24);
    p[25] = (w[6] >> 16) & 255;
    p[26] = (w[6] >> 8) & 255;
    p[27] = (w[6]) & 255;
    p[28] = (w[7] >> 24);
    p[29] = (w[7] >> 16) & 255;
    p[30] = (w[7] >> 8) & 255;
    p[31] = (w[7]) & 255;
  }
}

void unpack9(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack9\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 9) {
    p[0] = (w[0] >> 23);
    p[1] = (w[0] >> 14) & 511;
    p[2] = (w[0] >> 5) & 511;
    p[3] = (w[0] << 4) & 511;
    p[3] |= (w[1] >> 28);
    p[4] = (w[1] >> 19) & 511;
    p[5] = (w[1] >> 10) & 511;
    p[6] = (w[1] >> 1) & 511;
    p[7] = (w[1] << 8) & 511;
    p[7] |= (w[2] >> 24);
    p[8] = (w[2] >> 15) & 511;
    p[9] = (w[2] >> 6) & 511;
    p[10] = (w[2] << 3) & 511;
    p[10] |= (w[3] >> 29);
    p[11] = (w[3] >> 20) & 511;
    p[12] = (w[3] >> 11) & 511;
    p[13] = (w[3] >> 2) & 511;
    p[14] = (w[3] << 7) & 511;
    p[14] |= (w[4] >> 25);
    p[15] = (w[4] >> 16) & 511;
    p[16] = (w[4] >> 7) & 511;
    p[17] = (w[4] << 2) & 511;
    p[17] |= (w[5] >> 30);
    p[18] = (w[5] >> 21) & 511;
    p[19] = (w[5] >> 12) & 511;
    p[20] = (w[5] >> 3) & 511;
    p[21] = (w[5] << 6) & 511;
    p[21] |= (w[6] >> 26);
    p[22] = (w[6] >> 17) & 511;
    p[23] = (w[6] >> 8) & 511;
    p[24] = (w[6] << 1) & 511;
    p[24] |= (w[7] >> 31);
    p[25] = (w[7] >> 22) & 511;
    p[26] = (w[7] >> 13) & 511;
    p[27] = (w[7] >> 4) & 511;
    p[28] = (w[7] << 5) & 511;
    p[28] |= (w[8] >> 27);
    p[29] = (w[8] >> 18) & 511;
    p[30] = (w[8] >> 9) & 511;
    p[31] = (w[8]) & 511;
  }
}

void unpack10(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack10\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 10) {
    p[0] = (w[0] >> 22);
    p[1] = (w[0] >> 12) & 1023;
    p[2] = (w[0] >> 2) & 1023;
    p[3] = (w[0] << 8) & 1023;
    p[3] |= (w[1] >> 24);
    p[4] = (w[1] >> 14) & 1023;
    p[5] = (w[1] >> 4) & 1023;
    p[6] = (w[1] << 6) & 1023;
    p[6] |= (w[2] >> 26);
    p[7] = (w[2] >> 16) & 1023;
    p[8] = (w[2] >> 6) & 1023;
    p[9] = (w[2] << 4) & 1023;
    p[9] |= (w[3] >> 28);
    p[10] = (w[3] >> 18) & 1023;
    p[11] = (w[3] >> 8) & 1023;
    p[12] = (w[3] << 2) & 1023;
    p[12] |= (w[4] >> 30);
    p[13] = (w[4] >> 20) & 1023;
    p[14] = (w[4] >> 10) & 1023;
    p[15] = (w[4]) & 1023;
    p[16] = (w[5] >> 22);
    p[17] = (w[5] >> 12) & 1023;
    p[18] = (w[5] >> 2) & 1023;
    p[19] = (w[5] << 8) & 1023;
    p[19] |= (w[6] >> 24);
    p[20] = (w[6] >> 14) & 1023;
    p[21] = (w[6] >> 4) & 1023;
    p[22] = (w[6] << 6) & 1023;
    p[22] |= (w[7] >> 26);
    p[23] = (w[7] >> 16) & 1023;
    p[24] = (w[7] >> 6) & 1023;
    p[25] = (w[7] << 4) & 1023;
    p[25] |= (w[8] >> 28);
    p[26] = (w[8] >> 18) & 1023;
    p[27] = (w[8] >> 8) & 1023;
    p[28] = (w[8] << 2) & 1023;
    p[28] |= (w[9] >> 30);
    p[29] = (w[9] >> 20) & 1023;
    p[30] = (w[9] >> 10) & 1023;
    p[31] = (w[9]) & 1023;
  }
}

void unpack11(unsigned int* p, unsigned int* w, int BS) {
  int i;
  //printf("unpack11\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 11) {
    p[0] = (w[0] >> 21);
    p[1] = (w[0] >> 10) & 2047;
    p[2] = (w[0] << 1) & 2047;
    p[2] |= (w[1] >> 31);
    p[3] = (w[1] >> 20) & 2047;
    p[4] = (w[1] >> 9) & 2047;
    p[5] = (w[1] << 2) & 2047;
    p[5] |= (w[2] >> 30);
    p[6] = (w[2] >> 19) & 2047;
    p[7] = (w[2] >> 8) & 2047;
    p[8] = (w[2] << 3) & 2047;
    p[8] |= (w[3] >> 29);
    p[9] = (w[3] >> 18) & 2047;
    p[10] = (w[3] >> 7) & 2047;
    p[11] = (w[3] << 4) & 2047;
    p[11] |= (w[4] >> 28);
    p[12] = (w[4] >> 17) & 2047;
    p[13] = (w[4] >> 6) & 2047;
    p[14] = (w[4] << 5) & 2047;
    p[14] |= (w[5] >> 27);
    p[15] = (w[5] >> 16) & 2047;
    p[16] = (w[5] >> 5) & 2047;
    p[17] = (w[5] << 6) & 2047;
    p[17] |= (w[6] >> 26);
    p[18] = (w[6] >> 15) & 2047;
    p[19] = (w[6] >> 4) & 2047;
    p[20] = (w[6] << 7) & 2047;
    p[20] |= (w[7] >> 25);
    p[21] = (w[7] >> 14) & 2047;
    p[22] = (w[7] >> 3) & 2047;
    p[23] = (w[7] << 8) & 2047;
    p[23] |= (w[8] >> 24);
    p[24] = (w[8] >> 13) & 2047;
    p[25] = (w[8] >> 2) & 2047;
    p[26] = (w[8] << 9) & 2047;
    p[26] |= (w[9] >> 23);
    p[27] = (w[9] >> 12) & 2047;
    p[28] = (w[9] >> 1) & 2047;
    p[29] = (w[9] << 10) & 2047;
    p[29] |= (w[10] >> 22);
    p[30] = (w[10] >> 11) & 2047;
    p[31] = (w[10]) & 2047;
  }
}

void unpack12(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack12\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 12) {
	 //printf("Inside unpack12: %d\n", i);
    p[0] = (w[0] >> 20);
    p[1] = (w[0] >> 8) & 4095;
    p[2] = (w[0] << 4) & 4095;
    p[2] |= (w[1] >> 28);
    p[3] = (w[1] >> 16) & 4095;
    p[4] = (w[1] >> 4) & 4095;
    p[5] = (w[1] << 8) & 4095;
    p[5] |= (w[2] >> 24);
    p[6] = (w[2] >> 12) & 4095;
    p[7] = (w[2]) & 4095;
    p[8] = (w[3] >> 20);
    p[9] = (w[3] >> 8) & 4095;
    p[10] = (w[3] << 4) & 4095;
    p[10] |= (w[4] >> 28);
    p[11] = (w[4] >> 16) & 4095;
    p[12] = (w[4] >> 4) & 4095;
    p[13] = (w[4] << 8) & 4095;
    p[13] |= (w[5] >> 24);
    p[14] = (w[5] >> 12) & 4095;
    p[15] = (w[5]) & 4095;
    p[16] = (w[6] >> 20);
    p[17] = (w[6] >> 8) & 4095;
    p[18] = (w[6] << 4) & 4095;
    p[18] |= (w[7] >> 28);
    p[19] = (w[7] >> 16) & 4095;
    p[20] = (w[7] >> 4) & 4095;
    p[21] = (w[7] << 8) & 4095;
    p[21] |= (w[8] >> 24);
    p[22] = (w[8] >> 12) & 4095;
    p[23] = (w[8]) & 4095;
    p[24] = (w[9] >> 20);
    p[25] = (w[9] >> 8) & 4095;
    p[26] = (w[9] << 4) & 4095;
    p[26] |= (w[10] >> 28);
    p[27] = (w[10] >> 16) & 4095;
    p[28] = (w[10] >> 4) & 4095;
    p[29] = (w[10] << 8) & 4095;
    p[29] |= (w[11] >> 24);
    p[30] = (w[11] >> 12) & 4095;
    p[31] = (w[11]) & 4095;
  }
}

void unpack13(unsigned int* p, unsigned int* w, int BS) {
  int i;
   //printf("unpack13\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 13) {
    p[0] = (w[0] >> 19);
    p[1] = (w[0] >> 6) & 8191;
    p[2] = (w[0] << 7) & 8191;
    p[2] |= (w[1] >> 25);
    p[3] = (w[1] >> 12) & 8191;
    p[4] = (w[1] << 1) & 8191;
    p[4] |= (w[2] >> 31);
    p[5] = (w[2] >> 18) & 8191;
    p[6] = (w[2] >> 5) & 8191;
    p[7] = (w[2] << 8) & 8191;
    p[7] |= (w[3] >> 24);
    p[8] = (w[3] >> 11) & 8191;
    p[9] = (w[3] << 2) & 8191;
    p[9] |= (w[4] >> 30);
    p[10] = (w[4] >> 17) & 8191;
    p[11] = (w[4] >> 4) & 8191;
    p[12] = (w[4] << 9) & 8191;
    p[12] |= (w[5] >> 23);
    p[13] = (w[5] >> 10) & 8191;
    p[14] = (w[5] << 3) & 8191;
    p[14] |= (w[6] >> 29);
    p[15] = (w[6] >> 16) & 8191;
    p[16] = (w[6] >> 3) & 8191;
    p[17] = (w[6] << 10) & 8191;
    p[17] |= (w[7] >> 22);
    p[18] = (w[7] >> 9) & 8191;
    p[19] = (w[7] << 4) & 8191;
    p[19] |= (w[8] >> 28);
    p[20] = (w[8] >> 15) & 8191;
    p[21] = (w[8] >> 2) & 8191;
    p[22] = (w[8] << 11) & 8191;
    p[22] |= (w[9] >> 21);
    p[23] = (w[9] >> 8) & 8191;
    p[24] = (w[9] << 5) & 8191;
    p[24] |= (w[10] >> 27);
    p[25] = (w[10] >> 14) & 8191;
    p[26] = (w[10] >> 1) & 8191;
    p[27] = (w[10] << 12) & 8191;
    p[27] |= (w[11] >> 20);
    p[28] = (w[11] >> 7) & 8191;
    p[29] = (w[11] << 6) & 8191;
    p[29] |= (w[12] >> 26);
    p[30] = (w[12] >> 13) & 8191;
    p[31] = (w[12]) & 8191;
  }
}

void unpack16(unsigned int* p, unsigned int* w, int BS) {
  int i;
  // printf("unpack16\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 16) {
    p[0] = (w[0] >> 16);
    p[1] = (w[0]) & 65535;
    p[2] = (w[1] >> 16);
    p[3] = (w[1]) & 65535;
    p[4] = (w[2] >> 16);
    p[5] = (w[2]) & 65535;
    p[6] = (w[3] >> 16);
    p[7] = (w[3]) & 65535;
    p[8] = (w[4] >> 16);
    p[9] = (w[4]) & 65535;
    p[10] = (w[5] >> 16);
    p[11] = (w[5]) & 65535;
    p[12] = (w[6] >> 16);
    p[13] = (w[6]) & 65535;
    p[14] = (w[7] >> 16);
    p[15] = (w[7]) & 65535;
    p[16] = (w[8] >> 16);
    p[17] = (w[8]) & 65535;
    p[18] = (w[9] >> 16);
    p[19] = (w[9]) & 65535;
    p[20] = (w[10] >> 16);
    p[21] = (w[10]) & 65535;
    p[22] = (w[11] >> 16);
    p[23] = (w[11]) & 65535;
    p[24] = (w[12] >> 16);
    p[25] = (w[12]) & 65535;
    p[26] = (w[13] >> 16);
    p[27] = (w[13]) & 65535;
    p[28] = (w[14] >> 16);
    p[29] = (w[14]) & 65535;
    p[30] = (w[15] >> 16);
    p[31] = (w[15]) & 65535;
  }
}

void unpack20(unsigned int* p, unsigned int* w, int BS) {
  int i;
  // printf("unpack20\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 20) {
    p[0] = (w[0] >> 12);
    p[1] = (w[0] << 8) & ((1 << 20) - 1);
    p[1] |= (w[1] >> 24);
    p[2] = (w[1] >> 4) & ((1 << 20) - 1);
    p[3] = (w[1] << 16) & ((1 << 20) - 1);
    p[3] |= (w[2] >> 16);
    p[4] = (w[2] << 4) & ((1 << 20) - 1);
    p[4] |= (w[3] >> 28);
    p[5] = (w[3] >> 8) & ((1 << 20) - 1);
    p[6] = (w[3] << 12) & ((1 << 20) - 1);
    p[6] |= (w[4] >> 20);
    p[7] = (w[4]) & ((1 << 20) - 1);
    p[8] = (w[5] >> 12);
    p[9] = (w[5] << 8) & ((1 << 20) - 1);
    p[9] |= (w[6] >> 24);
    p[10] = (w[6] >> 4) & ((1 << 20) - 1);
    p[11] = (w[6] << 16) & ((1 << 20) - 1);
    p[11] |= (w[7] >> 16);
    p[12] = (w[7] << 4) & ((1 << 20) - 1);
    p[12] |= (w[8] >> 28);
    p[13] = (w[8] >> 8) & ((1 << 20) - 1);
    p[14] = (w[8] << 12) & ((1 << 20) - 1);
    p[14] |= (w[9] >> 20);
    p[15] = (w[9]) & ((1 << 20) - 1);
    p[16] = (w[10] >> 12);
    p[17] = (w[10] << 8) & ((1 << 20) - 1);
    p[17] |= (w[11] >> 24);
    p[18] = (w[11] >> 4) & ((1 << 20) - 1);
    p[19] = (w[11] << 16) & ((1 << 20) - 1);
    p[19] |= (w[12] >> 16);
    p[20] = (w[12] << 4) & ((1 << 20) - 1);
    p[20] |= (w[13] >> 28);
    p[21] = (w[13] >> 8) & ((1 << 20) - 1);
    p[22] = (w[13] << 12) & ((1 << 20) - 1);
    p[22] |= (w[14] >> 20);
    p[23] = (w[14]) & ((1 << 20) - 1);
    p[24] = (w[15] >> 12);
    p[25] = (w[15] << 8) & ((1 << 20) - 1);
    p[25] |= (w[16] >> 24);
    p[26] = (w[16] >> 4) & ((1 << 20) - 1);
    p[27] = (w[16] << 16) & ((1 << 20) - 1);
    p[27] |= (w[17] >> 16);
    p[28] = (w[17] << 4) & ((1 << 20) - 1);
    p[28] |= (w[18] >> 28);
    p[29] = (w[18] >> 8) & ((1 << 20) - 1);
    p[30] = (w[18] << 12) & ((1 << 20) - 1);
    p[30] |= (w[19] >> 20);
    p[31] = (w[19]) & ((1 << 20) - 1);
  }
}

void unpack32(unsigned int* p, unsigned int* w, int BS) {
  int i;
  // printf("unpack32\n");
  for (i = 0; i < BS; i += 32, p += 32, w += 32) {
    p[0] = w[0];
    p[1] = w[1];
    p[2] = w[2];
    p[3] = w[3];
    p[4] = w[4];
    p[5] = w[5];
    p[6] = w[6];
    p[7] = w[7];
    p[8] = w[8];
    p[9] = w[9];
    p[10] = w[10];
    p[11] = w[11];
    p[12] = w[12];
    p[13] = w[13];
    p[14] = w[14];
    p[15] = w[15];
    p[16] = w[16];
    p[17] = w[17];
    p[18] = w[18];
    p[19] = w[19];
    p[20] = w[20];
    p[21] = w[21];
    p[22] = w[22];
    p[23] = w[23];
    p[24] = w[24];
    p[25] = w[25];
    p[26] = w[26];
    p[27] = w[27];
    p[28] = w[28];
    p[29] = w[29];
    p[30] = w[30];
    p[31] = w[31];
  }
}

uint32_t ones[32] = {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535, 131071, 262143, 524287, 1048575, 2097151, 4194303, 8388607, 16777215, 33554431, 67108863, 134217727, 268435455, 536870911, 1073741823, 2147483647};

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
#define MASK_31 0x0000001F
#define MOD_32(X) ((X) & (MASK_31))
#define DIV_32(X) ((X) >> 5)

void unpack_pos0(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	*ret = 0;
}

void unpack_pos1(uint32_t *ret, unsigned int *w, int query_pos, int block_size)
{
	int bit_pos = 1 * (query_pos & (block_size-1));
	*ret = (w[DIV_32(bit_pos)] >> (MASK_31 - MOD_32(bit_pos))) & 1;
	
}

void unpack_pos2(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 2 * (pos & (block_size-1));
	
	*ret = ((w[DIV_32(bit_pos)] >> (MASK_31 - MOD_32(bit_pos))) & 1) << 1 | 
	       ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1);
		   

}

void unpack_pos3(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 3 * (pos & (block_size-1));
	
	*ret = ((w[DIV_32(bit_pos)] >> (MASK_31 - MOD_32(bit_pos))) & 1) << 2 | 
	       ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 1 |
		   ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1);


}

void unpack_pos4(uint32_t *ret, unsigned int *w, int query_pos, int block_size)
{
	int bit_pos = 4 * (query_pos & (block_size-1));
	
	*ret = ((w[DIV_32(bit_pos)] >> (MASK_31 - MOD_32(bit_pos))) & 1) << 3 | 
	       ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 2 |
		   ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1) << 1 |
		   ((w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1);


}

void unpack_pos5(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 5 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 27 - pos_in_word; // 32 - 5 = 27

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[5]); // ((1 << 5) - 1)
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[5]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}

/*
	*ret = ( (w[DIV_32(bit_pos+0)] >> (MASK_31 - MOD_32(bit_pos+0))) & 1 ) << 4 | 
	       ( (w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1 ) << 3 |
		   ( (w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1 ) << 2 |
		   ( (w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1 ) << 1 |
		   ( (w[DIV_32(bit_pos+4)] >> (MASK_31 - MOD_32(bit_pos+4))) & 1 );
*/

}

void unpack_pos6(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 6 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 26 - pos_in_word; // 32 - 6 = 26

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[6]);
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[6]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}
/*
	*ret = ((w[DIV_32(bit_pos+0)] >> (MASK_31 - MOD_32(bit_pos+0))) & 1) << 5 | 
	       ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 4 |
		   ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1) << 3 |
		   ((w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1) << 2 |
		   ((w[DIV_32(bit_pos+4)] >> (MASK_31 - MOD_32(bit_pos+4))) & 1) << 1 |
		   ((w[DIV_32(bit_pos+5)] >> (MASK_31 - MOD_32(bit_pos+5))) & 1);
*/  
}

void unpack_pos7(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 7 * (pos & (block_size - 1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 25 - pos_in_word; // 32 - 7 = 25

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[7]);
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[7]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}
	
	/*
	*ret = ((w[DIV_32(bit_pos+0)] >> (MASK_31 - MOD_32(bit_pos+0))) & 1) << 6 | 
	       ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 5 |
		   ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1) << 4 |
		   ((w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1) << 3 |
		   ((w[DIV_32(bit_pos+4)] >> (MASK_31 - MOD_32(bit_pos+4))) & 1) << 2 |
		   ((w[DIV_32(bit_pos+5)] >> (MASK_31 - MOD_32(bit_pos+5))) & 1) << 1 |
		   ((w[DIV_32(bit_pos+6)] >> (MASK_31 - MOD_32(bit_pos+6))) & 1);
*/
}

void unpack_pos8(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 8 * (pos & (block_size - 1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 24 - pos_in_word; // 32 - 8 = 24

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[8]);
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[8]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}

	// *ret = ((w[DIV_32(bit_pos+0)] >> (MASK_31 - MOD_32(bit_pos+0))) & 1) << 7 | 
	       // ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 6 |
		   // ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1) << 5 |
		   // ((w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1) << 4 |
		   // ((w[DIV_32(bit_pos+4)] >> (MASK_31 - MOD_32(bit_pos+4))) & 1) << 3 |
		   // ((w[DIV_32(bit_pos+5)] >> (MASK_31 - MOD_32(bit_pos+5))) & 1) << 2 |
		   // ((w[DIV_32(bit_pos+6)] >> (MASK_31 - MOD_32(bit_pos+6))) & 1) << 1 |
		   // ((w[DIV_32(bit_pos+7)] >> (MASK_31 - MOD_32(bit_pos+7))) & 1);
}

void unpack_pos9(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 9 * (pos & (block_size - 1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 23 - pos_in_word; // 32 - 9 = 23

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[9]);
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[9]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}
	
	
	// *ret = ((w[DIV_32(bit_pos+0)] >> (MASK_31 - MOD_32(bit_pos+0))) & 1) << 8 | 
	       // ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 7 |
		   // ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1) << 6 |
		   // ((w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1) << 5 |
		   // ((w[DIV_32(bit_pos+4)] >> (MASK_31 - MOD_32(bit_pos+4))) & 1) << 4 |
		   // ((w[DIV_32(bit_pos+5)] >> (MASK_31 - MOD_32(bit_pos+5))) & 1) << 3 |
		   // ((w[DIV_32(bit_pos+6)] >> (MASK_31 - MOD_32(bit_pos+6))) & 1) << 2 |
		   // ((w[DIV_32(bit_pos+7)] >> (MASK_31 - MOD_32(bit_pos+7))) & 1) << 1 |
		   // ((w[DIV_32(bit_pos+8)] >> (MASK_31 - MOD_32(bit_pos+8))) & 1);
		   

}

void unpack_pos10(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 10 * (pos & (block_size - 1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 22 - pos_in_word; // 32 - 10 = 22

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[10]);
	else
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[10]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	
	// *ret = ((w[DIV_32(bit_pos+0)] >> (MASK_31 - MOD_32(bit_pos+0))) & 1) << 9 | 
	       // ((w[DIV_32(bit_pos+1)] >> (MASK_31 - MOD_32(bit_pos+1))) & 1) << 8 |
		   // ((w[DIV_32(bit_pos+2)] >> (MASK_31 - MOD_32(bit_pos+2))) & 1) << 7 |
		   // ((w[DIV_32(bit_pos+3)] >> (MASK_31 - MOD_32(bit_pos+3))) & 1) << 6 |
		   // ((w[DIV_32(bit_pos+4)] >> (MASK_31 - MOD_32(bit_pos+4))) & 1) << 5 |
		   // ((w[DIV_32(bit_pos+5)] >> (MASK_31 - MOD_32(bit_pos+5))) & 1) << 4 |
		   // ((w[DIV_32(bit_pos+6)] >> (MASK_31 - MOD_32(bit_pos+6))) & 1) << 3 |
		   // ((w[DIV_32(bit_pos+7)] >> (MASK_31 - MOD_32(bit_pos+7))) & 1) << 2 |
		   // ((w[DIV_32(bit_pos+8)] >> (MASK_31 - MOD_32(bit_pos+8))) & 1) << 1 |
		   // ((w[DIV_32(bit_pos+9)] >> (MASK_31 - MOD_32(bit_pos+9))) & 1);	
	

}

void unpack_pos11(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 11 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 21 - pos_in_word; // 32 - 11 = 21

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[11]);
	else
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[11]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	
	// *ret = ((w[DIV_32(bit_pos+0)]  >> (MASK_31 - MOD_32(bit_pos+0)))  & 1) << 10 | 
	       // ((w[DIV_32(bit_pos+1)]  >> (MASK_31 - MOD_32(bit_pos+1)))  & 1) << 9  |
		   // ((w[DIV_32(bit_pos+2)]  >> (MASK_31 - MOD_32(bit_pos+2)))  & 1) << 8  |
		   // ((w[DIV_32(bit_pos+3)]  >> (MASK_31 - MOD_32(bit_pos+3)))  & 1) << 7  |
	       // ((w[DIV_32(bit_pos+4)]  >> (MASK_31 - MOD_32(bit_pos+4)))  & 1) << 6  |
		   // ((w[DIV_32(bit_pos+5)]  >> (MASK_31 - MOD_32(bit_pos+5)))  & 1) << 5  |
		   // ((w[DIV_32(bit_pos+6)]  >> (MASK_31 - MOD_32(bit_pos+6)))  & 1) << 4  |
	       // ((w[DIV_32(bit_pos+7)]  >> (MASK_31 - MOD_32(bit_pos+7)))  & 1) << 3  |
		   // ((w[DIV_32(bit_pos+8)]  >> (MASK_31 - MOD_32(bit_pos+8)))  & 1) << 2  |
		   // ((w[DIV_32(bit_pos+9)]  >> (MASK_31 - MOD_32(bit_pos+9)))  & 1) << 1  |
		   // ((w[DIV_32(bit_pos+10)] >> (MASK_31 - MOD_32(bit_pos+10))) & 1);


}

void unpack_pos12(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 12 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 20 - pos_in_word; // 32 - 12

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[12]);
	else
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[12]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;

	// *ret = ((w[DIV_32(bit_pos+0)]  >> (MASK_31 - MOD_32(bit_pos+0)))  & 1) << 11 | 
	       // ((w[DIV_32(bit_pos+1)]  >> (MASK_31 - MOD_32(bit_pos+1)))  & 1) << 10 |
		   // ((w[DIV_32(bit_pos+2)]  >> (MASK_31 - MOD_32(bit_pos+2)))  & 1) << 9  |
		   // ((w[DIV_32(bit_pos+3)]  >> (MASK_31 - MOD_32(bit_pos+3)))  & 1) << 8  |
	       // ((w[DIV_32(bit_pos+4)]  >> (MASK_31 - MOD_32(bit_pos+4)))  & 1) << 7  |
		   // ((w[DIV_32(bit_pos+5)]  >> (MASK_31 - MOD_32(bit_pos+5)))  & 1) << 6  |
		   // ((w[DIV_32(bit_pos+6)]  >> (MASK_31 - MOD_32(bit_pos+6)))  & 1) << 5  |
	       // ((w[DIV_32(bit_pos+7)]  >> (MASK_31 - MOD_32(bit_pos+7)))  & 1) << 4  |
		   // ((w[DIV_32(bit_pos+8)]  >> (MASK_31 - MOD_32(bit_pos+8)))  & 1) << 3  |
		   // ((w[DIV_32(bit_pos+9)]  >> (MASK_31 - MOD_32(bit_pos+9)))  & 1) << 2  |
		   // ((w[DIV_32(bit_pos+10)] >> (MASK_31 - MOD_32(bit_pos+10))) & 1) << 1  |
		   // ((w[DIV_32(bit_pos+11)] >> (MASK_31 - MOD_32(bit_pos+11))) & 1);
}

void unpack_pos13(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 13 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 19 - pos_in_word; // 32 - 13 = 19

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[13]);
	else
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[13]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	
	// *ret = ((w[DIV_32(bit_pos+0)]  >> (MASK_31 - MOD_32(bit_pos+0)))  & 1) << 12 | 
	       // ((w[DIV_32(bit_pos+1)]  >> (MASK_31 - MOD_32(bit_pos+1)))  & 1) << 11 |
		   // ((w[DIV_32(bit_pos+2)]  >> (MASK_31 - MOD_32(bit_pos+2)))  & 1) << 10 |
		   // ((w[DIV_32(bit_pos+3)]  >> (MASK_31 - MOD_32(bit_pos+3)))  & 1) << 9  |
	       // ((w[DIV_32(bit_pos+4)]  >> (MASK_31 - MOD_32(bit_pos+4)))  & 1) << 8  |
		   // ((w[DIV_32(bit_pos+5)]  >> (MASK_31 - MOD_32(bit_pos+5)))  & 1) << 7  |
		   // ((w[DIV_32(bit_pos+6)]  >> (MASK_31 - MOD_32(bit_pos+6)))  & 1) << 6  |
	       // ((w[DIV_32(bit_pos+7)]  >> (MASK_31 - MOD_32(bit_pos+7)))  & 1) << 5  |
		   // ((w[DIV_32(bit_pos+8)]  >> (MASK_31 - MOD_32(bit_pos+8)))  & 1) << 4  |
		   // ((w[DIV_32(bit_pos+9)]  >> (MASK_31 - MOD_32(bit_pos+9)))  & 1) << 3  |
		   // ((w[DIV_32(bit_pos+10)] >> (MASK_31 - MOD_32(bit_pos+10))) & 1) << 2  |
		   // ((w[DIV_32(bit_pos+11)] >> (MASK_31 - MOD_32(bit_pos+11))) & 1) << 1  |
		   // ((w[DIV_32(bit_pos+12)] >> (MASK_31 - MOD_32(bit_pos+12))) & 1);
		   	
	

}

void unpack_pos16(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 16 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 16 - pos_in_word; // 32 - 16 = 16

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[16]);
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[16]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}
	
	// *ret = ((w[DIV_32(bit_pos+0)]  >> (MASK_31 - MOD_32(bit_pos+0)))  & 1) << 15 | 
	       // ((w[DIV_32(bit_pos+1)]  >> (MASK_31 - MOD_32(bit_pos+1)))  & 1) << 14 |
		   // ((w[DIV_32(bit_pos+2)]  >> (MASK_31 - MOD_32(bit_pos+2)))  & 1) << 13 |
		   // ((w[DIV_32(bit_pos+3)]  >> (MASK_31 - MOD_32(bit_pos+3)))  & 1) << 12 |
	       // ((w[DIV_32(bit_pos+4)]  >> (MASK_31 - MOD_32(bit_pos+4)))  & 1) << 11 |
		   // ((w[DIV_32(bit_pos+5)]  >> (MASK_31 - MOD_32(bit_pos+5)))  & 1) << 10 |
		   // ((w[DIV_32(bit_pos+6)]  >> (MASK_31 - MOD_32(bit_pos+6)))  & 1) << 9  |
	       // ((w[DIV_32(bit_pos+7)]  >> (MASK_31 - MOD_32(bit_pos+7)))  & 1) << 8  |
		   // ((w[DIV_32(bit_pos+8)]  >> (MASK_31 - MOD_32(bit_pos+8)))  & 1) << 7  |
		   // ((w[DIV_32(bit_pos+9)]  >> (MASK_31 - MOD_32(bit_pos+9)))  & 1) << 6  |
		   // ((w[DIV_32(bit_pos+10)] >> (MASK_31 - MOD_32(bit_pos+10))) & 1) << 5  |
		   // ((w[DIV_32(bit_pos+11)] >> (MASK_31 - MOD_32(bit_pos+11))) & 1) << 4  |
		   // ((w[DIV_32(bit_pos+12)] >> (MASK_31 - MOD_32(bit_pos+12))) & 1) << 3  |
		   // ((w[DIV_32(bit_pos+13)] >> (MASK_31 - MOD_32(bit_pos+13))) & 1) << 2  |
		   // ((w[DIV_32(bit_pos+14)] >> (MASK_31 - MOD_32(bit_pos+14))) & 1) << 1  |
		   // ((w[DIV_32(bit_pos+15)] >> (MASK_31 - MOD_32(bit_pos+15))) & 1);
		   

}

void unpack_pos20(uint32_t *ret, unsigned int *w, int pos, int block_size)
{
	int bit_pos = 20 * (pos & (block_size-1));
	int pos_in_word = MOD_32(bit_pos);
	int shift = 12 - pos_in_word; // 32 - 20 = 12

	if (shift >= 0)
		*ret = ((w[DIV_32(bit_pos)] >> shift) & ones[20]);
	else
	{
		*ret = ((w[DIV_32(bit_pos)] << -shift) & ones[20]) | 
		       ((w[DIV_32(bit_pos)+1] >> (32 + shift)) & ones[-shift]) ;
	}
	
	// *ret = ((w[DIV_32(bit_pos+0)]  >> (MASK_31 - MOD_32(bit_pos+0)))  & 1) << 19 | 
	       // ((w[DIV_32(bit_pos+1)]  >> (MASK_31 - MOD_32(bit_pos+1)))  & 1) << 18 |
		   // ((w[DIV_32(bit_pos+2)]  >> (MASK_31 - MOD_32(bit_pos+2)))  & 1) << 17 |
		   // ((w[DIV_32(bit_pos+3)]  >> (MASK_31 - MOD_32(bit_pos+3)))  & 1) << 16 |
	       // ((w[DIV_32(bit_pos+4)]  >> (MASK_31 - MOD_32(bit_pos+4)))  & 1) << 15 |
		   // ((w[DIV_32(bit_pos+5)]  >> (MASK_31 - MOD_32(bit_pos+5)))  & 1) << 14 |
		   // ((w[DIV_32(bit_pos+6)]  >> (MASK_31 - MOD_32(bit_pos+6)))  & 1) << 13 |
	       // ((w[DIV_32(bit_pos+7)]  >> (MASK_31 - MOD_32(bit_pos+7)))  & 1) << 12 |
		   // ((w[DIV_32(bit_pos+8)]  >> (MASK_31 - MOD_32(bit_pos+8)))  & 1) << 11 |
		   // ((w[DIV_32(bit_pos+9)]  >> (MASK_31 - MOD_32(bit_pos+9)))  & 1) << 10 |
		   // ((w[DIV_32(bit_pos+10)] >> (MASK_31 - MOD_32(bit_pos+10))) & 1) << 9  |
		   // ((w[DIV_32(bit_pos+11)] >> (MASK_31 - MOD_32(bit_pos+11))) & 1) << 8  |
		   // ((w[DIV_32(bit_pos+12)] >> (MASK_31 - MOD_32(bit_pos+12))) & 1) << 7  |
		   // ((w[DIV_32(bit_pos+13)] >> (MASK_31 - MOD_32(bit_pos+13))) & 1) << 6  |
		   // ((w[DIV_32(bit_pos+14)] >> (MASK_31 - MOD_32(bit_pos+14))) & 1) << 5  |
		   // ((w[DIV_32(bit_pos+15)] >> (MASK_31 - MOD_32(bit_pos+15))) & 1) << 4  |
		   // ((w[DIV_32(bit_pos+16)] >> (MASK_31 - MOD_32(bit_pos+16))) & 1) << 3  |
		   // ((w[DIV_32(bit_pos+17)] >> (MASK_31 - MOD_32(bit_pos+17))) & 1) << 2  |
		   // ((w[DIV_32(bit_pos+18)] >> (MASK_31 - MOD_32(bit_pos+18))) & 1) << 1  |
		   // ((w[DIV_32(bit_pos+19)] >> (MASK_31 - MOD_32(bit_pos+19))) & 1);
		   

}

void unpack_pos32(unsigned *ret, unsigned int *w, int pos, int block_size)
{
	*ret = w[pos & (block_size - 1)];
}

