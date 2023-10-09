/**************************************************************
 *
 *                     uarray2b.c
 *
 *     Assignment: locality
 *     Authors: Maiah Islam (mislam07) and Yoda Ermias (yermia01)
 *     Date: Oct 8, 2023
 * 
 *     Contains the functions to create and retrieve information from a 
 *     UArray2B.
 *
 *
 **************************************************************/

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
        /* UArray2_T represents the container of all the blocks (UArray_T's) */
        UArray2_T blockData;

};


static void initializeBlocks(int col, int row, UArray2_T uArr2, void *elem, 
                             void *cl);
static void apply_freeUArrays(int col, int row, UArray2_T uArr2, void *elem, 
                              void *cl);


/********** UArray2b_new ********
 *
 * Allocates heap memory for an instance of a UArray2b
 * 
 * Inputs:
 *      - int width: the width of the data
 *      - int height: the height of the data
 *      - int size: the size of the element stored in the cell
 *      - int blockSize: the blockSize for each block
 *          
 * Return: 
 *      - A UArray2b_T struct
 *
 * Notes: 
 *      - Raises CRE if any of the information passed is less than or equal to
 *        zero
 *
 ************************/
T UArray2b_new (int width, int height, int size, int blockSize) 
{
        assert(width > 0 && height > 0 && size > 0 && blockSize > 0);
        
        T uArr2b;
        NEW(uArr2b);
        assert(uArr2b != NULL);


        uArr2b->width = width;
        uArr2b->height = height;
        uArr2b->size = size;
        uArr2b->blockSize = blockSize;
        uArr2b->blockData = UArray2_new(ceil((double) width / 
                                        (double) blockSize),
                                                ceil((double) 
                                                height/(double) blockSize), 
                                                sizeof(UArray_T));


        /* Creating UArray of blocksize * blocksize */
        UArray2_map_row_major(uArr2b->blockData, initializeBlocks, &uArr2b);

        return uArr2b;
}

/********** initializeBlocks ********
 *
 * Initializes heap memory for the blocks in the UArray2_T
 * 
 * Inputs:
 *      - int col: the current col in the UArray2b 
 *      - int row: the current row in the UArray2b
 *      - UArray2_T uArr2: void
 *      - void *elem: void
 *      - void *cl: the closure
 * Return: 
 *      - None
 *
 * Notes: 
 *      - Creates new allocations for UArray_T's and assigns them to
 *        the element at (col, row)
 *
 ************************/
void initializeBlocks(int col, int row, UArray2_T uArr2, void *elem, void *cl)
{
        (void) elem;
        (void) uArr2;
        T *temp = (T *) cl;
        int blockSize = (*temp)->blockSize;
        int size = (*temp)->size;
        *(UArray_T *) UArray2_at((*temp)->blockData, col, row) = 
                                UArray_new(blockSize * blockSize, size);
        
}



/********** UArray2b_new_64k_block ********
 *
 * Allocates heap memory for an instance of a UArray2b with
 * the maximum number of blocks under 64 kB for a specific size
 * 
 * Inputs:
 *      - int width: the width of the data
 *      - int height: the height of the data
 *      - int size: the size of the element stored in the cell
 *          
 * Return: 
 *      - A UArray2b_T struct
 *
 * Notes: 
 *      - Raises CRE if any of the information passed is less than zero
 *
 ************************/
T UArray2b_new_64K_block(int width, int height, int size)
{
        assert(width > 0 && height > 0 && size > 0);

        int blockSize;

        if(size >= 1024 * 64) {
                blockSize = 1;
        }
        else {
                blockSize = ceil(sqrt((double)(1024 * 64)/(double) size));
        }

        return UArray2b_new (width, height, size, blockSize);
}

/********** UArray2b_free ********
 *
 * Frees a UArray2b object
 * 
 * Inputs:
 *      - T *array2b: the address of a UArray2b_T object
 *          
 * Return: 
 *      - None (void)
 *
 * Notes: 
 *      - sets the passed object to NULL after freeing
 *
 ************************/
void UArray2b_free (T *array2b)
{   
        assert(array2b != NULL && *array2b != NULL);

        UArray2_map_row_major((*array2b)->blockData, apply_freeUArrays, NULL);
        
        UArray2_free(&(*array2b)->blockData);
        free((*array2b)->blockData);
        free(*array2b);

        *array2b = NULL;

}

/********** apply_freeUArrays ********
 *
 * Allocates heap memory for an instance of a UArray2b
 * 
 * Inputs:
 *      - int width: the width of the data
 *      - int height: the height of the data
 *      - int size: the size of the element stored in the cell
 *      - int blockSize: the blockSize for each block
 *          
 * Return: 
 *      - A UArray2b_T struct
 *
 * Notes: 
 *      - Raises CRE if any of the information passed is less than zero
 *
 ************************/
void apply_freeUArrays(int col, int row, UArray2_T uArr2, void *elem, void *cl)
{
        (void) row;
        (void) col;
        (void) uArr2;
        (void) cl;
        UArray_free((UArray_T *) elem);
}

/********** UArray2b_width ********
 *
 * Returns the width of the UArray2b_T
 * 
 * Inputs:
 *      - T array2b: the UArray2b_T object to retrieve the width from
 *          
 * Return: 
 *      - An integer representing the width of the UArray2b_T
 *
 * Notes: 
 *      - Raises CRE if a NULL pointer is passed as an argument
 *
 ************************/
int UArray2b_width (T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/********** UArray2b_height ********
 *
 * Returns the height of the UArray2b_T
 * 
 * Inputs:
 *      - T array2b: the UArray2b_T object to retrieve the height from
 *          
 * Return: 
 *      - An integer representing the height of the UArray2b_T
 *
 * Notes: 
 *      - Raises CRE if a NULL pointer is passed as an argument
 *
 ************************/
int UArray2b_height (T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/********** UArray2b_size ********
 *
 * Returns the size of the UArray2b_T
 * 
 * Inputs:
 *      - T array2b: the UArray2b_T object to retrieve the size from
 *          
 * Return: 
 *      - An integer representing the size of the UArray2b_T
 *
 * Notes: 
 *      - Raises CRE if a NULL pointer is passed as an argument
 *
 ************************/
int UArray2b_size (T array2b)
{
        assert(array2b != NULL);
        return array2b->size;
}

/********** UArray2b_blocksize ********
 *
 * Returns the blocksize of the UArray2b_T
 * 
 * Inputs:
 *      - T array2b: the UArray2b_T object to retrieve the blocksize from
 *          
 * Return: 
 *      - An integer representing the blocksize of the UArray2b_T
 *
 * Notes: 
 *      - Raises CRE if a NULL pointer is passed as an argument
 *
 ************************/
int UArray2b_blocksize(T array2b)
{
        assert(array2b != NULL);
        return array2b->blockSize;
}

/********** UArray2b_at ********
 *
 * Returns the element at the given col and row in the UArray2b_T
 * 
 * Inputs:
 *      - T array2b: the UArray2b_T object to retrieve data from
 *      - int col: the column to access in the UArray2b_T
 *      - int row: the row to access in the UArray2b_T
 *          
 * Return: 
 *      - A void pointer to the element stored at the column and row
 *        in the array2b
 *
 * Notes: 
 *      - Raises CRE if a NULL pointer is passed as an argument or if
 *        col/row are greater than the dimensions of the UArray2b_T
 *
 ************************/
void *UArray2b_at(T array2b, int col, int row)
{

        assert(array2b != NULL && col < array2b->width &&
               row < array2b->height);

        /* Find the block in the UArray2b */
        UArray_T *block = (UArray_T *) UArray2_at(array2b->blockData, 
                                                col / array2b->blockSize,
                                                row / array2b->blockSize);
        /* Find the element in the UArray */
        void *element = UArray_at(*block, array2b->blockSize * 
                                (col % array2b->blockSize) + (
                                row % array2b->blockSize));
        
        return element;
}

/********** UArray2b_map ********
 *
 * Traverses the UArray2b_T passed with block major mapping.
 * 
 * Inputs:
 *      - T array2b: the UArray2b_T object to retrieve the width from
 *      - void apply: the specific function to apply to every element in
 *                    the UArray2b_T
 *      - void *cl: the closure for the mapping function
 *          
 * Return: 
 *      - None
 *
 * Notes: 
 *      - Raises CRE if array2b is a NULL pointer
 *
 ************************/
void UArray2b_map(T array2b,
                    void apply(int col, int row, T array2b,
                    void *elem, void *cl),
                    void *cl)
{
        assert(array2b != NULL);

        int height = array2b->height;
        int width = array2b->width;
        UArray2_T UArray2 = array2b->blockData;

        for (int col = 0; col < UArray2_width(UArray2); col++) {
                for (int row = 0; row < UArray2_height(UArray2); row++) {
                UArray_T *block = UArray2_at(UArray2, col, row);
                /* Getting the first cell inside the blocks */
                int blockCol = array2b->blockSize * col; 
                int blockRow = array2b->blockSize * row; 

                /* Looping through the entire block */
                for (int cell = 0; cell < array2b->blockSize * 
                        array2b->blockSize; cell++) {
                        int cellCol = blockCol + cell / array2b->blockSize;
                        int cellRow = blockRow + cell % array2b->blockSize;

                        /* Checking if out of bound indices are accessed */
                        if (cellCol < width && cellRow < height){
                        apply(cellCol, cellRow, array2b, 
                                UArray_at(*block, cell), cl);
                        }
                }
                }
        }

}