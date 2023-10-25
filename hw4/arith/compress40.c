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



/* MAIN USED FOR TESTING PURPOSES */
int main() 
{
    FILE *fp = fopen("flowers.ppm", "r");
    assert(fp != NULL);

    /* Testing the image opening function */
    Pnm_ppm testImg = openImage(fp);
    int width = testImg->width;
    int height = testImg->height;

    assert(testImg != NULL);
    printf("image opening works!\n"); 

    /* Populating A2_and_Methods struct */
    A2_and_Methods testArr;
    NEW(testArr);

    testArr->array = testImg->pixels;
    testArr->methods = (void *) testImg->methods;
    testArr->denominator = testImg->denominator;

    
    /* Testing the DCT conversions */
    RGB_to_float(testArr);

    RGB_to_VCS(testArr);

    VCS_to_DCT(testArr);

    encodeImage(testArr, width, height);

    DCT_to_VCS(testArr);

    VCS_to_RGB(testArr);

    RGB_to_int(testArr);
    testImg->pixels = testArr->array;
    testImg->height = testArr->methods->height(testArr->array);
    testImg->width = testArr->methods->width(testArr->array);

    FILE *output = fopen("DCT_conversion_w-o_negs2.ppm", "w");
    Pnm_ppmwrite(output, testImg); //ppmdiff output of 0.103126
    fclose(output);

    free(testArr);

}


void compress40(FILE *input)
{

    /* Opening a PNM Image */
    Pnm_ppm image = openImage(input);
    assert(image != NULL);

    /* Create a UArray to Manipulate */

    /* Populate */
    // trimImage(image);
    // array2 will have AV_VCS and will be Uarray2
    // AnM will have VCS_DATA and will be blocked
    // VCS_Average(AnM,  array2)
    // DCT(AnM,  array2)



    printf("end of compress40\n");
}



void decompress40(FILE *input) 
{
    (void) input;
}








// void apply_printValues(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
// {  
//     (void) cl;
//     (void) i;
//     (void) j;
//     (void) array2;
//     // (void) elem;

//     // RGB_float pixel = elem;
//     // printf("red: %f\n", pixel->red);
//     // printf("green: %f\n", pixel->green);
//     // printf("blue: %f\n", pixel->blue);
//     // printf("\n");

//     // DCT_data data = elem;
//     // printf("a: %u\n", data->a);
//     // printf("b: %i\n", data->b);
//     // printf("c: %i\n", data->c);
//     // printf("d: %i\n", data->d);
//     // printf("Pb_avg: %u\n", data->PB_avg);
//     // printf("Pr_avg: %u\n\n", data->PR_avg);

//     VCS_avg averagedPixel = elem;
//     printf("Y1: %f\n", averagedPixel->Y[0]);
//     printf("Y2: %f\n", averagedPixel->Y[1]);
//     printf("Y3: %f\n", averagedPixel->Y[2]);
//     printf("Y4: %f\n", averagedPixel->Y[3]);
//     printf("Pb_avg: %u\n", averagedPixel->PB_avg);
//     printf("Pr_avg: %u\n\n", averagedPixel->PR_avg);

//     // printf("(%i, %i)\n", j, i);

// }