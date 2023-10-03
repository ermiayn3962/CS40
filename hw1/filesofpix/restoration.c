/*************************************************************************
 *
 *                     restoration.c
 *
 *     Assignment: filesofpix
 *     Authors:  Yoda Ermias (yermia01) and Cooper Golemme (cgolem01) 
 *     Date:     9/10/2023
 *
 *     Summary: 
 *          This program takes in a corrupted pgm file and outputs the
 *          uncorrupted version to standard output. The "restoration"
 *          program removes all non-digit characters from the image
 *          raster provided, converts the remaining elements into integers,
 *          then outputs the integers as charactes through a typecast.
 * 
 *      Notes:
 *          There is a known bug in the program. When "raw" pgm files
 *          are created, they display in VSCode's PGM Extention perfectly.
 *          However, the images don't display properly on the Mac Finder
 *          file explorer. This issue is nonexistant when displaying "plain"
 *          pgm files.
 * 
 *          There is also another known bug when uncorrupting the 
 *          turing-corrupt.pgm. A valgrind "Invalid read of size 1" is raised
 *          on the 8th row of the corrupted file. The error is at the
 *          copyString function on line 530.
 * 
 ************************************************************************/


#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include <atom.h>
#include <seq.h>
#include <table.h>
#include <assert.h>
#include <readaline.h>


/* Hanson Data Structure Methods */
extern const char *Atom_new(const char *str, int len);
extern Seq_T Seq_new(int hint);
extern void *Seq_addhi(Seq_T seq, void *x);
extern void *Seq_get(Seq_T seq, int i);
extern int Seq_length(Seq_T seq);
extern void Seq_free(Seq_T *seq);
extern Table_T Table_new (int hint,
       int cmp(const void *x, const void *y),
       unsigned hash(const void *key));
extern void *Table_put(Table_T table, const void *key, void *value);
 extern void *Table_get(Table_T table, const void *key);
extern void Table_free(Table_T *table);
extern void Table_map(Table_T table,
                            void apply(const void *key, void **value,
                                       void *cl), void *cl);
static void vfree(const void *key, void **value, void *cl);


/* Program Specific Functions */
static FILE *open_or_abort(char *fname, char *mode);
const char * getSequence(char *datapp, int length); 
char *convertToString(Seq_T seq);
void freeSeq (Seq_T seq);
int convertToInt(char *line, int numElements);
int removeSequence(char *line, Seq_T *row, int numElements);
void imageBuilder(Seq_T seq);
void buildLine (Seq_T *digit, Seq_T *uncrptLine, int *lineWidth);
const char * getCorrectSequence(char ** firstLine, char **secondLine,
                                int *numItem, FILE *f);
void restoration(FILE *f, Seq_T *image);
void headerBuilder(int width, int height);
char *copyString(char * copyFrom, int num_chars);


/********** main ********
 *
 * Opens a file and calls the restoration function.
 * Inputs:
 *	    Takes in an int argc which is the number of arguments
 *      given in the command line and also takes in a char **
 *      argv which contains the data passed on the command line.
 * Return: 
 *      Returns EXIT_SUCCESS upon a successful run of the program.
 * Notes: 
 *      Outputs the "raw" pgm stored in Seq_T image to standard output.
 ************************/
int main(int argc, char **argv)
{
        /* Checking command line arguments */
        assert(argc == 2);

        FILE *f = open_or_abort(argv[1], "rb");
        Seq_T image = Seq_new(0);
        restoration(f, &image);
        imageBuilder(image);
        

        freeSeq(image);
        Seq_free(&image);
        fclose(f);
        return EXIT_SUCCESS;
}


/*----------------------------------------------------------------*
|
|                  Main Restoration Functions
|
*----------------------------------------------------------------*/


/********** restoration ********
 *
 * Calls the functions required to restore a corrupted pgm 
 * file. Outputs the header of the "raw" image to standard output
 * Inputs:
 *	    Takes in a FILE *f which points to the file opened in 
 *      main. Takes in a Seq_T *image which houses the pixelValues
 *      for the image.
 * Return: 
 *      Indirectly returns an updated image through the Seq_T *image,
 *      but doesn't directly return anything.
 * Notes: 
 *      Throws a Checked Runtime Error if the file provided is empty.
 ************************/
void restoration(FILE *f, Seq_T *image)
{    
        const char *correct_sequence;
        char *firstLine, *secondLine;
        int num_chars_corrupted = 0;
        int num_chars_uncorrupted = 0;

        /* Getting the correct sequence and the two lines it appears in */
        correct_sequence = getCorrectSequence(&firstLine, &secondLine, 
                                              &num_chars_corrupted, f);

        /* Checking if the file provided is empty */
        assert(correct_sequence != NULL);

        /* Removing the sequence from the two lines it first appears in */
        num_chars_uncorrupted = removeSequence(firstLine, image,
                                               num_chars_corrupted);
        removeSequence(secondLine, image, num_chars_corrupted);

        char *currCorruptLine;
        const char *currCorruptSeq;
        int height = 2;

        /* Comparing each line's sequence with correct sequence */
        int num_chars = readaline(f, &currCorruptLine);
        while (feof(f) == 0 && currCorruptLine != NULL && num_chars != 0) {
                currCorruptSeq = getSequence(currCorruptLine, num_chars);

                if (currCorruptSeq == correct_sequence) {
                        height++;
                        num_chars_uncorrupted = removeSequence(currCorruptLine,
                                                               image, 
                                                               num_chars);
                }

                free(currCorruptLine);
                num_chars = readaline(f, &currCorruptLine);
        }

        headerBuilder(num_chars_uncorrupted, height);
        free(currCorruptLine);
        free(firstLine);
        free(secondLine);

}

/********** getCorrectSequence ********
 *
 * Goes through file provided and searches for the first two lines
 * which contain the same sequence. Returns the position of the two
 * lines as well as the correct sequence they share.
 * Inputs:
 *	    Takes in a char **firstLine and char **secondLine which store
 *      the first two lines that share the same sequence. Also takes
 *      in an int *num_chars which contains the number of characters
 *      in the lines read as well as a FILE *f which contains the file
 *      to read through.
 * Return: 
 *      Returns an Atom (const char *) containing the correct sequence.
 * Notes: 
 *      Returns NULL if the file provided is empty. This will then be caught
 *      by an assert statement in the restoration function where 
 *      getCorrectSequence is called.
 ************************/
const char * getCorrectSequence(char **firstLine, char **secondLine, 
                                int *num_chars, FILE *f)
{
        Table_T table = Table_new(0, NULL, NULL);

        char *datapp;
        *num_chars = readaline(f, &datapp);

        /* Looping through file to find the first two lines w/ same sequence */
        while (feof(f) == 0 && datapp != NULL) {
                const char *sequence = getSequence(datapp, *num_chars);


                
                char *line = Table_get(table, sequence);

                if (line == NULL) {
                        Table_put(table, sequence, datapp);
                        *num_chars = readaline(f, &datapp);
                        
                }
                else {
                        /* Creating new allocations for the data stored in 
                         * line and datapp since when the Table_T table is
                         * freed, it wipes what *firstLine and *secondLine 
                         * are pointing to.
                         */

                        int temp = 0;
                        while (line[temp] != '\n') {
                            temp++;
                        }

                        *firstLine = copyString(line, *num_chars);
                        *secondLine = copyString(datapp, *num_chars);
                        
                        free(datapp);
                        Table_map(table, vfree, NULL);
                        Table_free(&table);
                        return sequence;
                }
        }
    
    return NULL;
}

/********** getSequence ********
 *
 * Reads a C-String of a corrupted line and finds the 
 * non-digit character sequence for that line.
 * Inputs:
 *      Takes in a char *datapp which holds the corrupted line
 *      and an int length which contains the size of the C-string
 *      passed.
 * Return: 
 *      Returns an Atom (const char *) to the sequence of non-digit 
 *      characters found in the corrupted line.
 * Notes: 
 *      A memory leak in valgrind occurs in the "still reachable" section
 *      due to the use of Atom Structures. 
 ************************/
const char * getSequence(char *datapp, int length)
{
        Seq_T line = Seq_new(0);

        /* storing all non digit characters in a Seq_T */
        for (int i = 0; i < length; i++) {
                if (datapp[i] < '0' || datapp[i] > '9') {
                Seq_addhi(line, &datapp[i]);
                }
        }

        /* converting the Seq_T Line into an Atom_T */
        char *sequence = convertToString(line);

        const char *seqStr = Atom_new(sequence, Seq_length(line));

        Seq_free(&line);
        free(sequence);
        return seqStr;
}

/********** removeSequence ********
 *
 * Parses through the given C-String for digit characters.
 * Then converts and places those digit characters into their
 * own C-String.
 * Inputs:
 *	Takes in a char *line of a corrupted line, a Seq_T *row, and
 *      an int numElements which contains the size of the line passed.
 * Return: 
 *      Returns the size of the non-digit free version of the line.
 * Notes: 
 *      All converted int pixelValues are stored in the Seq_T * passed.
 ************************/
int removeSequence(char *line, Seq_T *row, int numElements)
{
        Seq_T digit = Seq_new(0);

        int c = 0;
        int num_chars = 0; 
        while (c < numElements) {

                /* Loops until the next nonDigit char is hit */
                while (isdigit(line[c]) != 0) {
                        Seq_addhi(digit, &line[c]);
                        c++;
                }
                
                if (Seq_length(digit) != 0) {
                        buildLine(&digit, row, &num_chars);
                }

                /* Resetting the Seq_T digit for next iteration in loop */ 
                Seq_free(&digit);
                digit = Seq_new(0);

                c++;
        }

        Seq_free(&digit);
        return num_chars;
}

/********** imageBuilder ********
 *
 * Reads in a Seq_T of Pixel values and outputs the 
 * information in the format of a "raw" pgm file
 * Inputs:
 *      Takes in a Seq_T seq which holds the integer values of
 *      the pixelValues.
 * Return: 
 *      Doesn't return anything specifically, but outputs data to the standard
 *      output.
 * Notes: 
 *      Converts the integer values stored in the passed sequence for 
 *      characters.
 ************************/
void imageBuilder(Seq_T seq)
{
        for (int i = 0; i < Seq_length(seq); i++) {
                char c = *(char *)Seq_get(seq, i);
                printf("%c", c);
        }

}

/********** headerBuilder ********
 *
 * Prints out the header for the "raw" pgm file.
 * Inputs:
 *	Takes in an int width and int height which respectively contain
 *      the values for the width and height of the "raw" pgm image.
 * Return: 
 *      Doesn't return anything specifically, but outputs data to the standard
 *      output.
 * Notes: 
 *      None.
 ************************/
void headerBuilder(int width, int height)
{
        printf("P5\n");
        printf("%i %i\n", width, height);
        printf("255\n");
}


/*----------------------------------------------------------------*
|
|                       Helper Functions
|
*----------------------------------------------------------------*/


/********** open_or_abort ********
 *
 * Opens a file specified from the command line
 * Inputs:
 *	Takes in a char *fname which contains the file name
 *      to open and another char *mode which specifies how to
 *      read the file.
 * Return: 
 *      Returns a FILE * to the file opened
 * Notes: 
 *      Outputs error messages if the file could not be opened.
 ************************/
static FILE *open_or_abort(char *fname, char *mode)
{
        FILE *fp = fopen(fname, mode);

        /* Checking if file was opened successfully */
        assert(fp != NULL);

        return fp;
}

/********** convertToString ********
 *
 * Converts a sequence to a C-String.
 * Inputs:
 *	Takes in a Seq_T seq which contains data to be converted into
 *      a C-String.
 * Return: 
 *      Returns a char * with the data from the provided sequence.
 * Notes: 
 *      None.
 ************************/
char *convertToString(Seq_T seq)
{
        char *sequence = malloc(Seq_length(seq) * sizeof(char));

        /* Checking if memory was allocated correctly */
        assert(sequence != NULL);

        for (int i = 0; i < Seq_length(seq); i++) {
                sequence[i] = *(char*) Seq_get(seq, i);
        }

        return sequence;
}

/********** buildLine ********
 *
 * Populates the sequence uncrptLine using the information from the
 * sequence digit. 
 * Inputs:
 *	Takes in a Seq_T *digit which points to the sequence holding the 
 *      digit characters that need to be converted to integers. Also takes
 *      in another Seq_T *uncrptLine which will house the converted digit
 *      chars from the sequence digit and an int *lineWidth which contains
 *      the number of rows in the "raw" pgm image.
 * Return: 
 *      Doesn't directly return anything. However, indirectly returns a 
 *      populated sequence uncrptLine and integer lineWidth.
 * Notes: 
 *      None.
 ************************/
void buildLine(Seq_T *digit, Seq_T *uncrptLine, int *lineWidth)
{
        char *pixelValue;
        int *convertedValue = malloc(sizeof(int));

        /* Checking if memory was allocated correctly */
        assert(convertedValue != NULL);

        pixelValue = convertToString(*digit);

        *convertedValue = convertToInt(pixelValue, Seq_length(*digit));

        /* Building the uncrptLine */
        Seq_addhi(*uncrptLine, convertedValue);
        *lineWidth = *lineWidth + 1;

        free(pixelValue);
}

/********** convertToInt ********
 *
 * Converts a C-String of chars into an integer.
 * Inputs:
 *	Takes in a char *line which contains the characters to convert
 *      to ints as well as an int numElements which contains the size
 *      of the C-String passed.
 * Return: 
 *      Returns the converted integer.
 * Notes: 
 *      None.
 ************************/
int convertToInt(char *line, int numElement)
{
        int result = 0;
        for (int i = 0; i < numElement; i++) {
                result = result * 10 + (line[i] - '0');
        }
        return result;
}

/********** freeSeq ********
 *
 * Frees the items stored in a sequence
 * Inputs:
 *	Takes in a Seq_T seq to iterate through
 * Return: 
 *      None.
 * Notes: 
 *      None.
 ************************/
void freeSeq (Seq_T seq)
{
        while (Seq_length(seq) != 0) {
                free(Seq_remhi(seq));
        }

}

/********** vfree ********
 *
 * The apply function for Table_map method in the <Table.h> library.
 * Inputs:
 *	Takes in void *'s for the table's key and cl. Takes in a void **value
 *      for the value of paired to the key in the table.
 * Return: 
 *      None.
 * Notes: 
 *      Frees the values stored inside the table.
 * 
 *      Leaves the key and cl arguments surpressed since they aren't used
 *      (function was directly copied from the Hanson Pages).
 ************************/
static void vfree(const void *key, void **value, void *cl) 
{
        (void) key;
        (void) cl;
        free(*value);
}

/********** copyString ********
 *
 * Allocates memory on the heap for the string that needs to be
 * copied and copies the C-String provided
 * Inputs:
 *	Takes in a char *copyFrom which contains a C-String and
 *      an int num_chars which contains the size of the C-String
 *      passed.
 * Return: 
 *      Returns a char * of the copied string
 * Notes: 
 *      Mainly used so that heap memory can be freed without data
 *      being lost.
 ************************/
char * copyString(char *copyFrom, int num_chars)
{
        Seq_T seq = Seq_new(0);
                for (int i = 0; i < num_chars; i++) {
                        char *c = malloc(sizeof(char));

                        /* Checking if memory was allocated correctly */
                        assert(c != NULL);

                        *c = copyFrom[i];
                        Seq_addhi(seq, c);
                }

        char *result = convertToString(seq);

        freeSeq(seq);
        Seq_free(&seq);
        
        return result;
}