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

#include "bitpack.h"
#include "um_instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <seq.h>
#include <assert.h>
#include <math.h>
#include <uarray.h>
#include <stdint.h>


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


static instruction unpack_word(uint32_t word);
static void call_commands(UM_Memory memory, instruction command, 
                          UArray_T registers, uint32_t *counter);
static void addition(UArray_T registers, int a, int b, int c);
static void bitwise_nand(UArray_T registers, int a, int b, int c);
static void multiplication(UArray_T registers, int a, int b, int c);
static void division(UArray_T registers, int a, int b, int c);
static void conditional_move(UArray_T registers, int a, int b, int c);
static void output(UArray_T registers, int c);
static void load_value(UArray_T registers, int a, uint32_t val);
static void input(UArray_T registers, int c);


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
//        printf("we made it heheh \n");

        Seq_T zero_segment = get_segment(memory, 0);
        // fprintf(stderr, "zero segment length: %i\n", Seq_length(zero_segment)); //testing


        /* Creating the registers */
        UArray_T registers = UArray_new(8, sizeof(uint32_t));
        assert(UArray_length(registers) == 8);
        
        // fprintf(stderr, "\n");
        // for (int i = 0; i < UArray_length(registers); i++) {
        //         uint32_t *val = UArray_at(registers, i);
        //         fprintf(stderr, "Value in register %i is %u\n", i, *val);
        // }
        // fprintf(stderr, "\n"); //testing


        uint32_t counter = 0;
        // fprintf(stderr, "length of 0th segment: %i\n", 
        //         Seq_length(zero_segment)); //testing
        while(counter < (uint32_t) Seq_length(zero_segment)) {
                // fprintf(stderr, "\nCOUNTER: %i\n", counter);

                // fprintf(stderr, "COUNTER IS %i\n", counter ); //testing 
                instruction temp = unpack_word(*(uint32_t *) Seq_get
                                   (zero_segment, counter));
                counter++;

                if (temp->command == HALT) {
                        // fprintf(stderr, "Halted XD\n");
                        free(temp);
                        UArray_free(&registers);
                        return;
                }

                call_commands(memory, temp, registers, &counter);
                assert(counter < (uint32_t) Seq_length(zero_segment));

                // fprintf(stderr, "\n");
                // for (int i = 0; i < UArray_length(registers); i++) {
                //         // fprintf(stderr, "Value in register %i is %u\n", i, *(uint32_t *) UArray_at(registers, i));
                // } //testing
                // fprintf(stderr, "\n");
                

                free(temp);
        } 
        UArray_free(&registers);
}


/********** call_commands ********
 *
 * Calls the command associated with a given instruction
 *
 * Parameters:
 *      UM_Memory memory: the struct containing the memory for the program
 *      instruction command: a pointer to an instruction struct
 *      UArray_T registers: the registers used by the program
 *      int *counter: the counter that keeps track of instructions executed
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
static void call_commands(UM_Memory memory, instruction command, 
                          UArray_T registers, uint32_t *counter)
{
        /* save values from struct for easier access */
        Um_opcode opcode = command->command;
        // fprintf(stderr, "Command: %u\n", command->command);
        uint32_t ra = command->a;
        // fprintf(stderr, "a: %u\n", command->a);

        uint32_t rb = command->b;
        // fprintf(stderr, "b: %u\n", command->b);

        uint32_t rc = command->c;
        // fprintf(stderr, "c: %u\n", command->c);

        uint32_t val = command->val;
        // fprintf(stderr, "val: %u\n", command->val);

        // switch
        // if (op == CMOV) {
        //         conditional_move(registers, ra, rb, rc);
        
        // } else if (op == SLOAD) {
        //         segmented_load(memory, registers, ra, rb, rc);

        // } else if (op == SSTORE) {
        //         segmented_store(memory, registers, ra, rb, rc);
                
        // } else if (op == ADD) {
        //         addition(registers, ra, rb, rc);
                
        // } else if (op == MUL) {
        //         multiplication(registers, ra, rb, rc);
                
        // } else if (op == DIV) {
        //         division(registers, ra, rb, rc);
                
        // } else if (op == NAND) {
        //         bitwise_nand(registers, ra, rb, rc);
                
        // } else if (op == ACTIVATE) {
        //         map_segment(memory, registers, rb, rc);
                
        // } else if (op == INACTIVATE) {
        //         unmap_segment(memory, registers, rc);
        // } else if (op == OUT) {
        //         output(registers, rc);
                
        // } else if (op == IN) {
        //         input(registers, rc);
                
        // } else if (op == LOADP) {
        //         load_program(memory, registers, counter, rb, rc);
                
        // } else if (op == LV) {
        //         load_value(registers, ra, val);
        // }   




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
instruction unpack_word(uint32_t word)
{ 
        // fprintf(stderr, "This is the word: %u\n", word);
        uint32_t a, b, c;
        uint32_t command = Bitpack_getu(word, 4, 28);

        // fprintf(stderr, "command is %u\n", command);

        if (command > 13) {
                exit(EXIT_FAILURE);
        } else {
                instruction loadVal = malloc(sizeof(*loadVal));
                assert(loadVal != NULL);
                loadVal->command = command;

                if (command == 13) {
                        a = Bitpack_getu(word, 3, 25);
                        int value = Bitpack_getu(word, 25,0);

                        // fprintf(stderr, "loadval\n");

                        loadVal->a = a;
                        // fprintf(stderr, "a is %u\n", loadVal->a);

                        loadVal->val = value;

                        /* These will not be used if instruction is LoadVal */
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

                        // fprintf(stderr, "a is %u\n", loadVal->a);
                        // fprintf(stderr, "b is %u\n", loadVal->b);
                        // fprintf(stderr, "c is %u\n", loadVal->c);
                        // fprintf(stderr, "command is %u\n", loadVal->command);
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
static void conditional_move(UArray_T registers, int a, int b, int c)
{
        assert(registers != NULL);
        
        // fprintf(stderr, "a: %u, b: %u, c: %u\n", a, b, c);
        
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);
        uint32_t *ra = UArray_at(registers, a);

        // fprintf(stderr, "\nIN CONDITIONAL MOVE\n");
        // fprintf(stderr, "\nra = %u\n", *ra);
        // fprintf(stderr, "\n rb = %u\n", *rb);
        // fprintf(stderr, "\n rc = %u\n", *rc);



        /* register c needs to hold 0 for the values to be updated */
        if (*rc != 0) {
                *ra = *rb;
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
static void addition(UArray_T registers, int a, int b, int c)
{
        assert(registers != NULL);
        // fprintf(stderr, "a: %u, b: %u, c: %u\n", a, b, c);
        
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);
        uint32_t *ra = UArray_at(registers, a);

        /* perform the computation and mod the result to fit in 32 bits */
        // fprintf(stderr, "rb + rc: %i\n", (*rb + *rc));
        // fprintf(stderr, "rb + rc mod 2^32: %i\n", (*rb + *rc) % (uint32_t) pow(2, 32));

        *ra = (*rb + *rc);      
}

/********** multiplication ********
 *
 * Processes the multiplication
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
static void multiplication(UArray_T registers, int a, int b, int c)
{
        assert(registers != NULL);
        // fprintf(stderr, "a: %u, b: %u, c: %u\n", a, b, c);
        
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);
        uint32_t *ra = UArray_at(registers, a);

        /* perform the computation and mod the result to fit in 32 bits */
        *ra = (*rb * *rc);
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
static void division(UArray_T registers, int a, int b, int c)
{
        assert(registers != NULL);
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);
        uint32_t *ra = UArray_at(registers, a);

        *ra = (*rb / *rc);
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
static void bitwise_nand(UArray_T registers, int a, int b, int c)
{
        assert(registers != NULL);
        // fprintf(stderr, "a: %u, b: %u, c: %u\n", a, b, c);
        
        uint32_t *rb = UArray_at(registers, b);
        uint32_t *rc = UArray_at(registers, c);
        uint32_t *ra = UArray_at(registers, a);

        /* perform the nand operation */
        *ra = ~(*rb & *rc);
        // fprintf(stderr, "NAND RESULT BEFORE:%i\n",  ~(*rb & *rc));
        // fprintf(stderr, "NAND RESULT BEFORE:%u\n", *ra);

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
static void output(UArray_T registers, int c)
{      
        assert(registers != NULL);
        uint32_t *rc = UArray_at(registers, c);
        // fprintf(stderr, "outputting\n");

        assert(*rc < 256);

        // fprintf(stderr, "outputting after\n");

        putchar(*rc);
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
static void input(UArray_T registers, int c)
{
        assert(registers != NULL);
        
        /* grab data and its destination */
        uint8_t data = getchar();
        // uint8_t data = fs

        uint32_t *rc = UArray_at(registers, c);

        if ((char) data == EOF) {
                *rc = ~0;
        } else {
                *rc = data;
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
static void load_value(UArray_T registers, int a, uint32_t val)
{
        assert(registers != NULL);
        uint32_t *ra = UArray_at(registers, a);
        *ra = val;
}
