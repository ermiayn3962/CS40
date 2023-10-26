// #include <bitpack.h>
// #include <assert.h>
// #include <stdio.h>
// #include <stdint.h>
// #include <stdlib.h>

// const unsigned SYSTEM_SIZE = 64;
// Except_T Bitpack_Overflow = { "Overflow packing bits" };

// static uint64_t getMask(unsigned width, unsigned lsb);

// bool Bitpack_fitsu(uint64_t n, unsigned width)
// {
//     assert(width <= SYSTEM_SIZE && width != 0);

//     // printf("This is n inside fitsu: %lu\n", n);
//     // printf("this is fitsu width: %u\n", width);

//     // uint64_t max = (1 << (width - 1)) - 1;
//     // uint64_t tester = n;
//     // tester = tester << (SYSTEM_SIZE - width);
//     // tester = tester >> (SYSTEM_SIZE - width);
    


//     return ((n >> width) == 0);
// }

// bool Bitpack_fitss(int64_t n, unsigned width)
// {
//     // printf("This is n inside fitss: %li\n", n);
//     // printf("this is fitss width: %u\n", width);

//     assert(width <= SYSTEM_SIZE && width != 0);
//     // printf("this is n: %lu\n", n);
//     // int64_t max_pos = (1 << (width - 2)) - 1;
//     // int64_t max_neg = (1 << (width - 1));

//     // printf("max_pos: %lu\n", max_pos);
//     // printf("max_neg: %lu\n", max_neg);
//     int64_t tester = n;
//     tester = tester << (SYSTEM_SIZE - width);
//     tester = tester >>(SYSTEM_SIZE - width);

//     // negative range
//     // return n >= max_neg && n <= max_pos;
//     return n == tester;
    
// }

// uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
// {
//     assert(width <= SYSTEM_SIZE);
//     assert(width + lsb <= SYSTEM_SIZE);

//     if (width == 0) {
//         return 0;
//     }

//     uint64_t mask = ~0;
//     mask = mask << (SYSTEM_SIZE - width);

//     // lsb or lsb - 1
//     mask = mask >> (SYSTEM_SIZE - (lsb + width));

//     // mask = mask << lsb;
//     // fprintf(stderr, "n: %lu \n", word & mask);
//     return word & mask;
// }

// uint64_t getMask(unsigned width, unsigned lsb)
// {
//     uint64_t mask = ~0;
//     mask = mask << (SYSTEM_SIZE - width);
//     mask = mask >> (SYSTEM_SIZE - (lsb + width));
//     return mask;
// }

// int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
// {
//     assert(width + lsb <= SYSTEM_SIZE);
//     if (width == 0) {
//         // fprintf(stderr, "width is 0\n");
//         return 0;
//     }
//     // fprintf(stderr, "word: %lu\n", word);
    
//     int64_t temp = word << (SYSTEM_SIZE - width - lsb);
    
//     int64_t signedVal = temp >> (SYSTEM_SIZE - width);
    
//     // fprintf(stderr, "val: %li\n", signedVal);

//     return signedVal;
// }

// uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
// {
//     // printf("width: %u\n", width);
//     // printf("lsb: %u\n", lsb);
//     // printf("value: %lu\n", value);

//     assert(width <= SYSTEM_SIZE);
//     assert(width + lsb <= SYSTEM_SIZE);
//     // fprintf(stderr, "width: %u\n", width);
//     if (!Bitpack_fitsu(value, width)) {
//         RAISE (Bitpack_Overflow);
//         exit(EXIT_FAILURE);
//     }

//     uint64_t unMask = ~getMask(width, lsb);
    
//     // lsb or lsb - 1
//     uint64_t shifted_val = value << lsb;
//     uint64_t word_without_val = word & unMask;
//     return word_without_val | shifted_val;
// }

// uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value)
// {
//     assert(width <= SYSTEM_SIZE);
//     assert(width + lsb <= SYSTEM_SIZE);
//     if (!Bitpack_fitss(value, width)) {
//         RAISE (Bitpack_Overflow);
//         exit(EXIT_FAILURE);
//     }
//     // printf("n before shift: %lu\n", value);
//     // printf("n after shift: %lu\n",  >> width);
//     uint64_t temp = value << (SYSTEM_SIZE - width);
    
//     int64_t signedVal = temp >> (SYSTEM_SIZE - width);


//     return Bitpack_newu(word, width, lsb, signedVal);
// }



#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <except.h>

#include "bitpack.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

static inline uint64_t ushift_right(uint64_t value, unsigned shift_amount);
static inline int64_t shift_right(int64_t value, unsigned shift_amount);
static inline uint64_t ushift_left(uint64_t value, unsigned shift_amount);
static inline int64_t shift_left(int64_t value, unsigned shift_amount);
static inline uint64_t ushift_64(uint64_t value);
static inline int64_t shift_64(int64_t value);

/* Tells whether unsigned argument n can be represented in width bits.
 * CRE if width is greater than 64.
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        
        unsigned max_value = pow(2, width) - 1;
        
        if (n > max_value)
                return false;

        return true;
}

/* Tells whether signed argument n can be represented in width bits.
 * CRE if width is greater than 64.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
        
        signed max_value = pow(2, width)/2 - 1;
        signed min_value = -(pow(2, width)/2);
        
        if (n > max_value || n < min_value)
                return false;

        return true;
}

/* Extracts a field representing an unsigned integer from a given word 
 * given the width of the field and the location of the field's least 
 * significant bit. CRE if width OR (width + lsb) is greater than 64.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        uint64_t mask = ~0;
        mask = ushift_right(mask, 64 - width);
        mask = ushift_left(mask, lsb);
        uint64_t extracted_word = (mask & word);
        extracted_word = ushift_right(extracted_word, lsb);

        return extracted_word;
}

/* Extracts a field representing a signed integer from a given word 
 * given the width of the field and the location of the field's least 
 * significant bit. CRE if width OR (width + lsb) is greater than 64.
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        uint64_t extracted_unsigned_word = Bitpack_getu(word, width, lsb);
        extracted_unsigned_word = 
                ushift_left(extracted_unsigned_word, 64 - width);
        int64_t extracted_word = (int64_t)extracted_unsigned_word;
        extracted_word = shift_right(extracted_word, 64 - width);
        
        return extracted_word;
}

/* Returns a new word in which the field of width width with least significant
 * bit at lsb has been replaced by an unsigned width-bit representation 
 * of value.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, 
                      unsigned lsb, uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if (!(Bitpack_fitsu(value, width)))
                RAISE(Bitpack_Overflow);

        uint64_t mask = ~0;
        mask = ushift_right(mask, 64 - width);
        mask = ushift_left(mask, lsb);
        mask = ~mask;

        uint64_t new_word = (word & mask);      
        value = ushift_left(value, lsb);
        new_word = (new_word | value);

        return new_word;
}

/* Returns a new word in which the field of width width with least significant
 * bit at lsb has been replaced by a signed width-bit representation of value.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, 
                      unsigned lsb, int64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if (!(Bitpack_fitss(value, width)))
                RAISE(Bitpack_Overflow);

        uint64_t mask = ~0;
        mask = ushift_right(mask, 64 - width);
        mask = ushift_left(mask, lsb);
        mask = ~mask;

        uint64_t new_word = (word & mask);
        
        value = shift_left(value, 64 - width);
        uint64_t unsigned_value = (uint64_t)value;
        unsigned_value = ushift_right(unsigned_value, 64 - width);
        unsigned_value = ushift_left(unsigned_value, lsb);
        
        new_word = (new_word | unsigned_value);

        int64_t signed_word = (int64_t)new_word;

        return signed_word;
}

/* shifts an unsigned value right by the shift amount. */
static inline uint64_t ushift_right(uint64_t value, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64)
                value = ushift_64(value);
        else
                value = value >> shift_amount;

        return value;
}

/* shifts a signed value right by the shift amount. */
static inline int64_t shift_right(int64_t value, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64)
                value = shift_64(value);
        else
                value = value >> shift_amount;

        return value;
}

/* shifts an unsigned value left by the shift amount. */
static inline uint64_t ushift_left(uint64_t value, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64)
                value = ushift_64(value);
        else
                value = value << shift_amount;

        return value;
}

/* shifts a signed value left by the shift amount. */
static inline int64_t shift_left(int64_t value, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64)
                value = shift_64(value);
        else
                value = value << shift_amount;

        return value;
}

/* shifts an unsigned value right by 64 (returns a bit-field
 * of all 0's).
 */
static inline uint64_t ushift_64(uint64_t value)
{
        value = 0;
        return value;
}

/* shifts a signed value right by 64 (returns a bit-field
 * of all 0's or all 1's depending on the sign).
 */
static inline int64_t shift_64(int64_t value)
{
        value = ~0;
        return value;
}