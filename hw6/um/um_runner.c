/**************************************************************
 *
 *                     um_runner.c
 *
 *      Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *     This module is used to start the program. The function defined here is
 *     accessible to the client.  
 *
 **************************************************************/

#include "um_runner.h"
#include "file_io.h"
#include "memory.h"

#include <uarray.h>


/********** run_um ********
 *
 * Runs the universal emulator
 *
 * Parameters:
 *     FILE *file: the file with the instructions
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      file to not be NULL
 *      
 * Notes:
 *     May CRE if file is NULL
 * 
 ************************/
void run_um(FILE *um_file)
{
    assert(um_file != NULL);

    UM_Memory memory = UM_Memory_New(um_file);

    process_instructions(memory);

    /* clean up memory */
    clean_up_memory(&memory);
    fclose(um_file);
   
}