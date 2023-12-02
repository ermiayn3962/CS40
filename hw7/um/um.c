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

        uint32_t **segments = calloc(500, sizeof(uint32_t *));
        int size = 0;
        int capacity = 500;
        int *segment_sizes = calloc(500, sizeof(int));
        int *ID = calloc(500, sizeof(int));
        int ID_size = 0;
        int ID_capacity = 500;
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
                zero_segment[i] = word; 
        }

        /* Check if zero seg is populated here*/
        size = 1; 
        segments[0] = zero_segment;
        segment_sizes[0] = zero_size;

        uint32_t *registers = calloc(8, sizeof(uint32_t));
        
        uint32_t counter = 0;
        bool loop = true;

        uint32_t command;
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t val;

        while (loop) {
                 uint32_t *zero_segment = segments[0];
                /* grab next instruction to be executed */
                command = (zero_segment[counter] & 4026531840) >> 28;
	
        if (command > 13) {
                exit(EXIT_FAILURE);
        } else {
                command = command;

                if (command == 13) {
                        a = (zero_segment[counter] & 234881024) >> 25;
                        val = zero_segment[counter] & 33554431;
                        /* b and c are not used with LoadVal instruction */
                        b = 0;
                        c = 0;

                } else {
                        /* sets values for the three-register commands */
                        a = (zero_segment[counter] & 448) >> 6;
                        b = (zero_segment[counter] & 56) >> 3;
                        c = zero_segment[counter] & 7;
                        command = command;
                }
        }       
                assert(counter < (uint32_t) segment_sizes[0]);
                counter++;

                if (command == LV) {
                        registers[a] = val;
                }
                else if (command == HALT) {
                        free(registers);
                        loop = false;
                        break;
                } else {
                        Um_opcode opcode = command;
                        uint32_t ra = a;
                        uint32_t rb = b;
                        uint32_t rc = c;
                /* switch statement to process instructions */
                int pos;
                uint32_t *new_seg;
                int id;
                uint8_t data;
                uint32_t *temp;
                int addy;
                switch (opcode) {
                        case CMOV:   
                                if (registers[rc] != 0) {
                                        registers[ra] = registers[rb];
                                }
                                break;
                        case SLOAD:  
                                registers[ra] = segments[registers[rb]][registers[rc]];

                                break;
                        case SSTORE: 
                                segments[registers[ra]][registers[rb]] = registers[rc];

                                break;
                        case ADD:    
                                registers[ra] = registers[rb] + registers[rc];   
                                
                                break;
                        case MUL:    
                                registers[ra] = (registers[rb] * registers[rc]);
                                break;
                        case DIV:    
                                registers[ra] = registers[rb] / registers[rc];

                                break;
                        case NAND:   
                                registers[ra] = ~(registers[rb] & registers[rc]);
                                break;
                        case HALT:
                                break;
                        case ACTIVATE:    
                                new_seg = calloc(registers[rc], sizeof(uint32_t));
                                if (ID_size == 0) {
                                        /* Just add a new segment to sequence directly */
                                        if (size == capacity) {
                                                capacity = capacity * 2 + 2;

                                                uint32_t **new_arr = calloc(capacity, sizeof(uint32_t *));

                                                for (int i = 0; i < size; i++) {
                                                        new_arr[i] = segments[i];
                                                }

                                                if (segments != NULL) {
                                                        free(segments);
                                                }

                                                segments = new_arr;

                                                int *new_seg_size = calloc(capacity, sizeof(int));
                                                
                                                for (int i = 0; i < size; i++) {
                                                        new_seg_size[i] = segment_sizes[i];
                                                }

                                                if (segment_sizes != NULL) {
                                                        free(segment_sizes);
                                                }
                                                
                                                segment_sizes = new_seg_size;
                                        }
                                        pos = size;
                                } else {
                                        /* reusing an unmapped segment */
                                        id = ID[ID_size - 1];
                                        pos = id;
                                        ID_size--;
                                }

                                if (segment_sizes[pos] != 0) {
                                        free(segments[pos]);
                                }

                                segments[pos] = new_seg;
                                segment_sizes[pos] = registers[rc];

                                size++;
                                registers[rb] = pos;  
                                
                                break;
                        case INACTIVATE:  
                                /* Getting the sequence of instructions at memory segment c*/
                                temp = segments[registers[rc]];
                                free(temp);

                                /* Putting an empty segment at $m[$r[c]] */
                                segment_sizes[registers[rc]] = 0;
                                size--;


                                /* place the ID in ID array to be reused later */
                                addy = registers[rc];

                                if (ID_size == ID_capacity) {
                                        ID_capacity = ID_capacity * 2 + 2;

                                        int *new_arr = calloc(ID_capacity, sizeof(int));

                                        for (int i = 0; i < ID_size; i++) {
                                                        new_arr[i] = ID[i];
                                        }

                                        if (ID != NULL) {
                                                free(ID);
                                        }

                                        ID = new_arr;

                                }
                                ID[ID_size] = addy;
                                ID_size++;
                                
                                break;
                        case OUT:   
                                putchar(registers[rc]);

                                break;
                        case IN:    
                                data = getchar();

                                if ((char) data == EOF) {
                                        registers[rc] = ~0;
                                } else {
                                        registers[rc] = data;
                                }

                                assert(!ferror(stdin));
                                break;
                        case LOADP:  
                                if (registers[rb] != 0) {
                                        int length = segment_sizes[registers[rb]];

                                        uint32_t *new_seg = calloc(length, sizeof(uint32_t));

                                        /* make a deep copy of the desired segment */
                                        uint32_t *target = segments[registers[rb]];
                                        for (int i = 0; i < length; i++) {
                                                new_seg[i] = target[i];
                                        }
                                        
                                        /* free memory associated with old zero segment */
                                        free(segments[0]);

                                        /* check that counter is in range */
                                        assert(registers[rc] < (uint32_t) length); 
                                        segment_sizes[0]= length; 
                                        segments[0] = new_seg;     
                                }

                                counter = registers[rc];
                                break;
                        case LV:     
                                break;
                        }

                }
        
        }       
        /* free memory */
        int length = capacity;
        for (int i = 0; i < length; i++)
        {
                if (segment_sizes[i] != 0) {
                        free(segments[i]);  
                }

        }

        free(ID);
        free(segments);
        free(segment_sizes);
        fclose(umfile);

        return 0;
}