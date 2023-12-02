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


FILE *open_file(char *filename);
void run_um(FILE *um_file);

UM_Memory UM_Memory_New(FILE *file);
void clean_up_memory(UM_Memory *memory);
uint32_t *get_segment(UM_Memory memory, int id);
int get_seg_size(UM_Memory memory, int id);
void map_segment(UM_Memory memory, uint32_t *registers, int b, int c);
void unmap_segment(UM_Memory memory, uint32_t *registers, int c);
void segmented_load(UM_Memory memory, uint32_t *registers, int a, int b, int c);
void segmented_store(UM_Memory memory, uint32_t *registers, int a, int b, int c);
void load_program(UM_Memory memory, uint32_t *registers, uint32_t *counter,
                  int b, int c);
int get_seg_size(UM_Memory memory, int id); 
static inline uint32_t getBytes(FILE *input, int num_bytes);
static inline void expand(UM_Memory memory);


static inline instruction unpack_word(uint32_t word);
static inline void call_commands(UM_Memory memory, instruction command, 
                          uint32_t *registers, uint32_t *counter);
static inline void addition(uint32_t *registers, int a, int b, int c);
static inline void bitwise_nand(uint32_t *registers, int a, int b, int c);
static inline void multiplication(uint32_t *registers, int a, int b, int c);
static inline void division(uint32_t *registers, int a, int b, int c);
static inline void conditional_move(uint32_t *registers, int a, int b, int c);
static inline void output(uint32_t *registers, int c);
static inline void load_value(uint32_t *registers, int a, uint32_t val);
static inline void input(uint32_t *registers, int c);

void process_instructions(UM_Memory memory);





int main(int argc, char **argv) 
{
        if (argc != 2) {
                fprintf(stderr, 
                "Usage: ./um some_program.um < testinput.txt > output.txt\n");
                
                exit(EXIT_FAILURE);
        }

        FILE *umfile = open_file(argv[1]);
        
        /* Checking if the file is empty */
        if (NULL != umfile) {
                fseek(umfile, 0, SEEK_END);
                int size = ftell(umfile);

                assert(size != 0);
                fseek(umfile, 0, SEEK_SET);
        }

        run_um(umfile);


        return 0;
}

/********** open_file ********
 *
 * Opens a provided file
 *
 * Parameters:
 *    char *filename: the filename from command line
 *
 * Return: 
 *      a file pointer
 *
 * Expects
 *      filename to not be NULL
 *      
 * Notes:
 *     May CRE if opened file is NULL
 * 
 ************************/
FILE *open_file(char *filename)
{
        /* creates a FILE pointer */
        FILE *file;

        /* checks that the file exists and reads through stdin if not */
        if (filename != NULL) {
                file = fopen(filename, "rb");
                if (file == NULL){
                        fprintf(stderr, "File: %s, cannot be opened\n", 
                                filename);
                        exit(EXIT_FAILURE);
                }
        } else {
                file = stdin;
                assert(file != NULL);
        }
    
        return file;
}

/********** run_um ********
 *
 * Runs the universal emulator
 *
 * Parameters:
 *     FILE *file: the file with the instructions
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      file to not be NULL
 *      
 * Notes:
 *     May CRE if file is NULL
 * 
 ************************/
void run_um(FILE *um_file)
{
    assert(um_file != NULL);

    UM_Memory memory = UM_Memory_New(um_file);

    process_instructions(memory);

    /* clean up memory */
    clean_up_memory(&memory);
    fclose(um_file);
   
}

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
        // uint32_t *temp = memory->segments[registers[b]];
        // registers[a] = temp[registers[c]];

        registers[a] = memory->segments[registers[b]][registers[c]];


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
        // uint32_t data = registers[c];

        // uint32_t *temp = memory->segments[registers[a]];

        // temp[registers[b]] = data;

        memory->segments[registers[a]][registers[b]] = registers[c];




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
                // uint32_t *old_seg = memory->segments[0];
                // free(old_seg);

                free(memory->segments[0]);

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
                // int *temp = Seq_get((*memory)->ID, i);
                // free(temp);
                free(Seq_get((*memory)->ID, i));
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

 /********** process_instructions ********
 *
 * Processes the instructions for a provided program
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
void process_instructions(UM_Memory memory)
{
        assert(memory != NULL);

        /* Creating the registers */
        uint32_t *registers = calloc(8, sizeof(uint32_t));
        
        uint32_t counter = 0;
        bool loop = true;

        while (loop) {
                 uint32_t *zero_segment = memory->segments[0];
                /* grab next instruction to be executed */
                instruction temp = unpack_word(zero_segment[counter]);

                // uint32_t size = (uint32_t) memory->segment_sizes[0];
                 
                assert(counter < (uint32_t) memory->segment_sizes[0]);
                counter++;

                if (temp->command == LV) {
                        load_value(registers, temp->a, temp->val);
                        
                        // free(temp);

                }
                else if (temp->command == HALT) {
                        free(temp);
                        free(registers);
                        loop = false;
                        return;
                } else {
                    call_commands(memory, temp, registers, &counter);
                    // free(temp);

                }
                free(temp);

        
        } 

        free(registers);

}


/********** call_commands ********
 *
 * Calls the command associated with a given instruction
 *
 * Parameters:
 *      UM_Memory memory: the struct containing the memory for the program
 *      instruction command: a pointer to an instruction struct
 *      UArray_T registers: the registers used by the program
 *      uint32_t *counter: the counter that keeps track of instructions executed
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      instruction command, UArray_T registers, int *counter to not be NULL
 *      
 * Notes:
 *     May CRE if instruction command, UArray_T registers, int *counter is NULL
 * 
 ************************/
static inline void call_commands(UM_Memory memory, instruction command, 
                          uint32_t *registers, uint32_t *counter)
{
        /* save values from struct for easier access */
        Um_opcode opcode = command->command;
        uint32_t ra = command->a;
        uint32_t rb = command->b;
        uint32_t rc = command->c;
        // uint32_t val = command->val;
     
        /* switch statement to process instructions */
        switch (opcode) {
		case CMOV:   
                        // conditional_move(registers, ra, rb, rc);
                        if (registers[rc] != 0) {
                                registers[ra] = registers[rb];
                        }
		        break;
		case SLOAD:  
                        segmented_load(memory, registers, ra, rb, rc);
		        break;
		case SSTORE: 
                        segmented_store(memory, registers, ra, rb, rc);
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
                        division(registers, ra, rb, rc);
		        break;
		case NAND:   
                        // bitwise_nand(registers, ra, rb, rc);
                        registers[ra] = ~(registers[rb] & registers[rc]);

		        break;
                case HALT:
                        return;
		case ACTIVATE:    
                        map_segment(memory, registers, rb, rc);
		        break;
		case INACTIVATE:  
                        unmap_segment(memory, registers, rc);
			break;
		case OUT:   
                        // output(registers, rc);
                        putchar(registers[rc]);

		        break;
        	case IN:    
                        input(registers, rc);
		        break;
		case LOADP:  
                        load_program(memory, registers, counter, rb, rc);
		        break;
		case LV:     
                        // load_value(registers, ra, val);
                        break;
		}
}


/********** unpack_word ********
 *
 * Unpacks values from a 32-bit word
 *
 * Parameters:
 *     uint32_t word: the word to be unpacked
 *
 * Return: 
 *      A pointer to an instruction struct
 *
 * Expects
 *      Nothing
 *      
 * Notes:
 *     May exit if command is not within command range or CRE if memory
 *     allocation fails 
 * 
 ************************/
static inline instruction unpack_word(uint32_t word)
{ 
        uint32_t a, b, c;
        uint32_t command = (word & 4026531840) >> 28;

	
        if (command > 13) {
                exit(EXIT_FAILURE);
        } else {
                instruction loadVal = malloc(sizeof(*loadVal));
                assert(loadVal != NULL);
                loadVal->command = command;

                if (command == 13) {
                        a = (word & 234881024) >> 25;
                        int value = word & 33554431;



                        loadVal->a = a;
                        loadVal->val = value;
                        /* b and c are not used with LoadVal instruction */
                        loadVal->b = 0;
                        loadVal->c = 0;

                } else {
                        /* sets values for the three-register commands */

                        a = (word & 448) >> 6;
                        b = (word & 56) >> 3;
                        c = word & 7;

                        loadVal->a = a;
                        loadVal->b = b;
                        loadVal->c = c;
                        loadVal->command = command;
                }
                return loadVal; 
        }       
}


/********** conditional_move ********
 *
 * Processes the conditional move command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL
 * 
 ************************/
static inline void conditional_move(uint32_t *registers, int a, int b, int c)
{
        assert(registers != NULL);

        /* register c needs to not hold 0 for the values to be updated */
        if (registers[c] != 0) {
                registers[a] = registers[b];
        }
}

/********** addition ********
 *
 * Processes the addition command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL
 * 
 ************************/
static inline void addition(uint32_t *registers, int a, int b, int c)
{
        assert(registers != NULL);   

        registers[a] = registers[b] + registers[c];   
}

/********** multiplication ********
 *
 * Processes the multiplication command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL
 * 
 ************************/
static inline void multiplication(uint32_t *registers, int a, int b, int c)
{
        assert(registers != NULL);
        
        registers[a] = (registers[b] * registers[c]);
}

/********** division ********
 *
 * Processes the division command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL
 * 
 ************************/
static inline void division(uint32_t *registers, int a, int b, int c)
{
        assert(registers != NULL);

        registers[a] = registers[b] / registers[c];
}

/********** bitwise_nand ********
 *
 * Processes the bitwise nand command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int a: a register value
 *     int b: a register value
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL
 * 
 ************************/
static inline void bitwise_nand(uint32_t *registers, int a, int b, int c)
{
        assert(registers != NULL);
      
        registers[a] = ~(registers[b] & registers[c]);
}

/********** output ********
 *
 * Processes the output command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL or if outputted value is larger than 255
 * 
 ************************/
static inline void output(uint32_t *registers, int c)
{      
        putchar(registers[c]);
}

/********** input ********
 *
 * Processes the input command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int c: a register value
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL or if there is an error getting the input
 * 
 ************************/
static inline void input(uint32_t *registers, int c)
{
        
        /* grab data and its destination */
        uint8_t data = getchar();

        if ((char) data == EOF) {
                // *rc = ~0;
                registers[c] = ~0;
        } else {
                // *rc = data;
                registers[c] = data;
        }

        assert(!ferror(stdin));
}

/********** load_value ********
 *
 * Processes the load value command
 *
 * Parameters:
 *     UArray_T registers: the register array
 *     int a: a register value
 *     uint32_t val: the value to be loaded
 *
 * Return: 
 *      Nothing (void)
 *
 * Expects
 *      registers to not be NULL
 *      
 * Notes:
 *     May CRE if registers is NULL
 * 
 ************************/
static inline void load_value(uint32_t *registers, int a, uint32_t val)
{
        registers[a] = val;
}