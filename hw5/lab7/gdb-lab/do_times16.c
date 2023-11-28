#include <stdlib.h>
#include <stdio.h>
#include "times16.h"

int main(int argc, char**argv) 
{
	(void)argc;
	(void)argv;
        int i;
	int j;
        for (i = 1; i < 11; i++) {
		j = times16(i);
                printf("%d times 16 is %d\n", i, j);        
        }

        return 0;   
}


