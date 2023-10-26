/**************************************************************
 *
 *                     bitpack.c
 *
 *     Assignment: arith
 *     Authors: Cooper Golemme (cgolem01) and Yoda Ermias (yermia01)
 *     Date: Oct 26, 2023
 * 
 *     Contains the functions described in bitpack.h to put and
 *     retrieve bits from a word with 64 bits
 *
 *
 **************************************************************/


#include <bitpack.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Size of system memory representation */
const unsigned SYSTEM_SIZE = 64;

/* Exception bit overflow */
Except_T Bitpack_Overflow = { "Overflow packing bits" };

static uint64_t mask(unsigned width, unsigned lsb);


/* Bitpack_fitsu
 * Description: checks if unsigned value can be represented with width number of
 *              bits
 * 
 * Input: n - unsigned value to check if fits
 *        width - number of bits to see if n fits into
 * 
 * Output: bool - true if fits, false if not fit
 * 
 * Assertions: C.R.E if width exceeds SYSTEM_SIZE
 *             C.R.E if width = 0
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= SYSTEM_SIZE && width != 0);

    uint64_t tester = n;
    tester = tester << (SYSTEM_SIZE - width);
    tester = tester >> (SYSTEM_SIZE - width);
    
    return (n == tester);
}


/* Bitpack_fitss
 * Description: checks if signed value can be represented with width number of
 *              bits
 * 
 * Input: n - signed value to check if fits
 *        width - number of bits to see if n fits into
 * 
 * Output: bool - true if fits, false if not fit
 * 
 * Assertions: C.R.E if width exceeds SYSTEM_SIZE
 *             C.R.E if width = 0
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= SYSTEM_SIZE && width != 0);

    int64_t tester = n;
    tester = tester << (SYSTEM_SIZE - width);
    tester = tester >>(SYSTEM_SIZE - width);
    return n == tester; 
}


/* Bitpack_getsu
 * Description: gets an unsigned value in word at specified location

 * Input: word - word to get value from
 *        width - width of value to get
 *        lsb - starting bit of value to get
 * 
 * Output: uint64_t representing the value got by function
 * 
 * Assertions: C.R.E if width exceeds SYSTEM_SIZE
 *             C.R.E if width + lsb exceeds SYSTEM_SIZE (try to put value in a 
 *             space past the SYSTEM_SIZE)
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);

    if (width == 0) return 0;

    word = word << (SYSTEM_SIZE - (width + lsb));
    word = word >> (SYSTEM_SIZE - width);
    return word;
}


/* Bitpack_getss
 * Description: gets a signed value in word at specified location
 
 * Input: word - word to get value from
 *        width - width of value to get
 *        lsb - starting bit of value to get
 * 
 * Output: int64_t representing the value got by function
 * 
 * Assertions: C.R.E if width exceeds SYSTEM_SIZE
 *             C.R.E if width + lsb exceeds SYSTEM_SIZE (try to put value in a 
 *             space past the SYSTEM_SIZE)
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);
    if (width == 0) return 0;

    int64_t temp = word << (SYSTEM_SIZE - width - lsb);
    int64_t signedVal = temp >> (SYSTEM_SIZE - width);
    
    return signedVal;
}


/* Bitpack_news
 * Description: puts an unsigned value in word at specified location

 * Input: word - word to put value in 
 *        width - width value to put in
 *        lsb - the bit the value should start
 *        value - the value to put in word
 * 
 * Output: updated word with value in it.
 * 
 * Assertions: C.R.E if width exceeds SYSTEM_SIZE
 *             C.R.E if width + lsb exceeds SYSTEM_SIZE (try to put value in a 
 *             space past the SYSTEM_SIZE)
 *             Bitpack_Overflow and exit failure if value does not fit in the 
 *             width provided to this function (more on that in Bitpack_fitsu)
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);

    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
        exit(EXIT_FAILURE);
    }

    uint64_t shifted_val = value << lsb;

    uint64_t unMask = ~mask(width, lsb);
    uint64_t word_without_val = word & unMask;

    return word_without_val | shifted_val;
}


/* Bitpack_news
 * Description: puts a signed value in word at specified location

 * Input: word - word to put value in 
 *        width - width value to put in
 *        lsb - the bit the value should start
 *        value - the value to put in word
 * 
 * Output: updated word with value in it.
 * 
 * Assertions: C.R.E if width exceeds SYSTEM_SIZE
 *             C.R.E if width + lsb exceeds SYSTEM_SIZE (try to put value in a 
 *             space past the SYSTEM_SIZE)
 *             Bitpack_Overflow and exit failure if value does not fit in the 
 *             width provided to this function (more on that in Bitpack_fitss)
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);
    if (!Bitpack_fitss(value, width)) {
        RAISE(Bitpack_Overflow);
        exit(EXIT_FAILURE);
    }
    uint64_t temp = value << (SYSTEM_SIZE - width);
    int64_t signedVal = temp >> (SYSTEM_SIZE - width);

    return Bitpack_newu(word, width, lsb, signedVal);
}


/* get_mask
 * Description: creates a mask for bits in position from [lsb + width, lsb]

 * Input: width - width of desired mask (number of 1s in mask)
 *        lsb - start of mask in uint64
 * 
 * Output: uint64 with 0s everywhere and 1s between [lsb + width, lsb]
 */
uint64_t mask(unsigned width, unsigned lsb)
{
    uint64_t mask = ~0;
    mask = mask << (SYSTEM_SIZE - width);
    mask = mask >> (SYSTEM_SIZE - (lsb + width));
    return mask;
}