/**************************************************************
 *
 *                     um.c
 *
 *     Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *     This module is responsible for processing the command line and
 *     kickstarting the program.
 *
 **************************************************************/

#include "um_runner.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

FILE *open_file(char *filename);

int main(int argc, char **argv) 
{
        if (argc != 2) {
                fprintf(stderr, 
                "Usage: ./um some_program.um < testinput.txt > output.txt\n");
                
                exit(EXIT_FAILURE);
        }

        FILE *umfile = open_file(argv[1]);
        
        /* Checking if the file is empty */
        if (NULL != umfile) {
                fseek(umfile, 0, SEEK_END);
                int size = ftell(umfile);

                assert(size != 0);
                fseek(umfile, 0, SEEK_SET);
        }

        run_um(umfile);


        return 0;
}

/********** open_file ********
 *
 * Opens a provided file
 *
 * Parameters:
 *    char *filename: the filename from command line
 *
 * Return: 
 *      a file pointer
 *
 * Expects
 *      filename to not be NULL
 *      
 * Notes:
 *     May CRE if opened file is NULL
 * 
 ************************/
FILE *open_file(char *filename)
{
        /* creates a FILE pointer */
        FILE *file;

        /* checks that the file exists and reads through stdin if not */
        if (filename != NULL) {
                file = fopen(filename, "rb");
                if (file == NULL){
                        fprintf(stderr, "File: %s, cannot be opened\n", 
                                filename);
                        exit(EXIT_FAILURE);
                }
        } else {
                file = stdin;
                assert(file != NULL);
        }
    
        return file;
}