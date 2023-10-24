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

/* STRUCTS HERE FOR TESTING PURPOSES */
typedef struct VCS_avg {
    float Y[4];
    unsigned PB_avg, PR_avg;
} *VCS_avg;

typedef struct DCT_data{
    unsigned a;
    signed b, c, d;
    unsigned PB_avg, PR_avg;
} *DCT_data;
extern void VCS_to_DCT(A2_and_Methods arr_and_meth);
extern void DCT_to_VCS(A2_and_Methods arr_and_meth);



#endif