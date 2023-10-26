/**************************************************************
 *
 *                     image_manipulation.c
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

#include "image_manipulation.h"

/* openImage
 * Description: opens and image from input file and returns the image
 *
 * Input: input - FILE to open image from
 * 
 * Output: Pnm_ppm image from file
 * 
 * Exceptions:  C.R.E image from Pnm_ppmread is null
 */
Pnm_ppm openImage(FILE *input) 
{
        Pnm_ppm image = Pnm_ppmread(input, uarray2_methods_blocked);
        assert(image != NULL);
        return image;
}

/* trimImage
 * Description: trims odd sized images to fit to even sized image
 *
 * Input: width - pointer to width value so can be updated
 *        height - pointer to height value so can be updated
 * 
 * Output: none (indirectly returns the updated width and height)
 * 
 * Exceptions: none (except Hanson's)
 */
void trimImage(int *width, int *height)
{
        /* Checking if height is even */
        if ((*height % 2) != 0) {
                *height = *height - 1;
        }
        /* Checking if width is even */
        if ((*width % 2) != 0) {
                *width = *width - 1;
        }
}

/* initializeUArray2
 * Description: creates a new A2Methods_UArray2 with the same dimensions as 
 *              A2Methods_UArray2 passed in but with potentially different size 
 *              of elements or blocksize.
 * 
 * Input: arr_and_meth - UArray2 to create a new UArray2 based on
 *        size - desired size of elements in new UArray2
 *        blocksize - desired blocksize of new UArray2
 * 
 * Output: new UArray2 with size, blocksize from params and height and width of 
 *         UArray2 passed.
 * 
 * Exceptions:  C.R.E caused by checkOutput
 *              C.R.E if cannot allocate space for the new array
 */
A2Methods_UArray2 initializeUArray2(A2_and_Methods arr_and_meth, int size, 
                                    int blocksize)
{
        checkInput(arr_and_meth);

        int width = arr_and_meth->methods->width(arr_and_meth->array);
        int height = arr_and_meth->methods->height(arr_and_meth->array);

        trimImage(&width, &height);

        A2Methods_UArray2 newArray = arr_and_meth->methods->new_with_blocksize(
                                        width, height, size, blocksize);
        assert(newArray != NULL);
        return newArray;
}


/* checkInput
 * Description: Converts the data from the Pnm_rgb structs to
 *              VCS_data structs
 * Input:   
 *      A2_and_Methods input - the input to assert if any of its
 *                             variables are not NULL
 *
 * Output: None 
 * 
 * Details: Asserts if the struct passed or its varialbes are not NULL
 * 
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL.
 */
void checkInput(A2_and_Methods input)
{
        assert(input != NULL);
        assert(input->array != NULL);
        assert(input->methods != NULL);
}