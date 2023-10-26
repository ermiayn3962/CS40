/**************************************************************
 *
 *                     codeword.h
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (cgolem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions to pack/unpack and output/read 
 *     32-bit words.
 *
 *
 **************************************************************/

#include <bitpack.h>
#include "vcs_conversion.h"
#include "image_manipulation.h"
#include <stdint.h>

extern void encodeImage(A2_and_Methods dct_compress, int width, int height);
extern A2Methods_UArray2 decodeImage(FILE *input);