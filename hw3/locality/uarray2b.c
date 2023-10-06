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
void initializeBlocks(int col, int row, UArray2_T uArr2, void *elem, void *cl);

void apply_freeUArrays(int col, int row, UArray2_T uArr2, void *elem, void *cl);


void populate_cells(int col, int row, T array2b,
                    void *elem, void *cl);


// int main(){

//     UArray2b_T test = UArray2b_new(9, 9, sizeof(int), 3);
//     // assert(test->height == 9);
//     // assert(test->blockSize == 3);
//     // assert(test->width == 9);
//     // assert(test->size == 3);
//     // assert(test->blockData != NULL);

//     // printf("This is the height: %i\n", UArray2b_height(test));
//     // printf("Width is %d\n", UArray2b_width(test));
//     // printf("Size is %d\n", UArray2b_size(test));
//     // printf("BlockSize: %i\n", UArray2b_blocksize(test));


//     /* Populate UArray2b */
   
//     // /* Loop through the blocks */
//     // int num = 0;
//     // for (int row = 0; row < test->height/test->blockSize; row ++){
//     //     for (int col = 0; col < test->width/test->blockSize; col ++) {
//     //         UArray_T block = *(UArray_T *) UArray2_at(test->blockData, col / test->blockSize, row / test->blockSize);
//     //         // (void) block;
//     //         for (int k = 0; k < test->blockSize * test->blockSize; k++) {
//     //             // printf("Index (using k): %i\n", k);
//     //             //int *elem = (int *) UArray_at(*block, k);
//     //             printf("num: %i\n", num);
//     //             *(int *) UArray_at(block, k) = num;

//     //             printf("elem in UArray2b: %i\n", *(int *) UArray2b_at(test, col, row));
//     //             printf("from UArray_at: %i\n", *(int *) UArray_at(block, k));

//     //             // *elem = num;
//     //             num++;
//     //             // printf("Index (using k): %i\n", k);
//     //             // printf("Index (using math): %i\n", test->blockSize * (row % test->blockSize) + (col % test->blockSize));


//     //         }
//     //         printf("\n");
//     //         // printf("This is the block (%i, %i)\n", col / test->blockSize, row / test->blockSize);

//     //     }
//     // }


//     // printf("This is the element at (4,5): %i\n", *(int *) UArray2b_at(test, 4, 5));


//     int num = 0;

//     // printf("This is what's sent into the mapping function: %i\n", i);
//     UArray2b_map(test, populate_cells, &num);
//     // //*(int *) UArray2b_at(test, 0, 0) = 3;
//     // printf("This is the element at (0,0): %i\n", *(int *) UArray2b_at(test, 0, 0));
//     // printf("This is the element at (6,4): %i\n", *(int *) UArray2b_at(test, 6, 4));

//     // printf("This is the element at (8,8): %i\n", *(int *) UArray2b_at(test, 8, 8));

    


//     // UArray2b_new_64K_block(5, 7, 3);
//     // UArray2b_new_64K_block(5, 7, 1024*64);

//     /* Print using block mapping */

    
//     UArray2b_free (&test);
//     printf("We work!\n");
    

// }

void populate_cells(int col, int row, T array2b,
                    void *elem, void *cl)
{
    (void) col;
    (void) row;
    (void) array2b;
    (void) elem;
    // printf("This is the cl in apply function: %i\n", *(int *) cl);
    printf("(%i, %i)\n", col, row);
    printf("this is the cell before: %i\n", *(int *)UArray2b_at(array2b, col, row));
    *(int *)UArray2b_at(array2b, col, row)  = *(int *) cl;
    printf("this is the cell after: %i\n", *(int *)UArray2b_at(array2b, col, row));

    // *(int *) elem = *(int *) cl;
    printf("This is the elem in the uarr2b: %i\n", *(int *)UArray2b_at(array2b, col, row));
    *(int *) cl = *(int *) cl + 1;

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

    // printf("UArray2 height %d\n", UArray2_height(uArr2b->blockData));
    // printf("UArray2 width %d\n", UArray2_width(uArr2b->blockData));
    //create UArray of blocksize * blocksize

    UArray2_map_row_major(uArr2b->blockData, initializeBlocks, &uArr2b);

    // for (int i = 0; i < UArray2_height(uArr2b->blockData); i++) {
    //     for (int j = 0; j < UArray2_width(uArr2b->blockData); j++) {
    //         UArray_T *block = UArray2_at(uArr2b->blockData, i, j);
    //         *block = UArray_new(blockSize * blockSize, size);
    //     }
    // }

    return uArr2b;
}

void initializeBlocks(int col, int row, UArray2_T uArr2, void *elem, void *cl)
{
    (void) elem;
    (void) uArr2;
    UArray2b_T  *temp = (UArray2b_T *) cl;
    int blockSize = (*temp)->blockSize;
    int size = (*temp)->size;
    *(UArray_T *) UArray2_at((*temp)->blockData, col, row) = UArray_new(blockSize * blockSize, size);
    
}



// /* new blocked 2d array: blocksize as large as possible provided
// * block occupies at most 64KB (if possible)
// */
T UArray2b_new_64K_block(int width, int height, int size)
{
    int blockSize;
    if(size >= 1024 * 64) {
        blockSize = 1;
    }
    else {
        blockSize = ceil(sqrt((double)(1024 * 64)/(double) size));
    }
    //printf("blockSize: %i\n", blockSize);
    return UArray2b_new (width, height, size, blockSize);
}

void UArray2b_free (T *array2b)
{   
    assert(array2b != NULL && *array2b != NULL);
    UArray2_map_row_major((*array2b)->blockData, apply_freeUArrays, NULL);
    UArray2_free(&(*array2b)->blockData);
    free((*array2b)->blockData);

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
void *UArray2b_at(T array2b, int col, int row)
{
    // printf("This is the passed col: %i\n", col);
    // printf("This is the passed row: %i\n", row);

    assert(array2b != NULL && col < array2b->width && row < array2b->height);
    /* Find the block in the UArray2b */
    //printf("Block: (%d, %d)\n", col / array2b->blockSize, row / array2b->blockSize);
    UArray_T *block = (UArray_T *) UArray2_at(array2b->blockData, col / array2b->blockSize, row / array2b->blockSize);

    //printf("Cell: %i\n", array2b->blockSize * col % array2b->blockSize + row % array2b->blockSize);
    
    void *element = UArray_at(*block, array2b->blockSize * (col % array2b->blockSize) + (row % array2b->blockSize));
    //printf("elem: %i\n", *(int *)element);

    //printf("elem: %i\n", *(int *)element);

    
    /* Within block call at to get cell */
    return element;
}

/* visits every cell in one block before moving to another block */
void UArray2b_map(T array2b,
                    void apply(int col, int row, T array2b,
                    void *elem, void *cl),
                    void *cl)
{


    /* CODE HERE DOESN'T WORK WITH RECTANGULAR UARRAY2'S */

    // int height = array2b->height;
    // int width = array2b->width;
    // int blockSize = array2b->blockSize;
    
    // for (int row = 0; row < height/blockSize; row ++){  // MIGHT HAVE TO ADD CEILING FUNCTION
    //     for (int col = 0; col < width/blockSize; col ++) {
    //         // UArray_T *block = (UArray_T *) UArray2_at(array2b->blockData, col /blockSize, row / blockSize);
    //         // (void) block;
    //         printf("Row: %i\n", row);
    //         printf("Col: %i\n", col);
    //         for (int k = 0; k < blockSize * blockSize; k++) {
    //             int tinyRow = k/blockSize;
    //             printf("tinyRow: %i\n", tinyRow);
    //             int tinyCol = k % blockSize; //gets us tiny in 3x3
    //             printf("tinyCol: %i\n", tinyCol);
                
    //             int bigBlockRow = row * blockSize + tinyRow;
    //             printf("bigBlockRow: %i\n", bigBlockRow);

    //             int bigBlockCol = col * blockSize + tinyCol;
    //             printf("bigBlockCol: %i\n", bigBlockCol);

                
    //             void *elem = UArray2b_at(array2b, bigBlockCol, bigBlockRow);
    //             apply(bigBlockCol, bigBlockRow, array2b, elem, cl);

    //         }

    //         // void *elem = UArray2b_at(array2b, col, row);
    //         // apply(col, row, array2b, elem, cl);



    //     }
    // }



    int height = array2b->height;
    int width = array2b->width;

    for (int col = 0; col < UArray2_width(array2b->blockData); col++) {
        for (int row = 0; row < UArray2_height(array2b->blockData); row++) {
            UArray_T *block = UArray2_at(array2b->blockData, col, row);
            /* Getting the first cell inside the blocks */
            int blockCol = array2b->blockSize * col; 
            int blockRow = array2b->blockSize * row; 

            /* Looping through the entire block */
            for (int cell = 0; cell < array2b->blockSize * array2b->blockSize; cell++) {
                int cellCol = blockCol + cell / array2b->blockSize;
                int cellRow = blockRow + cell % array2b->blockSize;

                /* Double checking if out of bound indices are accessed */
                if (cellCol < width && cellRow < height){
                    apply(cellCol, cellRow, array2b, UArray_at(*block, cell), cl);
                }
            }
        }
    }

}
/*
* it is a checked run-time error to pass a NULL T
* to any function in this interface
*/