#define RGB_CONVERSION_INCLUDED
#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>
#include <pnm.h>

/* colored pixel (float) */
typedef struct Pnm_rgb_float {
        float red, green, blue;
} *Pnm_rgb_float;
typedef struct VCS_data
{
        float Y, Pb, Pr;
} *VCS_data;
typedef struct A2_and_Methods {
        A2Methods_UArray2 array;
        A2Methods_T methods;
} *A2_and_Methods;


extern void RGB_to_float(Pnm_ppm image);
extern void RGB_to_int(Pnm_ppm image);

extern void RGB_to_VCS(A2Methods_UArray2 array, A2Methods_T methods);
extern void VCS_to_RGB(A2Methods_UArray2 array, A2Methods_T methods);

