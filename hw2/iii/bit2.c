/**************************************************************
 *
 *                     bit2.c
 *
 *     Assignment: iii
 *     Authors: Liam Smith (lsmith26) and Yoda Ermias (yermia01)
 *     Date: Sep 28, 2023
 * 
 *     Contains elements that are used for the creation of a Bit2_T, which
 *     relies on the Hanson bit.h interface for foundational functionality.
 *
 *
 **************************************************************/

#include "bit2.h"
#include <stdlib.h>
#include <stdio.h>
#include "bit.h"
#include "mem.h"
#include "assert.h"
#include <stdbool.h>

#define T Bit2_T

struct T
{
        /* holds all the Bit_T vectors */
        Bit_T bVector;
        int width;
        int height;
};

/* File specific helper function */
bool validArgument(Bit2_T bArr, int col, int row);

/********** Bit2_new ********
 *
 * Creates a Bit2_T instance, updates its member variables based on row (width)
 * and column (height) dimsensions, and allocates space of a Bit vector 
 * according to these dimensions as a 1D vector.
 * 
 * Inputs:
 *      - int col_dim: an integer representing the width of a Bit2_T
 *      - int row_dim: an integer representing the height of a Bit2_T
 *          
 * Return: a Bit2_T structure with updated height, width, and allocated
 * dimensions based on height and width.
 *
 *
 * Notes: 
 *      - allocation for space using NEW
 *      - asserts creation of bit2
 *
 ************************/
T Bit2_new(int col_dim, int row_dim)
{
        /* Creating a new instance of the struct */
        T bit2;
        NEW(bit2);
        assert(bit2 != NULL);

        /* Initializing member variables */
        bit2->height = row_dim;
        bit2->width = col_dim;

        /* Initializing Bit_T bVector */
        bit2->bVector = Bit_new(col_dim * row_dim);
        assert(bit2->bVector != NULL);

        return bit2;
}

/********** Bit2_free ********
 *
 * Frees the memory occupied by a Bit2_T instance used during initialization
 * and allocation of the data.
 * 
 * Inputs: 
 *      - T *bArr_p: a pointer to a Bit2_T instance with width, height, 
 *      and a bit vector with its length based on width and height dimensions.
 *          
 * Return: 
 *      None (void)
 *
 *
 * Notes: 
 *      - asserts bArr_p before and after assertions to check existance and
 *      deletion after freeing
 *
 ************************/
void Bit2_free(T *bArr_p)
{
        assert(bArr_p != NULL);

        /* frees bit vector */
        Bit_free(&(*bArr_p)->bVector);
        assert((*bArr_p)->bVector == NULL);
        free(*bArr_p);
        bArr_p = NULL;
        assert(bArr_p == NULL);
}

/********** Bit2_width ********
 *
 * Returns the horizontal width of a Bit2_T as an integer, accessed as a member
 * variable of the Bit2_T struct
 * 
 * Inputs:
 *      - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      its length based on width and height dimensions.
 *          
 * Return: an integer representing the horizontal width of a Bit2_T
 *
 *
 * Notes: asserts that bArr exists
 *
 ************************/
int Bit2_width(T bArr)
{
        assert(bArr != NULL);
        /* width returned as an integer */
        return bArr->width;
}

/********** Bit2_height ********
 *
 * Returns the vertical height of a Bit2_T as an integer, accessed as a member
 * variable of the Bit2_T struct
 * 
 * Inputs:
 *      - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      length based on width and height dimensions.
 *          
 * Return: an integer representing the vertical height of a Bit2_T
 *
 *
 * Notes: asserts that bArr exists
 
 ************************/
int Bit2_height(T bArr)
{
        assert(bArr != NULL);

        /* height returned as an integer */
        return bArr->height;
}

/********** Bit2_put ********
 *
 * Updates the value of a bit at a given column and row index, and returns the
 * bit that existed previously at this index.
 * 
 * Inputs:
 *      - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      length based on width and height dimensions.
 *      - int col: a column index as an integer used for idenifying the bit's
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit's
 *                  row
 *      - int value: the new bit value as an integer that will replace the 
 *                  old bit at a given index
 * 
 * Return: an integer representing the pre-replaced bit at the index where
 *          a new bit is being added
 *
 *
 * Notes: 
 *        - asserts that col and row values are valid
 *        - asserts that bArr exists
 *
 ************************/
int Bit2_put(T bArr, int col, int row, int value)
{
        assert(bArr != NULL);

        /* asserts that valid column and row are bassed to Bit2_T */
        assert(validArgument(bArr, col, row));

        /* asserts that value is valid */
        assert(value > -1);

        /* returns bit location in 1D bit array and updates with new value  */
        return Bit_put(bArr->bVector, row * bArr->width + col, value);
}

/********** Bit2_get ********
 *
 * Accesses the bit at a specific column and row index, as an integer
 * 
 * Inputs:
 *      - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      length based on width and height dimensions.
 *      - int col: a column index as an integer used for idenifying the bit's
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit's
 *                  row
 *          
 * Return: as an integer, the bit at an index based on column and row locations
 *
 *
 * Notes: 
 *        - asserts that validArgument returns true based on col row indeces
 *        - asserts that bArr exists
 *
 ************************/
int Bit2_get(T bArr, int col, int row)
{
        assert(bArr != NULL);

        /* asserts that valid column and row are bassed to Bit2_T */
        assert(validArgument(bArr, col, row));

        /* returns bit location in 1D bit array */
        return Bit_get(bArr->bVector, row * bArr->width + col);
}

/********** validArgument ********
 *
 * Validates that width and height dimensions called in other functions
 * correspond to indeces that exists within a Bit2_T
 * 
 * Inputs:
 *      - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      length based on width and height dimensions.
 *      - int col: a column index as an integer used for idenifying the bit
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit
 *                  row
 *          
 * Return: bool valued as true if the row and column arguments correspond to
 * proper dimensions, and false otherwise.
 *
 *
 * Notes: 
 *      - asserts valid column and row values 
 *        - asserts that bArr exists
 *
 ************************/
bool validArgument(T bArr, int col, int row)
{    
        assert(bArr != NULL);

        /* asserts valid column and row values */
        assert(col < Bit2_width(bArr));
        assert(col >= 0);
        assert(row < Bit2_height(bArr));
        assert(row >= 0);

        return true;
}

/********** Bit2_map_row_major ********
 *
 * Iterates through a Bit2_T using row major functionality. Row major means that
 * every index of a row is incrimented before row is incrimented.
 * 
 * Inputs:
 *     - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      length based on width and height dimensions.
 *     - void apply: function which is client-applied in a row-major context
 *         involving the UArray2_T.
 *     - void *cl: pointer to a variable controlled by the void apply function.
 * 
 * Return: none (void)
 *
 *
 * Notes: 
 *         - asserts that bArr exists
 *
 ************************/
void Bit2_map_row_major(T bArr, void apply(int col, int row,
                        T a, int value, void *a_cl), void *cl)
{
        assert(bArr != NULL);

        /* initialize column and row values */
        int col = 0;
        int row = 0;   

        /* looping through entire row before incrimenting row */
        for (int i = 0; i < bArr->width * bArr->height; i++) {
                col = i % bArr->width;

                /* at start of new row, incriment row and reset column */
                if (i % bArr->width == 0 && i != 0) {
                        col = 0;
                        row++;
                }
                
                /* accessing bit value at row column index */
                int value = Bit2_get(bArr, col, row);

                /* apply function defined by client */
                apply(col, row, bArr, value, cl);
        }
}

/********** Bit2_map_col_major ********
 *
 * Iterates through a Bit2_T using column major functionality. Column major 
 * means that every index of a column is incrimented before column is 
 * incrimented.
 * 
 * Inputs:
 *      - T bArr: A Bit2_T instance with width, height, and a bit vector with
 *      length based on width and height dimensions.
 *     - void apply: function which is client-applied in a column-major context
 *         involving the UArray2_T.
 *     - void *cl: pointer to a variable controlled by the void apply function.
 * 
 * Return: none (void)
 *
 *
 * Notes: 
 *        - asserts that bArr exists
 *
 *
 ************************/
void Bit2_map_col_major(T bArr, void apply(int col, int row,
                        T a, int value, void *a_cl), void *cl)
{
        assert(bArr != NULL);

        /* initialize column and row values */
        int col = 0;
        int row = 0;   

        /* looping through entire row before incrimenting row */
        for (int i = 0; i < bArr->width * bArr->height; i++) {
                row = i % bArr->height;

                /* at start of new column, incriment col and reset row */
                if (i % bArr->height == 0 && i != 0) {
                        row = 0;
                        col++;
                }
                
                /* accessing bit value at row column index */
                int value = Bit2_get(bArr, col, row);

                /* apply function defined by client */
                apply(col, row, bArr, value, cl);
        }
}