/**************************************************************
 *
 *                     codeword.c
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (cgolem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions to pack/unpack and output/read 
 *     32-bit words.
 *
 *
 **************************************************************/

#include <mem.h>
#include <stdint.h>
#include <stdlib.h>
#include "codeword.h"
#include "image_manipulation.h"

/* length of the encoded reprentation of 2x2 block of data in image */
static const int WORD_LENGTH = 32;

/* number of bytes that a word of WORD_LENGTH occupies */
static const int BYTES_IN_WORD = WORD_LENGTH / 8;


static void applyDecode(int i, int j, A2Methods_UArray2 dct, void *elem,
                        void *file_ptr);
static void applyEncode(int i, int j, A2Methods_UArray2 dct, void *elem,
                        void *cl);
static void putBytes(int64_t word, int bytes);
uint64_t getBytes(FILE *input, int num_bytes);


/* encodeImage
 * Description: writes an encoded representation of dct_compress array to 
 *              stdout
 * 
 * Input: A2_and_Methods dct_compress - uarray to encode
 *        int orig_width - origional width of uncompressed image
 *        int orig_height - origional height of uncompressed image
 * 
 * Output: prints the codewords to standard output
 * 
 * Details: calls the applyEncode function using map_default to encode each 
 *          dct_data element of array to a single WORD_LENGTH size code word.
 *          Writes these codewords row-major(top-left to top-right) wise to 
 *          stdout.
 * 
 * Exceptions: C.R.E if input passed is NULL or 0 is passed for width or height
 * 
 */
void encodeImage(A2_and_Methods dct_compress, int orig_width, int orig_height)
{
        checkInput(dct_compress);
        assert(orig_height > 0);
        assert(orig_width > 0);

        printf("COMP40 Compressed image format 2\n%u %u\n", 
                orig_width, orig_height);

        dct_compress->methods->map_default(dct_compress->array, applyEncode,
                                           NULL);
}


/* decodeImage
 * Description: decodes encoded reprentation of an image using data from input
 *              provided to this function.
 * 
 * Input: FILE *input - file to decode
 * 
 * Output: A2Methods_UArray2 type with DCT_data representing the image 
 * 
 * Details: reads from standard input codewords and uncompresses them
 * 
 * Exceptions: C.R.E if header is incorrect. Should be:
 *
 *               COMP40 Compressed image format 2
 *               width height
 *               *(data)*
 * 
 *             or if input passed is NULL
 */
A2Methods_UArray2 decodeImage(FILE *input)
{
        assert(input != NULL);

        unsigned width = 0;
        unsigned height = 0;
        
        int read = fscanf(input,
                        "COMP40 Compressed image format 2\n%u %u",
                        &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        int width_dct = width / 2;
        int height_dct = height / 2;

        A2Methods_UArray2 dct_array;
        dct_array = uarray2_methods_blocked->new_with_blocksize(width_dct, 
                                                                height_dct,
                                                                sizeof(struct
                                                                DCT_data),
                                                                1);
        assert(dct_array != NULL);

        uarray2_methods_blocked->map_default(dct_array, applyDecode, input);
        return dct_array;
}


/* applyEncode
 * Description: endodes data from UArray2 with DCT_data and creates a codeword
 *              for each element of that UArray2 based on lsb values provided 
 *              in specification of this assignment.
 *            
 * Input: col, int, dct,cl - unused parameters(needed to match apply function
 *                       params)
 *        elem - DCT_data of element of UArray2 that this function is called on
 * 
 * Output: Outputs each codeword to standard output
 * 
 * Details: encodes each codeword using the DCT_data struct's information 
 * 
 */
static void applyEncode(int col, int row, A2Methods_UArray2 dct, void *elem, 
                        void *cl)
{
        (void) col; 
        (void) row; 
        (void) dct;
        (void) cl; 

        DCT_data dct_elem = elem;
        uint64_t word = 0;

        word = Bitpack_newu(word, 6, 26, dct_elem->a);
        word = Bitpack_news(word, 6, 20, dct_elem->b);
        word = Bitpack_news(word, 6, 14, dct_elem->c);
        word = Bitpack_news(word, 6, 8, dct_elem->d);
        word = Bitpack_newu(word, 4, 4, dct_elem->PB_avg);
        word = Bitpack_newu(word, 4, 0, dct_elem->PR_avg);
        
        putBytes(word, BYTES_IN_WORD);  
}


/* applyDecode
 * Description: decodes data in input file and populates elements of UArray2 
 *              with DCT_data based on the code word decoding scheme.
 * 
 * Input: col, row, dct - unused parameters (needed to match apply function 
 *                                           params)
 *        elem - element of UArray2 that this function is called on.
 *        file_ptr - file to decode a single word from
 * 
 * Output: None (indirectly returns an updated DCT_data struct)
 * 
 * Details: reads WORD_LENGTH bits from file, creates a word with WORD_LENGTH
 *          bits, and creates DCT_data structs by calling Bitpack_get to get 
 *          bits from the word based on lsb values from the spec
 * 
 * Exceptions: None (excluding Hanson's)
 */
static void applyDecode(int col, int row, A2Methods_UArray2 dct, void *elem, 
                        void *file_ptr)
{
        (void) col;
        (void) row; 
        (void) dct;

        FILE *file = file_ptr;
        DCT_data dct_elem = elem;

        uint64_t word = getBytes(file, BYTES_IN_WORD);
        
        dct_elem->a = Bitpack_getu(word, 6, 26);
        dct_elem->b = Bitpack_gets(word, 6, 20);
        dct_elem->c = Bitpack_gets(word, 6, 14);
        dct_elem->d = Bitpack_gets(word, 6, 8);
        dct_elem->PB_avg = Bitpack_getu(word, 4, 4);
        dct_elem->PR_avg = Bitpack_getu(word, 4, 0);
}


/* getBytes
 * Description: Gets specified number of bytes from file and returns a 64 bit
 *              int representing the bytes read from the file.
 * 
 * Input: input - file to read from
 *        num_bytes - number of bytes to read from file
 * 
 * Output: uint64_t with data read from file
 * 
 * Details: last byte read from file is in right-most position in uint64
 *          Each subsequent byte is then stored starting at 8 positions to the
 *          left of the last byte read.
 * 
 * Exceptions: causes C.R.E if this function is called when there are no more 
 *             bytes to be read from the file or if the funciton is called when
 *             there are less than word_size number of bytes left to read in
 *             input file
 */
uint64_t getBytes(FILE *input, int num_bytes)
{
        uint64_t word = 0;

        int start = (num_bytes * 8) - 8;
        for (int lsb = start; lsb >= 0; lsb -= 8) {
                int byte = getc(input);
                assert(byte != EOF);

                word = Bitpack_newu(word, 8, lsb, byte);
        }

        return word;
}


/* putBytes
 * Description: Writes specified number of bytes from parameterized word to 
 *              stdout starting with the left-most bytes of the word, ending 
 *              with the bytes in the right most postion of the word. 
 *  
 * Input: word - word (64 bits of information) to write to stdout
 *        bytes - number of bytes to write from the word
 * 
 * Output: prints the codeword to standard output
 * 
 * Details: uses Bitpack_gets() to retrieve 8 bit portions of the codeword to
 *          print
 * Exceptions: None (excluding Hanson's)
 */
void putBytes(int64_t word, int bytes)
{
        for (int step = 8; step <= bytes * 8; step += 8) {
                /* WORD_LENGTH - step is the lsb to get each byte in word */
                uint64_t word_part = Bitpack_gets(word, 8, WORD_LENGTH - step);
                putchar((char)word_part);
        }
}