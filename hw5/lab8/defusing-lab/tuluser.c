#include <stdio.h>
#include <stdlib.h>

#include "tul.h"

void explode_bomb(void)
{
        printf("Boom (just for practice)!\n");
        exit(1);
}

int main()
{
        char buf[1024];
        struct three_unsigned_longs longs;

        fgets(buf, sizeof(buf), stdin);
        read_three_unsigned_longs(buf, &longs);

        printf("Read %lu %lu %lu; %lu + %lu == %lu\n",
               longs.a, longs.b, longs.c,
               longs.a, longs.b, longs.a + longs.b);

        if (longs.a + longs.b != longs.c)
                explode_bomb();

        return EXIT_SUCCESS;
}
