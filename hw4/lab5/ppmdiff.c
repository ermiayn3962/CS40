#include <stdlib.h>
#include <stdbool.h>
#include <pnm.h>
#include <a2methods.h>
#include <math.h>
#include <a2blocked.h>

static A2Methods_T methods;
typedef A2Methods_UArray2 A2;

void apply_pixel_data(int i, int j, A2 array2, void *elem, void *cl);
double mean_squared_diff(Pnm_ppm source_image, Pnm_ppm dest_image);
bool correctDim(Pnm_ppm larger, Pnm_ppm smaller);
int calc_denominator(Pnm_ppm larger_image);
Pnm_ppm make_A2(char *filename);
FILE *open_or_fail(char *filename, char *mode);

typedef struct image_and_sum {
    Pnm_ppm image;
    double sum;
    double source_denom;
} image_and_sum;

int main(int argc, char **argv)
{

        if (argc != 3) {
                fprintf(stderr, "USAGE: ./ppmdiff source_image_name other_image_name\n");
                return EXIT_FAILURE;
        }
        methods = uarray2_methods_blocked;
        char *source_name = argv[1];
        char *dest_name = argv[2];
        Pnm_ppm source_image = make_A2(source_name);
        Pnm_ppm dest_image = make_A2(dest_name);
        (void) source_image;
        (void) dest_image;


        double msdiff = mean_squared_diff(source_image, dest_image);

        printf("%f\n", msdiff);

        return EXIT_SUCCESS;
}




void apply_pixel_data(int i, int j, A2 array2, void *elem, void *cl)
{
        (void) array2;
        
        image_and_sum *data = cl;

        Pnm_rgb rgb_source = elem;
        Pnm_rgb rgb_dest = methods->at(data->image->pixels, i, j);

        double s_denominator = data->source_denom;
        double d_denominator = data->image->denominator;
        
        double square_red_diff = pow((double)rgb_source->red / s_denominator - (double)rgb_dest->red / d_denominator, 2);
        double square_blue_diff = pow((double)rgb_source->blue / s_denominator - (double)rgb_dest->blue /d_denominator, 2);
        double square_green_diff = pow((double)rgb_source->green / s_denominator - (double)rgb_dest->green / d_denominator, 2);

        data->sum += square_red_diff + square_blue_diff + square_green_diff;
}

double mean_squared_diff(Pnm_ppm source_image, Pnm_ppm dest_image)
{
        Pnm_ppm larger_image;
        Pnm_ppm smaller_image;

        if (source_image->height * source_image->width > dest_image->height * dest_image->width) {
                larger_image = source_image;
                smaller_image = dest_image;
        } else {
                larger_image = dest_image;
                smaller_image = source_image;
        }
        
        if (!correctDim(larger_image, smaller_image)){
            exit(EXIT_FAILURE);
        }

        image_and_sum data;
        data.image = larger_image;
        data.sum = 0;
        data.source_denom = smaller_image->denominator;

        methods->map_default(smaller_image->pixels, apply_pixel_data, &data);

        double numerator = data.sum;  
        double denominator;

        denominator = calc_denominator(smaller_image);

        return sqrt(numerator / denominator);

}


bool correctDim(Pnm_ppm larger, Pnm_ppm smaller)
{

        if ((int) (larger->height - smaller->height) < 2 && (int) (larger->width - smaller->width) < 2 &&
            (int) (larger->height - smaller->height) >= 0 && (int)(larger->width - smaller->width) >= 0) {
                return true;        
        }

        return false;
}

int calc_denominator(Pnm_ppm image)
{
        return 3 * image->height * image->width;
}

Pnm_ppm make_A2(char *filename)
{
        /* if filename is null, read through stdin, otherwise read fp becomes
        a pointer to filename */
        FILE *fp = (filename == NULL) ? stdin : open_or_fail(filename, "rb");

        /* data created after file is read and mapped according to method */
        Pnm_ppm data = Pnm_ppmread(fp, methods);
        fclose(fp);

        return data;
}

FILE *open_or_fail(char *filename, char *mode)
{
        FILE *fp = fopen(filename, mode);

        /* checks if file can be opened */
        if (fp == NULL){
                fprintf(stderr, "File: %s could not be opened\n", filename);
                exit(EXIT_FAILURE);
        }
        
        return fp;
}