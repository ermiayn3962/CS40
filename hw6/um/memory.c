/**************************************************************
 *
 *                     memory.c
 *
 *     Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *     This module contains the implementation for the universal emulator's
 *     memory
 *
 **************************************************************/
#include "memory.h"
#include "bitpack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>


/*  UM_Memory
*   Used to stores the memory used in the universal emulator
*   
*   Seq_T segments: represents the memory segments
*   Seq_T ID: represents the location of unmapped segments   
*/
struct UM_Memory 
{
        /* Data Structures */
        Seq_T segments;
        Seq_T ID;

};


static void clean_instructions(UM_Memory memory, int segPos);
static Seq_T populate_sequence(FILE *file);
// static void update_ids(UM_Memory memory, int id);
static uint32_t getBytes(FILE *input, int num_bytes);



/********** UM_Memory_New ********
 *
 * Creates and returns a new UM_Memory struct
 *
 * Parameters:
 *     FILE *file: pointer to the file being read in
 *
 * Return: 
 *      A UM_Memory struct
 *
 * Expects
 *      FILE *file to not be null
 *      
 * Notes:
 *     May CRE if file or any allocated memory is NULL.
 * 
 ************************/
UM_Memory UM_Memory_New(FILE *file)
{
        assert(file != NULL);
        
        UM_Memory mem = malloc(sizeof(*mem));
        assert(mem != NULL);

        Seq_T segments = Seq_new(0);
        assert(segments != NULL);

        Seq_T zero_segment = populate_sequence(file);
        Seq_addhi(segments, zero_segment);


        mem->ID = Seq_new(0);
        mem->segments = segments;

        return mem;
}

/********** get_segment ********
 *
 * Returns the sequence associated with a specific memory segment
 *
 * Parameters:
 *      UM_Memory memory: the memory struct containing the segments
 *      int id: the desired segment
 *     
 * Return: 
 *      The sequence located at specified ID
 *
 * Expects
 *      memory to not be NULL     
 * 
 * Notes:
 *      May CRE if id is out of segment range or if memory is NULL
 *     
 * 
 ************************/
Seq_T get_segment(UM_Memory memory, int id)
{
        assert(memory != NULL);
        assert(id < Seq_length(memory->segments));
        
        return Seq_get(memory->segments, id);
}


/********** populate_sequence ********
 *
 * Populates the zero segment with data from the file
 *
 * Parameters:
 *     FILE *file: the file being read from
 *
 * Return: 
 *      A sequence representing the zero segment
 *
 * Expects
 *      FILE *file to not be null
 *      
 * Notes:
 *     May CRE if file or allocated memory is NULL
 * 
 ************************/
static Seq_T populate_sequence(FILE *file)
{
        assert(file != NULL);
        // fprintf(stderr, "inside populate sequence\n");
        Seq_T temp = Seq_new(0);

        uint32_t *instruction;

        // fprintf(stderr, " before while\n");

        while (!feof(file)) {
        // fprintf(stderr, "inside while\n");

                instruction = malloc(sizeof(uint32_t));

                assert(instruction != NULL);
                *instruction = getBytes(file, 4);
            
                // fprintf(stderr, "instruction: %u\n", *instruction);  
                Seq_addhi(temp, instruction);  
        }  

        return temp;
}

/* getBytes
 * Description: Gets specified number of bytes from file and returns a 64 bit
 *              int representing the bytes read from the file.
 * 
 * Input: input - file to read from
 *        num_bytes - number of bytes to read from file
 * 
 * Output: uint32_t with data read from file
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
static uint32_t getBytes(FILE *input, int num_bytes)
{
        assert(input != NULL);
        uint32_t word = 0;
        // fprintf(stderr, "inside getBytes\n");
        int start = (num_bytes * 8) - 8;
        for (int lsb = start; lsb >= 0; lsb -= 8) {
                int byte = getc(input);
                // assert(byte != EOF);
                if (byte != EOF) {
                        word = Bitpack_newu(word, 8, lsb, byte);

                }

                
        }

        return word;
}

/********** map_segment ********
 *
 * Creates a new segment for memory to be stored
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     UArray_T registers: the register array
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory and UArray_T registers to not be NULL
 *      
 * Notes:
 *     May CRE if memory or registers is NULL
 * 
 ************************/  
void map_segment(UM_Memory memory, UArray_T registers, int b, int c)
{
        assert(memory != NULL && registers != NULL);
        
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);

        int pos;
        if (Seq_length(memory->ID) == 0) {
                /* Appending a new segment empty */
                Seq_addhi(memory->segments, Seq_new(0));

                /* Adding empty segment to segment IDs */
                // int *addy = malloc(sizeof(*addy));
                // *addy = Seq_length(memory->segments) - 1;
                // Seq_addhi(memory->ID, addy);
                // pos = *addy;
                pos = Seq_length(memory->segments) - 1;
                // fprintf(stderr, "made new segment\n");

                // *rb = pos;
        } else {
                // fprintf(stderr, "reused an id\n");
                int *id = (int *) Seq_remlo(memory->ID);
                pos = *id;
                free(id);
                // update_ids(memory, pos);
                // *rb = pos;
        }
        // fprintf(stderr, "mapping: pos is %i\n", pos);


        /* Creating a new segment of size c */
        Seq_T new_seg = Seq_new(0);
        for (uint32_t i = 0; i < *rc; i++) {
                uint32_t *instruction = malloc(sizeof(*instruction));
                *instruction = 0;
                Seq_addhi(new_seg, instruction);
        } 

        Seq_T temp = Seq_get(memory->segments, pos);
        // clean_instructions(memory, pos); //CHECK THIS

        Seq_free(&temp);
        Seq_put(memory->segments, pos, new_seg);



        // fprintf(stderr, "reg b contains %u\n", *(uint32_t *) UArray_at(registers, b));

        *rb = pos;      
        // fprintf(stderr, "After mapping, rb = %u\n", *(uint32_t *) UArray_at(registers, b));
}


/********** update_ids ********
 *
 * Creates an updated segment ID sequence without the inputted id 
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     int id: the segment id
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory to not be NULL
 *      
 * Notes:
 *     May CRE if memory is NULL
 * 
 ************************/
// static void update_ids(UM_Memory memory, int id) 
// {
//         assert(memory != NULL);
//         Seq_T newIDs = Seq_new(0);

//         for (int i = 0; i < Seq_length(memory->ID); i++) {
//                 if (*(int *) Seq_get(memory->ID, i) != id) {
//                         Seq_addhi(newIDs, Seq_get(memory->ID, i));
//                 } else {
//                         int *temp = Seq_get((memory)->ID, i);
//                         free(temp);

//                 }
//         }
        
//         Seq_free(&(memory)->ID);
//         memory->ID = newIDs;
// }


/********** unmap_segment ********
 *
 * Unmaps a segment from memory
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     UArray_T registers: the register array
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory and UArray_T registers to not be NULL
 *      
 * Notes:
 *     May CRE if memory or registers is NULL
 * 
 ************************/
void unmap_segment(UM_Memory memory, UArray_T registers, int c)
{   
        assert(memory != NULL && registers != NULL);
        
        uint32_t *rc = UArray_at(registers, c);

        /* Getting the sequence of instructions at memory segment c*/
        Seq_T temp = Seq_get(memory->segments, *rc);

        clean_instructions(memory, *rc);

        Seq_free(&temp);

        /* Putting an empty segment at $m[$r[c]] */
        Seq_put(memory->segments, *rc, Seq_new(0));
        assert((Seq_T) Seq_get(memory->segments, *rc) != NULL);
        assert(Seq_length(Seq_get(memory->segments, *rc)) == 0);

        int *addy = malloc(sizeof(*rc));
        *addy = *rc;

        Seq_addhi(memory->ID, addy); 

        // free(addy);
}


/********** segmented_load ********
 *
 * Sets a register to hold a value from the memory segments
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory and UArray_T registers to not be NULL
 *      
 * Notes:
 *     May CRE if memory or registers is NULL
 * 
 ************************/
void segmented_load(UM_Memory memory, UArray_T registers, int a, int b, int c)
{
        assert(memory != NULL && registers != NULL);
        
        uint32_t *ra = UArray_at(registers, a);
        uint32_t *rb = UArray_at(registers, b);
        // fprintf(stderr, "reg b in seg load is %u\n", (b));

        // fprintf(stderr, "reg rb in seg load is %u\n", (*rb));
        uint32_t *rc = UArray_at(registers, c);
        // fprintf(stderr, "reg c in seg load is %u\n", (*rc));

        
        // fprintf(stderr, "SIZE OF SEGMENT IS: %i\n", Seq_length(Seq_get(memory->segments, *rb)));
        // fprintf(stderr, "Value at 4408: %i\n", *(int *)Seq_get(Seq_get(memory->segments, 0), 4408));
        // fprintf(stderr, "SIZE OF SEGMENT IS: %i\n", Seq_length(memory->segments));

        Seq_T curr_seq = Seq_get(memory->segments, *rb);
        *ra = *(uint32_t *) Seq_get(curr_seq, *rc);
}


/********** segmented_store ********
 *
 * Loads a memory segment with a specific value
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory and UArray_T registers to not be NULL
 *      
 * Notes:
 *     May CRE if memory or registers is NULL
 * 
 ************************/
void segmented_store(UM_Memory memory, UArray_T registers, int a, int b, int c)
{
        assert(memory != NULL && registers != NULL);
       
        // fprintf(stderr, "a: %i, b: %i, c: %i\n", a, b, c);
        
        uint32_t *ra = UArray_at(registers, a);
        // fprintf(stderr, "ra contains %u\n", *ra);
        
        uint32_t *rb = UArray_at(registers, b);
        // fprintf(stderr, "rb contains %u\n", *rb);

       /* This is the data to be stored */
        uint32_t *rc = UArray_at(registers, c);
        // fprintf(stderr, "rc contains %u\n", *rc);

   

        uint32_t *data = malloc(sizeof(*data));
        *data = *rc;

        Seq_T curr_seq = Seq_get(memory->segments, *ra);
        
        uint32_t *temp = Seq_get(curr_seq, *rb);
        free(temp); 
        
        Seq_put(curr_seq, *rb, data);

}


/********** load_program ********
 *
 * Loads a new program into the zero segment
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     UArray_T registers: the register array
 *     uint32_t *counter: a pointer to the program counter
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory and UArray_T registers to not be NULL
 *      
 * Notes:
 *     May CRE if memory or registers is NULL
 * 
 ************************/
void load_program(UM_Memory memory, UArray_T registers, uint32_t *counter, 
                  int b, int c)
{
        assert(memory != NULL && registers != NULL);

        /* segment to be duplicated  */
        uint32_t *rb = UArray_at(registers, b);
        /* rc will be the counter */ 
        uint32_t *rc = UArray_at(registers, c);
        // fprintf(stderr, "rb: %u\n", *rb);

        // if (*rb != 0) {
                // Seq_T dup = Seq_get(memory->segments, *rb); //is this duplicating or grabbing the actual segment??
                Seq_T dup = Seq_new(0); 

                for (int i = 0; i < Seq_length(Seq_get(memory->segments, *rb)); i++) {
                        uint32_t *instruction = malloc(sizeof(uint32_t *));
                        *instruction = *(uint32_t *) Seq_get(Seq_get(memory->segments, *rb), i);
                        fprintf(stderr, "instruction: %u\n", *instruction);
                        Seq_addhi(dup, instruction);
                }
                
                // clean_instructions(memory, 0);
                Seq_put(memory->segments, 0, dup);
                assert(*rc < (uint32_t) Seq_length(dup));       
        // }

        // fprintf(stderr, "PRINTING WORDS IN 0 SEGMENT\n");
        // for (int i = 0; i < Seq_length(Seq_get(memory->segments, 0)); i++) {
        //         uint32_t word = *(uint32_t *) Seq_get(dup, i);
        //         fprintf(stderr, "word at %i is %u\n", i, word);
        // }
        // fprintf(stderr, "\n");

        /* make sure counter isn't out of bounds */
        // fprintf(stderr, "rc = %u, length of dup = %u\n", *rc, Seq_length(dup));
        
        *counter =  *rc;

}


/********** clean_instructions ********
 *
 * Frees memory used by instructions within a segment
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *     int segPos: the current segment position
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory to not be NULL
 *      
 * Notes:
 *     May CRE if memory is NULL
 * 
 ************************/
static void clean_instructions(UM_Memory memory, int segPos)
{
        assert(memory != NULL);

        int length = Seq_length(Seq_get(memory->segments, segPos));
        for (int j = 0; j < length; j++) {
                uint32_t *num= (uint32_t *) Seq_get(Seq_get(memory->segments,
                                                            segPos), j);
                free(num);
        }
}

/********** clean_up_memory ********
 *
 * Frees all memory allocated in program
 *
 * Parameters:
 *     UM_Memory memory: the struct containing memory for the program
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      UM_Memory memory to not be NULL
 *      
 * Notes:
 *     May CRE if memory is NULL
 * 
 ************************/
void clean_up_memory(UM_Memory *memory)
{
        //DO NOT FORGET TO DELETE
        //NOTE: we have have to clean up other segments besides 0th
        
        assert(memory != NULL);
        assert(*memory != NULL);
        

        /* freeing segments */
        for (int i = 0; i < Seq_length((*memory)->segments); i++)
        {
                /* clean up instructions at current segment */
                clean_instructions(*memory, i);

                Seq_T temp = Seq_get((*memory)->segments, i);
                Seq_free(&temp);
        }

        /* freeing the segment IDs */
        for (int i = 0; i < Seq_length((*memory)->ID); i++)
        {
                int *temp = Seq_get((*memory)->ID, i);
                free(temp);
        }

        
        /* free segment sequence, register array, and memory struct */
        Seq_free(&(*memory)->segments);
        Seq_free(&(*memory)->ID);
        free(*memory);

}

