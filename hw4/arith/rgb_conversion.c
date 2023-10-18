#include "rgb_conversion.h"

#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

static A2Methods_UArray2 initializeUArray2(Pnm_ppm image);
static void apply_RGB_to_VCS(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_RGB_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_RGB_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);


void RGB_to_float(Pnm_ppm image) 
{
    //create a UArray2 based on image's pixel
    A2Methods_UArray2 float_pixels = initializeUArray2(image);

    //call the mapping function on image's pixel UArray2
    image->methods->map_default(float_pixels, apply_RGB_to_float, image);
    
    //replace the pixel map in image w/ the new UArray2 w/ float pixels 
    image->pixels = float_pixels;
}   

void RGB_to_int(Pnm_ppm image) 
{
    //create a UArray2 based on image's pixel
    A2Methods_UArray2 int_pixels = initializeUArray2(image);

    //call the mapping function on image's pixel UArray2
    image->methods->map_default(int_pixels, apply_RGB_to_int, image);
    printf("this is the pixel at (0,0) \n");
    Pnm_rgb pixel = image->methods->at(int_pixels, 0, 0);
    printf("red: %i\n", pixel->red);
    printf("green: %i\n", pixel->green);

    printf("blue: %i\n", pixel->blue);



    //replace the pixel map in image w/ the new UArray2 w/ float pixels
    image->pixels = int_pixels;
    // image->methods->map_default(int_pixels, apply_printValues, NULL);
} 
void RGB_to_VCS(A2Methods_UArray2 array, A2Methods_T methods){
    assert(array);
    int height = methods->height(array);
    int width = methods->width(array);
    int size = sizeof(VCS_data);


    A2Methods_UArray2 dest_A2 = methods->new(width, height, size);

    A2_and_Methods dest;
    NEW(dest);
    dest->methods = methods;
    dest->array = dest_A2;

    methods->map_default(array, apply_RGB_to_VCS, dest);
}
void VCS_to_RGB(A2Methods_UArray2 array, A2Methods_T methods){
    assert(array);
    assert(methods);

    // methods->map_default(array, apply_VCS_to_RGB, dest);
}

void apply_RGB_to_VCS(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) array2;
    Pnm_rgb_float rgb_float = elem;

    VCS_data vcs_pixel_source;
    NEW(vcs_pixel_source);

    float g = rgb_float->green;
    float r = rgb_float->red;
    float b = rgb_float->blue;

    vcs_pixel_source->Y = 0.299 * r + 0.587 * g + 0.114 * b; 
    vcs_pixel_source->Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
    vcs_pixel_source->Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;

    A2_and_Methods dest = cl;
    VCS_data vcs_pixel_dest = dest->methods->at(dest->array, i, j);
    
    *vcs_pixel_dest = *vcs_pixel_source;
}

A2Methods_UArray2 initializeUArray2(Pnm_ppm image)
{

    int width = image->width;
    int height = image->height;
    int size = image->methods->size(image->pixels); //MIGHT HAVE TO USE SIZE_OF HERE

    A2Methods_UArray2 float_pixels = image->methods->new(width, height, size);
    assert(float_pixels != NULL);

    return float_pixels;
}

void apply_RGB_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) i;
    (void) j;
    (void) array2;

    Pnm_ppm source = cl;
    A2Methods_T methods = (void *) source->methods;
    Pnm_rgb_float pixel_float = elem;
    unsigned denominator = source->denominator;
    Pnm_rgb pixel_int = methods->at(source->pixels, i, j);

    pixel_float->red = (float) pixel_int->red / (float) denominator;
    pixel_float->green = (float) pixel_int->green / (float) denominator;
    pixel_float->blue = (float) pixel_int->blue / (float) denominator;

}

void apply_RGB_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) i;
    (void) j;
    (void) array2;

    Pnm_ppm source = cl;
    A2Methods_T methods = (void *) source->methods;
    Pnm_rgb pixel_int;
    NEW(pixel_int); //Don't know if we need this
    assert(pixel_int != NULL);
    unsigned denominator = source->denominator;
    Pnm_rgb_float pixel_float = methods->at(source->pixels, i, j);

    printf("This is the denom: %u\n", denominator);
    

    float temp = denominator * pixel_float->red;
    printf("deno * red: %i\n", (int) temp);
    float red = pixel_float->red * denominator;
    float green = pixel_float->green * denominator;
    float blue = pixel_float->blue * denominator;

    pixel_int->red = (int) red;
    pixel_int->green = (int) green;
    pixel_int->blue = (int) blue;

    *(Pnm_rgb) elem = *(Pnm_rgb) pixel_int;
    printf("This is the red: %i\n", ((Pnm_rgb) elem)->red);
    printf("This is the green: %i\n", ((Pnm_rgb) elem)->green);
    printf("This is the blue: %i\n", ((Pnm_rgb) elem)->blue);
    printf("\n");

}