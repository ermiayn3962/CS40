#include "image_manipulation.h"


Pnm_ppm openImage(FILE *input) 
{
    A2Methods_T methods = uarray2_methods_blocked;
    assert(methods != NULL);

    Pnm_ppm image = Pnm_ppmread(input, methods);
    assert(image != NULL);
    return image;
}

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

A2Methods_UArray2 initializeUArray2(A2_and_Methods arr_and_meth, int size, int blocksize)
{
    // printf("This is the int size: printed as unsigned %lu\n", size);

    assert(arr_and_meth != NULL);
    assert(arr_and_meth->array != NULL);
    int width = arr_and_meth->methods->width(arr_and_meth->array);
    int height = arr_and_meth->methods->height(arr_and_meth->array);

    trimImage(&width, &height);

    A2Methods_UArray2 newArray = arr_and_meth->methods->new_with_blocksize(width, height, size, blocksize);
    assert(newArray != NULL);

    return newArray;
}