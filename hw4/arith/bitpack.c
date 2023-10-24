#include <bitpack.h>
#include <assert.h>
#include <stdio.h>

const unsigned SYSTEM_SIZE = 64;

static uint64_t getMask(unsigned width, unsigned lsb);

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    printf("n: %lu\n", n);
    assert(width <= SYSTEM_SIZE);

    uint64_t max = (1 << (width - 1)) - 1;
    return n <= max;
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= SYSTEM_SIZE);
    int64_t max_pos = (1 << (width - 2)) - 1;
    int64_t max_neg = (1 << (width - 1));


    // negative range
    return n >= max_neg && n <= max_pos;
    
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);

    uint64_t mask = ~0;
    mask = mask << (SYSTEM_SIZE - (width + lsb - 1));

    // lsb or lsb - 1
    mask = mask >> (SYSTEM_SIZE - width);

    mask = mask << (lsb - 1);

    return word & mask;
}

uint64_t getMask(unsigned width, unsigned lsb)
{
    uint64_t mask = ~0;
    mask = mask << (SYSTEM_SIZE - (width + lsb - 1));
    mask = mask >> (SYSTEM_SIZE - width);
    mask = mask << (lsb - 1);

    return mask;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= SYSTEM_SIZE);
    return Bitpack_getu(word, width, lsb);
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);
    assert(Bitpack_fitsu(value, width));

    uint64_t unMask = ~getMask(width, lsb);
    
    // lsb or lsb - 1
    uint64_t shifted_val = value << (lsb - 1);
    uint64_t word_without_val = word & unMask;
    return word_without_val | shifted_val;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value)
{
    assert(width <= SYSTEM_SIZE);
    assert(width + lsb <= SYSTEM_SIZE);
    assert(Bitpack_fitss(value, width));


    return Bitpack_newu(word, width, lsb, value);
}
Except_T Bitpack_Overflow = { "Overflow packing bits" };
