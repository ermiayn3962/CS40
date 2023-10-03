/*************************************************************************
 *
 *                     readaline.c
 *
 *     Assignment: filesofpix
 *     Authors:  Yoda Ermias (yermia01) and Cooper Golemme (cgolem01) 
 *     Date:     9/10/2023
 *
 *     Summary:
 *          This file holds the implementation for the readaline function
 *          necessary for the restoration.c program file. The readaline
 *          function will directly output the length of the line read and
 *          indirectly output (through the char **datapp pointer) the line
 *          read from the file.
 *    
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <readaline.h>

int getArraySize(FILE *f);

/********** readaline ********
 *
 * Reads a line in from a provided file and returns
 * a C-String of that line and the length of the line
 * Inputs:
 *	Takes in a FILE *inputfd to the specific file to read and
 *      a char **datapp which is a pointer to a char * which holds
 *      the C-String.
 * Return: 
 *      Directly returns the size of the line read, but also
 *      indirectly returns the line read from the file through
 *      the char **datapp pointer.
 * Notes: 
 *      C-String line is created with an extra index to accomodate
 *      the ending sentinel.
 * 
 *      Works with any line regardless of the size of that line.
 * 
 *      Throws checked runtime errors when either or both arguments
 *      are NULL, when an error occurs reading from the file, and
 *      when memory allocation fails.
 * 
 *      Doesn't free heap memory used. The location which calls 
 *      readaline is responsible for freeing memory.
 ************************/
size_t readaline(FILE *inputfd, char **datapp)
{
        /* Checking if arguments are valid */
        assert(inputfd != NULL && datapp != NULL);

        int widthCounter = 0;
        int num_elements = getArraySize(inputfd);
        
        char *line;
        if (num_elements != 0) {
                line = (char*) malloc(num_elements * sizeof(char));
        }

        int curr = fgetc(inputfd);

        /* Check if there was an error reading from the file */
        assert(ferror(inputfd) == 0);

        /* Checking if the end of the file has been reached */
        if (curr == EOF) {
                *datapp = NULL;
                return 0; 
        }

        /* Checking if memory was allocated correctly */
        assert(line != NULL);

        while ((curr != '\n') && (feof(inputfd) == 0) && (curr != EOF)) {
                line[widthCounter] = curr;
                

                widthCounter++;
                curr = fgetc(inputfd);
        }

        if (curr == '\n') {
                line[widthCounter] = curr;
                widthCounter++;    
        }

        *datapp = line;
        return widthCounter;
}


/********** getArraySize ********
 *
 * Reads a line from the inputted file and returns the size
 * of the file to create an array for C-Strings with it.
 * Inputs:
 *	Takes in a FILE *f to the specific file to read 
 * Return: 
 *      The width of the line the File pointer is currently on
 * Notes: 
 *      Makes use of File position methods to preserve the 
 *      original spot the File pointer was initially located
 *      at.
 * 
 *      Uses the fseek() method to reset File pointer to initial
 *      position before returning the size of the line.
 * 
 *      Increases the size count by one for the '\n' char since
 *      they must be stored in the line.
 ************************/
int getArraySize(FILE *f)
{
        /* Saving current location of file */
        int currFilePos = ftell(f);
        int arraySize = 0;

        int curr = fgetc(f);

        while ((curr != '\n') && (feof(f) == 0)) {
                arraySize++;
                curr = fgetc(f);
        }

        /* Resetting file pointer position to previously saved position */
        fseek(f, currFilePos, SEEK_SET);

        if (curr == '\n') {
                arraySize++;
        }

        return arraySize;
}
