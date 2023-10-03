#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "except.h"
#include "pnmrdr.h"

#define T Pnmrdr_T
//typedef struct T *T;

static FILE *open_or_abort(char *fname, char *mode);
int findNumOfPixels(T pnmFile);
extern T Pnmrdr_new(FILE *fp);
extern  Pnmrdr_mapdata Pnmrdr_data(T rdr); 
extern const Except_T Pnmrdr_Badformat;
extern void Pnmrdr_free(T *rdr);
Pnmrdr_mapdata getHeader(T pnmFile);
void convertToRaw(T pnmFile);
extern unsigned Pnmrdr_get(T rdr);
//void printPgm(Pnmrdr_mapdata, T pnmFile);


int main(int argc, char **argv)
{
    if (argc < 2){
        fprintf(stderr, "There are two few arguments to execute program\n");
        exit(errno);
    }

    FILE *f = open_or_abort(argv[1], "rb");
    T pnmFile = Pnmrdr_new(f);

    convertToRaw(pnmFile);

    fclose(f);
    Pnmrdr_free(&pnmFile);
    return EXIT_SUCCESS;
}

//opens file from command line
static FILE *open_or_abort(char *fname, char *mode)
{
    FILE *fp = fopen(fname, mode);
    if (fp == NULL) {
            int rc = errno;
            fprintf(stderr,
                    "Could not open file %s with mode %s\n",
                    fname,
                    mode);
            exit(rc);
    }
    return fp;
}

//finds the number of pixels for a file
int findNumOfPixels(T pnmFile)
{
    Pnmrdr_mapdata map = getHeader(pnmFile);

    return map.width * map.height;
}

Pnmrdr_mapdata getHeader(T pnmFile)
{
    Pnmrdr_mapdata map = Pnmrdr_data(pnmFile); 

    return map;
}

//Converts plain pgm to raw pgm

void convertToRaw(T pnmFile)
{
    Pnmrdr_mapdata map = getHeader(pnmFile);
    map.type = 5;

    printf("P%d\n%d %d\n%d\n", map.type, map.width, map.height, 
            map.denominator);

    for (unsigned int i = 0; i < map.height; i++){
        for (unsigned int j = 0; j < map.width; j++){
           printf("%d ", Pnmrdr_get(pnmFile));
        }
        printf("\n");
    }
   
}


