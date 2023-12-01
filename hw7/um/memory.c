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
#include <bitpack.h>
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
        uint32_t **segments;

        int size;
        int capacity;
        int *segment_sizes;

        Seq_T ID;


};


static inline uint32_t getBytes(FILE *input, int num_bytes);
static inline void expand(UM_Memory memory);


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

        mem->segments = calloc(500, sizeof(uint32_t *)); 
        mem->capacity = 500;

        int *segment_sizes = calloc(500, sizeof(int));
        mem->segment_sizes = segment_sizes;


        int zero_size = 0;


        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);

        assert(fileSize != 0);
        fseek(file, 0, SEEK_SET);

        zero_size = (int) fileSize / 4;


        uint32_t *zero_segment = calloc(zero_size, sizeof(uint32_t));
        

        for (int i = 0; i < zero_size; i++) {
                zero_segment[i] = getBytes(file, 4); 
        }

        /* Check if zero seg is populated here*/


        mem->size = 1; 
        mem->segments[0] = zero_segment;
        mem->segment_sizes[0] = zero_size;


        mem->ID = Seq_new(0); 
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
uint32_t *get_segment(UM_Memory memory, int id)
{
        assert(memory != NULL);
        assert(id <= memory->size);
        
        return memory->segments[id];
}

int get_seg_size(UM_Memory memory, int id) 
{
        assert(memory != NULL);
        assert(id < memory->size);

        
        return memory->segment_sizes[id];
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
static inline uint32_t getBytes(FILE *input, int num_bytes)
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
void map_segment(UM_Memory memory, uint32_t *registers, int b, int c)
{
        assert(memory != NULL && registers != NULL);
        
        int pos;

        uint32_t *new_seg = calloc(registers[c], sizeof(uint32_t));
        if (Seq_length(memory->ID) == 0) {
                /* Just add a new segment to sequence directly */
                if (memory->size == memory->capacity) {
                        expand(memory);
                }
                pos = memory->size;
        } else {
                /* reusing an unmapped segment */
                int *id = (int *) Seq_remlo(memory->ID);
                pos = *id;
                free(id);

        }

        if (memory->segment_sizes[pos] != 0) {
                free(memory->segments[pos]);
        }

        memory->segments[pos] = new_seg;
        memory->segment_sizes[pos] = registers[c];

        memory->size++;


        
        registers[b] = pos;   
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
void unmap_segment(UM_Memory memory, uint32_t *registers, int c)
{   
        assert(memory != NULL && registers != NULL);

        /* Getting the sequence of instructions at memory segment c*/
        uint32_t *temp = memory->segments[registers[c]];
        free(temp);

        /* Putting an empty segment at $m[$r[c]] */
        memory->segment_sizes[registers[c]] = 0;
        memory->size--;


        /* place the ID in ID sequence to be reused later */
        int *addy = malloc(sizeof(*addy));
        *addy = registers[c];

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
void segmented_load(UM_Memory memory, uint32_t *registers, int a, int b, int c)
{
        assert(memory != NULL && registers != NULL);
        

        /* grab the sequence from rb and the element to be loaded from rc */
        uint32_t *temp = memory->segments[registers[b]];
        registers[a] = temp[registers[c]];
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
void segmented_store(UM_Memory memory, uint32_t *registers, int a, int b, int c)
{
        assert(memory != NULL && registers != NULL);
       

       /* The data to be stored comes from rc */
        uint32_t data = registers[c];

        uint32_t *temp = memory->segments[registers[a]];

        temp[registers[b]] = data;
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
void load_program(UM_Memory memory, uint32_t *registers, uint32_t *counter, 
                  int b, int c)
{
        assert(memory != NULL && registers != NULL);

     

        if (registers[b] != 0) {
                int length = memory->segment_sizes[registers[b]];

                uint32_t *new_seg = calloc(length, sizeof(uint32_t));

                /* make a deep copy of the desired segment */
                uint32_t *target = memory->segments[registers[b]];
                for (int i = 0; i < length; i++) {
                        new_seg[i] = target[i];
                }
                
                /* free memory associated with old zero segment */
                uint32_t *old_seg = memory->segments[0];
                free(old_seg);

                /* check that counter is in range */
                assert(registers[c] < (uint32_t) length); 
                memory->segment_sizes[0]= length; 
                memory->segments[0] = new_seg;     
        }

        *counter =  registers[c];
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
        int length = (*memory)->capacity;
        for (int i = 0; i < length; i++)
        {
                /* clean up instructions at current segment */
                if ((*memory)->segment_sizes[i] != 0) {
                        free((*memory)->segments[i]);  
                }

        }

        /* freeing the segment IDs */
        for (int i = 0; i < Seq_length((*memory)->ID); i++)
        {
                int *temp = Seq_get((*memory)->ID, i);
                free(temp);
        }

        /* free segment sequence, register array, and memory struct */
        free((*memory)->segments);
        free((*memory)->segment_sizes);

        Seq_free(&(*memory)->ID);
        free(*memory);
}


static inline void expand(UM_Memory memory) 
{
        //increase the capacity
        memory->capacity = memory->capacity * 2 + 2;

        uint32_t **new_arr = calloc(memory->capacity, sizeof(uint32_t *));

        //copy existing elements over
        for (int i = 0; i < memory->size; i++) {
                new_arr[i] = memory->segments[i];
        }

        //get rid of what's at my_arr currently
        if (memory->segments != NULL) {
                free(memory->segments);
        }

        //set my_arr to have the new elements with its update capacity
        memory->segments = new_arr;

        //need to expand segment sizes
        int *new_seg_size = calloc(memory->capacity, sizeof(int));
        
        for (int i = 0; i < memory->size; i++) {
                new_seg_size[i] = memory->segment_sizes[i];
        }

        if (memory->segment_sizes != NULL) {
                free(memory->segment_sizes);
        }

        //set my_arr to have the new elements with its update capacity
        memory->segment_sizes = new_seg_size;
 }