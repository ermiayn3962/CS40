/**************************************************************
 *
 *                     vcs_conversion.c
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

#include <uarray.h>
#include <mem.h>
#include <assert.h>

#include "vcs_conversion.h"
#include "image_manipulation.h"

/*   VCS_avg
 *   Description: Represents data from 2x2 grid of image. 
 *                Intermediate step from VCS_avg_cell to DCT_data.
 * 
 *   Y - an array of 4 Y values as floats where Y[0] = (0, 0), Y[1], (1, 0)
 *       Y[0] = (0, 0), Y[1] = (1, 0), Y[2] = (1, 1), where (col, row) of 2x2
 *       block of image.
 *   PB_avg - average PB for 2x2 grid of image (scaled, quantized average)
 *   PR_avg - average PR for 2x2 grid of image (scaled, quantized average)
 */
typedef struct VCS_avg {
        float Y[4];
        unsigned PB_avg, PR_avg;
} *VCS_avg;



/*   VCS_avg_cell
 *   Description: Represents a single pixel of the image
 *
 *   Y - Y value of pixel
 *   PB_avg - average PB for 2x2 grid of image (raw average i.e float)
 *   PR_avg - average PR for 2x2 grid of image (raw average i.e float)
 */
typedef struct VCS_avg_cell {
    float Y;
    float PB_avg, PR_avg;
} *VCS_avg_cell;

/* Intermediate Step */
static void VCS_average(A2_and_Methods arr_and_meth);

/* Applys for each conversion */
static void apply_VCS_average(int i, int j, A2Methods_UArray2 array2, 
                                void *elem, void *cl);
static void apply_VCS_to_DCT(int i, int j, A2Methods_UArray2 array2, 
                                void *elem, void *cl);
static void apply_DCT_to_VCS(int i, int j, A2Methods_UArray2 array2, 
                                void *elem, void *cl);

/* Helper functions */
static void create_average(VCS_data *vcs_data_arr, VCS_avg average_pixel);
static A2Methods_UArray2 create_1x1_UArray2b(A2_and_Methods arr_and_meth,
                                                int size);

/* Quantize functions */
static int quantize(float val);
static float reverse_quantize (signed x);


/* DCT_to_VCS
 * Description: converts DCT_data elements of dct_array to vcs_avg_cell using
 *              inverse DCT
 * 
 * Input: dct_array - UArray2 with DCT_data to convert
 * 
 * Output: none (changes paramteterized A2_and_Methods to have an array with
 *                 vcs_avg_cell data)
 * 
 * Details: maps the blocksize 1 UArray2 of DCT_data structs to a blocksize 2
 *          UArray2 of VCS_avg_cell structs
 * 
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 *
 */
void DCT_to_VCS(A2_and_Methods dct_array)
{
        checkInput(dct_array);

        /* Create a 2 x 2 UArray2b of VCS_avg structs */       
        int width = dct_array->methods->width(dct_array->array);
        int height = dct_array->methods->height(dct_array->array);
        int blocksize = 2;

        width = width * blocksize;
        height = height * blocksize;


        A2Methods_UArray2 vcs_avg_array = dct_array->methods->
                                new_with_blocksize(width, height, 
                                        sizeof(struct VCS_avg_cell), 2);
        assert(vcs_avg_array != NULL);
        /* Mapping throught dct_array->array and populating vcs_avg_array */
        A2_and_Methods vcs_avg_arr_and_meth;
        NEW(vcs_avg_arr_and_meth);
        assert(vcs_avg_arr_and_meth != NULL);

        vcs_avg_arr_and_meth->array = vcs_avg_array;
        vcs_avg_arr_and_meth->methods = dct_array->methods;

        dct_array->methods->map_default(dct_array->array, apply_DCT_to_VCS,
                                        vcs_avg_arr_and_meth);
        
        /* Replacing dct_array with the vcs_avg_array */
        dct_array->methods->free(&dct_array->array);
        FREE(vcs_avg_arr_and_meth);
        dct_array->array = vcs_avg_array;
}


/* VCS_to_DCT
 * Description: converts VCS_data to DCT_data for each VCS_data in 
 *              vcs_pixel_array by calling VCS_average to first convert 
 *              VCS_data to VCS_avg.
 * 
 * Input: vcs_pixel_array - array with VCS_data elements
 * 
 * Output: none (changes vcs_pixel_array)
 * 
 * Details: vcs_pixel array goes from dimensions height * width to height / 2
 *          * width / 2. The data inside vcs_pixel_array goes from VCS_data 
 *          structs to DCT_data structs.
 * 
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 */
void VCS_to_DCT(A2_and_Methods vcs_pixel_array)
{
        checkInput(vcs_pixel_array);

        /* intermediate step to get averages */
        VCS_average(vcs_pixel_array);

        /* Create a UArray of DCT_Data structs */
        A2Methods_UArray2 dct_array = create_1x1_UArray2b(vcs_pixel_array,
                                                sizeof(struct DCT_data));
        assert(dct_array != NULL);

        vcs_pixel_array->methods->map_default(dct_array, apply_VCS_to_DCT, 
                                                vcs_pixel_array);
                        
        vcs_pixel_array->methods->free(&vcs_pixel_array->array);
        vcs_pixel_array->array = dct_array;
}


/* VCS_average
 * Description: converts VCS_data to VCS_avg for each VCS_data in 
 *              vcs_pixel_array
 * 
 * Input: vcs_pixel_array - array with VCS_data elements
 * 
 * Output: none -- changes vcs_pixel_array
 * 
 * Details: vcs_pixel array goes from dimenstions height * width to height / 2
 *          * width / 2. The data inside vcs_pixel_array goes from VCS_data 
 *          structs to VCS_avg structs.
 * 
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 */
void VCS_average(A2_and_Methods vcs_pixel_array)
{
        checkInput(vcs_pixel_array);

        A2Methods_UArray2 vcs_averages = create_1x1_UArray2b(vcs_pixel_array,
                                                sizeof(struct VCS_avg));
        assert(vcs_averages != NULL);

        vcs_pixel_array->methods->map_default(vcs_averages,
                                                apply_VCS_average,
                                                vcs_pixel_array);
        vcs_pixel_array->methods->free(&vcs_pixel_array->array);
        vcs_pixel_array->array = vcs_averages;
}


/* apply_VCS_to_DCT
 * Description: converts VCS_avg in UArray2 passed in through the closure 
                argument to DCT_data structs in the UArray2 this function is 
                called on.

 * Inputs: col - column index for elem in UArray2 at current iteration of apply
 *         row - row index for elem in UArray2 at current iteration of apply
 *         vcs_array - unused (for matching apply function parameters)
 *         elem - element of array2 at current iteration of apply
 *         cl - closure argument, uarray2 with VCS_avg pixel data.
 * 
 * Outputs: None -- updates source uarray2 with DCT data.
 * 
 * Details: Converts using discrete cosign transformation to represent 4 Y 
 *          values as values: a, b, c, d
 * 
 * Exceptions: None (excluding Hanson's)
 */
void apply_DCT_to_VCS(int col, int row, A2Methods_UArray2 vcs_array, 
                      void *elem, void *cl)
{
        (void) vcs_array;

        A2_and_Methods vcs_dest = cl;
        DCT_data blockData = elem;

        int h = vcs_dest->methods->height(vcs_dest->array);
        int w = vcs_dest->methods->width(vcs_dest->array);
        int blocksize = vcs_dest->methods->blocksize(vcs_dest->array);
        int len = blocksize * blocksize;

        /* Mapping the 1 x 1 blocked UArray2 to the 2 x 2 blocked UArray2 */
        float a = blockData->a / 511.0;
        float b = reverse_quantize(blockData->b);
        float c = reverse_quantize(blockData->c);
        float d = reverse_quantize(blockData->d);

        float Y[4];
        Y[0] = a - b - c + d;
        Y[1] = a - b + c - d;
        Y[2] = a + b - c - d;
        Y[3] = a + b + c + d;

        int i0 = blocksize * col; 
        int j0 = blocksize * row; 
        for (int cell = 0; cell < len; cell++) {
                int outer_i = i0 + cell / blocksize;
                int outer_j = j0 + cell % blocksize;

                if (outer_i < w && outer_j < h) {
                VCS_avg_cell pixel = vcs_dest->methods->at(vcs_dest->array,
                                                        outer_i,
                                                        outer_j);
                pixel->Y = Y[cell];
                pixel->PB_avg = Arith40_chroma_of_index(blockData->PB_avg);
                pixel->PR_avg = Arith40_chroma_of_index(blockData->PR_avg);
                }
        }
}

/* apply_VCS_to_DCT
 * Description: converts VCS_avg in UArray2 passed in through the closure 
                argument to DCT_data structs in the UArray2 this function is 
                called on.
                
 * Inputs: col - column index for elem in UArray2 at current iteration of apply
 *         row - row index for elem in UArray2 at current iteration of apply
 *         array2 - unused (for matching apply function parameters)
 *         elem - element of array2 at current iteration of apply
 *         cl - closure argument, uarray2 with VCS_avg pixel data.
 * 
 * Outputs: None -- updates source uarray2 with DCT data.
 * 
 * Details: Converts using discrete cosign transformation to represent 4 Y 
 *          values as values: a, b, c, d
 * 
 * Exceptions: None (excluding Hanson's)
 */
void apply_VCS_to_DCT(int col, int row, A2Methods_UArray2 array2, void *elem,
                      void *cl)
{
        (void) array2;

        A2_and_Methods source = cl;
        DCT_data dct_pixel = elem;

        VCS_avg averagedPixel = source->methods->at(source->array, col, row);

        float Y1 = averagedPixel->Y[0];
        float Y2 = averagedPixel->Y[1];
        float Y3 = averagedPixel->Y[2];
        float Y4 = averagedPixel->Y[3];
        
        float a_tmp, b_tmp, c_tmp, d_tmp;
        a_tmp = (Y4 + Y3 + Y2 + Y1) / 4.0;
        b_tmp = (Y4 + Y3 - Y2 - Y1) / 4.0;
        c_tmp = (Y4 - Y3 + Y2 - Y1) / 4.0;
        d_tmp = (Y4 - Y3 - Y2 + Y1) / 4.0;

        /* Convert a to a 9-bit quantized unsigned value */
        unsigned a = round(a_tmp * 511);

        /* Convert b, c, d to 5-bit quantized signed values */
        signed b = quantize(b_tmp);
        signed c = quantize(c_tmp);
        signed d = quantize(d_tmp);

        /* Set the a, b, c, d, Pb_avg, Pr_avg values inside the struct */
        dct_pixel->a = a;
        dct_pixel->b = b;
        dct_pixel->c = c;
        dct_pixel->d = d;
        dct_pixel->PB_avg = averagedPixel->PB_avg;
        dct_pixel->PR_avg = averagedPixel->PR_avg;
}


/* apply_VCS_average
 * Description: converts VCS_data in UArray2 passed in through the closure 
 *              to VCS_avg structs in the UArray2 this function is 
 *              called on.
 *
 * Inputs: col: column index for elem in UArray2 at current iteration of apply
 *         row: row index for elem in UArray2 at current iteration of apply
 *         array2: unused (for matching apply function parameters)
 *         elem: element of array2 at current iteration of apply
 *         cl: closure argument, uarray2 with VCS_data pixel data.
 * 
 * Outputs: None -- updates source uarray2 with DCT data.
 * 
 * Details: Calculates average Pb and Pr values for each pixel in a 2x2 block 
 *          in image. Also stores all 4 Y values for future converstion using
 *          DCT step.
 * 
 * Exceptions: None (excluding Hanson's)
 */
void apply_VCS_average(int col, int row, A2Methods_UArray2 array2, void *elem,
                       void *cl)
{
        (void) array2;

        VCS_avg averagedPixel = elem;

        A2_and_Methods vcs_array = cl;

        int h = vcs_array->methods->height(vcs_array->array);
        int w = vcs_array->methods->width(vcs_array->array);
        int b = vcs_array->methods->blocksize(vcs_array->array);
        int len = 4;
        
        /* Mapping from the 1x1 blocked UArray2 to the 2x2 blocked UArray2 */
        int i0 = b * col; 
        int j0 = b * row; 

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
        create_average(pixels_in_block, averagedPixel);
}

/* create_average
 * Description: converts 4 VCS_data structs to an VCS_avg struct
 *
 * Input: vcs_data_arr - array of VCS_data representing individual pixels
 *        average_pixel - VCS_avg struct representing average PR and PB of 4 
 *                        pixels passed in and the Y values of them
 * 
 * Output: none -- updates average_pixel field to store average pixel
 * 
 * Details: calculates the average Pb and Pr and quantizes it using
 *          Arith40_index_of_chroma()
 * Exceptions: C.R.E when input is NULL
 */
void create_average(VCS_data *vcs_data_arr, VCS_avg average_pixel)
{
        assert(vcs_data_arr != NULL);
        assert(average_pixel != NULL);
        float sumPr = 0;
        float sumPb = 0;
        for (int i = 0; i < 4; i++) {
                average_pixel->Y[i] = vcs_data_arr[i]->Y;
                sumPr += vcs_data_arr[i]->Pr;
                sumPb += vcs_data_arr[i]->Pb;
        }
        average_pixel->PB_avg = Arith40_index_of_chroma(sumPb / 4.0);
        average_pixel->PR_avg = Arith40_index_of_chroma(sumPr / 4.0);
}

/* create_1x1_UArray2b
 * Description: creates a 1 by 1 blocked array based on dimensions of source 
 *              array provided, scaling down the source array by its blocksize.
 * 
 * Input: source - array to scale down to 1 by 1
 *        size - size of elements to store in scaled down array
 * 
 * Output: scaled UArray2 with blocksize 1
 * 
 * Details: reduces the width and height and creates a UArray2
 * 
 * Exceptions: C.R.E if new array could not be alloc'ed
 */
A2Methods_UArray2 create_1x1_UArray2b(A2_and_Methods source, int size)
{
        int blocksize = source->methods->blocksize(source->array);

        int width = source->methods->width(source->array);
        width = (width + blocksize - 1) / blocksize;

        int height = source->methods->height(source->array);
        height = (height + blocksize - 1) / blocksize;

        A2Methods_UArray2 new_array = source->methods->new_with_blocksize(
                                                                        width,
                                                                        height,
                                                                        size,
                                                                        1);
        assert(new_array != NULL);
        return new_array;
}

/* quantize
 * Description: Takes in a float and bounds its range to [-0.3, 0.3]
 *              then returns a 5 bit signed integer representing the float 
 * 
 * Input: a float
 * 
 * Details: bounds float to range to [-0.3, 0.3] then returns a 5 bit signed
 *          integer representing the float by multiplying by 50.
 * 
 * Output: an int ranging from [-15, 15]
 * 
 * Exception: None
 */
static signed quantize (float x)
{
        float min = -0.3;
        float max = 0.3;

        if (x > max) return 15;
        else if (x < min) return -15;

        return round(x * 50);
}

/* reverse_quantize
 * Description: bounds integer to [-15, 15] and then returns a float 
 *              corresponding to a value in this range from [-0.3, 0.3]
 * 
 * Input: an int
 * 
 * Output: a float ranging from [-0.3, 0.3]
 * 
 * Exceptions: None
 */
static float reverse_quantize (signed x)
{
        signed min = -15;
        signed max = 15;

        if (x > max) return 0.3;
        else if (x < min) return -0.3;

        return x / 50.0;
}