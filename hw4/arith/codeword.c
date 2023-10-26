#include "codeword.h"
#include <mem.h>
#include <stdint.h>
#include <stdlib.h>

static const int WORD_LENGTH = 32;
int decodeval = 0;

static void applyDecode(int i, int j, A2Methods_UArray2 dct, void *elem, void *file_ptr);
static void applyEncode(int i, int j, A2Methods_UArray2 dct, void *elem, void *cl);
// static int getBits(int num_bits, FILE *input);
static void putBytes(int64_t word, int bytes, int *num);
uint64_t getBytes(int num_bytes, FILE *input);

void encodeImage(A2_and_Methods dct_compress, int orig_width, int orig_height)
{
    printf("COMP40 Compressed image format 2\n%u %u\n", orig_width, orig_height);
    int val = 0;
    dct_compress->methods->map_default(dct_compress->array, applyEncode, &val);
    fprintf(stderr, "numTimes put byptes: %i\n", val);
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
    // int blocksize = 2;
    int width_dct = (width) / 2;
    int height_dct = (height) / 2;

    A2Methods_UArray2 dct_array = uarray2_methods_blocked->new_with_blocksize(width_dct, height_dct,  sizeof(struct DCT_data), 1);


    uarray2_methods_blocked->map_default(dct_array, applyDecode, input);
    fprintf(stderr, "num decode: %d \n", decodeval);
    return dct_array;
}

static void applyEncode(int i, int j, A2Methods_UArray2 dct, void *elem, void *cl)
{
    (void) i; (void) j; (void) dct;
    (void) cl; (void) elem;

    // fprintf(stderr, "(%i, %i)\n", i, j);

    DCT_data dct_elem = elem;
    uint64_t word = 0x0;
    // fprintf(stderr, "a: %u\n",  dct_elem->a);
    // fprintf(stderr, "b: %i\n",  dct_elem->b);
    // fprintf(stderr, "c: %i\n",  dct_elem->c);
    // fprintf(stderr, "d: %i\n", dct_elem->d);
    // fprintf(stderr, "PB_avg: %u\n",  dct_elem->PB_avg);
    // fprintf(stderr, "PR_avg: %u\n", dct_elem->PR_avg);
    // fprintf(stderr, "\n");

    // printf("value: %u\n", dct_elem->a);

    // printf("value b: %i\n", dct_elem->b);

    // printf("value c: %i\n", dct_elem->c);
    word = Bitpack_newu(word, 9, 23, dct_elem->a);
    word = Bitpack_news(word, 5, 18, dct_elem->b);
    word = Bitpack_news(word, 5, 13, dct_elem->c);
    word = Bitpack_news(word, 5, 8, dct_elem->d);
    word = Bitpack_newu(word, 4, 4, dct_elem->PB_avg);
    word = Bitpack_newu(word, 4, 0, dct_elem->PR_avg);
    
    putBytes(word, 4, cl);  
}

static void applyDecode(int i, int j, A2Methods_UArray2 dct, void *elem, void *file_ptr)
{
    (void) i; (void) j; (void) dct;
    FILE *file = file_ptr;
    DCT_data dct_elem = elem;

    uint64_t word = getBytes(4, file);
    
    dct_elem->a = Bitpack_getu(word, 9, 23);
    dct_elem->b = Bitpack_gets(word, 5, 18);

    dct_elem->c = Bitpack_gets(word, 5, 13);
    dct_elem->d = Bitpack_gets(word, 5, 8);
    dct_elem->PB_avg = Bitpack_getu(word, 4, 4);
    dct_elem->PR_avg = Bitpack_getu(word, 4, 0);

    // fprintf(stderr, "a: %u\n",dct_elem->a);
    // fprintf(stderr, "b: %i\n",dct_elem->b);
    // fprintf(stderr, "c: %i\n",dct_elem->c);
    // fprintf(stderr, "d: %i\n",dct_elem->d);
    // fprintf(stderr, "Pb_avg: %u\n",dct_elem->PB_avg);
    // fprintf(stderr, "Pr_avg: %u\n",dct_elem->PR_avg);
    // fprintf(stderr, "\n");


    //  fprintf(stderr, "Pb_avg: %u\n",dct_elem->PB_avg);
    // fprintf(stderr, "Pr_avg: %u\n",dct_elem->PR_avg);
    // fprintf(stderr, "a: %u\n",dct_elem->a);
    // fprintf(stderr, "b: %i\n",dct_elem->b);
    // fprintf(stderr, "c: %i\n",dct_elem->c);
    // fprintf(stderr, "d: %i\n",dct_elem->d);


}

// int getBits(int num_bits, FILE *input)
// {
//     int data = 0;
//     for (int i = num_bits - 1; i >= 0; i--) {
//         int bit = getc(input) << i;
//         data = data | bit;
//     }
//     return data;
// }
uint64_t getBytes(int num_bytes, FILE *input)
{
    (void) num_bytes;
    uint64_t word = 0;
    // fprintf(stderr, "\n");
    for (int lsb = 24; lsb >= 0; lsb -= 8) {
        // fprintf(stderr,"%i: ", lsb);
        int byte = getc(input);
        // fprintf(stderr, "byte from word: %i\n", byte);
        // if ( byte == EOF){
        //     // fprintf(stderr, "\n%d\n", decodeval);
        //     // exit(0);

        //     // fprintf(stderr, "EOF\n");
        //     // return word;
        // }
        // //     return word;
        // }
        word = Bitpack_newu(word, 8, lsb, byte);
        // fprintf(stderr, "word: %lu\n\n", word);

    }
    // fprintf(stderr,"\n");

    decodeval++;
    return word;
}

void putBytes(int64_t word, int bytes, int *num)
{
    for (int step = 8; step <= bytes * 8; step += 8) {
        uint64_t word_part = Bitpack_gets(word, 8, WORD_LENGTH - step);
        // fprintf(stderr, "word_part: %lu\n", word_part);
        putchar((char)word_part);
    }
    // fprintf(stderr, "\n");

    *num += 1;
}