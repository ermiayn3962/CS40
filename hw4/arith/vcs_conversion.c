#include "vcs_conversion.h"
#include "image_manipulation.h"

#include <uarray.h>
#include <mem.h>
#include <assert.h>

/*  VCS_avg
*   Represents data from 2x2 grid of image. Intermediate step from VCS_avg_cell
*   to DCT_data.
*   Y - an array of 4 Y values as floats where Y[0] = (0, 0), Y[1], (1, 0)
*       Y[0] = (0, 0), Y[1] = (1, 0), Y[2] = (1, 1), where (col, row) of 2x2
*       block of image.
*   PB_avg - average PB for 2x2 grid of image (scaled, quantized average)
*   PR_avg - average PR for 2x2 grid of image (scaled, quantized average)
*/
// typedef struct VCS_avg {
//     float Y[4];
//     unsigned PB_avg, PR_avg;
// } *VCS_avg;

/*  DCT_data
*   Represents data from 2x2 grid of image. Final data representation before
*   encoding.
*   a - A value from DCT conversion, scaled to 9 bit signed int
*   b - B value from DCT conversion, scaled to 5 bit unsigned it
*   c - C value from DCT conversion, scaled to 5 bit unsigned it
*   d - D value from DCT conversion, scaled to 5 bit unsigned it
*   PB_avg - average PB, scaled to a 4 bit unsigned int
*   PR_avg - average PR, scaled to a 4 bit unsigned int
*/
// typedef struct DCT_data{
//     unsigned a;
//     signed b, c, d;
//     unsigned PB_avg, PR_avg;
// } *DCT_data;

/*  VCS_avg_cell
*   Represents a single pixel of the image
*   Y - Y value of pixel
*   PB_avg - average PB for 2x2 grid of image (raw average i.e float)
*   PR_avg - average PR for 2x2 grid of image (raw average i.e float)
*/
typedef struct VCS_avg_cell {
    float Y;
    float PB_avg, PR_avg;
} *VCS_avg_cell;

static void VCS_average(A2_and_Methods arr_and_meth);

// static void apply_populateUArray2(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_VCS_average(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_VCS_to_DCT(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static void apply_DCT_to_VCS(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);

static int quantize(float val);
static float reverse_quantize (signed x);
// static float mapToRange(float val, float minRange, float maxRange);
void apply_print(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
static VCS_avg create_average(VCS_data *pixeldata);


static A2Methods_UArray2 createUArray2(A2_and_Methods arr_and_meth, int size);

/*
        TO DO

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
            // fprintf(stderr, "PB_avg: %u\n", blockData->PB_avg);
            // fprintf(stderr, "PR_avg: %u\n", blockData->PR_avg);


            if (blockData->PB_avg > 15){
                blockData->PB_avg = 15;
            }
            if (blockData->PR_avg > 15){
                blockData->PR_avg = 15;
            }
          
            pixel->PB_avg = Arith40_chroma_of_index(blockData->PB_avg);
            pixel->PR_avg = Arith40_chroma_of_index(blockData->PR_avg);
        }

    }
    // FREE(blockData);
}

/* VCS_average
 * Description: converts VCS_data to DCT_data for each VCS_data in 
 *              vcs_pixel_array by calling VCS_average to first convert VCS_data
 *              to VCS_avg.
 * Input: vcs_pixel_array - array with VCS_data elements
 * Output: none -- changes vcs_pixel_array
 * Changes: vcs_pixel array goes from dimenstions height * width to height / 2 
 *          * width / 2. The data inside vcs_pixel_array goes from VCS_data 
 *          structs to DCT_data structs.
 */
void VCS_to_DCT(A2_and_Methods vcs_pixel_array)
{
    // vcs_pixel_array->methods->map_default(vcs_pixel_array->array, apply_print, NULL);
    //  printf("height of vcs_data: %i\n", vcs_pixel_array->methods->height(vcs_pixel_array->array));
    //  printf("width of vcs_data: %i\n", vcs_pixel_array->methods->width(vcs_pixel_array->array));
    //get averages of the VCS_data
    VCS_average(vcs_pixel_array);
    // vcs_pixel_array->methods->map_default(vcs_pixel_array->array, apply_print, NULL);

    // //Create a UArray of DCT_Data structs
    A2Methods_UArray2 dct_array = createUArray2(vcs_pixel_array, sizeof(struct DCT_data));

    
    //Map through dct_array and populating it with converted values from vcs_pixel_array->array
    vcs_pixel_array->methods->map_default(dct_array, apply_VCS_to_DCT, vcs_pixel_array);
    // vcs_pixel_array->methods->map_default(dct_array, apply_print, NULL);

    // //Replacing vcs_pixel_array->array with the dct_array
    // FREE(vcs_pixel_array->array);
    vcs_pixel_array->array = dct_array;
    

}

/* VCS_average
 * Description: converts VCS_data to VCS_avg for each VCS_data in 
 *              vcs_pixel_array
 * Input: vcs_pixel_array - array with VCS_data elements
 * Output: none -- changes vcs_pixel_array
 * Changes: vcs_pixel array goes from dimenstions height * width to height / 2 
 *          * width / 2. The data inside vcs_pixel_array goes from VCS_data 
 *          structs to VCS_avg structs.
 */
void VCS_average(A2_and_Methods vcs_pixel_array)
{
    A2Methods_UArray2 vcs_averages = createUArray2(vcs_pixel_array,
                                                   sizeof(struct VCS_avg));
    vcs_pixel_array->methods->map_default(vcs_averages,
                                          apply_VCS_average,
                                          vcs_pixel_array);
    vcs_pixel_array->array = vcs_averages;
}

/* apply_VCS_average
 * Description: converts VCS_data in UArray2 passed in through the closure 
                to VCS_avg structs in the UArray2 this function is 
                called on.
 * Inputs: i: column index for elem in UArray2 at current iteration of apply
 *         j: row index for elem in UArray2 at current iteration of apply
 *         uarray2: unused (for matching apply function parameters)
 *         elem: element of array2 at current iteration of apply
 *         cl: closure argument, uarray2 with VCS_data pixel data.
 * Outputs: None -- updates source uarray2 with DCT data.
 * Notes: Calculates average Pb and Pr values for each pixel in a 2x2 block in 
 *        image. Also stores all 4 Y values for future converstion using DCT 
 *        step.
 */
void apply_VCS_average(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
    (void) array2;

    VCS_avg averagedPixel = elem;

    A2_and_Methods vcs_array = cl;

    int h = vcs_array->methods->height(vcs_array->array);
    int w = vcs_array->methods->width(vcs_array->array);
    int b = vcs_array->methods->blocksize(vcs_array->array);
    int len = 4;
  
    /* Mapping from the 1 x 1 blocked UArray2 to the 2 x 2 blocked UArray2 */
    int i0 = b * i; 
    int j0 = b * j; 

    VCS_data pixels_in_block[4];
    for (int cell = 0; cell < len; cell++) {
        int outer_i = i0 + cell / b;
        int outer_j = j0 + cell % b;

        if (outer_i < w && outer_j < h) {
            VCS_data vcs_pixel = vcs_array->methods->at(vcs_array->array, 
                                                           outer_i,
                                                           outer_j);
            pixels_in_block[cell] = vcs_pixel;
        }
    }
    *averagedPixel = *create_average(pixels_in_block);
}

/* create_average
 *
 */
VCS_avg create_average(VCS_data *pixeldata)
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

    averagePixel->PB_avg = Arith40_index_of_chroma(sumPb / 4.0);
    //mapToRange(sumPb / 4.0, -0.5, 0.5)

    averagePixel->PR_avg = Arith40_index_of_chroma(sumPr / 4.0);
    return averagePixel;
}

/* apply_VCS_to_DCT
 * Description: converts VCS_avg in UArray2 passed in through the closure 
                argument to DCT_data structs in the UArray2 this function is 
                called on.
 * Inputs: i: column index for elem in UArray2 at current iteration of apply
 *         j: row index for elem in UArray2 at current iteration of apply
 *         uarray2: unused (for matching apply function parameters)
 *         elem: element of array2 at current iteration of apply
 *         cl: closure argument, uarray2 with VCS_avg pixel data.
 * Outputs: None -- updates source uarray2 with DCT data.
 * Notes: Converts using discrete cosign transformation to represent 4 Y values 
 *        as values: a, b, c, d
 */
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
    // if (averagedPixel->PB_avg > 15 || averagedPixel->PR_avg > 15){

    // fprintf(stderr, "PB_Avg in encode: %u\n", averagedPixel->PB_avg);
    // fprintf(stderr, "PR_Avg in encode: %u\n", averagedPixel->PR_avg);
    // }


    dct_pixel->PB_avg = averagedPixel->PB_avg;
    dct_pixel->PR_avg = averagedPixel->PR_avg;
    
    // FREE(averagedPixel);
}

A2Methods_UArray2 createUArray2(A2_and_Methods source, int size)
{
    int blocksize = source->methods->blocksize(source->array);

    int width = source->methods->width(source->array);
    width = (width  + blocksize - 1) / blocksize;

    int height = source->methods->height(source->array);
    height = (height  + blocksize - 1) / blocksize;

    A2Methods_UArray2 new_array = source->methods->new_with_blocksize(width, 
                                                                      height,
                                                                      size, 
                                                                      1);
    assert(new_array != NULL);


    // source->methods->map_default(new_array, apply_populateUArray2, NULL);

    return new_array;
}
// /* apply_populateUArray2
//  *
//  *
//  */
// void apply_populateUArray2(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
// {
//     (void) i; (void) j; (void) array2; (void) cl;

//     VCS_avg cell = elem;
//     NEW(cell);
//     assert(cell != NULL);
// }

/* mapToRange
 * Description: bounds value to being between minRange and maxRange by rounding 
 *              values greater than the maxRange down to maxRange and values 
 *              less than the minRange down to the minRange. For floats
 * Inputs: val - value to bound
 *         minRange - minimum value that value can be
 *         maxRange - maximum value that value can be
 * Outputs: bounded floatvalue between [minRange, maxRange]
 */
float mapToRange(float val, float minRange, float maxRange)
{
    if (val >= maxRange){
        return maxRange;
    }
    else if (val <= minRange){
        return minRange;
    }
    return val;
}

/* quantize
 * Description: Takes in a float and bounds its range to [-0.3, 0.3]
 *              then returns a 5 bit signed integer representing the float 
 * Input: a float
 * Output: an int ranging from [-15, 15]
 */
static signed quantize (float x)
{
    float min = -0.3;
    float max = 0.3;


    if (x > max) {
       return 15;
    }
    else if (x < min) {
       return -15;
    }
    return round ((x * 50));
}

/* reverse_quantize
 * Description: bounds integer to [-15, 15] and then returns a float 
 *              corresponding to a value in this range from [-0.3, 0.3]
 * Input: an int
 * Output: a float ranging from [-0.3, 0.3]
 */
static float reverse_quantize (signed x)
{
        signed min = -15;
        signed max = 15;


        if (x > max) {
            return 0.3;
        }
        else if (x < min) {
            return -0.3;
        }
        return (x) / 50.0;
}

