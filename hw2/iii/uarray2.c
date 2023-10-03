/**************************************************************
 *
 *                     uarray2.c
 *
 *     Assignment: iii
 *     Authors: Liam Smith (lsmith26) and Yoda Ermias (yermia01)
 *     Date: Sep 28, 2023
 * 
 *     Contains elements that are used for the creation of a UArray2_t, which
 *     relies on the Hanson uarray.h interface for foundational functionality.
 *
 *
 **************************************************************/

#include "uarray2.h"
#include <stdlib.h>
#include <stdio.h>
#include "uarray.h"
#include "mem.h"
#include "assert.h"
#include <stdbool.h>

#define T UArray2_T

struct T
{
        int width;
        int height;
        int elementSize;
        UArray_T vert_uArr;
};


/********** UArray2_new ********
 *
 * Purpose:
 *     - allocates space on the heap for a 2D array
 *       with the given row count, column count, and size of element that
 *      will be stored.
 * 
 * Inputs: 
 *     - int row_dim: 
 *            the amount of rows to allocate space for
 *     - int col_dim: 
 *            the amount of columns allocate space for
 *     - int elemSize:
 *            the size of a speficic element type that will be stored in the
 *            array
 *	    
 * Return: 
 *     - outputs a pointer the UArray2 struct.
 *
 * Notes: 
 *     - assert memory allocation and creation of UArrays
 *     - allocates sizeof(void *) on the heap. 
 * 
 ************************/
T UArray2_new(int col_dim, int row_dim, int elemSize)
{
        assert(col_dim > 0 && row_dim > 0 && elemSize > 0);
        /* Create an instance of our UArray_2 */
        T uarray2;
        NEW(uarray2);

        assert(uarray2 != NULL);

        /* Initialize the other variables in our instance of UArray2_T */
        uarray2->height = row_dim;
        uarray2->width = col_dim;
        uarray2->elementSize = elemSize;

        /* Initialize the member variable UArray_T */
        uarray2->vert_uArr = UArray_new(col_dim, sizeof(UArray_T));
        assert(uarray2->vert_uArr != NULL);

        for (int i = 0; i < col_dim; i++) {
                /* Creates an instance of a UArray_T */
                UArray_T horizontal = UArray_new(row_dim, elemSize);
                assert(horizontal != NULL);

                /* Gets the address in vert. array for the hori. UArray */
                UArray_T *horizontal_p = UArray_at(uarray2->vert_uArr, i);
                assert(horizontal != NULL);

                /* dereferenced uarray index gets created horizontal uarray */
                *horizontal_p = horizontal;
        }
        
        return uarray2;
}

/********** UArray2_free ********
 *
 * Purpose:
 *     - Frees the heap memory used during the 
 *       initialization and allocation of the UArray2 instance.
 * 
 * Inputs:
 *     - T *uArr_p:
 *            holds a pointer of the T instance
 *            to free.
 *	    
 * Return: 
 *     - none (void)
 *
 * Notes: 
 *     - assigns the passed T uArray to NULL.
 *     - Doesn't directly return anything, however
 *       indirectly returns NULL to the passed T *uArr_p.
 *     - asserts created UArrays before and after freeing
 *
 ************************/
void UArray2_free(T *uArr_p)
{
        assert(uArr_p != NULL);

        /* frees each horizontal UArray_T */
        for (int i = 0; i < UArray2_width(*uArr_p); i++) {
                /* Creates an instance of a UArray_T */
                UArray_T *horizontal = UArray_at((*uArr_p)->vert_uArr, i);
                assert(horizontal != NULL);
                UArray_free(horizontal);
        }

        /* frees outer vertical UArray_T */
        UArray_free(&(*uArr_p)->vert_uArr);
        assert((*uArr_p)->vert_uArr == NULL);


        /* frees pointer to UArray_2_T */
        free(*uArr_p);
        uArr_p = NULL;
        assert(uArr_p == NULL);
}

/********** UArray2_width ********
 *
 * Purpose:
 *     - returns the width (horizontal measure) of the 2D array.
 * 
 * Inputs:
 *     - T uArr:
 *            holds a passed-by-value instance of a UArray.
 *	    
 * Return: 
 *     - returns an integer which is the width of the 2D
 *       UArray.
 *
 * Notes: 
 *     - asserts that uArr exists
 *
 ************************/
int UArray2_width(T uArr)
{
        assert(uArr != NULL);
        /* returns width as an integer */
        return uArr->width;
}

/********** UArray2_height ********
 *
 * Purpose:
 *     - returns the height (vertical measure) of the 2D array.
 * 
 * Inputs:
 *     - T uArr:
 *            holds a passed-by-value instance of a UArray.
 *	    
 * Return: 
 *     - returns an integer which is the height of the 2D
 *       UArray.
 *
 * Notes: 
 *     - asserts that uArr exists
 *
 ************************/
int UArray2_height(T uArr)
{
        assert(uArr != NULL);
        /* returns height as an integer */
        return uArr->height;
}

/********** UArray2_size ********
 *
 * Purpose:
 *     - returns the size of an element capable of being stored in the uArr. 
 * 
 * Inputs:
 *     - T uArr:
 *            holds a passed-by-value instance of a UArray.
 *	    
 * Return: 
 *     - returns an integer which is the number of elements 
 *       inside the 2D array.
 *
 * Notes: 
 *     - asserts that uArr exists
 *
 ************************/
int UArray2_size(T uArr)
{
        assert(uArr != NULL);
        /* returns elementSize as an integer */
        return uArr->elementSize;
}

/********** UArray2_at ********
 *
 * Purpose:
 *     -  Access the element at the given [row][col] index
 * Inputs:
 *	    - T uArr:
 *              holds a passed-by-value instance of a UArray
 *      - int row:
 *              the vertical index where the element is being accessed
 *      - int col:
 *              the vertical index where the element is being accessed
 * Return: 
 *      - the element that exists at a given index, in the form of a void
 *         pointer to match the element type
 *
 * Notes: 
 *      - asserts that row and column value are valid in bounds values
 *      - asserts that uArr exists
 *      - asserts that horizontal UArrays exist
 
 ************************/
void *UArray2_at(T uArr, int col, int row)
{
        /* asserts that row and column value are valid in bounds values */
        assert(uArr != NULL);
        assert(row < uArr->height);
        assert(col < uArr->width);

        /* Grabbing the horizontal uArray from the vertical uArray */
        UArray_T *horizontal = UArray_at(uArr->vert_uArr, col);
        assert(horizontal != NULL);

        
        /* creates a void pointer based on the value existing within a
        horizontal array */
        void *test = UArray_at(*horizontal, row);
        assert(test != NULL);

        return test;
}

/********** UArray2_map_row_major ********
 *
 * Purpose:
 *     - Iterates through an instance of a UArray2
 *       using row major iteration
 * 
 * Inputs:
 *     - T uArr:
 *            holds the specific instance to a UArray2.
 *     - void apply:
 *            function which is applied to every 
 *            element in the UArray2.
 *     - void *cl:
 *            pointer to a variable needed by the void apply
 *            function pointer.
 *	    
 * Return: 
 *     - None.
 *
 * Notes: 
 *     - an out of bounds reference will call a checked runtime error
 *     - Modifies the elements inside the UArray2 based on instructions
 *       provided in the void apply function.
 *     - asserts that uArr exists
 *     - asserts that first pointer is not NULL
 * 
 ************************/
void UArray2_map_row_major(T uArr, void apply(int col, int row, 
                T a, void *p1, void *p2), void *cl)
{
        assert(uArr != NULL);
        /* mapping according to row major logic looping through entire row
        before incrimentation to next */
        for (int row = 0; row < uArr->height; row++) {
                for (int col = 0; col < uArr->width; col++) {

                        /* creation of void pointer based on element at
                        row column index, passed through to client defined
                        apply function */
                        void *p1 = UArray2_at(uArr, col, row);
                        assert(p1 != NULL);
                        apply(col, row, uArr, p1, cl); 
                }
        }
}

/********** UArray2_map_col_major ********
 *
 * Purpose:
 *     - Iterates through an instance of a UArray2
 *       using column major iteration
 * 
 * Inputs:
 *     - T uArr:
 *            holds the specific instance to a UArray2.
 *     - void apply:
 *            function which is applied to every 
 *            element in the UArray2.
 *     - void *cl:
 *            pointer to a variable needed by the void apply
 *            function pointer.
 *	    
 * Return: 
 *     - None.
 *
 *
 * Notes: 
 *     - Modifies the elements inside the UArray2 based on instructions
 *       provided in the void apply 
 *     - asserts that uArr exists
 *     - asserts that first pointer is not NULL

 *
 ************************/
void UArray2_map_col_major(T uArr, void apply(int col, int row, 
                T a, void *p1, void *p2), void *cl)
{
        assert(uArr != NULL);
        /* mapping according to row major logic looping through entire column
        before incrimentation to next */
        for (int col = 0; col < uArr->width; col++) {
                for (int row = 0; row < uArr->height; row++) {

                        /* creation of void pointer based on element at
                        row column index, passed through to client defined
                        apply function */
                        void *p1 = UArray2_at(uArr, col, row);
                        assert(p1 != NULL);
                        apply(col, row, uArr, p1, cl);
                }
        }
}

#undef T