#ifndef IMAGE_MANIPULATION_INCLUDED
#define IMAGE_MANIPULATION_INCLUDED

#include <stdio.h>
#include <pnm.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>


typedef struct A2_and_Methods {
        A2Methods_UArray2 array;
        A2Methods_T methods;

        unsigned denominator;
} *A2_and_Methods;


extern void trimImage(int *width, int *height);
extern Pnm_ppm openImage(FILE *input);
extern A2Methods_UArray2 initializeUArray2(A2_and_Methods arr_and_meth, int size, int blocksize);


#endif