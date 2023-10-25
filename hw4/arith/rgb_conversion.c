#include "rgb_conversion.h"

#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

#include <math.h>

static void apply_RGB_to_VCS(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_VCS_to_RGB(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_RGB_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_RGB_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static int checkDenom(int val);
// static float round_to_range (float num, float lo, float hi);


// static float round_to_range (float num, float lo, float hi)
// {
//         if (num < lo) {
//                 num = lo;
//         } else if (num > hi) {
//                 num = hi;   
//         }
//         return num;
// }

void RGB_to_float(A2_and_Methods arr_and_meth) 
{
    assert(arr_and_meth != NULL);
    assert(arr_and_meth->array != NULL);
    assert(arr_and_meth->methods != NULL);

    //create a UArray2 based on image's pixel
    // printf("RGB_to_float UArray\n");
    A2Methods_UArray2 float_pixels = initializeUArray2(arr_and_meth, 
                                                      sizeof(struct RGB_float), 2);

    //call the mapping function on image's pixel UArray2
    arr_and_meth->methods->map_default(float_pixels, apply_RGB_to_float, arr_and_meth);
    
    //replace the pixel map in image w/ the new UArray2 w/ float pixels 
    free(arr_and_meth->array);
    arr_and_meth->array = float_pixels;
}   

void apply_RGB_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) i;
    (void) j;
    (void) array2;

    A2_and_Methods source = cl;
    
    Pnm_rgb pixel_int = source->methods->at(source->array, i, j);
    RGB_float pixel_float = elem;

    unsigned denominator = source->denominator;

    pixel_float->red = (float) pixel_int->red / (float) denominator;
    pixel_float->green = (float) pixel_int->green / (float) denominator;
    pixel_float->blue = (float) pixel_int->blue / (float) denominator;

}


void RGB_to_int(A2_and_Methods arr_and_meth) 
{
    //create a UArray2 based on image's pixel

    A2Methods_UArray2 int_pixels = initializeUArray2(arr_and_meth, sizeof(struct Pnm_rgb), 2);

    //call the mapping function on image's pixel UArray2
    arr_and_meth->methods->map_default(int_pixels, apply_RGB_to_int, arr_and_meth);
  
    //replace the pixel map in image w/ the new UArray2 w/ float pixels
    arr_and_meth->array = int_pixels;
} 

void apply_RGB_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) i;
    (void) j;
    (void) array2;
    (void) elem;

    A2_and_Methods source = cl;

    RGB_float pixel_float = source->methods->at(source->array, i, j);
    Pnm_rgb pixel_int = elem;

    unsigned denominator = source->denominator;
    
    
    float red = pixel_float->red * denominator;
    float green = pixel_float->green * denominator;
    float blue = pixel_float->blue * denominator;
    printf("color r: %f\n", red);
    printf("color g: %f\n", green);
    printf("color b: %f\n", blue);
    



    pixel_int->red = checkDenom(round(red));
    pixel_int->green = checkDenom(round(green));
    pixel_int->blue = checkDenom(round(blue));

}


void RGB_to_VCS(A2_and_Methods arr_and_meth)
{
    /* Initializing a UArray2 of VCS_data structs */
    // printf("RGB_to_VCS UArray\n");

    A2Methods_UArray2 VCS_array = initializeUArray2(arr_and_meth, sizeof(struct VCS_data), 2);

    arr_and_meth->methods->map_default(VCS_array, apply_RGB_to_VCS, arr_and_meth);
    
    arr_and_meth->array = VCS_array;
}

void apply_RGB_to_VCS(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) array2;

    A2_and_Methods source = cl;

    VCS_data pixel_vcs;
    NEW(pixel_vcs);
    assert(pixel_vcs != NULL);

    RGB_float rgb_float = source->methods->at(source->array, i, j);
    float g = rgb_float->green;
    float r = rgb_float->red;
    float b = rgb_float->blue;

    pixel_vcs->Y = 0.299 * r + 0.587 * g + 0.114 * b; 
    pixel_vcs->Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
    pixel_vcs->Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;


    *(VCS_data) elem = *pixel_vcs;
}


void VCS_to_RGB(A2_and_Methods arr_and_meth)
{

    printf("VCS_to_RGB UArray\n");

    A2Methods_UArray2 RGB_float_arr = initializeUArray2(arr_and_meth, sizeof(struct RGB_float), 2);

    arr_and_meth->methods->map_default(RGB_float_arr, apply_VCS_to_RGB, arr_and_meth);

    arr_and_meth->array = RGB_float_arr;
}


void apply_VCS_to_RGB(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) array2;

    A2_and_Methods source = cl;

    RGB_float pixel_rgb_float;
    NEW(pixel_rgb_float);
    assert(pixel_rgb_float != NULL);

    VCS_data pixel_VCS = source->methods->at(source->array, i, j);
    
    float y = pixel_VCS->Y;
    float pb = pixel_VCS->Pb;
    float pr = pixel_VCS->Pr;

    pixel_rgb_float->red = 1.0 * y + 0.0 * pb + 1.402 * pr;
    pixel_rgb_float->green = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
    pixel_rgb_float->blue = 1.0 * y + 1.772 * pb + 0.0 * pr;

    *(RGB_float) elem = *pixel_rgb_float;

}

static int checkDenom(int val)
{
    if (val > 255){
        return 255; 
    }
    else if( val < 0){
        return 0;
    }
    return val;
}



