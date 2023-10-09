/**************************************************************
 *
 *                     transformations.c
 *
 *     Assignment: locality
 *     Authors: Maiah Islam (mislam07) and Yoda Ermias (yermia01)
 *     Date: Oct 8, 2023
 * 
 *     Contains the functions to transform a given source image
 *
 *
 **************************************************************/

#include "transformations.h"

static A2Methods_UArray2 createTransformed(Pnm_ppm source, int rotation);

static void updateSource(Pnm_ppm *source, A2Methods_UArray2 transformedImg, 
                         A2Methods_T methods);

static void apply_rotate180(int col, int row, A2Methods_UArray2 UArr2, 
                            void *elem, void *cl);

static void apply_rotate90(int col, int row, A2Methods_UArray2 UArr2, 
                           void *elem, void *cl);

static void apply_rotate270(int col, int row, A2Methods_UArray2 UArr2, 
                            void *elem, void *cl);

static void make_report(Pnm_ppm source, int rotation, double time_used, 
                        char *time_file_name, int mappingType);



/********** transform ********
 *
 * Applies the user selected transformations to the provided source
 * image.
 * 
 * Inputs:
 *      - Pnm_ppm source: ppm image to transform
 *      - int rotation: the rotation that will be applied to the image
 *      - A2Methods_T methods: the set of functions available for the image.
 *      - char *time_file_name: the name of the output file for the timer
 *      - int mappingType: the representation for the mapping type applied 
 *                         during image transformation
 *          
 * Return: 
 *      - None (void function)
 *
 *
 * Notes: 
 *      - None
 *
 ************************/
void transform(Pnm_ppm source, int rotation, A2Methods_T methods, 
               char *time_file_name, int mappingType)
{       

        CPUTime_T timer;
        A2Methods_UArray2 transformedImg;

        
        /* Checking if a A2Methods_UArray2 needs to be initialized */
        if (rotation != 0) {
                transformedImg = createTransformed(source, rotation);
        }

        /* Checking if the time flag was used */
        if (time_file_name != NULL) {
                timer = CPUTime_New();
                CPUTime_Start(timer);
        }

        /* Transformations */
        if (rotation == 90) {
                /* flip width and height */
                source->methods->map_default(transformedImg, apply_rotate90,
                                             source);
        } else if (rotation == 180) {
                source->methods->map_default(transformedImg, apply_rotate180, 
                                             source);
        } else if (rotation == 270) {
                /* flip width and height */
                source->methods->map_default(transformedImg, apply_rotate270,
                                             source);
        }
        
        double time_used;
        if (time_file_name != NULL) {
                time_used = CPUTime_Stop(timer);
        }
        

        if (rotation != 0) {
                updateSource(&source, transformedImg, methods);
        } 
        

        if (time_file_name != NULL) {
                make_report(source, rotation, time_used, time_file_name, 
                            mappingType);
                CPUTime_Free(&timer);
        }
        
        Pnm_ppmwrite(stdout, source);
}

/********** make_report ********
 *
 * Creates the timer report for the transformations conducted on 
 * the source image.
 * 
 * Inputs:
 *      - Pnm_ppm source: ppm image to transform
 *      - int rotation: the rotation that will be applied to the image
 *      - double time_used: the amount of time used to transform the image
 *      - char *time_file_name: the name of the output file for the timer
 *      - int mappingType: the representation for the mapping type applied 
 *                         during image transformation
 *          
 * Return: 
 *      - None (void function)
 *
 *
 * Notes: 
 *      - Creates a file using the provided timer file name with the 
 *        appropriate
 *        timing information 
 *
 ************************/
void make_report(Pnm_ppm source, int rotation, double time_used, 
                 char *time_file_name, int mappingType)
{
        FILE *timings_file = fopen(time_file_name, "a");
        double time_per_pixel = time_used / (source->height * source->width);

        
        fprintf(timings_file, "Rotation: %i, Width: %i, Height: %i\n", 
                rotation, source->width, source->height);
        if (mappingType == 0) {
                fprintf(timings_file, "Mapping Type: Row Major\n");
                
        } else if (mappingType == 1) {
                fprintf(timings_file, "Mapping Type: Col Major\n");
                
        } else if (mappingType == 2) {
                fprintf(timings_file, "Mapping Type: Block Major\n");
                
        }
        fprintf(timings_file, "Total Time (ns): %f\n", time_used);
        fprintf(timings_file, "Average Time Per Pixel (ns): %f\n",
                time_per_pixel);
        fprintf(timings_file, "\n");


        fclose(timings_file);

}

/********** createTransformed ********
 *
 *  Create a copy of the source to store transformed image in with the 
 *  appropriate new dimensions.
 * 
 * Inputs:
 *      - Pnm_ppm source: ppm image to be transformed
 *      - int rotation: the rotation to be performed
 *             
 * Return: 
 *      - a copy of the source image with new dimensions
 *
 * Notes: 
 *      - none
 *
 ************************/
A2Methods_UArray2 createTransformed(Pnm_ppm source, int rotation)
{       
        int width, height;

        /* Flips the height and row depending on the rotation */
        if (rotation == 90 || rotation == 270) {
                width = source->methods->height(source->pixels);
                height = source->methods->width(source->pixels);
       
        }
        else {
                width = source->methods->width(source->pixels);
                height = source->methods->height(source->pixels);
        }

        int size = source->methods->size(source->pixels);

        return source->methods->new(width, height, size);
}


/********** updateSource ********
 *
 *  Update the source image after the transformation has been performed.
 * 
 * Inputs:
 *      - Pnm_ppm *source: ppm image that was transformed
 *      - A2Methods_UArray2 transformedImg: the transformed pixels
 *      - A2Methods_T methods: the set of functions available for the image
 *             
 * Return: 
 *      - nothing (void)
 *
 * Notes: 
 *      - none
 *
 ************************/
void updateSource(Pnm_ppm *source, A2Methods_UArray2 transformedImg, 
                  A2Methods_T methods)
{
        methods->free(&(*source)->pixels);
        (*source)->pixels = transformedImg;
        (*source)->width = methods->width(transformedImg);
        (*source)->height = methods->height(transformedImg);
}


/********** apply_rotate90 ********
 *
 *  Transform the source image with a 90 degree rotation.
 * 
 * Inputs:
 *      - int col: column of current pixel
 *      - int row: row of current pixel
 *      - A2Methods_UArray2 UArr2: voided
 *      - void *elem: voided
 *      - void *cl: the pnm_ppm struct representing the source image
 *          
 * Return: 
 *      - nothing (void)
 *
 * Notes: 
 *      - none
 *
 ************************/
void apply_rotate90(int col, int row, A2Methods_UArray2 UArr2, void *elem, 
                    void *cl)
{
        (void) UArr2;
        (void) elem;

        Pnm_ppm img = cl; /* this is the source image */
        int height = img->height;

        int ogCol = height - 1 - col;
        int ogRow = row;

        *(Pnm_rgb) elem = *(Pnm_rgb) img->methods->at(img->pixels, ogRow, 
                                                      ogCol);

}


/********** apply_rotate180 ********
 *
 *  Transform the source image with a 180 degree rotation.
 * 
 * Inputs:
 *      - int col: column of current pixel
 *      - int row: row of current pixel
 *      - A2Methods_UArray2 UArr2: voided
 *      - void *elem: voided
 *      - void *cl: the pnm_ppm struct representing the source image
 *          
 * Return: 
 *      - nothing (void)
 *
 * Notes: 
 *      - none
 *
 ************************/
void apply_rotate180(int col, int row, A2Methods_UArray2 UArr2, void *elem, 
                     void *cl) 
{
        (void) UArr2;
        (void) elem;
        Pnm_ppm img = cl; /* this is the source image */
        int height = img->height;
        int width = img->width;

        int ogCol = width - col - 1;
        int ogRow = height - row - 1;

        *(Pnm_rgb) elem = *(Pnm_rgb) img->methods->at(img->pixels, ogCol, 
                                                      ogRow);
}


/********** apply_rotate270 ********
 *
 *  Transform the source image with a 270 degree rotation.
 * 
 * Inputs:
 *      - int col: column of current pixel
 *      - int row: row of current pixel
 *      - A2Methods_UArray2 UArr2: voided
 *      - void *elem: voided
 *      - void *cl: the pnm_ppm struct representing the source image
 *          
 * Return: 
 *      - nothing (void)
 *
 * Notes: 
 *      - none
 *
 ************************/
void apply_rotate270(int col, int row, A2Methods_UArray2 UArr2, void *elem, 
                     void *cl)
{
        (void) UArr2;
        (void) elem;

        Pnm_ppm img = cl; /* this is the source image */
        int width = img->width;

        int ogCol = width - 1 - row;
        int ogRow = col;

        *(Pnm_rgb) elem = *(Pnm_rgb) img->methods->at(img->pixels, ogCol, 
                                                      ogRow);

}