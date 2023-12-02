/**************************************************************
 *
 *                     um.c
 *
 *     Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *     This module is responsible for processing the command line and
 *     kickstarting the program.
 *
 **************************************************************/

// #include "um_runner.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <bitpack.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <seq.h>



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

typedef struct UM_Memory *UM_Memory;


/*  instruction
*   Represents a single instruction from a 32-bit word
*   
*   uint32_t command: represents the instruction to be executed
*   uint32_t a: represents the value at register A
*   uint32_t b: represents the value at register B, if needed
*   uint32_t c: represents the value at register C, if needed
*   uint32_t val: represents the value to be stored, if needed
*/
typedef struct instruction {
        uint32_t command;
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t val;

} *instruction;


/* Assigns each command to a value between 0 and 13 */
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


int main(int argc, char **argv) 
{
        if (argc != 2) {
                fprintf(stderr, 
                "Usage: ./um some_program.um < testinput.txt > output.txt\n");
                
                exit(EXIT_FAILURE);
        }

        FILE *umfile;

        /* checks that the file exists and reads through stdin if not */
        if (argv[1] != NULL) {
                umfile = fopen(argv[1], "rb");
                if (umfile == NULL){
                        fprintf(stderr, "File: %s, cannot be opened\n", 
                                argv[1]);
                        exit(EXIT_FAILURE);
                }
        } else {
                umfile = stdin;
                assert(umfile != NULL);
        }
        
        /* Checking if the file is empty */
        if (NULL != umfile) {
                fseek(umfile, 0, SEEK_END);
                int size = ftell(umfile);

                assert(size != 0);
                fseek(umfile, 0, SEEK_SET);
        }

         assert(umfile != NULL);

        // UM_Memory memory = UM_Memory_New(umfile);
        UM_Memory mem = malloc(sizeof(*mem));
        assert(mem != NULL);

        mem->segments = calloc(500, sizeof(uint32_t *)); 
        mem->capacity = 500;

        int *segment_sizes = calloc(500, sizeof(int));
        mem->segment_sizes = segment_sizes;


        int zero_size = 0;
        fseek(umfile, 0, SEEK_END);
        size_t fileSize = ftell(umfile);
        assert(fileSize != 0);
        fseek(umfile, 0, SEEK_SET);

        zero_size = (int) fileSize / 4;
        uint32_t *zero_segment = calloc(zero_size, sizeof(uint32_t));
        for (int i = 0; i < zero_size; i++) {
                uint32_t word = 0;
                int start = (4 * 8) - 8;

                for (int lsb = start; lsb >= 0; lsb -= 8) {
                        int byte = getc(umfile);
                        if (byte != EOF) {
                                word = Bitpack_newu(word, 8, lsb, byte);

                        }
                }
                // zero_segment[i] = getBytes(umfile, 4); 
                zero_segment[i] = word; 



        }

        /* Check if zero seg is populated here*/
        mem->size = 1; 
        mem->segments[0] = zero_segment;
        mem->segment_sizes[0] = zero_size;
        mem->ID = Seq_new(0); 

        //END MEMORY NEW

        // process_instructions(mem);
        uint32_t *registers = calloc(8, sizeof(uint32_t));
        
        uint32_t counter = 0;
        bool loop = true;

        while (loop) {
                 uint32_t *zero_segment = mem->segments[0];
                /* grab next instruction to be executed */
                // instruction temp = unpack_word(zero_segment[counter]);
                // int32_t a, b, c;
                uint32_t command = (zero_segment[counter] & 4026531840) >> 28;

                instruction temp = malloc(sizeof(*temp));
                assert(temp != NULL);
	
        if (command > 13) {
                exit(EXIT_FAILURE);
        } else {
                temp->command = command;

                if (command == 13) {
                        temp->a = (zero_segment[counter] & 234881024) >> 25;
                        temp->val = zero_segment[counter] & 33554431;
                        /* b and c are not used with LoadVal instruction */
                        temp->b = 0;
                        temp->c = 0;

                } else {
                        /* sets values for the three-register commands */
                        temp->a = (zero_segment[counter] & 448) >> 6;
                        temp->b = (zero_segment[counter] & 56) >> 3;
                        temp->c = zero_segment[counter] & 7;
                        temp->command = command;
                }
        }       
                assert(counter < (uint32_t) mem->segment_sizes[0]);
                counter++;

                if (temp->command == LV) {
                        // load_value(registers, temp->a, temp->val);
                        registers[temp->a] = temp->val;
                }
                else if (temp->command == HALT) {
                        free(temp);
                        free(registers);
                        loop = false;
                        break;
                } else {
                        // call_commands(mem, temp, registers, &counter);
                        Um_opcode opcode = temp->command;
                        uint32_t ra = temp->a;
                        uint32_t rb = temp->b;
                        uint32_t rc = temp->c;
        // uint32_t val = command->val;
     
        /* switch statement to process instructions */
                        int pos;
                        uint32_t *new_seg;
                        int *id;
                        uint8_t data;
                        uint32_t *temp;
                        int *addy;
                        switch (opcode) {
                                case CMOV:   
                                        // conditional_move(registers, ra, rb, rc);
                                        if (registers[rc] != 0) {
                                                registers[ra] = registers[rb];
                                        }
                                        break;
                                case SLOAD:  
                                        // segmented_load(mem, registers, ra, rb, rc);
                                        registers[ra] = mem->segments[registers[rb]][registers[rc]];

                                        break;
                                case SSTORE: 
                                        // segmented_store(mem, registers, ra, rb, rc);
                                        mem->segments[registers[ra]][registers[rb]] = registers[rc];

                                        break;
                                case ADD:    
                                        // addition(registers, ra, rb, rc);
                                        registers[ra] = registers[rb] + registers[rc];   
                                        
                                        break;
                                case MUL:    
                                        // multiplication(registers, ra, rb, rc);
                                        registers[ra] = (registers[rb] * registers[rc]);
                                        break;
                                case DIV:    
                                        registers[ra] = registers[rb] / registers[rc];

                                        break;
                                case NAND:   
                                        // bitwise_nand(registers, ra, rb, rc);
                                        registers[ra] = ~(registers[rb] & registers[rc]);
                                        break;
                                case HALT:
                                        // return;
                                        break;
                                case ACTIVATE:    
                                        new_seg = calloc(registers[rc], sizeof(uint32_t));
                                        if (Seq_length(mem->ID) == 0) {
                                                /* Just add a new segment to sequence directly */
                                                if (mem->size == mem->capacity) {
                                                        // expand(mem);
                                                        mem->capacity = mem->capacity * 2 + 2;

                                                        uint32_t **new_arr = calloc(mem->capacity, sizeof(uint32_t *));

                                                        //copy existing elements over
                                                        for (int i = 0; i < mem->size; i++) {
                                                                new_arr[i] = mem->segments[i];
                                                        }

                                                        //get rid of what's at my_arr currently
                                                        if (mem->segments != NULL) {
                                                                free(mem->segments);
                                                        }

                                                        //set my_arr to have the new elements with its update capacity
                                                        mem->segments = new_arr;

                                                        //need to expand segment sizes
                                                        int *new_seg_size = calloc(mem->capacity, sizeof(int));
                                                        
                                                        for (int i = 0; i < mem->size; i++) {
                                                                new_seg_size[i] = mem->segment_sizes[i];
                                                        }

                                                        if (mem->segment_sizes != NULL) {
                                                                free(mem->segment_sizes);
                                                        }

                                                        //set my_arr to have the new elements with its update capacity
                                                        mem->segment_sizes = new_seg_size;
                                                }
                                                pos = mem->size;
                                        } else {
                                                /* reusing an unmapped segment */
                                                id = (int *) Seq_remlo(mem->ID);
                                                pos = *id;
                                                free(id);

                                        }

                                        if (mem->segment_sizes[pos] != 0) {
                                                free(mem->segments[pos]);
                                        }

                                        mem->segments[pos] = new_seg;
                                        mem->segment_sizes[pos] = registers[rc];

                                        mem->size++;
                                        registers[rb] = pos;  
                                        
                                        break;
                                case INACTIVATE:  
                                        /* Getting the sequence of instructions at memory segment c*/
                                        temp = mem->segments[registers[rc]];
                                        free(temp);

                                        /* Putting an empty segment at $m[$r[c]] */
                                        mem->segment_sizes[registers[rc]] = 0;
                                        mem->size--;


                                        /* place the ID in ID sequence to be reused later */
                                        addy = malloc(sizeof(*addy));
                                        *addy = registers[rc];

                                        Seq_addhi(mem->ID, addy); 
                                        break;
                                case OUT:   
                                        // output(registers, rc);
                                        putchar(registers[rc]);

                                        break;
                                case IN:    
                                        // input(registers, rc);
                                        data = getchar();

                                        if ((char) data == EOF) {
                                                // *rc = ~0;
                                                registers[rc] = ~0;
                                        } else {
                                                // *rc = data;
                                                registers[rc] = data;
                                        }

                                        assert(!ferror(stdin));
                                        break;
                                case LOADP:  
                                        if (registers[rb] != 0) {
                                                int length = mem->segment_sizes[registers[rb]];

                                                uint32_t *new_seg = calloc(length, sizeof(uint32_t));

                                                /* make a deep copy of the desired segment */
                                                uint32_t *target = mem->segments[registers[rb]];
                                                for (int i = 0; i < length; i++) {
                                                        new_seg[i] = target[i];
                                                }
                                                
                                                /* free memory associated with old zero segment */
                                                free(mem->segments[0]);

                                                /* check that counter is in range */
                                                assert(registers[rc] < (uint32_t) length); 
                                                mem->segment_sizes[0]= length; 
                                                mem->segments[0] = new_seg;     
                                        }

                                        counter = registers[rc];
                                        break;
                                case LV:     
                                        // load_value(registers, ra, val);
                                        break;
                                }


                                }
                                free(temp);
        
        } 

        /* clean up memory */
        // clean_up_memory(&mem);
        int length = (mem)->capacity;
        for (int i = 0; i < length; i++)
        {
                /* clean up instructions at current segment */
                if ((mem)->segment_sizes[i] != 0) {
                        free((mem)->segments[i]);  
                }

        }
        /* freeing the segment IDs */
        for (int i = 0; i < Seq_length((mem)->ID); i++)
        {
                // int *temp = Seq_get((*memory)->ID, i);
                // free(temp);
                free(Seq_get((mem)->ID, i));
        }
        /* free segment sequence, register array, and memory struct */
        free((mem)->segments);
        free((mem)->segment_sizes);
        Seq_free(&(mem)->ID);
        free(mem);
        fclose(umfile);


        return 0;
}