/**************************************************************
 *
 *                     sudoku.c
 *
 *     Assignment: iii
 *     Authors: Liam Smith (lsmith26) and Yoda Ermias (yermia01)
 *     Date: Sep 28, 2023
 * 
 *     For properly formatted 9 x 9 files, the files are interpreted as
 *     sudoku solutions or not accordng to typical game rules.
 *
 *
 **************************************************************/

#include "uarray2.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "pnmrdr.h"


FILE *get_file_to_read(const char *file_name);
bool checkFileValidity(Pnmrdr_T image);

UArray2_T populateUArray2(Pnmrdr_T image);
void populateHelper(int col, int row, UArray2_T uArr, void *value, void *null);

void resetSlots(int arr[]);
bool checkSlots(int arr[]);

bool checkSquare(UArray2_T uArr);
bool checkRow(UArray2_T uArr);
bool checkColumn(UArray2_T uArr);
bool checkSudoku(UArray2_T uArr);


int main(int argc, char **argv)
{
        /* one argument corresponds to stdin and two arguments corresponds
        to a file being passed as input */
        assert(argc <= 2);
        FILE *fp = NULL;

        if (argc == 1) {
                fp = stdin;
                assert(fp != NULL);

        } else if (argc == 2) {
                fp = get_file_to_read(argv[1]);
                assert(fp != NULL);
        }

        /* image is created based on input */
        Pnmrdr_T image = Pnmrdr_new(fp);

        /* if file is not valid 9 x 9 format, exit failure is called */
        if (!checkFileValidity(image)) {
                fclose(fp);
                return EXIT_FAILURE;
        }
        
        /* resets file pointer */
        fseek(fp, SEEK_SET, 0);
        assert(fp != NULL);

        /* recreates image for new passing through and population */
        image = Pnmrdr_new(fp);
        UArray2_T sudoku = populateUArray2(image);
        assert(sudoku != NULL);

        /* checking for sudoku solution */
        bool isSudoku = checkSudoku(sudoku);

        /* free sudoku UArray and Pnmrdr image and closes */
        UArray2_free(&sudoku);
        Pnmrdr_free(&image);
        fclose(fp);

        /* returns success if solution valid, false otherwise */
        if (isSudoku) {
                return EXIT_SUCCESS;

        } else {
                return EXIT_FAILURE;
        }
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

/**********checkFileValidity********
 *
 * Checks sudoku file as valid input based on proper format 
 * with nine rows, columns and maxval. A Pnmrdr image is 
 * temporarily created and freed to allow for checking.
 *             
 * Inputs:
 *      - Pnmrdr_T image: holds the pnm file to check the validity of
 * 
 * Return: 
 *      bool true if image is valid, false otherwise
 * 
 * 
 * Notes:
 *      - Checks if the size of the pnm is 9 x 9, otherwise
 *      returns false and ends program with EXIT_FAILURE.
 * 
 *      - Checks if the values inside the Pnmrsr_T are in the 
 *        range of 1-9, otherwise returns false for file validity.
 * 
 ************************/
bool checkFileValidity(Pnmrdr_T image)
{
        /* creates data with width height and denominator variables */
        Pnmrdr_mapdata data = Pnmrdr_data(image);

        /* bool valid to check file validity */
        bool valid = false;

        /* valid becomes true if height and width are of valid sudoku format */
        if (data.width == 9 && data.height == 9 && data.denominator == 9) {
                valid = true;
        /* if invalid input, false is returned and image is freed */
        } else {
                Pnmrdr_free(&image);
                assert(image != NULL);
                return false;
        }

        /* if valid dimensions, values are checked to be between 1 and 9 */
        for (unsigned int i = 0; i < data.width * data.height; i++) {
                unsigned curr = Pnmrdr_get(image);
                if (curr < 1 || curr > 9) {
                        valid = false;
                }
        }
        
        /* image is freed */
        Pnmrdr_free(&image);
        assert(image == NULL);

        return valid; 
}

/**********populateUarray2********
 *
 * Creates a UArray2_T and populates it with a valid sudoku puzzle including
 * nine rows and columns populated with integer values. Population occurs using
 * row major function where rows are populated in succession.
 *              
 * Inputs:
 *      - Pnmrdr_T image: holds the pnm file to check the validity of
 *
 * Return: 
 *      sudokuArray as a populated UArray2_T with valid sudoku values
 *      
 * Notes: 
 *      - None
 * 
 ************************/
UArray2_T populateUArray2(Pnmrdr_T image) 
{
        /* data is created with width and height variables */
        Pnmrdr_mapdata data = Pnmrdr_data(image);

        /* UArray2_T is created based on width and height dimsensions
        with space allocated for integers */
        UArray2_T sudokuArray = UArray2_new(data.width, data.height, 
                                sizeof(int));
        assert(sudokuArray != NULL);
        
        /* population occurs through row major funcitonality */
        UArray2_map_row_major(sudokuArray, populateHelper, &image);

        return sudokuArray;
}

/**********populateHelper********
 *
 * Helper function that serves as an "apply" function called in
 * UArray2_map_row_major. Assists with reading values and populating
 * a UArray2_T of sudoku values.
 *            
 * Inputs:  
 *      - int col: voided parameter
 *      - int row: voided parameter
 *      - UArray2_T uArr: voided parameter
 *      - void *value: a void pointer converted to an integer pointer
 *      - void *image: a void pointer converted to a Pnmrdr_T instance so
 *                 Pnmrdr_get is able to access the unsigned bit value
 * 
 * Return: 
 *      none (void)
 *      
 * Notes: 
 *      - Used as an apply function to the mapping functions.
 *      - Sets the indices of the UArray2_T uArr to the bit from the
 *        Pnmrdr_T passed in the closure argument.
 *      - asserts that uArr exists
 * 
 ************************/
void populateHelper(int col, int row, UArray2_T uArr, void *value, void *image)
{
        assert(uArr != NULL);

        /* creation of bit as a pnmrdr unsigned value from image */
        unsigned bit = Pnmrdr_get(*(Pnmrdr_T *)image);

        /* voided variables */
        (void) col;
        (void) row;
        (void) uArr;

        /* bit is converted to an integer pointer */
        *(int *)value = (int)bit;
}

/**********checkSquare********
 *
 * Checks if a 3 x 3 square in a sudoku file contains every integer from 1 to 9
 * which are valid sudoku values if every value appears with no repeats in a
 * square.
 *           
 * Inputs: 
 *      - UArray2_T uArr: A UArray2_T as a "vertical" UArray populated with
 *              "horizontal" UArrays. Here, the populated values come are in
 *              a 9 x 9 sudoku format where vertical arrays are columns and
 *              horizontal arrays are rows.
 * 
 * Return: 
 *      bool valued true if the square contains valid values, false otherwise
 *      
 * Notes: 
 *      - asserts that uArr exists
 *              
 ************************/
bool checkSquare(UArray2_T uArr)
{   
        assert(uArr != NULL);

        /* initlaization of column and row position variables */
        int col_pos = 0;
        int row_pos = 0;

        /* slots array will hold counts of integers 1 - 9 */
        int slots[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        /* Looping through the 3x3 boxes and incrimenting slots based on 
        occurances of values 1 - 9 */
        for (int box = 0; box < 9; box++) {

                /* Iterating through the rows of the 3x3 box */
                for (int row = row_pos; row < row_pos + 3; row++) {

                        /* Iterating through the columns of the 3x3 box */
                        for (int col = col_pos; col < col_pos + 3; col++) {
                                int value = *(int *) UArray2_at(uArr, col, row);
                                slots[value - 1]++; 
                        }
                }
        
                /* for each square, checks that slots contains one of each 
                value 1 - 9 which means each square is valid */
                if (!checkSlots(slots)) {
                        return false;
                }

                /* resets slots array for next square to be read */
                resetSlots(slots);

                /* incrimenting of column positon until the end is reached */
                if (col_pos < 9) {
                        col_pos += 3;
                }

                /* once at the end of a column, resetting to the start of 
                the column and incrimenting to the next rows */
                if (row_pos < 9 && col_pos == 9) {
                        row_pos += 3;
                        col_pos -= 9;
                }
        }
        return true;
}

/**********checkRow********
 *
 * Checks if a 1 x 9 horizontal row contains every integer from 1 to 9, which
 * are valid sudoku values if every value appears with no repeats in a row.
 *         
 * Inputs:
 *      - UArray2_T uArr: A UArray2_T as a "vertical" UArray populated with
 *              "horizontal" UArrays. Here, the populated values come are in
 *              a 9 x 9 sudoku format where vertical arrays are columns and
 *              horizontal arrays are rows.
 *    
 * Return: 
 *      bool true if the row contains valid values, false otherwise
 * 
 *      
 * Notes: 
 *      - asserts that uArr exists
 *              
 ************************/
bool checkRow(UArray2_T uArr)
{
        assert(uArr != NULL);

        /* slots array will hold counts of integers 1 - 9 */
        int slots[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        /* loops through all rows, converting UArray2_at return to 
        ints, the populating a slots array*/
        for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                        int value = *(int *) UArray2_at(uArr, j, i);
                        slots[value - 1]++; 
                }
                /* Check if the slots have values 0 or 1 and set boolean */
                if (!checkSlots(slots)) {
                        return false;
                }
                /* Reset slots to initialized values */
                resetSlots(slots);
        }
        return true;
}

/**********checkColumn********
 *
 * Checks if a 1 x 9 vertical column contains every integer from 1 to 9, which
 * are valid sudoku values if every value appears with no repeats in a column.
 *         
 * Inputs:
 *      - UArray2_T uArr: A UArray2_T as a "vertical" UArray populated with
 *              "horizontal" UArrays. Here, the populated values come are in
 *              a 9 x 9 sudoku format where vertical arrays are columns and
 *              horizontal arrays are rows.
 *    
 * Return: 
 *      bool true if the column contains valid values, false otherwise
 * 
 * Notes: 
 *      - asserts that uArr exists.
 *              
 ************************/
bool checkColumn(UArray2_T uArr)
{
        assert(uArr != NULL);

        /* slots array will hold counts of integers 1 - 9 */
        int slots[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        /* loops through all columns, converting UArray2_at return to 
        ints, the populating a slots array */
        for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                        int value = *(int *) UArray2_at(uArr, i, j);
                        slots[value - 1]++; 
                }
                /* Check if the slots have values 0 or 1 and set boolean */
                if (!checkSlots(slots)) {
                        return false;
                }
                /* Reset slots */
                resetSlots(slots);
        }
        return true;
}

/**********checkSudoku********
 *
 * Checks that every row, column, and box in the sudoku UArray2_T is valid.
 *           
 * Inputs:   
 *      - UArray2_T uArr: A UArray2_T as a "vertical" UArray populated with
 *              "horizontal" UArrays. Here, the populated values come are in
 *              a 9 x 9 sudoku format where vertical arrays are columns and
 *              horizontal arrays are rows.
 * 
 * Return: 
 *      bool values true if the sudoku array is a valid solution, false
 *      otherwise
 * 
 * Notes: 
 *     - asserts that uArr exists
 * 
 ************************/
bool checkSudoku(UArray2_T uArr)
{
        assert(uArr != NULL);

        /* checks square, row, and column boolean values to ensure that
        entire sudoku puzzle is a valid solution */

        bool square = checkSquare(uArr);
        bool row = checkRow(uArr);
        bool column = checkColumn(uArr);
        
        return (square && row && column);
}

/**********resetSlots********
 *
 * Resets an integer array of values created during the reading of values
 * to validate sudoku rows, columns, and squares. Every value in the array
 * is reset to zero to serve as an initialized counter array.
 *          
 * Inputs:    
 *      - int arr[]: an integer array of values that holds values based on the
 *                   count of different integers that appear in a sudoku row, 
 *                   column, or square.
 * 
 * Return: 
 *      None (void)
 * 
 * Notes: 
 *      - Resets the integers inside the slot int arr[] so they can be used
 *        used on the next 3 x 3 submap.
 *              
 ************************/
void resetSlots(int arr[]) 
{
        /* reinitializes values of slot array to zeros to represent empty */
        for (int i = 0; i < 9; i++) {
                arr[i] = 0;
        }
}

/**********checkSlots********
 *
 * Checks that all nine slots of an integer array have a value of 1, meaning 
 * the sudoku row, column, or box is valid, or not.
 *          
 * Inputs:    
 *      - int arr[]: an integer array of values that holds values based on the
 *          count of different integers that appear in a sudoku row, column, or
 *          square.

 * 
 * Return: bool valued true if all 1s are read, false otherwise
 * 
 *      
 * Notes: 
 *       - Checks if the slot int arr[] only contains 1's inside each index
 *         since every index represents a digit 1-9 and the number stored in
 *         the indice are the counts of those digits.
 *              
 ************************/
bool checkSlots(int arr[])
{
        /* checks that all slot values are equal to 1, returning true if so */
        for (int i = 0; i < 9; i++) {
                if (arr[i] != 1) {
                        return false;
                }
        }
        return true;
}