/**************************************************************
 *
 *                     unblackededges.c
 *
 *     Assignment: iii
 *     Authors: Liam Smith (lsmith26) and Yoda Ermias (yermia01)
 *     Date: Sep 28, 2023
 * 
 *     For a pbm image of bits, black edges and adjacent black bits
 *     are convered to white.
 *
 *
 **************************************************************/

#include "bit2.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "pnmrdr.h"
#include "stack.h"

FILE *get_file_to_read(const char *file_name);
bool checkFileValidity(Pnmrdr_T image);

Bit2_T populateBit2(Pnmrdr_T image);
void populateHelper(int col, int row, Bit2_T bArr, int value, void *image);

void checkAdjacentBit(Bit2_T *bArr, int col, int row);
void checkEdges(Bit2_T *bArr, int col, int row);
void removeEdges(Bit2_T *bArr);
struct blackBit *newBlackBit(Bit2_T bArr, int col, int row);

void printUnblackedImage(Bit2_T bArr, Pnmrdr_T image);

struct blackBit
{
        int bit;
        int col;
        int row;
};

int main(int argc, char **argv)
{
        /* one argument corresponds to stdin and two arguments corresponds
        to a file being passed as input */
        assert(argc <= 2);
        FILE *fp = NULL;

        if (argc == 1) {
                fp = get_file_to_read(NULL);
                assert(fp != NULL);

        } else if (argc == 2) {
                fp = get_file_to_read(argv[1]);
                assert(fp != NULL);
        }

        /* image is created based on input */
        Pnmrdr_T image = Pnmrdr_new(fp); 

        /* if file is not valid 9 x 9 format, exit failure is called */
        if (!checkFileValidity(image)) {
                Pnmrdr_free(&image);
                return EXIT_FAILURE;
        } 

        /* resets file pointer */
        fseek(fp, SEEK_SET, 0);
        assert(fp != NULL);


        /* recreates image for new passing through and population */
        image = Pnmrdr_new(fp);
        Bit2_T bVector = populateBit2(image);
        assert(bVector != NULL);


        /* removes edges and prints image with updated white bits */
        removeEdges(&bVector);
        printUnblackedImage(bVector, image);

        /* frees bit vector, pnmrdr, and closes file */
        Bit2_free(&bVector);
        Pnmrdr_free(&image);
        fclose(fp);
        return EXIT_SUCCESS;
}


/**********printUnblackedImage********
 *
 * 
 * Prints an updated image with unblacked edges, containing just zero and one
 * bit values.
 *             
 * Inputs:
 *      - Pnmrdr_T image: holds the updated pnm file with zero and one bit
 *      values
 *      - Bit2_T barr: A Bit2_T instance with width, height, and a bit vector 
 *      with length based on width and height dimensions.
 * 
 * Return: bool valued true if image is valid, false otherwise
 * 
 * 
 * Notes: 
 *      - asserts that data and width are not zero
 *      - asserts that bArr exists
 *              
 *
 ************************/
void printUnblackedImage(Bit2_T bArr, Pnmrdr_T image)
{
        assert(bArr != NULL);

        /* creates pnmrdr data with height and width values */
        Pnmrdr_mapdata data = Pnmrdr_data(image);

        /* prints according to P1 format */
        printf("P1\n%u %u\n", data.width, data.height);

        /* looping through Bit to print black and white values */
        for (int i = 0; i < Bit2_height(bArr); i++) {
                for (int j = 0; j < Bit2_width(bArr); j++) {
                        printf("%i", Bit2_get(bArr, j, i));
                        if (j != Bit2_width(bArr) - 1) {
                                printf(" "); 
                        }
                }
                printf("\n");
        }
}

/**********checkFileValidity********
 *
 * Checks bit file as valid input based on proper format with valid width and
 * height. A Pnmrdr image is temporarily created and freed to allow
 * for checking.
 *             
 * Inputs:
 *      - Pnmrdr_T image: holds the pnm file to check the validity of
 * 
 * Return: bool valued true if image is valid, false otherwise
 * 
 * 
 * Notes: 
 *      - asserts that data and width are not zero
 *              
 *
 ************************/
bool checkFileValidity(Pnmrdr_T image)
{
        /* creates pnmrdr data with height and width values */
        Pnmrdr_mapdata data = Pnmrdr_data(image);

        /* bool valid to check file validity */
        bool valid = false;

        /* assert that file width and height are greater than zero */
        assert(data.width != 0);
        assert(data.height != 0);

        /* valid set to true if data and height are greater than zero */
        if (data.width > 0 && data.height > 0) {
                valid = true;
        }

        /* freeing of image that was temporarily used for valid checks */
        Pnmrdr_free(&image);
        return valid;
}

/**********populateUarray2********
 *
 * Creates a Bit2_T and populates it with a valid bit map including
 * rows and columns populated with 0 and 1 values. Population occurs using
 * row major function where rows are populated in succession.
 *              
 * Inputs:
 *      - Pnmrdr_T image: holds the pnm file to be populated with bit values
 *
 * Return: bVector as a populated Bit2_T with valid bit values
 *      
 * Notes: 
 *       - asserts that bVector exists
 *              
 *
 ************************/
Bit2_T populateBit2(Pnmrdr_T image)
{
        /* creation of pnmrdr image with height and width values */
        Pnmrdr_mapdata data = Pnmrdr_data(image);

        /* creation of Bit2_T based on width and height dimensions */
        Bit2_T bVector = Bit2_new(data.width, data.height);
        assert(bVector != NULL);

        /* bVector is populated with image values based on row major */
        Bit2_map_row_major(bVector, populateHelper, &image);

        return bVector;
}

/**********populateHelper********
 *
 * Helper function that serves as an "apply" function called in
 * Bit22_map_row_major. Assists with reading values and populating
 * a Bit2_T of bit values.
 *            
 * Inputs:  
 *      - int col: a column index as an integer used for idenifying the bit's
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit's
 *                  row
 *      - Bit2_T bArr: A Bit2_T instance with width, height, and a bit vector 
 *                   with length based on width and height dimensions.
 *      - int value: bit value that is stored in the Bit2_T
 *      - void *image: a void pointer converted to a Pnmrdr_T instance so
 *                 Pnmrdr_get is able to access the bit value
 * 
 * Return: none (void)
 *      
 * Notes: 
 *      - asserts that bArr exists
 *              
 *
 ************************/
void populateHelper(int col, int row, Bit2_T bArr, int value, void *image)
{
        assert(bArr != NULL);

        /* value is cast from image to an integer then is put into Bit2_T */
        value = (int) Pnmrdr_get(*(Pnmrdr_T *)image);

        Bit2_put(bArr, col, row, value);
}

/**********removeEdges********
 *
 * Removes the edges of a pbm file. Calls on the checkEdge function to determine
 * whether or not a black bit is read, so the connected black bits can be
 * converted to white.
 *     
 * Inputs:        
 *      - Bit2_T *bArr: A Bit2_T instance with width, height, and a bit vector 
 *      with length based on width and height dimensions.
 *
 * Return: none (void)
 * 
 *      
 * Notes: 
 *      - asserts that bArr exists
 *              
 ************************/
void removeEdges(Bit2_T *bArr)
{
        assert(bArr != NULL);

        /* checks for top and bottom rows */
        for (int col = 0; col < Bit2_width(*bArr); col++) {
                /* 0th row */
                checkEdges(bArr, col, 0);
                
                /* height-1th row */
                checkEdges(bArr, col, Bit2_height(*bArr) - 1);
        }
        
        /* checks for left and right columns */
        for (int row = 0; row < Bit2_height(*bArr); row++) {
                /* 0th col */
                checkEdges(bArr, 0, row);

                /* width - 1 col */
                checkEdges(bArr, Bit2_width(*bArr) - 1, row);
        }
}

/**********checkEdges********
 *
 * Determines if a bit on an edge is black or white. If the edge is black,
 * checkAdjacentBit is called to ensure that all black bits can be converted.
 *     
 * Inputs:    
 *      - Bit2_T *bArr: A Bit2_T instance with width, height, and a bit 
 *              vector with length based on width and height dimensions.
 *      - int col: a column index as an integer used for idenifying the bit's
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit's
 *                  row
 * 
 * Return: none (void)
 * 
 *      
 * Notes: 
 *      - asserts that bArr exists
 *              
 ************************/
void checkEdges(Bit2_T *bArr, int col, int row)
{
        assert(bArr != NULL);

        /* Checking if bit is black */
        if (Bit2_get(*bArr, col, row) == 1) {
                checkAdjacentBit(bArr, col, row);
        }
}

/**********checkAdjacentBit********
 *
 * Checks adjacent bits if a bit is black by indexing column and row and
 * asserting that bits outside of range are not attempted to be read.
 *     
 * Inputs:        
 *      - Bit2_T *bArr: A Bit2_T instance with width, height, and a bit vector 
 *      with length based on width and height dimensions.
 *      - int col: a column index as an integer used for idenifying the bit's
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit's
 *                  row
 *
 * Return: none (void)
 * 
 *      
 * Notes: 
 *      - asserts that bArr exists
 *      - asserts that created bits exist
 *              
 *
 ************************/
void checkAdjacentBit(Bit2_T *bArr, int col, int row) 
{
        assert(bArr != NULL);

        /* creation of Stack that will hold black bits */
        Stack_T blackBits = Stack_new();

        /* assertion that stack was created */
        assert(blackBits != NULL);

        /* creation of intiial bit used for foundational search */
        struct blackBit *initialBit = newBlackBit(*bArr, col, row);
        assert(initialBit != NULL);


        /* pushing of black bit onto stack */
        Stack_push(blackBits, initialBit);

        /* while stack is not empty, black bits continue to be found */
        while (Stack_empty(blackBits) != 1) {

                /* bit stored with struct that holds bit, row, and column */
                struct blackBit *cBit = (struct blackBit *)Stack_pop(blackBits);
                assert(cBit != NULL);
     
                /* conversion from black to white bits at black location */
                Bit2_put(*bArr, cBit->col, cBit->row, 0);

                /* Checks up */
                if (cBit->row - 1 > 0) {
                        if (Bit2_get(*bArr, cBit->col, cBit->row - 1) == 1) {
                                struct blackBit *upBit = newBlackBit(*bArr, 
                                        cBit->col, cBit->row - 1);
                                assert(upBit != NULL);
                                Stack_push(blackBits, upBit);
                        }
                }

                /* Checks right */
                if (cBit->col + 1 < Bit2_width(*bArr)) {
                        if (Bit2_get(*bArr, cBit->col + 1, cBit->row) == 1) {
                                struct blackBit *rightBit = newBlackBit(*bArr, 
                                        cBit->col + 1, cBit->row);
                                assert(rightBit != NULL);
                                Stack_push(blackBits, rightBit);
                        }
                }

                /* Checks down */
                if (cBit->row + 1 < Bit2_height(*bArr)) {
                        if (Bit2_get(*bArr, cBit->col, cBit->row + 1) == 1) {
                                struct blackBit *downBit = newBlackBit(*bArr, 
                                        cBit->col, cBit->row + 1);
                                assert(downBit != NULL);
                                Stack_push(blackBits, downBit);
                        }
                }
    

                /* Checks left */
                if (cBit->col - 1 > 0) {
                        if (Bit2_get(*bArr, cBit->col - 1, cBit->row) == 1) {
                                struct blackBit *leftBit = newBlackBit(*bArr, 
                                        cBit->col - 1, cBit->row);
                                assert(leftBit != NULL);
                                Stack_push(blackBits,leftBit);
                        
                        }
                }
                /* freeing of bit */
                free(cBit);
        }
        /* freeing of stack */
        Stack_free(&blackBits);
}

/**********newBlackBit********
 *
 * A struct is set with a bit value and its location via row and column
 * indeces. The bit is returned with these values as struct members.
 *     
 * Inputs:        
 *      - Bit2_T bArr: A Bit2_T instance with width, height, and a bit vector 
 *      with length based on width and height dimensions.
 *      - int col: a column index as an integer used for idenifying the bit's
 *                  column
 *      - int row: a row index as an integer used for idenifying the bit's
 *                  row
 * 
 * Return: struct blackBit * as a struct containing bit values and their
 * row and column indeces.
 * 
 *      
 * Notes: 
 *       - asserts that currBit was malloced   
 *      - asserts that bArr exists    
 *
 ************************/
struct blackBit *newBlackBit(Bit2_T bArr, int col, int row) 
{
        assert(bArr != NULL);

        /* malloc of space for currBit */
        struct blackBit *currBit = malloc(sizeof(struct blackBit));
        assert(currBit != NULL);

        /* population of currBit's values based on bit, row, and col */
        currBit->bit = Bit2_get(bArr, col, row);
        currBit->col = col;
        currBit->row = row;

        return currBit;
}

/**********get_file_to_read********
 *
 * Purpose:
 *      Opens a file if program is reading from
 *      a file or sets the reading location to
 *      standard input if no file is supplied.
 *     
 * Inputs:  
 *      Reads in a const char *file_name which 
 *      holds the name of the file to read from.   
 * 
 * Return:
 *      Returns a FILE * to the file opened or to
 *      stdin depending on what is passed.
 *  
 * Notes:
 *      -If a NULL pointer is passed as the file_name
 *      the function sets the program to read from
 *      stdin.
 *      Credit to filesofpix solution

 *
 ************************/
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