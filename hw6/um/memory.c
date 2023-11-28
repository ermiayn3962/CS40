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


static void clean_instructions(Seq_T segment);
static Seq_T populate_sequence(FILE *file);
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
        Seq_T temp = Seq_new(0);

        uint32_t *instruction;

        while (!feof(file)) {
                instruction = malloc(sizeof(uint32_t));
                assert(instruction != NULL);

                /* add instructions to the zero segment*/
                *instruction = getBytes(file, 4);
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
        int start = (num_bytes * 8) - 8;

        for (int lsb = start; lsb >= 0; lsb -= 8) {
                int byte = getc(input);
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
                /* Just add a new segment to sequence directly */
                Seq_addhi(memory->segments, Seq_new(0));
                pos = Seq_length(memory->segments) - 1;
               
        } else {
                /* reusing an unmapped segment */
                int *id = (int *) Seq_remlo(memory->ID);
                pos = *id;
                free(id);
        }

        /* Creating a new segment of size c */
        Seq_T new_seg = Seq_new(0);
        for (uint32_t i = 0; i < *rc; i++) {
                uint32_t *instruction = malloc(sizeof(*instruction));
                *instruction = 0;
                Seq_addhi(new_seg, instruction);
        } 

        Seq_T temp = Seq_get(memory->segments, pos);
        Seq_free(&temp);
        Seq_put(memory->segments, pos, new_seg);

        *rb = pos;      
}



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

        clean_instructions(temp);
        Seq_free(&temp);

        /* Putting an empty segment at $m[$r[c]] */
        Seq_put(memory->segments, *rc, Seq_new(0));
        assert((Seq_T) Seq_get(memory->segments, *rc) != NULL);
        assert(Seq_length(Seq_get(memory->segments, *rc)) == 0);

        /* place the ID in ID sequence to be reused later */
        int *addy = malloc(sizeof(*addy));
        *addy = *rc;

        Seq_addhi(memory->ID, addy); 
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
        uint32_t *rc = UArray_at(registers, c);

        /* grab the sequence from rb and the element to be loaded from rc */
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
       
        uint32_t *ra = UArray_at(registers, a);
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);

       /* The data to be stored comes from rc */
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

        if (*rb != 0) {
                Seq_T dup = Seq_new(0); 

                /* make a deep copy of the desired segment */
                int length = Seq_length(Seq_get(memory->segments, *rb));
                for (int i = 0; i < length; i++) {
                        uint32_t *instruction = malloc(sizeof(*instruction));
                        Seq_T curr_seq = Seq_get(memory->segments, *rb);
                        *instruction = *(uint32_t *) Seq_get(curr_seq, i);
                        Seq_addhi(dup, instruction);
                }
                
                /* free memory associated with old zero segment */
                Seq_T old_seg = Seq_put(memory->segments, 0, dup);
                clean_instructions(old_seg);
                Seq_free(&old_seg);

                /* check that counter is in range */
                length = Seq_length(Seq_get(memory->segments, 0));
                assert(*rc < (uint32_t) length);       
        }

        *counter =  *rc;
}


/********** clean_instructions ********
 *
 * Frees memory used by instructions within a segment
 *
 * Parameters:
 *     Seq_T segment: the segment whose elements are to be cleared
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      Seq_T segment to not be NULL
 *      
 * Notes:
 *     May CRE if segment is NULL
 * 
 ************************/
static void clean_instructions(Seq_T segment)
{
        assert(segment != NULL);

        int length = Seq_length(segment);

        /* delete the instructions allocated in each segment */
        for (int j = 0; j < length; j++) {
                uint32_t *num= (uint32_t *) Seq_get(segment, j);
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
        assert(memory != NULL);
        assert(*memory != NULL);

        /* freeing segments */
        for (int i = 0; i < Seq_length((*memory)->segments); i++)
        {
                /* clean up instructions at current segment */
                clean_instructions(Seq_get((*memory)->segments, i));

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