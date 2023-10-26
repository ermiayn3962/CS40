/**************************************************************
 *
 *                     rgb_conversion.h
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (golem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions to convert an RGB pixel from a integer
 *     representation to a float representation. Also contains
 *     functions to convert from an RGB to video color space and
 *     vice versa.
 *
 *
 **************************************************************/

#ifndef RGB_CONVERSION_INCLUDED
#define RGB_CONVERSION_INCLUDED

#include "image_manipulation.h"

#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>
#include <pnm.h>

/*  VCS_data
*   Represents the Video Color Space representation of a RGB pixel
*   for a 2x2 blocked of UArray2. 
*
*   Y - the luminocity of the pixel
*   Pb - the blue side channel
*   Pr - the red side channel
*/
typedef struct VCS_data
{
        float Y, Pb, Pr;
} *VCS_data;


extern void RGB_to_float(A2_and_Methods array_and_methods);
extern void RGB_to_int(A2_and_Methods array_and_methods);

extern void RGB_to_VCS(A2_and_Methods array_and_methods);
extern void VCS_to_RGB(A2_and_Methods array_and_methods);

#endif