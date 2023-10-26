/**************************************************************
 *
 *                     vcs_conversion.h
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (cgolem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions to convert from video color space
 *     to discrete cosine space and vice versa.
 *
 *
 **************************************************************/

#ifndef VCS_CONVERSION_INCLUDED
#define VCS_CONVERSION_INCLUDED

#include <a2methods.h>
#include <a2blocked.h>
#include <uarray.h>
#include <uarray2b.h>

#include <uarray2.h>
#include <assert.h>
#include <arith40.h>
#include <math.h>

#include "rgb_conversion.h"

/*  DCT_data
*   Represents data from 2x2 grid of image. Final data representation before
*   encoding.
*   a - A value from DCT conversion, scaled to 9 bit signed int
*   b - B value from DCT conversion, scaled to 5 bit unsigned int
*   c - C value from DCT conversion, scaled to 5 bit unsigned int
*   d - D value from DCT conversion, scaled to 5 bit unsigned int
*   PB_avg - average PB, scaled to a 4 bit unsigned int
*   PR_avg - average PR, scaled to a 4 bit unsigned int
*/
typedef struct DCT_data{
    unsigned a;
    signed b, c, d;
    unsigned PB_avg, PR_avg;
} *DCT_data;

extern void VCS_to_DCT(A2_and_Methods arr_and_meth);
extern void DCT_to_VCS(A2_and_Methods arr_and_meth);



#endif