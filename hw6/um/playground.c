#include "memory.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <uarray.h>
#include <stdint.h>

// typedef struct UM_Memory *UM_Memory;


int main(int argc, char **argv) 
{
    // assert(argc == 2);
    // run_um(argv[1]);
    (void) argc;


    printf("in da playground \n");
    UArray_T registers = UArray_new(8, sizeof(uint32_t));

    uint32_t *r1 = UArray_at(registers, 1);
    *r1 = 1;

    uint32_t *r2 = UArray_at(registers, 2);
    *r2 = 2;

    uint32_t *r4 = UArray_at(registers, 4);
    *r4 = 4;


    // uint32_t word1 = 268435729;
    
    
    // void *temp = unpack_word(word1);
    // (void) temp;

    // printf("load_val test\n");
    // uint32_t word3 = 3489660928;

    // unpack_word(word3);
    // test();

    /* Memory Tests */
    FILE *test_file = fopen(argv[1], "rb");
    UM_Memory test_memory = UM_Memory_New(test_file);
    assert(test_memory != NULL);

    fprintf(stderr, "RUNNING MAP_SEGMENT TEST\n\n");
    // map_segment_test(test_memory, 0, 1, 7);

    // fprintf(stderr, "RUNNING MAP_SEGMENT TEST\n\n");
    // map_segment_test(0, 2, 7, test_memory);

    // fprintf(stderr, "RUNNING MAP_SEGMENT TEST\n\n");
    // map_segment_test(0, 3, 2, test_memory);


    // fprintf(stderr, "RUNNING UNMAP_SEGMENT TESTS\n\n");
    // unmap_segment_test(0, 0, 1, test_memory);

    // fprintf(stderr, "RUNNING UNMAP_SEGMENT TESTS\n\n");
    // unmap_segment_test(0, 0, 3, test_memory);

    // fprintf(stderr, "RUNNING MAP_SEGMENT TEST\n\n");
    // map_segment_test(0, 1, 9, test_memory);

    fprintf(stderr, "RUNNING SEGMENTED STORE TEST\n\n");
    // segmented_store_test(test_memory, registers, 1, 2, 4);

   



    clean_up_memory(&test_memory);
    // assert(test_memory->UArray_length(registers) == 8);
    fclose(test_file);



    return 0;
}
