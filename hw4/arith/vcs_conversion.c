#include "vcs_conversion.h"
#include "image_manipulation.h"

#include <uarray.h>
#include <mem.h>
#include <assert.h>


// typedef struct VCS_avg {
//     float Y[4];
//     unsigned PB_avg, PR_avg;
// } *VCS_avg;

// typedef struct DCT_data{
//     unsigned a;
//     signed b, c, d;
//     unsigned PB_avg, PR_avg;
// } *DCT_data;

typedef struct VCS_avg_cell {
    float Y;
    float PB_avg, PR_avg;
} *VCS_avg_cell;

static void VCS_average(A2_and_Methods arr_and_meth);

static void apply_populateUArray2(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_VCS_average(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_VCS_to_DCT(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_DCT_to_VCS(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);

static int quantize(float val);
static float reverse_quantize (signed x);
static float mapToRange(float val, float minRange, float maxRange);
void apply_print(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static VCS_avg createAverage(VCS_data *pixeldata);


static A2Methods_UArray2 createUArray2(A2_and_Methods arr_and_meth, int size);

/*
        TO DO

- Create DCT_to_VCS function

- Create a function that bounds values in a specified range

- Free Memory

- Add asserts

*/


void apply_print(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) i;
    (void) j;
    (void) array2;
    (void) cl;

    // VCS_avg averagedPixel = elem;

    //  printf("Y[0]: %f\n", averagedPixel->Y[0]);
    // printf("Y[1]: %f\n", averagedPixel->Y[1]);
    // printf("Y[2]: %f\n", averagedPixel->Y[2]);
    // printf("Y[3]: %f\n", averagedPixel->Y[3]);



    // printf("Pb_avg: %u\n", averagedPixel->PB_avg);
    // printf("Pr_avg: %u\n", averagedPixel->PR_avg);

    // DCT_data data = elem;
    // printf("a: %u\n", data->a);
    // printf("b: %i\n", data->b);
    // printf("c: %i\n", data->c);
    // printf("d: %i\n", data->d);
    // printf("Pb_avg: %u\n", data->PB_avg);
    // printf("Pr_avg: %u\n\n", data->PR_avg);

    // VCS_data data = elem;
    // printf("Y value: %f\n", data->Y);

    // VCS_avg averagedPixel = elem;
    // printf("Y1: %f\n", averagedPixel->Y[0]);
    // printf("Y2: %f\n", averagedPixel->Y[1]);
    // printf("Y3: %f\n", averagedPixel->Y[2]);
    // printf("Y4: %f\n", averagedPixel->Y[3]);
    // printf("Pb_avg: %u\n", averagedPixel->PB_avg);
    // printf("Pr_avg: %u\n\n", averagedPixel->PR_avg);

    VCS_avg_cell data = elem;
    printf("Pixel Y: %f\n", data->Y);
    printf("Pixel PB_avg: %f\n", data->PB_avg);
    printf("Pixel PR_avg: %f\n\n", data->PB_avg);



}


void DCT_to_VCS(A2_and_Methods dct_arr_and_meth)
{
    /* Create a 2 x 2 UArray2b of VCS_avg structs */       
    int width = dct_arr_and_meth->methods->width(dct_arr_and_meth->array);
    int height = dct_arr_and_meth->methods->height(dct_arr_and_meth->array);
    int blocksize = 2;

    width = width * blocksize;
    height = height * blocksize;


    A2Methods_UArray2 vcs_array = dct_arr_and_meth->methods->new_with_blocksize(width, height, sizeof(struct VCS_avg_cell), 2);

    /* Mapping throught dct_arr_and_meth->array and populating vcs_array */
    A2_and_Methods vcs_array_and_meth;
    NEW(vcs_array_and_meth);
    vcs_array_and_meth->array = vcs_array;
    vcs_array_and_meth->methods = dct_arr_and_meth->methods;

    // call on smaller one map to bigger: cl is bigger
    
    dct_arr_and_meth->methods->map_default(dct_arr_and_meth->array, apply_DCT_to_VCS, vcs_array_and_meth);


    /* Replacing dct_arr_and_meth with the vcs_array */
    // FREE(dct_arr_and_meth->array);
    dct_arr_and_meth->array = vcs_array;
    // dct_arr_and_meth->methods->map_default(dct_arr_and_meth->array, apply_print, NULL);

}

/* 
        TO DO
    - Access the left most corner of the UArray
        - access the four cells in the block
        - populate the four cell
*/
void apply_DCT_to_VCS(int i, int j, A2Methods_UArray2 vcs_array, void *elem, void *cl)
{
    (void) vcs_array;

    A2_and_Methods vcs_dest = cl;
    DCT_data blockData = elem;


    int h = vcs_dest->methods->height(vcs_dest->array);
    int w = vcs_dest->methods->width(vcs_dest->array);
    int blocksize = vcs_dest->methods->blocksize(vcs_dest->array);
    int len = blocksize * blocksize;

    /* Mapping from the 1 x 1 blocked UArray2 to the 2 x 2 blocked UArray2 */

    float a = blockData->a / 63.0;
    float b = reverse_quantize(blockData->b);
    float c = reverse_quantize(blockData->c);
    float d = reverse_quantize(blockData->d);

    // checkDenom(blockData->a);
    // checkDenom(blockData->b);
    // checkDenom(blockData->c);
    // checkDenom(blockData->d);


    float Y[4];
    Y[0] = a - b - c + d;
    Y[1] = a - b + c - d;
    Y[2] = a + b - c - d;
    Y[3] = a + b + c + d;

    int i0 = blocksize * i; 
    int j0 = blocksize * j; 
    for (int cell = 0; cell < len; cell++) {
        int outer_i = i0 + cell / blocksize;
        int outer_j = j0 + cell % blocksize;

        // printf("outer: (%i, %i)\n", outer_i, outer_j);
        if (outer_i < w && outer_j < h) {
            VCS_avg_cell pixel = vcs_dest->methods->at(vcs_dest->array,
                                                       outer_i,
                                                       outer_j);
            pixel->Y = Y[cell];
            pixel->PB_avg = Arith40_chroma_of_index(blockData->PB_avg);
            pixel->PR_avg = Arith40_chroma_of_index(blockData->PR_avg);
        }

    }
    // FREE(blockData);
}

void VCS_to_DCT(A2_and_Methods arr_and_meth)
{
    // arr_and_meth->methods->map_default(arr_and_meth->array, apply_print, NULL);
     printf("height of vcs_data: %i\n", arr_and_meth->methods->height(arr_and_meth->array));
     printf("width of vcs_data: %i\n", arr_and_meth->methods->width(arr_and_meth->array));
    //get averages of the VCS_data
    VCS_average(arr_and_meth);
    // arr_and_meth->methods->map_default(arr_and_meth->array, apply_print, NULL);

    // //Create a UArray of DCT_Data structs
    A2Methods_UArray2 dct_array = createUArray2(arr_and_meth, sizeof(struct DCT_data));

    
    //Map through dct_array and populating it with converted values from arr_and_meth->array
    arr_and_meth->methods->map_default(dct_array, apply_VCS_to_DCT, arr_and_meth);
    // arr_and_meth->methods->map_default(dct_array, apply_print, NULL);

    // //Replacing arr_and_meth->array with the dct_array
    // FREE(arr_and_meth->array);
    arr_and_meth->array = dct_array;
    

}

void VCS_average(A2_and_Methods arr_and_meth)
{
    A2Methods_UArray2 vcs_averages = createUArray2(arr_and_meth, sizeof(struct VCS_avg));
    assert(vcs_averages != NULL);

    // arr_and_meth->methods->map_default(arr_and_meth->array, apply_print, NULL);
    arr_and_meth->methods->map_default(vcs_averages, apply_VCS_average, arr_and_meth);
    // arr_and_meth->methods->map_default(vcs_averages, apply_print, NULL);
    
    // FREE(arr_and_meth->array);
    arr_and_meth->array = vcs_averages;
}

void apply_VCS_average(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) array2;
    (void) elem;

    VCS_avg averagedPixel = elem;

    A2_and_Methods arr_and_meth = cl;

    int h = arr_and_meth->methods->height(arr_and_meth->array);
    int w = arr_and_meth->methods->width(arr_and_meth->array);
    int b = arr_and_meth->methods->blocksize(arr_and_meth->array);
    int len = 4;
  
    /* Mapping from the 1 x 1 blocked UArray2 to the 2 x 2 blocked UArray2 */
    int i0 = b * i; 
    int j0 = b * j; 

    VCS_data pixels_in_block[4];
    for (int cell = 0; cell < len; cell++) {
        int outer_i = i0 + cell / b;
        int outer_j = j0 + cell % b;

        if (outer_i < w && outer_j < h) {
            VCS_data vcs_pixel = arr_and_meth->methods->at(arr_and_meth->array, 
                                                           outer_i,
                                                           outer_j);
            pixels_in_block[cell] = vcs_pixel;
        }
    }
    *averagedPixel = *createAverage(pixels_in_block);
}

// takes in a array of pixels and computes the average pixel
//  FREEs the pixels
// Quantizes it
// WORKS !!!!
VCS_avg createAverage(VCS_data *pixeldata)
{
    VCS_avg averagePixel;
    NEW(averagePixel);
    float sumPr = 0;
    float sumPb = 0;
    for (int i = 0; i < 4; i++) {
        sumPr += pixeldata[i]->Pr;
        sumPb += pixeldata[i]->Pb;
        averagePixel->Y[i] = pixeldata[i]->Y;
        //think about
        // FREE(pixeldata);
    }

    averagePixel->PB_avg = Arith40_index_of_chroma(mapToRange(sumPb / 4.0, -0.5, 0.5));

    averagePixel->PR_avg = Arith40_index_of_chroma(mapToRange(sumPr / 4.0, -0.5, 0.5));
    return averagePixel;
}

void apply_VCS_to_DCT(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) array2;

    A2_and_Methods source = cl;
    DCT_data dct_pixel = elem;

    VCS_avg averagedPixel = source->methods->at(source->array, i, j);

    float a_tmp, b_tmp, c_tmp, d_tmp;
    float Y1 = averagedPixel->Y[0];
    float Y2 = averagedPixel->Y[1];
    float Y3 = averagedPixel->Y[2];
    float Y4 = averagedPixel->Y[3];
    
    a_tmp = (Y4 + Y3 + Y2 + Y1) / 4.0;
    b_tmp = (Y4 + Y3 - Y2 - Y1) / 4.0;
    c_tmp = (Y4 - Y3 + Y2 - Y1) / 4.0;
    d_tmp = (Y4 - Y3 - Y2 + Y1) / 4.0;
    // printf("a_tmp: %f\n", a_tmp);
    // printf("b_tmp: %f\n", b_tmp);
    // printf("c_tmp: %f\n", c_tmp);
    // printf("d_tmp: %f\n\n", d_tmp);

    // 15 is the best ppm diff
    /* Convert b, c, d to 5-bit quantized signed values */
    signed b = quantize(b_tmp);
    signed c = quantize(c_tmp);
    signed d = quantize(d_tmp);

    /* Convert a to a 9-bit quantized unsigned value */
    unsigned a = round(a_tmp * 63.0);
    // printf("a: %u\n", a);

    // printf("after quantizing\n");
    // printf("a: %i\n", a);
    // printf("b: %i\n", b);
    // printf("c: %i\n", c);
    // printf("d: %i\n", d);
    // printf("\n");

    /* Set the a, b, c, d, Pb_avg, Pr_avg values inside the struct */
    dct_pixel->a = (unsigned) a;
    dct_pixel->b = b;
    dct_pixel->c = c;
    dct_pixel->d = d;

    dct_pixel->PB_avg = averagedPixel->PB_avg;
    dct_pixel->PR_avg = averagedPixel->PR_avg;
    
    // FREE(averagedPixel);
}

A2Methods_UArray2 createUArray2(A2_and_Methods arr_and_meth, int size)
{
    int blocksize = arr_and_meth->methods->blocksize(arr_and_meth->array);

    int width = arr_and_meth->methods->width(arr_and_meth->array);
    width = (width  + blocksize - 1) / blocksize;

    int height = arr_and_meth->methods->height(arr_and_meth->array);
    height = (height  + blocksize - 1) / blocksize;

    A2Methods_UArray2 VCS_averages = arr_and_meth->methods->new_with_blocksize(width, height, size, 1);
    assert(VCS_averages);

    arr_and_meth->methods->map_default(VCS_averages, apply_populateUArray2, NULL);

    return VCS_averages;
}

void apply_populateUArray2(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) i; (void) j; (void) array2; (void) cl;

    VCS_avg cell = elem;
    NEW(cell);
    assert(cell != NULL);
}

float mapToRange(float val, float minRange, float maxRange)
{
    if (val >= maxRange){
        printf("\ninside max range\n");
        return maxRange;
    }
    else if (val <= minRange){
        printf("\ninside min range\n");

        return minRange;
    }
    // (-range) + (val + range) * (1.0 / (range * 2.0))
    return val;
}

// signed quantize(float val, float minRange, float maxRange, int quantizeVal)
// {
//     float scaledVal = mapToRange(val, minRange, maxRange);
//     // printf("This is the scaled: %f\n", scaledVal);
//     // return round(scaledVal * (((1 << (bit_width - 1)) - 1) / maxRange));
//     // printf("(int) scaledVal * quantizeVal = %i\n", (int)(scaledVal * quantizeVal));
//     printf("(signed) scaledVal * quantizeVal = %d\n\n",(signed) (round(scaledVal * quantizeVal)) );

//     if ((signed) (round(scaledVal * quantizeVal)) > quantizeVal){
         
//     }

//     return round(scaledVal * quantizeVal);
// }

static signed quantize (float x)
{
    float min = -0.3;
    float max = 0.3;


    if (x > max){
        x = max;
    }
    else if (x < min){
        x = min;
    }
    return round ((x * 104) - 0.1);
}

static float reverse_quantize (signed x)
{
        return (x + 0.1) / 104;
}

