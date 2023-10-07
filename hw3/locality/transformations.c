#include "transformations.h"

static A2Methods_UArray2 createTransformed(Pnm_ppm source, int rotation);

static void updateSource(Pnm_ppm *source, A2Methods_UArray2 transformedImg, A2Methods_T methods);

static void apply_rotate180(int col, int row, A2Methods_UArray2 UArr2, void *elem, void *cl);

static void apply_rotate90(int col, int row, A2Methods_UArray2 UArr2, void *elem, void *cl);

static void rotate180(Pnm_ppm source, int rotation, A2Methods_T methods);

static void rotate90(Pnm_ppm source, int rotation, A2Methods_T methods);



void transform(Pnm_ppm image, int rotation, A2Methods_T methods)
{        
        if (rotation == 90) {
                /* pixel (i, j) in the original becomes pixel (h − j − 1, i) in the rotated image. */
                /* flip width and height */
                rotate90(image, rotation, methods);
                
        } else if (rotation == 180) {
                /* keep width and height the same */
               rotate180(image, rotation, methods);
                /* When the image is rotated 180 degrees, pixel (i, j) becomes pixel (w − i − 1, h − j − 1). */
        } else { /* for 0 degree */
                printf("Inside zero\n");
                FILE *fp = fopen("test3.ppm", "wb");
                Pnm_ppmwrite(fp, image); 
        }
}

A2Methods_UArray2 createTransformed(Pnm_ppm source, int rotation)
{       
        int width, height;
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

void updateSource(Pnm_ppm *source, A2Methods_UArray2 transformedImg, A2Methods_T methods)
{
        methods->free(&(*source)->pixels);
        (*source)->pixels = transformedImg;
        (*source)->width = methods->width(transformedImg);
        (*source)->height = methods->height(transformedImg);
}

void rotate90(Pnm_ppm source, int rotation, A2Methods_T methods)
{
        A2Methods_UArray2 transformedImg = createTransformed(source, rotation);
        
        source->methods->map_default(transformedImg, apply_rotate90, source);

        updateSource(&source, transformedImg, methods);

        FILE *fp = fopen("test.ppm", "w");

        Pnm_ppmwrite(fp, source);
        fclose(fp);
}



/* Apply function that rotates the image */
void apply_rotate90(int col, int row, A2Methods_UArray2 UArr2, void *elem, void *cl)
{
        (void) UArr2;
        (void) elem;

        Pnm_ppm img = cl; // this is the source
        int height = img->height;

        int ogCol = height - 1 - col;
        int ogRow = row;

        *(Pnm_rgb) elem = *(Pnm_rgb) img->methods->at(img->pixels, ogRow, ogCol);

}


void rotate180(Pnm_ppm source, int rotation, A2Methods_T methods)
{
        A2Methods_UArray2 transformedImg = createTransformed(source, rotation);
        
        source->methods->map_default(transformedImg, apply_rotate180, source);
        
        updateSource(&source, transformedImg, methods);

        FILE *fp = fopen("test5.ppm", "w");

        Pnm_ppmwrite(fp, source);
        fclose(fp);

}

void apply_rotate180(int col, int row, A2Methods_UArray2 UArr2, void *elem, void *cl) 
{
        (void) UArr2;
        (void) elem;
        Pnm_ppm img = cl; // this is the source
        int height = img->height;
        int width = img->width;

        int ogCol = width - col - 1;
        int ogRow = height - row - 1;

        *(Pnm_rgb) elem = *(Pnm_rgb) img->methods->at(img->pixels, ogCol, ogRow);
}












