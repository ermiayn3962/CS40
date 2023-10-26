/**************************************************************
 *
 *                     image_manipulation.h
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (golem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions for manipulating an image and its
 *     UArray2 representation.
 *
 *
 **************************************************************/

#ifndef IMAGE_MANIPULATION_INCLUDED
#define IMAGE_MANIPULATION_INCLUDED

#include <stdio.h>
#include <pnm.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <assert.h>

/* A2_and_Methods
 * Struct that wraps an array type and its methods for abstraction purposes
 * array - A2Methods_UArray2
 * methods - A2Methods_T
 */
typedef struct A2_and_Methods {
        A2Methods_UArray2 array;
        A2Methods_T methods;
} *A2_and_Methods;


extern void trimImage(int *width, int *height);
extern Pnm_ppm openImage(FILE *input);
extern A2Methods_UArray2 initializeUArray2(A2_and_Methods arr_and_meth,
                                           int size, int blocksize);
extern void checkInput(A2_and_Methods input);


#endif