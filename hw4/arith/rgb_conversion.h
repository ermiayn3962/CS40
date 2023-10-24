#ifndef RGB_CONVERSION_INCLUDED
#define RGB_CONVERSION_INCLUDED

#include "image_manipulation.h"

#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>
#include <pnm.h>

/* colored pixel (float) */
typedef struct RGB_float {
        float red, green, blue;
} *RGB_float;


typedef struct VCS_data
{
        float Y, Pb, Pr;
} *VCS_data;




extern void RGB_to_float(A2_and_Methods array_and_methods);
extern void RGB_to_int(A2_and_Methods array_and_methods);

extern void RGB_to_VCS(A2_and_Methods array_and_methods);
extern void VCS_to_RGB(A2_and_Methods array_and_methods);

#endif