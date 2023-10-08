#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "transformations.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

FILE *get_file_to_read(const char *file_name);



/*
TODO:
- Assert/Edge Cases
- Style
- README

*/


int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        int mappingType = 0;
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                        mappingType = 1;
                        methods->map_default = methods->map_col_major;
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                        mappingType = 2;
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];

                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        /* Opening and checking the image file */
        FILE *fp = NULL;

       // printf("argc: %i\n", argc);
        if (*argv[argc - 1] != '-' && argc > 1) {
                fp = get_file_to_read(argv[argc - 1]);
                assert(fp != NULL);

        } else {
                fp = get_file_to_read(NULL);
                assert(fp != NULL);
        }

        
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        /* Reading in image and making the struct to hold it */
        transform(image, rotation, methods, time_file_name, mappingType);

        Pnm_ppmfree(&image);
        fclose(fp);
        

}

FILE *get_file_to_read(const char *file_name)
{
        /* creates a FILE pointer */
        FILE *file;

        /* checks that the file exists and reads through stdin if not */
        if (file_name != NULL) {
                file = fopen(file_name, "rb");
                assert(file != NULL);
        } else {
                file = stdin;
                assert(file != NULL);
        }
    
        return file;
}