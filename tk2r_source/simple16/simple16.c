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

////
// This is an implementation of Simple16 algorithm for sorted integer arrays.
// The basic ideas are based on papers from:
//   1. http://www2008.org/papers/pdf/p387-zhangA.pdf 
//   2. http://www2009.org/proceedings/pdf/p401.pdf 
//
// The maximum possible integer value Simple16 can encode is < 2^28 (this is 
// dertermined by the Simple16 algorithm itself). Therefore, in order to use
// Simple16, the application must write their own code to encode numbers in 
// the range of [2^28, 2^32). A simple way is just write those numbers as 
// 32-bit integers (that is, no compression for very big numbers). 
//
// Alternative implementations:
//   * C++ http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/s16_coding.cc
//   * Java https://github.com/hyan/kamikaze/blob/master/src/main/java/com/kamikaze/pfordelta/Simple16.java
//
// This code is based on an implementation in C++ of the Poly-IR-Toolkit. 
// It's available at http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/s16_coding.cc
//

#include <stdio.h>
#include <stdlib.h>
#include "simple16.h"

unsigned int cbits[16][28] = 
  { {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,2,2,2,2,2,2,2,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,0,0,0,0,0,0,0},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {4,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {3,4,4,4,4,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,5,5,5,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {4,4,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {6,6,6,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,5,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {10,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {14,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };

unsigned int bit_mask[16][28] =
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,3,3,3,3,3,3,3,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,0,0,0,0,0,0,0},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{15,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{7,15,15,15,15,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{15,15,15,15,15,15,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{31,31,31,31,15,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{15,15,31,31,31,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{63,63,63,31,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{31,31,63,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{127,127,127,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1023,511,511,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{16383,16383,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{268435455,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};
	
unsigned int bit_shift_cumulative[16][28] =
{
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27},
	{0,2,4,6,8,10,12,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,0,0,0,0,0,0},
	{0,1,2,3,4,5,6,7,9,11,13,15,17,19,21,22,23,24,25,26,27,28,0,0,0,0,0,0},
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,18,20,22,24,26,28,0,0,0,0,0,0},
	{0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,4,7,10,13,16,19,22,25,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,3,7,11,15,19,22,25,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,4,8,12,16,20,24,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,5,10,15,20,24,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,4,8,13,18,23,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,6,12,18,23,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,5,10,16,22,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,7,14,21,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,10,19,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,14,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int s16_cnum[16] = {28, 21, 21, 21, 14, 9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 1};

//
// Compress an integer array using Simple16
// Parameters:
//    input pointer to the array of integers to compress
//    output pointer to the array of compressed integers
//    size number of integers to compress
// Returns:
//    the number of compressed integers
//
// Idea of compress algorithm:
//
// - do the next 28 numbers fit into one bit each?
//   - if yes: use that case 
//   - if no: do the next 21 numbers fits in an array of 7 integers of 2 bits and 14 integers of 1 bit each?
//      - if yes: use that case 
//      - if no: do the next 21 numbers fits in an array of 7 integers of 1 bit and 7 integers 2 bits and 7 integers of 1 bit each?
//      ... and so on .
int s16_compress(unsigned int* input, unsigned int* output, int size)
{
	int left = size;
	unsigned int* tmp = output;
	int ret;

	while (left > 0)
	{
		ret = s16_encode(tmp, input, left);
		input += ret;
		left -= ret;
		tmp++;
	}

	return tmp - output;
}

int s16_encode(unsigned int* w, unsigned int* p, unsigned int size)
{
	unsigned int k, j, m, o;

	for (k = 0; k < 16; k++)
	{
		*w = k << 28;
		m = (s16_cnum[k] < (int)size) ? s16_cnum[k] : size;
		
		for (j = 0, o = 0; (j < m) && (*(p + j) < (unsigned int) (1 << cbits[k][j])); j++)
		{
			*w += (p[j] << o);
			o += cbits[k][j];
		}
		
		if (j == m)
		{
			(p) += m;
			(w)++;
			break;
		}
	}

	return m;
}



//
// Decompress an integer array using Simple16
// Parameters:
//    input pointer to the array of compressed integers to decompress
//    output pointer to the array of integers
//    size number of integers to decompress
// Returns:
//    the number of processed integers
//
int s16_decompress(unsigned int* input, unsigned int* output, int size) {
  unsigned int *_p = output;
  int num;
  unsigned int* tmp = input;
  int left = size;
 
  while (left > 0) {
    num = s16_decode(tmp, _p);
    _p += num;
    left -= num;
    tmp++;
  }

  return tmp - input;
}

int s16_decode(unsigned int *w, unsigned int *p) {
  int _k = (*w) >> 28;
  switch (_k) {
    case 0:
      *p = (*w) & 1;
      p++;
      *p = (*w >> 1) & 1;
      p++;
      *p = (*w >> 2) & 1;
      p++;
      *p = (*w >> 3) & 1;
      p++;
      *p = (*w >> 4) & 1;
      p++;
      *p = (*w >> 5) & 1;
      p++;
      *p = (*w >> 6) & 1;
      p++;
      *p = (*w >> 7) & 1;
      p++;
      *p = (*w >> 8) & 1;
      p++;
      *p = (*w >> 9) & 1;
      p++;
      *p = (*w >> 10) & 1;
      p++;
      *p = (*w >> 11) & 1;
      p++;
      *p = (*w >> 12) & 1;
      p++;
      *p = (*w >> 13) & 1;
      p++;
      *p = (*w >> 14) & 1;
      p++;
      *p = (*w >> 15) & 1;
      p++;
      *p = (*w >> 16) & 1;
      p++;
      *p = (*w >> 17) & 1;
      p++;
      *p = (*w >> 18) & 1;
      p++;
      *p = (*w >> 19) & 1;
      p++;
      *p = (*w >> 20) & 1;
      p++;
      *p = (*w >> 21) & 1;
      p++;
      *p = (*w >> 22) & 1;
      p++;
      *p = (*w >> 23) & 1;
      p++;
      *p = (*w >> 24) & 1;
      p++;
      *p = (*w >> 25) & 1;
      p++;
      *p = (*w >> 26) & 1;
      p++;
      *p = (*w >> 27) & 1;
      p++;
      break;
    case 1:
      *p = (*w) & 3;
      p++;
      *p = (*w >> 2) & 3;
      p++;
      *p = (*w >> 4) & 3;
      p++;
      *p = (*w >> 6) & 3;
      p++;
      *p = (*w >> 8) & 3;
      p++;
      *p = (*w >> 10) & 3;
      p++;
      *p = (*w >> 12) & 3;
      p++;
      *p = (*w >> 14) & 1;
      p++;
      *p = (*w >> 15) & 1;
      p++;
      *p = (*w >> 16) & 1;
      p++;
      *p = (*w >> 17) & 1;
      p++;
      *p = (*w >> 18) & 1;
      p++;
      *p = (*w >> 19) & 1;
      p++;
      *p = (*w >> 20) & 1;
      p++;
      *p = (*w >> 21) & 1;
      p++;
      *p = (*w >> 22) & 1;
      p++;
      *p = (*w >> 23) & 1;
      p++;
      *p = (*w >> 24) & 1;
      p++;
      *p = (*w >> 25) & 1;
      p++;
      *p = (*w >> 26) & 1;
      p++;
      *p = (*w >> 27) & 1;
      p++;
      break;
    case 2:
      *p = (*w) & 1;
      p++;
      *p = (*w >> 1) & 1;
      p++;
      *p = (*w >> 2) & 1;
      p++;
      *p = (*w >> 3) & 1;
      p++;
      *p = (*w >> 4) & 1;
      p++;
      *p = (*w >> 5) & 1;
      p++;
      *p = (*w >> 6) & 1;
      p++;
      *p = (*w >> 7) & 3;
      p++;
      *p = (*w >> 9) & 3;
      p++;
      *p = (*w >> 11) & 3;
      p++;
      *p = (*w >> 13) & 3;
      p++;
      *p = (*w >> 15) & 3;
      p++;
      *p = (*w >> 17) & 3;
      p++;
      *p = (*w >> 19) & 3;
      p++;
      *p = (*w >> 21) & 1;
      p++;
      *p = (*w >> 22) & 1;
      p++;
      *p = (*w >> 23) & 1;
      p++;
      *p = (*w >> 24) & 1;
      p++;
      *p = (*w >> 25) & 1;
      p++;
      *p = (*w >> 26) & 1;
      p++;
      *p = (*w >> 27) & 1;
      p++;
      break;
    case 3:
      *p = (*w) & 1;
      p++;
      *p = (*w >> 1) & 1;
      p++;
      *p = (*w >> 2) & 1;
      p++;
      *p = (*w >> 3) & 1;
      p++;
      *p = (*w >> 4) & 1;
      p++;
      *p = (*w >> 5) & 1;
      p++;
      *p = (*w >> 6) & 1;
      p++;
      *p = (*w >> 7) & 1;
      p++;
      *p = (*w >> 8) & 1;
      p++;
      *p = (*w >> 9) & 1;
      p++;
      *p = (*w >> 10) & 1;
      p++;
      *p = (*w >> 11) & 1;
      p++;
      *p = (*w >> 12) & 1;
      p++;
      *p = (*w >> 13) & 1;
      p++;
      *p = (*w >> 14) & 3;
      p++;
      *p = (*w >> 16) & 3;
      p++;
      *p = (*w >> 18) & 3;
      p++;
      *p = (*w >> 20) & 3;
      p++;
      *p = (*w >> 22) & 3;
      p++;
      *p = (*w >> 24) & 3;
      p++;
      *p = (*w >> 26) & 3;
      p++;
      break;
    case 4:
      *p = (*w) & 3;
      p++;
      *p = (*w >> 2) & 3;
      p++;
      *p = (*w >> 4) & 3;
      p++;
      *p = (*w >> 6) & 3;
      p++;
      *p = (*w >> 8) & 3;
      p++;
      *p = (*w >> 10) & 3;
      p++;
      *p = (*w >> 12) & 3;
      p++;
      *p = (*w >> 14) & 3;
      p++;
      *p = (*w >> 16) & 3;
      p++;
      *p = (*w >> 18) & 3;
      p++;
      *p = (*w >> 20) & 3;
      p++;
      *p = (*w >> 22) & 3;
      p++;
      *p = (*w >> 24) & 3;
      p++;
      *p = (*w >> 26) & 3;
      p++;
      break;
    case 5:
      *p = (*w) & 15;
      p++;
      *p = (*w >> 4) & 7;
      p++;
      *p = (*w >> 7) & 7;
      p++;
      *p = (*w >> 10) & 7;
      p++;
      *p = (*w >> 13) & 7;
      p++;
      *p = (*w >> 16) & 7;
      p++;
      *p = (*w >> 19) & 7;
      p++;
      *p = (*w >> 22) & 7;
      p++;
      *p = (*w >> 25) & 7;
      p++;
      break;
    case 6:
      *p = (*w) & 7;
      p++;
      *p = (*w >> 3) & 15;
      p++;
      *p = (*w >> 7) & 15;
      p++;
      *p = (*w >> 11) & 15;
      p++;
      *p = (*w >> 15) & 15;
      p++;
      *p = (*w >> 19) & 7;
      p++;
      *p = (*w >> 22) & 7;
      p++;
      *p = (*w >> 25) & 7;
      p++;
      break;
    case 7:
      *p = (*w) & 15;
      p++;
      *p = (*w >> 4) & 15;
      p++;
      *p = (*w >> 8) & 15;
      p++;
      *p = (*w >> 12) & 15;
      p++;
      *p = (*w >> 16) & 15;
      p++;
      *p = (*w >> 20) & 15;
      p++;
      *p = (*w >> 24) & 15;
      p++;
      break;
    case 8:
      *p = (*w) & 31;
      p++;
      *p = (*w >> 5) & 31;
      p++;
      *p = (*w >> 10) & 31;
      p++;
      *p = (*w >> 15) & 31;
      p++;
      *p = (*w >> 20) & 15;
      p++;
      *p = (*w >> 24) & 15;
      p++;
      break;
    case 9:
      *p = (*w) & 15;
      p++;
      *p = (*w >> 4) & 15;
      p++;
      *p = (*w >> 8) & 31;
      p++;
      *p = (*w >> 13) & 31;
      p++;
      *p = (*w >> 18) & 31;
      p++;
      *p = (*w >> 23) & 31;
      p++;
      break;
    case 10:
      *p = (*w) & 63;
      p++;
      *p = (*w >> 6) & 63;
      p++;
      *p = (*w >> 12) & 63;
      p++;
      *p = (*w >> 18) & 31;
      p++;
      *p = (*w >> 23) & 31;
      p++;
      break;
    case 11:
      *p = (*w) & 31;
      p++;
      *p = (*w >> 5) & 31;
      p++;
      *p = (*w >> 10) & 63;
      p++;
      *p = (*w >> 16) & 63;
      p++;
      *p = (*w >> 22) & 63;
      p++;
      break;
    case 12:
      *p = (*w) & 127;
      p++;
      *p = (*w >> 7) & 127;
      p++;
      *p = (*w >> 14) & 127;
      p++;
      *p = (*w >> 21) & 127;
      p++;
      break;
    case 13:
      *p = (*w) & 1023;
      p++;
      *p = (*w >> 10) & 511;
      p++;
      *p = (*w >> 19) & 511;
      p++;
      break;
    case 14:
      *p = (*w) & 16383;
      p++;
      *p = (*w >> 14) & 16383;
      p++;
      break;
    case 15:
      *p = (*w) & ((1 << 28) - 1);
      p++;
      break;
  }
  return s16_cnum[_k];
}

