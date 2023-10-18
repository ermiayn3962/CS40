#include <stdlib.h>
#include <stdio.h>
#include <compress40.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>
#include <mem.h>

#include "rgb_conversion.h"

static Pnm_ppm openImage(FILE *input);
static void trimImage(Pnm_ppm image);

//testing functions
void apply_printValues(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);


/* MAIN USED FOR TESTING PURPOSES */
int main() 
{
    FILE *fp = fopen("flowers.ppm", "r");
    assert(fp != NULL);

    /* Testing the image opening function */
    Pnm_ppm testImg = openImage(fp);
    assert(testImg != NULL);
    printf("image opening works!\n"); 

    /* Testing the trimming function */
    printf("This is testImg height before: %u\n", testImg->height);
    printf("This is testImg width before: %u\n", testImg->width);
    trimImage(testImg);
    printf("This is testImg height after: %u\n", testImg->height);
    printf("This is testImg width after: %u\n", testImg->width);

    /* Testing the RGB to float conversion function */
    RGB_to_float(testImg);
    testImg->methods->map_default(testImg->pixels, apply_printValues, NULL);

    /* Testing the RGB to int conversion function */
    RGB_to_int(testImg);
    testImg->methods->map_default(testImg->pixels, apply_printValues, NULL);

}


void compress40(FILE *input)
{

    /* Opening a PNM Image */
    Pnm_ppm image = openImage(input);
    assert(image != NULL);

    /* Trimming image */
    trimImage(image);


    printf("end of compress40\n");
}



void decompress40(FILE *input) 
{
    (void) input;
}


Pnm_ppm openImage(FILE *input) 
{
    A2Methods_T methods = uarray2_methods_blocked;
    assert(methods != NULL);

    Pnm_ppm image = Pnm_ppmread(input, methods);
    assert(image != NULL);
    return image;
}

void trimImage(Pnm_ppm image)
{
    unsigned height = image->height;
    unsigned width = image->width;

    /* Checking if height is even */
    if ((height % 2) != 0) {
        image->height = height - 1;
    }
    /* Checking if width is even */
    if ((width % 2) != 0) {
        image->width = width - 1;
    }
}



void apply_printValues(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{  
    (void) cl;
    (void) i;
    (void) j;
    (void) array2;

    Pnm_rgb pixel = elem;
    printf("red: %u\n", pixel->red);
    printf("green: %u\n", pixel->green);
    printf("blue: %u\n", pixel->blue);
    printf("\n");

}