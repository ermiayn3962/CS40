/*************************************************************************
 *
 *                     restoration.c
 *
 *     Assignment: filesofpix
 *     Authors:  Yoda Ermias (yermia01) and Cooper Golemme (cgolem01) 
 *     Date:     9/10/2023
 *
 *     Summary:
 *          
 *    
 *
 ************************************************************************/


#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include <atom.h>
#include <seq.h>
#include <table.h>
#include <readaline.h>


/* Hanson Data Structure Methods */
extern const char *Atom_new(const char *str, int len);
extern Seq_T Seq_new(int hint);
extern void *Seq_addhi(Seq_T seq, void *x);
extern void *Seq_get(Seq_T seq, int i);
extern int Seq_length(Seq_T seq);
extern void Seq_free(Seq_T *seq);
extern void *Seq_remhi(Seq_T seq);
extern void *Seq_put(Seq_T seq, int i, void *x);
extern Table_T Table_new (int hint,
       int cmp(const void *x, const void *y),
       unsigned hash(const void *key));
extern void *Table_put(Table_T table, const void *key,
       void *value);
 extern void *Table_get   (Table_T table, const void *key);
extern void Table_free(Table_T *table);
extern void   Table_map    (Table_T table,
                            void apply(const void *key, void **value, void *cl),
                            void *cl);
static void vfree(const void *key, void **value, void *cl);

/* Program Specific Functions */
static FILE *open_or_abort(char *fname, char *mode);
const char * getSequence(char *datapp, int length); 
char *convertToString(Seq_T seq);
void freeSeq (Seq_T seq);
int convertToInt(char *line, int numElements);
int removeSequence(char *line, Seq_T *row, int numElements);
void imageBuilder(Seq_T seq, int numChar);
void buildLine (Seq_T *digit, Seq_T *uncrptLine, int *lineWidth);
const char * getCorrectSequence(char ** firstLine, char **secondLine, int *numItem, FILE *f);
void restoration(FILE *f);
void headerBuilder(int width, int height);
void printSeq(Seq_T seq);
char *copyString(char * copyFrom, int num_chars);

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 2){
        fprintf(stderr, "There are two few/many arguments to execute program\n");
        exit(EXIT_FAILURE);
    }
    FILE *f = open_or_abort(argv[1], "rb");
    restoration(f);

    fclose(f);
    return EXIT_SUCCESS;
}

/*----------------------------------------------------------------*
|
|                  Main Restoration Functions
|
*----------------------------------------------------------------*/
void restoration(FILE *f)
{    
    Seq_T image = Seq_new(0);
    const char *correct_sequence;
    char *firstLine, *secondLine;
    int num_chars_corrupted = 0;
    int num_chars_uncorrupted = 0;

    correct_sequence = getCorrectSequence(&firstLine, &secondLine, &num_chars_corrupted, f);
    fprintf(stderr, "this is seq: %s\n", correct_sequence);
    fprintf(stderr, "this is first: %s\n", firstLine);
    fprintf(stderr, "this is second: %s\n", secondLine);



    (void) num_chars_corrupted;
    (void) num_chars_uncorrupted;
    (void) correct_sequence;
    (void) firstLine;
    (void) secondLine;
    (void) image;
    //num_chars_uncorrupted = removeSequence(firstLine, &image, num_chars_corrupted);
 

    // removeSequence(secondLine, &image, num_chars_corrupted);

    // char *currCorruptLine;
    // const char *currCorruptSeq;

    // int height = 2;
    // while (feof(f) == 0 && currCorruptLine != NULL) {
    //     int num_chars = readaline(f, &currCorruptLine);

    //     currCorruptSeq = getSequence(currCorruptLine, num_chars);

    //     if (currCorruptSeq == correct_sequence) {
    //         height++;
    //         num_chars_uncorrupted = removeSequence(currCorruptLine, &image, num_chars);
    //     }
    //     free(currCorruptLine);
    // }

    // fprintf(stderr, "This is the width: %i\n", num_chars_uncorrupted);
    // fprintf(stderr, "This is the height: %i\n", height);

    // headerBuilder(num_chars_uncorrupted, height);
    // imageBuilder(image, num_chars_uncorrupted);

    // free(firstLine);
    // free(secondLine);
    // freeSeq(image);
    // Seq_free(&image);
}

void printSeq(Seq_T seq)
{
    printf("This is the length of seq: %i\n", Seq_length(seq));
    for (int i = 0; i < Seq_length(seq); i++) {
        printf("%i ", *(int *)Seq_get(seq, i));
    }
    printf("\n");
}


const char * getCorrectSequence(char **firstLine, char **secondLine, int *num_chars, FILE *f){
    Table_T table = Table_new(0, NULL, NULL);

    char *datapp;
    int temp = 0;
    *num_chars = readaline(f, &datapp);

    while (feof(f) == 0 && datapp != NULL){

        const char *sequence = getSequence(datapp, *num_chars);
        
        char *line = Table_get(table, sequence);

        if (line == NULL){
            Table_put(table, sequence, datapp);
            temp = readaline(f, &datapp);
            if (temp != 0) {
                *num_chars = temp;
            }

        }
        else {
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

char * copyString(char * copyFrom, int num_chars)
{
    Seq_T seq = Seq_new(0);
        for (int i = 0; i < num_chars; i++){
            char *c = malloc(sizeof(char));
            *c = copyFrom[i];
            Seq_addhi(seq, c);
        }
    char *result = convertToString(seq);
    freeSeq(seq);
    Seq_free(&seq);
    return result;
}

/********** getSequence ********
 *
 * Reads a C-String of a corrupted line and finds the 
 * non-digit character sequence for that line
 * Inputs:
 *		Takes in a char *datapp which holds the corrupted line
 *      and an int length which contains the size of the C-string
 *      passed.
 * Return: 
 *      Returns a char * to the sequence of non-digit characters found
 *      in the corrupted line
 * Notes: 
 *      A memory leak in valgrind occurs in the "still reachable" section
 *      due to the use of Atom Structure. 
 ************************/
const char * getSequence(char *datapp, int length)
{
    Seq_T line = Seq_new(0);

    //storing all non digit characters in a Seq_T
    for (int i = 0; i < length; i++) {
        if (datapp[i] < '0' || datapp[i] > '9') {
            Seq_addhi(line, &datapp[i]);
        }
    }

    //converting that Seq_T Line into an Atom_T
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
 *		Takes in a char *line of a corrupted line, an int numElements which
 *      contains the size of the line passed, and a char **result which will
 *      send back the non-digit free version of the line.
 * Return: 
 *      Returns the size of the non-digit free version of the line
 * Notes: 
 *      Space and newline characters are preserved in the removal of non-digit
 *      characters.
 ************************/
int removeSequence(char *line, Seq_T *row, int numElements)
{
    //Seq_T uncrptLine = Seq_new(0);
    Seq_T digit = Seq_new(0);

    int c = 0;
    int num_chars = 0; //TODO delete this
    while (c < numElements){

        //When a digit char is found, loops till the next non-digit char
        while (isdigit(line[c]) != 0) {
            Seq_addhi(digit, &line[c]);
            c++;
        }
            
        if (Seq_length(digit) != 0) {
            buildLine(&digit, row, &num_chars);

        }

        //Resetting digit sequence 
        Seq_free(&digit);
        digit = Seq_new(0);
        c++;
    }

    Seq_free(&digit);
    // fprintf(stderr, "this is seq_len: %i\n", Seq_length(*row));
    // fprintf(stderr, "this is the counted: %i\n", num_chars);
    return num_chars;

}



/********** imageBuilder ********
 *
 * Reads a C-string of a uncorrupted line and outputs the 
 * information in the format of a "raw" pgm file
 * Inputs:
 *		Takes in a char *line which contains the information of
 *      the uncorrupted line. Takes in an int num_chars which holds
 *      the number of characters the char *line holds. Then, lastly, takes
 *      in an int height which is the number of original rows before the 
 *      corruption.
 * Return: 
 *      Doesn't return anything specifically, but outputs data to the standard
 *      output.
 * Notes: 
 *      Converts the ascii decimal values for ' ' and '\n' and prints them as
 *      characters.
 ************************/
void imageBuilder(Seq_T seq, int numChars)
{
    (void) numChars;
    for (int i = 0; i < Seq_length(seq); i++) {
        char c = *(char *)Seq_get(seq, i);
        printf("%c", c);
        //printf("%i ", *(int *)Seq_get(seq, i));
        // if (c != '\n'){
        //     printf("%c", c);

        // }
    }

}

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
 *		Takes in a char *fname which contains the file name
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
    if (fp == NULL) {
            fprintf(stderr,
                    "Could not open file %s with mode %s\n",
                    fname,
                    mode);
            exit(EXIT_FAILURE);
    }
    return fp;
}

/********** convertToString ********
 *
 * Converts a sequence to a C-String.
 * Inputs:
 *		Takes in a Seq_T seq to be converted
 * Return: 
 *      Returns a char * with the data held inside the provided sequence.
 * Notes: 
 *      None
 ************************/
char *convertToString(Seq_T seq)
{
    char *sequence = malloc(Seq_length(seq) * sizeof(char));
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
 *		Takes in a Seq_T *digit which points to the sequence holding the 
 *      digit characters that need to be converted to integers. Also takes
 *      in another Seq_T *uncrptLine which will house the converted digit
 *      chars from the sequence digit.
 * Return: 
 *      Doesn't directly return anything. However, indirectly returns a 
 *      populated sequence uncrptLine and a reset sequence digit.
 * Notes: 
 *      Adds a space to the sequence uncrptLine after each converted int.
 ************************/
void buildLine(Seq_T *digit, Seq_T *uncrptLine, int *lineWidth)
{
    char *pixelValue;
    int *convertedValue = malloc(sizeof(int));

    pixelValue = convertToString(*digit);

    *convertedValue = convertToInt(pixelValue, Seq_length(*digit));

    //checkMaxVal(maxVal, *convertedValue);
    //Building the line w/ spaces between digits
    Seq_addhi(*uncrptLine, convertedValue);
    *lineWidth = *lineWidth + 1;
    free(pixelValue);
}

/********** convertToInt ********
 *
 * Converts a C-String of chars into an integer.
 * Inputs:
 *		Takes in a char *line which contains the characters to convert
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

// void checkMaxVal(int *max_val, int currDigit)
// {
//     if (*max_val < currDigit) {
//         *max_val = currDigit;
//     }
// }

/********** freeSeq ********
 *
 * Frees the items stored in a sequence
 * Inputs:
 *		Takes in a Seq_T seq to iterate through
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

static void vfree(const void *key, void **value, void *cl) 
{
    (void) key;
    (void) cl;
   free(*value);
}

