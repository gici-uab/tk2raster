/*
Copyright 2012 Christopher Hoobin. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

THIS SOFTWARE IS PROVIDED BY CHRISTOPHER HOOBIN ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CHRISTOPHER HOOBIN OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation
are those of the authors and should not be interpreted as representing
official policies, either expressed or implied, of Christopher Hoobin.
*/

#ifndef SIMPLE9_H
#define SIMPLE9_H

#include <stdint.h>
#include <stdio.h>

#define UINT32_BITS (sizeof(uint32_t) * CHAR_BIT)

#define SELECTOR_MASK 0x0000000F

#define SELECTOR_BITS 4

#define CODE_BITS (UINT32_BITS - SELECTOR_BITS)

#define MAX_VALUE ((1UL << CODE_BITS) - 1)

#define NSELECTORS 9

struct {
    uint32_t nitems;
    uint32_t nbits;
	uint32_t mask;
    uint32_t nwaste;
} selectors[NSELECTORS] = {
    {28,  1, 0x0001,  0},
    {14,  2, 0x0003,  0},
    { 9,  3, 0x0007,  1},
    { 7,  4, 0x000F,  0},
    { 5,  5, 0x001F,  3},
    { 4,  7, 0x007F,  0},
    { 3,  9, 0x01FF,  1},
    { 2, 14, 0x03FF,  0},
    { 1, 28, 0x0FFF, 0}
	
    // {28,  1, 0},
    // {14,  2, 0},
    // { 9,  3, 1},
    // { 7,  4, 0},
    // { 5,  5, 3},
    // { 4,  7, 0},
    // { 3,  9, 1},
    // { 2, 14, 0},
    // { 1, 28, 0},
};

uint32_t vbyte_encode(uint32_t value, FILE *fp);

uint32_t vbyte_decode(uint32_t *value, FILE *fp);


/* Performs a simple9 encoding of n elements from array. The result is
 * written to the file stream.
 *
 * Returns the number of bytes written to the file stream. */
uint32_t simple9_encode(uint32_t *array, uint32_t n, FILE *fp);

/* Decodes a simple9 encoding read from a file stream.
 *
 * Memory for *array is allocated inside the function and n is also
 * set. The caller is responsible for freeing memory.
 *
 * Returns the number of bytes read from the file stream. */
uint32_t simple9_decode(uint32_t **array, uint32_t *n, FILE *fp);

uint32_t simple9_get_cell(uint32_t **array, uint32_t *n, FILE *fp, uint32_t pos);

uint32_t simple9_decode_unrolled(uint32_t **array, uint32_t *n, FILE *fp);

#endif /* SIMPLE9_H */
