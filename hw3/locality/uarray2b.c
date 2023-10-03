#include "uarray2b.h"
#include "uarray2.h"
#include "uarray.h"
#include "assert.h"
#include "mem.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define T UArray2b_T

struct UArray2b_T {
    int width;
    int height;
    int blockSize;
    int size;
    UArray2_T blockData;

};


void apply_initializeUArray2(int col, int row, UArray2_T uArr, void *element, void *cl);
void apply_freeUArrays(int col, int row, UArray2_T uArr2, void *elem, void *cl);



int main(){

    UArray2b_T test = UArray2b_new(5, 7, 3, 3);
    assert(test->height == 7);
    assert(test->blockSize == 3);
    assert(test->width == 5);
    assert(test->size == 3);
    assert(test->blockData != NULL);

    printf("This is the height: %i\n", UArray2b_height(test));
    printf("Width is %d\n", UArray2b_width(test));
    printf("Size is %d\n", UArray2b_size(test));
    printf("BlockSize: %i\n", UArray2b_blocksize(test));


    /* Populate UArray2b */
    // int num = 0;
    // for (int i = 0; i < 3; i++) {
    //     for (int j = 0; j < 2; j++) {
    //         UArray_T block = UArray2_at(test->blockData, i / test->blockSize, j / test->blockSize);
    //         *(int *)UArray_at(block, 0) = num;
    //         num++;
    //         printf("This is the int in cell (%i, %i): %i\n", i, j, *(int *) UArray2b_at(test, i, j));

    //     }
    // }

    int num = 0;
    for (int i = 0; i < UArray2_height(test->blockData); i++) {
        for (int j = 0; j < UArray2_width(test->blockData); j++) {
            UArray_T block = UArray2_at(test->blockData, ceil((double) j / (double) test->blockSize), ceil((double) i / (double) test->blockSize));
            *(int *)UArray_at(block, j - (test->blockSize * j / test->blockSize)) = num; 
            // printf("This is the int in cell (%i, %i): %i\n", i, j, *(int *) UArray2b_at(test, i, j));
            // num++;


        }
    }

    /* Print using block mapping */

    
    //UArray2b_free (&test);
    printf("We work!\n");

}


/*
* new blocked 2d array
* blocksize = square root of # of cells in block.
* blocksize < 1 is a checked runtime error
*/
UArray2b_T UArray2b_new (int width, int height, int size, int blockSize) 
{
    //assert all ints
    assert(width > 0 && height > 0 && size > 0 && blockSize > 0);
    
    //allocate space for the Uarray2_T
    // UArray2b_T *uArr2b = (T*) malloc(sizeof(T));
    // UArray2b_T uArr2b = (UArray2b_T)malloc(sizeof(UArray2b_T));
    T uArr2b;
    NEW(uArr2b);
    assert(uArr2b != NULL);


    //set them to the struct
        //take ceiling of width/blocksize and made that * blocksize the width
        //take ceiling of height/blocksize and made that * blocksize the height

    uArr2b->width = width;
    uArr2b->height = height;
    uArr2b->size = size;
    uArr2b->blockSize = blockSize;
    uArr2b->blockData = UArray2_new(ceil((double) width / (double) blockSize),
                                        ceil((double) height/(double) blockSize), sizeof(UArray_T));

    printf("UArray2 height %d\n", UArray2_height(uArr2b->blockData));
    printf("UArray2 width %d\n", UArray2_width(uArr2b->blockData));
    //create UArray of blocksize * blocksize
   

    return uArr2b;
}



// /* new blocked 2d array: blocksize as large as possible provided
// * block occupies at most 64KB (if possible)
// */
// T UArray2b_new_64K_block(int width, int height, int size) 
// {

// }
void UArray2b_free (T *array2b)
{   
    assert(array2b != NULL && *array2b != NULL);
    UArray2_map_row_major((*array2b)->blockData, apply_freeUArrays, NULL);
    UArray2_free(&(*array2b)->blockData);

    free(*array2b);

}

void apply_freeUArrays(int col, int row, UArray2_T uArr2, void *elem, void *cl)
{
    (void) row;
    (void) col;
    (void) uArr2;
    (void) cl;
    UArray_free((UArray_T *) elem);
}
int UArray2b_width (T array2b)
{
    assert(array2b != NULL);
    return array2b->width;
}
int UArray2b_height (T array2b)
{
    assert(array2b != NULL);
    return array2b->height;
}
int UArray2b_size (T array2b)
{
    assert(array2b != NULL);
    return array2b->size;
}
int UArray2b_blocksize(T array2b)
{
    assert(array2b != NULL);
    return array2b->blockSize;
}
// /* return a pointer to the cell in the given column and row.
// * index out of range is a checked run-time error
// */
void *UArray2b_at(T array2b, int column, int row)
{
    /* Find the block in the UArray2b */
    UArray_T block = UArray2_at(array2b->blockData, column / array2b->blockSize, row / array2b->blockSize);

    /* Within block call at to get cell */
    return UArray_at(block, array2b->blockSize * (column % array2b->blockSize) + (row % array2b->blockSize));
}
// /* visits every cell in one block before moving to another block */
// void UArray2b_map(T array2b,
//                     void apply(int col, int row, T array2b,
//                     void *elem, void *cl),
//                     void *cl)
// {
//     for (int i = 0; i < UArray2_height(array2b->blockData); i++) {
//         for (int j = 0; j < UArray2_width(array2b->blockData); j++) {
//             // UArray_T block = UArray2_at(array2b->blockData, i / array2b->blockSize, j / array2b->blockSize);
//             // *(int *)UArray_at(block, 0) 
//             // printf("This is the int in cell (%i, %i): %i\n", i, j, *(int *) UArray2b_at(array2b, i, j));

//         }
//     }

// }
// /*
// * it is a checked run-time error to pass a NULL T
// * to any function in this interface
// */