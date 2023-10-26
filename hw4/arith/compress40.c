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

//testing functions
void apply_printValues(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);



// /* MAIN USED FOR TESTING PURPOSES */
// int main() 
// {
    

    

//     // free(testArr);

// }


void compress40(FILE *input)
{
    Pnm_ppm testImg = openImage(input);
    assert(testImg != NULL);

    /* Populating A2_and_Methods struct */
    A2_and_Methods testArr;
    NEW(testArr);

    testArr->array = testImg->pixels;
    testArr->methods = (void *) testImg->methods;
    testArr->denominator = testImg->denominator;

    
    /* Testing the DCT conversions */
    RGB_to_float(testArr);
    int width = testArr->methods->width(testArr->array);
    int height = testArr->methods->height(testArr->array);


    RGB_to_VCS(testArr);
    VCS_to_DCT(testArr);
    encodeImage(testArr, width, height);
}



void decompress40(FILE *input) 
{
    A2_and_Methods testArr;
    NEW(testArr);

    testArr->methods = uarray2_methods_blocked;
    testArr->array = decodeImage(input);
    // testArr->methods->map_default(testArr->array, apply_printValues, NULL);

    DCT_to_VCS(testArr);
    // testArr->methods->map_default(testArr->array, apply_printValues, NULL);



    VCS_to_RGB(testArr);
    RGB_to_int(testArr);

    Pnm_ppm testImg;
    NEW(testImg);
    testImg->pixels = testArr->array;
    testImg->height = testArr->methods->height(testArr->array);
    testImg->width = testArr->methods->width(testArr->array);
    testImg->denominator = 255;
    testImg->methods = testArr->methods;


    FILE *output = fopen("DCT_conversion_final.ppm", "w");
    Pnm_ppmwrite(output, testImg);
    fclose(output);
}








void apply_printValues(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{  
    (void) cl;
    (void) i;
    (void) j;
    (void) array2;
    // (void) elem;

    // RGB_float pixel = elem;
    // printf("red: %f\n", pixel->red);
    // printf("green: %f\n", pixel->green);
    // printf("blue: %f\n", pixel->blue);
    // printf("\n");

    // DCT_data data = elem;
    // fprintf(stderr, "a: %u\n", data->a);
    // fprintf(stderr, "b: %i\n", data->b);
    // fprintf(stderr, "c: %i\n", data->c);
    // fprintf(stderr, "d: %i\n", data->d);
    // fprintf(stderr, "Pb_avg: %u\n", data->PB_avg);
    // fprintf(stderr, "Pr_avg: %u\n\n", data->PR_avg);

    VCS_avg averagedPixel = elem;
    fprintf(stderr, "Y1: %f\n", averagedPixel->Y[0]);
    fprintf(stderr, "Y2: %f\n", averagedPixel->Y[1]);
    fprintf(stderr, "Y3: %f\n", averagedPixel->Y[2]);
    fprintf(stderr, "Y4: %f\n", averagedPixel->Y[3]);
    fprintf(stderr, "Pb_avg: %u\n", averagedPixel->PB_avg);
    fprintf(stderr, "Pr_avg: %u\n\n", averagedPixel->PR_avg);

    // printf("(%i, %i)\n", j, i);

}