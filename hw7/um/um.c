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

#include "bitpack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <seq.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct UM_Memory 
{
        /* Data Structures */
        Seq_T segments;
        Seq_T ID;

} *UM_Memory;


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

static inline FILE *open_file(char *filename);
static inline void run_um(FILE *um_file);
static inline void process_instructions(UM_Memory memory);
static inline void clean_instructions(Seq_T segment);
static inline Seq_T populate_sequence(FILE *file);
static inline uint32_t getBytes(FILE *input, int num_bytes);

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
static inline UM_Memory UM_Memory_New(FILE *file);
static inline void clean_up_memory(UM_Memory *memory);

static inline Seq_T get_segment(UM_Memory memory, int id);

static inline void map_segment(UM_Memory memory, uint32_t *registers, int b, int c);
static inline void unmap_segment(UM_Memory memory, uint32_t *registers, int c);

static inline void segmented_load(UM_Memory memory, uint32_t *registers, int a, int b, int c);
static inline void segmented_store(UM_Memory memory, uint32_t *registers, int a, int b, int c);

static inline void load_program(UM_Memory memory, uint32_t *registers, uint32_t *counter,
                  int b, int c);


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
static inline FILE *open_file(char *filename)
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
static inline void run_um(FILE *um_file)
{
    assert(um_file != NULL);

    UM_Memory memory = UM_Memory_New(um_file);

    process_instructions(memory);

    /* clean up memory */
    clean_up_memory(&memory);
    fclose(um_file);
   
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
static inline void process_instructions(UM_Memory memory)
{
        assert(memory != NULL);

        /* Creating the registers */
        // UArray_T registers = UArray_new(8, sizeof(uint32_t));
        uint32_t *registers = calloc(8, sizeof(uint32_t));
        // assert(UArray_length(registers) == 8);
        
        uint32_t counter = 0;
        bool loop = true;

        while (loop) {
                Seq_T zero_segment = get_segment(memory, 0);
                /* grab next instruction to be executed */
                instruction temp = unpack_word(*(uint32_t *) Seq_get
                                   (zero_segment, counter));
                                   
                assert(counter < (uint32_t) Seq_length(zero_segment));
                counter++;

                if (temp->command == HALT) {
                        free(temp);
                        // UArray_free(&registers);
                        free(registers);
                        loop = false;
                        return;
                }
        
                call_commands(memory, temp, registers, &counter);
                free(temp);
        } 
        // UArray_free(&registers);
        free(registers);

}

/*  UM_Memory
*   Used to stores the memory used in the universal emulator
*   
*   Seq_T segments: represents the memory segments
*   Seq_T ID: represents the location of unmapped segments   
*/


/**************************************************************
 *
 *                     um_instructions.c
 *
 *     Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *  This is the implementation for the processing of instructions.
 *
 **************************************************************/






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
        uint32_t val = command->val;
     
        /* switch statement to process instructions */
        switch (opcode) {
		case CMOV:   
                        conditional_move(registers, ra, rb, rc);
		        break;
		case SLOAD:  
                        segmented_load(memory, registers, ra, rb, rc);
		        break;
		case SSTORE: 
                        segmented_store(memory, registers, ra, rb, rc);
		        break;
		case ADD:    
                        addition(registers, ra, rb, rc);
			break;
		case MUL:    
                        multiplication(registers, ra, rb, rc);
			break;
		case DIV:    
                        division(registers, ra, rb, rc);
		        break;
		case NAND:   
                        bitwise_nand(registers, ra, rb, rc);
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
                        output(registers, rc);
		        break;
        	case IN:    
                        input(registers, rc);
		        break;
		case LOADP:  
                        load_program(memory, registers, counter, rb, rc);
		        break;
		case LV:     
                        load_value(registers, ra, val);
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
        uint32_t command = Bitpack_getu(word, 4, 28);

        if (command > 13) {
                exit(EXIT_FAILURE);
        } else {
                instruction loadVal = malloc(sizeof(*loadVal));
                assert(loadVal != NULL);
                loadVal->command = command;

                if (command == 13) {
                        a = Bitpack_getu(word, 3, 25);
                        int value = Bitpack_getu(word, 25,0);

                        loadVal->a = a;
                        loadVal->val = value;
                        /* b and c are not used with LoadVal instruction */
                        loadVal->b = 0;
                        loadVal->c = 0;

                } else {
                        /* sets values for the three-register commands */
                        a = Bitpack_getu(word, 3, 6);
                        b = Bitpack_getu(word, 3, 3);
                        c = Bitpack_getu(word, 3, 0);

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

        // uint32_t rb = registers[b];
        // uint32_t rc = registers[c];
        // uint32_t ra = registers[a];

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

        // uint32_t *rb = registers[b];
        // uint32_t *rc = registers[c];
        // uint32_t *ra = registers[a];

        // *ra = (*rb + *rc);   

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
        
        // uint32_t *rb = registers[b];
        // uint32_t *rc = registers[c];
        // uint32_t *ra = registers[a];

        // *ra = (*rb * *rc);
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

        // uint32_t *rb = registers[b];
        // uint32_t *rc = registers[c];
        // uint32_t *ra = registers[a];

        // *ra = (*rb / *rc);

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
        
        // uint32_t *rb = registers[b];
        // uint32_t *rc = registers[c];
        // uint32_t *ra = registers[a];

        

        // /* perform the nand operation */
        // *ra = ~(*rb & *rc);

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
        // assert(registers != NULL);

        // uint32_t *rc = UArray_at(registers, c);
        // uint32_t *rc = registers[c];


        // assert(*rc < 256);

        // putchar(*rc);
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
        // assert(registers != NULL);
        
        /* grab data and its destination */
        uint8_t data = getchar();
        // uint32_t *rc = UArray_at(registers, c);
        // uint32_t *rc = registers[c];
        

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
        // assert(registers != NULL);
        // uint32_t *ra = UArray_at(registers, a);
        // uint32_t *ra = registers[a];
        
        
        // *ra = val;

        registers[a] = val;
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
static inline UM_Memory UM_Memory_New(FILE *file)
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
static inline Seq_T get_segment(UM_Memory memory, int id)
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
static inline Seq_T populate_sequence(FILE *file)
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
static inline void map_segment(UM_Memory memory, uint32_t *registers, int b, int c)
{
        assert(memory != NULL && registers != NULL);
        
        // uint32_t *rb = UArray_at(registers, b);
        // uint32_t *rc = UArray_at(registers, c);
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
        for (uint32_t i = 0; i < registers[c]; i++) {
                uint32_t *instruction = malloc(sizeof(*instruction));
                *instruction = 0;
                Seq_addhi(new_seg, instruction);
        } 

        Seq_T temp = Seq_get(memory->segments, pos);
        Seq_free(&temp);
        Seq_put(memory->segments, pos, new_seg);

        // *rb = pos;   
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
static inline void unmap_segment(UM_Memory memory, uint32_t *registers, int c)
{   
        assert(memory != NULL && registers != NULL);
        
        // uint32_t rc = registers[c];

        /* Getting the sequence of instructions at memory segment c*/
        Seq_T temp = Seq_get(memory->segments, registers[c]);

        clean_instructions(temp);
        Seq_free(&temp);

        /* Putting an empty segment at $m[$r[c]] */
        Seq_put(memory->segments, registers[c], Seq_new(0));
        assert((Seq_T) Seq_get(memory->segments, registers[c]) != NULL);
        assert(Seq_length(Seq_get(memory->segments, registers[c])) == 0);

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
static inline void segmented_load(UM_Memory memory, uint32_t *registers, int a, int b, int c)
{
        assert(memory != NULL && registers != NULL);
        
        // uint32_t *ra = UArray_at(registers, a);
        // uint32_t *rb = UArray_at(registers, b);
        // uint32_t *rc = UArray_at(registers, c);

        /* grab the sequence from rb and the element to be loaded from rc */
        Seq_T curr_seq = Seq_get(memory->segments, registers[b]);
        registers[a] = *(uint32_t *) Seq_get(curr_seq, registers[c]);
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
static inline void segmented_store(UM_Memory memory, uint32_t *registers, int a, int b, int c)
{
        assert(memory != NULL && registers != NULL);
       
        // uint32_t *ra = UArray_at(registers, a);
        // uint32_t *rb = UArray_at(registers, b);
        // uint32_t *rc = UArray_at(registers, c);

       /* The data to be stored comes from rc */
        uint32_t *data = malloc(sizeof(*data));
        *data = registers[c];

        Seq_T curr_seq = Seq_get(memory->segments, registers[a]);
        
        uint32_t *temp = Seq_get(curr_seq, registers[b]);
        free(temp); 
        
        Seq_put(curr_seq, registers[b], data);
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
static inline void load_program(UM_Memory memory, uint32_t *registers, uint32_t *counter, 
                  int b, int c)
{
        assert(memory != NULL && registers != NULL);

        /* segment to be duplicated  */
        // uint32_t *rb = UArray_at(registers, b);
        // uint32_t rb = registers[b];
        /* rc will be the counter */ 
        // uint32_t *rc = UArray_at(registers, c);
        // uint32_t rc = registers[c];


        if (registers[b] != 0) {
                Seq_T dup = Seq_new(0); 

                /* make a deep copy of the desired segment */
                int length = Seq_length(Seq_get(memory->segments, registers[b]));
                for (int i = 0; i < length; i++) {
                        uint32_t *instruction = malloc(sizeof(*instruction));
                        Seq_T curr_seq = Seq_get(memory->segments, registers[b]);
                        *instruction = *(uint32_t *) Seq_get(curr_seq, i);
                        Seq_addhi(dup, instruction);
                }
                
                /* free memory associated with old zero segment */
                Seq_T old_seg = Seq_put(memory->segments, 0, dup);
                clean_instructions(old_seg);
                Seq_free(&old_seg);

                /* check that counter is in range */
                length = Seq_length(Seq_get(memory->segments, 0));
                assert(registers[c] < (uint32_t) length);       
        }

        *counter =  registers[c];
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
static inline void clean_instructions(Seq_T segment)
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
static inline void clean_up_memory(UM_Memory *memory)
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