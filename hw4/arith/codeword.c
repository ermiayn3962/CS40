#include "codeword.h"
#include <mem.h>

static const int WORD_LENGTH = 32;

static void applyDecode(int i, int j, A2Methods_UArray2 dct, void *elem, void *file_ptr);
static void applyEncode(int i, int j, A2Methods_UArray2 dct, void *elem, void *cl);
static int getBits(int num_bits, FILE *input);
static void putBytes(int64_t word, int bytes);

void encodeImage(A2_and_Methods dct_compress, int orig_width, int orig_height)
{
    printf("COMP40 Compressed image format 2\n%u %u", orig_width, orig_height);
    dct_compress->methods->map_default(dct_compress->array, applyEncode, NULL);
}

A2_and_Methods decodeImage(FILE *input)
{
    unsigned width = 0;
    unsigned height = 0;
    int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", &width, &height);
    assert(read == 2);

    int c = getc(input);
    assert(c == '\n');

    //create 2dArray with abcdPrPB stuff
    int blocksize = 2;
    int width_dct = (width + blocksize - 1) / 2;
    int height_dct = (height + blocksize - 1) / 2;


    A2Methods_UArray2 dct_array = uarray2_methods_blocked->new_with_blocksize(width_dct, height_dct,  sizeof(struct DCT_data), 1);

    uarray2_methods_blocked->map_default(dct_array, applyDecode, input);
    return dct_array;
}

static void applyEncode(int i, int j, A2Methods_UArray2 dct, void *elem, void *cl)
{
    (void) i; (void) j; (void) dct;
    (void)cl;

    DCT_data dct_elem = elem;
    int64_t word = 0;

    Bitpack_newu(word, 9, 23, dct_elem->a);
    Bitpack_news(word, 5, 18, dct_elem->b);
    Bitpack_news(word, 5, 13, dct_elem->c);
    Bitpack_news(word, 5, 8, dct_elem->d);
    Bitpack_newu(word, 4, 4, dct_elem->PB_avg);
    Bitpack_newu(word, 4, 0, dct_elem->PR_avg);
    
    putBytes(word, 4);  
}

static void applyDecode(int i, int j, A2Methods_UArray2 dct, void *elem, void *file_ptr)
{
    (void) i; (void) j; (void) dct;
    FILE *file = file_ptr;
    DCT_data dct_elem = elem;
    dct_elem->a = getBits(9, file);
    dct_elem->b = getBits(5, file);
    dct_elem->c = getBits(5, file);
    dct_elem->d = getBits(5, file);
    dct_elem->PB_avg = getBits(4, file);
    dct_elem->PR_avg = getBits(4, file);
}

int getBits(int num_bits, FILE *input)
{
    int data = 0;
    for (int i = num_bits - 1; i >= 0; i--) {
        int bit = getc(input) << i;
        data = data | bit;
    }
    return data;
}

void putBytes(int64_t word, int bytes)
{
    for (int step = 8; step <= bytes * 8; step += 8) {
        char word_part = Bitpack_gets(word, 8, WORD_LENGTH - step);
        putchar(word_part);
    }  
}