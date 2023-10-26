/**************************************************************
 *
 *                     rgb_conversion.c
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (cgolem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions to convert an RGB pixel to video 
 *     color space and vice versa.
 *
 *
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <math.h>

#include "rgb_conversion.h"


/*  RGB_float
*   Represents the Pnm_rgb int data as floats for a every pixel of image.
*   
*   red - a float representation of the red value of Pnm_rgb
*   green - a float representation of the green value of Pnm_rgb
*   blue - a float representation of the blue value of Pnm_rgb
*/
typedef struct RGB_float {
        float red, green, blue;
} *RGB_float;


/* chosen denomonator for Ppm image */
static const int DENOMINATOR = 255; 

/* apply functions */
static void apply_RGB_to_VCS(int i, int j, A2Methods_UArray2 array2, 
                             void *elem, void *cl);
static void apply_VCS_to_RGB(int i, int j, A2Methods_UArray2 array2, 
                             void *elem, void *cl);
static void apply_RGB_to_int(int i, int j, A2Methods_UArray2 array2, 
                             void *elem, void *cl);
static void apply_RGB_to_float(int i, int j, A2Methods_UArray2 array2, 
                                void *elem, void *cl);

/* helpers */
void checkInput(A2_and_Methods input);
static int checkDenom(int val);



/* RGB_to_float
 * Description: Converts an array of Pnm_rgb pixels to their float 
 *              representations
 * Input: rgb_int_array - an A2_and_Methods struct which holds a 
 *                        UArray2 of Pnm_rgb structs.
 * Output: None (indirectly outputs a UArray2 of RGB_float structs by 
 *              updating the array in the A2_and_Methods struct 
 *              passed in)
 * Details: Creates a new UArray2 of RGB_float structs, maps through 
 *          the new array and converts the data from rgb_int_array 
 *          to the data held in the RGB_float struct
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 */
void RGB_to_float(A2_and_Methods rgb_int_array) 
{
        checkInput(rgb_int_array);

        /* Create a UArray2 based on passed struct's array */
        A2Methods_UArray2 float_pixels = initializeUArray2(rgb_int_array, 
                                                sizeof(struct RGB_float), 2);
        assert(float_pixels != NULL);

        /* Converting the Pnm_rgb struct data to RGB_float data */
        rgb_int_array->methods->map_default(float_pixels, apply_RGB_to_float,
                                            rgb_int_array);
        
        rgb_int_array->array = float_pixels;
}   


/* RGB_to_int
 * Description: Converts an array of RGB_float pixels to their int 
 *              representations
 * Input: rgb_float_array - an A2_and_Methods struct which holds a 
 *                        UArray2 of RGB_float structs.
 * Output: None (indirectly outputs a UArray2 of Pnm_rgb structs by 
 *              updating the array in the A2_and_Methods struct 
 *              passed in)
 * Details: Creates a new UArray2 of Pnm_rgb structs, maps through 
 *          the new array and converts the data from rgb_float_array 
 *          to the data held in the Pnm_rgb struct
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 */
void RGB_to_int(A2_and_Methods rgb_float_array) 
{
        checkInput(rgb_float_array);

        /* Create a UArray2 based on passed struct's array */
        A2Methods_UArray2 rgb_int_array = initializeUArray2(rgb_float_array, 
                                                sizeof(struct Pnm_rgb), 2);
        assert(rgb_int_array != NULL);

        /* Converting the RGB_float struct data into Pnm_rgb data */
        rgb_float_array->methods->map_default(rgb_int_array, apply_RGB_to_int,
                                              rgb_float_array);
        
        rgb_float_array->methods->free(&rgb_float_array->array);
        rgb_float_array->array = rgb_int_array;
} 


/* RGB_to_VCS
 * Description: Converts an array of RGB_float pixels to their  
 *              video color space (VCS) representations
 * Input: rgb_float_array - an A2_and_Methods struct which holds a 
 *                        UArray2 of RGB_float structs.
 * Output: None (indirectly outputs a UArray2 of VCS_data structs by 
 *              updating the array in the A2_and_Methods struct 
 *              passed in)
 * Details: Creates a new UArray2 of VCS_data structs, maps through 
 *          the new array and converts the data from rgb_float_array 
 *          to the data held in the VCS_data struct
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 */
void RGB_to_VCS(A2_and_Methods rgb_float_array)
{
        checkInput(rgb_float_array);

        /* Create a UArray2 based on passed struct's array */
        A2Methods_UArray2 VCS_array = initializeUArray2(rgb_float_array, 
                                                sizeof(struct VCS_data), 2);
        assert(VCS_array != NULL);

        /* Converting the RGB_float struct data into VCS_data struct data */
        rgb_float_array->methods->map_default(VCS_array, apply_RGB_to_VCS, 
                                                rgb_float_array);

        rgb_float_array->methods->free(&rgb_float_array->array);
        rgb_float_array->array = VCS_array;
}


/* VCS_to_RGB
 * Description: Converts an array of VCS_data structs to their  
 *              video color space (VCS) representations
 * Input: vcs_array - an A2_and_Methods struct which holds a 
 *                        UArray2 of VCS_data structs.
 * Output: None (indirectly outputs a UArray2 of RGB_float structs by 
 *              updating the array in the A2_and_Methods struct 
 *              passed in)
 * Details: Creates a new UArray2 of RGB_float structs, maps through 
 *          the new array and converts the data from vcs_array 
 *          to the data held in the RGB_float struct
 * Exceptions: causes C.R.E if the A2_and_Methods struct passed is 
 *             NULL or one of the variables in the struct is NULL. 
 */
void VCS_to_RGB(A2_and_Methods vcs_array)
{
        /* Create a UArray2 based on passed struct's array */
        A2Methods_UArray2 RGB_float_array = initializeUArray2(vcs_array, 
                                                sizeof(struct RGB_float), 2);
        assert(RGB_float_array != NULL);
        
        /* Converting the VCS_data struct data into RGB_float struct data */
        vcs_array->methods->map_default(RGB_float_array, apply_VCS_to_RGB, 
                                        vcs_array);

        vcs_array->methods->free(&vcs_array->array);
        vcs_array->array = RGB_float_array;
}


/* apply_RGB_to_float
 * Description: Converts the data from the Pnm_rgb structs to
 *             RGB_float structs
 * Input:  
 *      int col - column of the UArray2
 *      int row - row of the UArray2
 *      A2Methods_UArray2 array2 - the UArray2 being mapped
 *      void *elem - the specific RGB_float in the array2
 *      void *cl - an A2_and_Methods struct to get data from
 * Output: None (converts and updates data from Pnm_rgb to 
 *               RGB_float)
 * Details: Converts each element in the mapped UArray to a
 *          RGB_float struct
 * Exceptions: None (excluding Hanson's)
 */
void apply_RGB_to_float(int col, int row, A2Methods_UArray2 array2, void *elem,
                        void *cl)
{
        (void) array2;

        A2_and_Methods source = cl;
        RGB_float pixel_float = elem;
        
        Pnm_rgb pixel_int = source->methods->at(source->array, col, row);

        pixel_float->red = (float) pixel_int->red / (float) DENOMINATOR;
        pixel_float->green = (float) pixel_int->green / (float) DENOMINATOR;
        pixel_float->blue = (float) pixel_int->blue / (float) DENOMINATOR;
}


/* apply_RGB_to_int
 * Description: Converts the data from the RGB_float structs to
 *              Pnm_rgb structs
 * Input:  
 *      int col - column of the UArray2
 *      int row - row of the UArray2
 *      A2Methods_UArray2 array2 - the UArray2 being mapped
 *      void *elem - the specific Pnm_rgb struct in the array2
 *      void *cl - an A2_and_Methods struct to get data from
 * Output: None (converts and updates data from RGB_float to 
 *               Pnm_rgb)
 * Details: Converts each element in the mapped UArray to a
 *          Pnm_rgb struct
 * Exceptions: None (excluding Hanson's)
 */
void apply_RGB_to_int(int col, int row, A2Methods_UArray2 array2, void *elem,
                      void *cl)
{
        (void) array2;

        A2_and_Methods source = cl;
        Pnm_rgb pixel_int = elem;

        RGB_float pixel_float = source->methods->at(source->array, col, row);

        float red = pixel_float->red * DENOMINATOR;
        float green = pixel_float->green * DENOMINATOR;
        float blue = pixel_float->blue * DENOMINATOR;

        pixel_int->red = checkDenom(round(red));
        pixel_int->green = checkDenom(round(green));
        pixel_int->blue = checkDenom(round(blue));
}


/* apply_RGB_to_VCS
 * Description: Converts the data from the Pnm_rgb structs to
 *             VCS_data structs
 * Input:  
 *      int col - column of the UArray2
 *      int row - row of the UArray2
 *      A2Methods_UArray2 array2 - the UArray2 being mapped
 *      void *elem - the specific VCS_data in the array2
 *      void *cl - an A2_and_Methods struct to get data from
 * Output: None (converts and updates data from RGB_float structs to 
 *               VCS_data struct data)
 * Details: Converts each element in the mapped UArray to a
 *          VCS_data struct
 * Exceptions: None (excluding Hanson's)
 */
void apply_RGB_to_VCS(int col, int row, A2Methods_UArray2 array2, void *elem, 
                      void *cl)
{
        (void) array2;

        A2_and_Methods source = cl;
        VCS_data pixel_vcs = elem;

        RGB_float rgb_float = source->methods->at(source->array, col, row);
        float g = rgb_float->green;
        float r = rgb_float->red;
        float b = rgb_float->blue;

        pixel_vcs->Y = 0.299 * r + 0.587 * g + 0.114 * b; 
        pixel_vcs->Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        pixel_vcs->Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
}


/* apply_VCS_to_RGB
 * Description: Converts the data from the VCS_data structs to
 *              RGB_float structs
 * Input:  
 *      int col - column of the UArray2
 *      int row - row of the UArray2
 *      A2Methods_UArray2 array2 - the UArray2 being mapped
 *      void *elem - the specific RGB_float struct in the array2
 *      void *cl - an A2_and_Methods struct to get data from
 * Output: None (converts and updates data from VCS_data structs to 
 *               RGB_float struct data)
 * Details: Converts each element in the mapped UArray to a
 *          RGB_float struct
 * Exceptions: None (excluding Hanson's)
 */
void apply_VCS_to_RGB(int col, int row, A2Methods_UArray2 array2, void *elem, 
                      void *cl)
{
        (void) array2;

        A2_and_Methods source = cl;
        RGB_float pixel_rgb_float = elem;

        VCS_data pixel_VCS = source->methods->at(source->array, col, row);
        float y = pixel_VCS->Y;
        float pb = pixel_VCS->Pb;
        float pr = pixel_VCS->Pr;

        pixel_rgb_float->red = 1.0 * y + 0.0 * pb + 1.402 * pr;
        pixel_rgb_float->green = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        pixel_rgb_float->blue = 1.0 * y + 1.772 * pb + 0.0 * pr;
}


/* checkDenom
 * Description: Checks if a value is greater than the denominator
 *              of the image.
 * Input:  
 *     int val - the value to check if its less than the denominator
 * Output: Returns either the upper or lower bounds of the range if 
 *         val is outside the range or returns val if it's in the range 
 * Details: Checks if the input is within 0 <= val <= 255
 * Exceptions: None
 */
int checkDenom(int val)
{
        if (val > 255) return 255; 
        else if(val < 0) return 0;

        return val;
}
