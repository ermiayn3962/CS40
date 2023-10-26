/**************************************************************
 *
 *                     compress40.c
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (cgolem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions to compress and decompress an image
 *
 *
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <compress40.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>
#include <mem.h>

#include "rgb_conversion.h"
#include "image_manipulation.h"
#include "vcs_conversion.h"
#include "codeword.h"

/* compress40
 * Description: compresses an image passed 
 * Input: input - ppm file to compress
 * Output: writes plain text compression to stdout
 * Details: calls each step of compression RGB_to_int, VCS_to_RGB, DCT_to_VCS,
 *          encodeImage to compress ppm passed in to a plain text output that
 *          represents the image in less bytes.
 * Assertions: C.R.E. if image opening fails
 *             All exceptions from submodules in DCT_to_VCS, VCS_to_RGB, 
 *             RGB_to_int, and decodeImage
 */
void compress40(FILE *input)
{
        Pnm_ppm testImg = openImage(input);
        assert(testImg != NULL);

        A2_and_Methods arr_and_methods;
        NEW(arr_and_methods);
        assert(arr_and_methods != NULL);

        arr_and_methods->array = testImg->pixels;
        arr_and_methods->methods = (void *) testImg->methods;
        int width = arr_and_methods->methods->width(arr_and_methods->array);
        int height = arr_and_methods->methods->height(arr_and_methods->array);

        RGB_to_float(arr_and_methods);
        RGB_to_VCS(arr_and_methods);
        VCS_to_DCT(arr_and_methods);
        encodeImage(arr_and_methods, width, height);

        arr_and_methods->methods->free(&arr_and_methods->array);
        FREE(arr_and_methods);
        Pnm_ppmfree(&testImg);
}

/* decompress40
 * Description: Takes in a file input and runs all decompression steps. Outputs
 *              a decompressed ppm image.
 * Input: input - file to decompress
 * Output: outputs ppm image to stdout
 * Details: calls each step of decompression DCT_to_VCS, VCS_to_RGB, RGB_to_int
 *          and creates a new Pnm_ppm and writes this to stdout
 * Exceptions: Hanson C.R.E. if any memory allocation fails
 *             All exceptions from submodules in DCT_to_VCS, VCS_to_RGB, 
 *             RGB_to_int, and decodeImage
 */
void decompress40(FILE *input) 
{
        A2_and_Methods arr_and_methods;
        NEW(arr_and_methods);
        assert(arr_and_methods != NULL);

        arr_and_methods->methods = uarray2_methods_blocked;

        arr_and_methods->array = decodeImage(input);
        DCT_to_VCS(arr_and_methods);
        VCS_to_RGB(arr_and_methods);
        RGB_to_int(arr_and_methods);

        Pnm_ppm image;
        NEW(image);
        assert(image);
        
        image->pixels = arr_and_methods->array;
        image->methods = arr_and_methods->methods;
        image->height = arr_and_methods->methods->height(
                                                arr_and_methods->array);
        image->width = arr_and_methods->methods->width(
                                                arr_and_methods->array);
        image->denominator = 255;
        
        Pnm_ppmwrite(stdout, image);

        Pnm_ppmfree(&image);
        FREE(arr_and_methods);
}