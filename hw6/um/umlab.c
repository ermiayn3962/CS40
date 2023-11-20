
/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>

#include <stdio.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(Um_register ra, uint32_t val);
Um_instruction output(Um_register c);
Um_instruction input(Um_register c);

/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction division(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        //which should generate a bitpacked UM instruction using the provided opcode and three register identifiers.
        Um_instruction word = 0;
        word = (int) Bitpack_newu(word, 4, 28, op);
        word = (int) Bitpack_newu(word, 3, 0, rc);
        word = (int) Bitpack_newu(word, 3, 3, rb);
        word = (int) Bitpack_newu(word, 3, 6, ra);


        return word;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction word = 0;
        
        word = (int) Bitpack_newu(word, 3, 25, ra);
        word = (int) Bitpack_newu(word, 4, 28, 13);
        word = (int) Bitpack_newu(word, 25, 0, val);

        return word;
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);

}

Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction mult(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction map(Um_instruction b, Um_instruction c)
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap(Um_instruction c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction store_seg(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction load_seg(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(SLOAD, a, b, c);
}
static inline Um_instruction load_prg(Um_instruction b, Um_instruction c)
{
        return three_register(LOADP, 0, b, c);
}
static inline Um_instruction cond_mv(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(CMOV, a, b, c);
}
static inline Um_instruction nand(Um_instruction a, Um_instruction b, Um_instruction c)
{
        return three_register(NAND, a, b, c);
}



/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        /* size of instruction must be less than size of a pointer 8 bytes */
        assert(sizeof(inst) <= sizeof(uintptr_t));
        /* casting instruction to a uintptr_t and then casting that to a void pointer, needs to be void because SeqAddhi types must match */
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        /* output file and stream have to exist */
        assert(output != NULL && stream != NULL);
        /* getting stream length */
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {

                /* making UM-instruction from the first element in sequence */
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);

                /* looping through in big endian order for word */
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        /* printing a char associated with each byte in the word */
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */
void build_load_val_single(Seq_T stream)
{
        append(stream, loadval(r0, 'b'));
        append(stream, halt());
}


void build_load_program(Seq_T stream) 
{

        append(stream, loadval(r0, 0));
        append(stream, loadval(r1, '!'));
        append(stream, loadval(r3, 11));

        append(stream, load_prg(r0, r3));

        append(stream, halt());
        append(stream, halt());
        append(stream, halt());
        append(stream, halt());
        append(stream, halt());
        append(stream, halt());
        append(stream, halt());

        append(stream, loadval(r1, 'W'));
        append(stream, output(r1));
        append(stream, halt());


}

void build_cond_mv_test(Seq_T stream)
{
        /* Conditions */
        append(stream, loadval(r0, 0));
        append(stream, loadval(r1, 1));

        /* Values */
        append(stream, loadval(r2, 'O'));
        append(stream, loadval(r3, 'N'));


        append(stream, cond_mv(r2, r3, r0));
        append(stream, output(r2));

        append(stream, cond_mv(r2, r3, r1));
        append(stream, output(r2));

        append(stream, halt());

}

void build_division_test(Seq_T stream)
{
        append(stream, loadval(r0, 90));
        append(stream, loadval(r1, 2));

        append(stream, division(r3, r0, r1));

        append(stream, output(r3));
        append(stream, halt());
}


void build_segment_test(Seq_T stream)
{
        /* Creating a mapped segment */
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 7));
        append(stream, map(r1, r2));

        /* Storing inside $m[1][1]*/
        append(stream, loadval(r3, 'z'));
        append(stream, store_seg(r1, r1, r3));

        /* Loading from segments*/
        append(stream, load_seg(r4, r1, r1));
        append(stream, output(r4));

        append(stream, halt());

}

void build_mapping_test(Seq_T stream)
{
        append(stream, loadval(r0, 1));

        append(stream, loadval(r2, 2));

        append(stream, loadval(r1, 9));
        
        append(stream, map(r0, r1));

        append(stream, map(r2, r1));

        append(stream, unmap(r0));

        append(stream, unmap(r2));

        append(stream, map(r2, r1));

        append(stream, halt());

}

void build_memory_test(Seq_T stream)
{
        /* Loading values into registers */
        append(stream, loadval(r0, 1));
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 3));
        append(stream, loadval(r3, 4));
        append(stream, loadval(r5, 9));
        append(stream, loadval(r6, 'H'));
        append(stream, loadval(r7, 'i'));

        /* Mapping new segments */
        append(stream, map(r0, r5));
        append(stream, map(r1, r5));
        append(stream, map(r2, r5));
        append(stream, map(r3, r5));

        /* Unmapping segments */
        append(stream, unmap(r0));
        append(stream, unmap(r1));
        append(stream, unmap(r2));

        /* Storing data inside segments */
        append(stream, store_seg(r3, r0, r6));
        append(stream, store_seg(r3, r2, r7));

        /* Loading data inside segments */
        append(stream, load_seg(r4, r3, r0));
        append(stream, load_seg(r5, r3, r2));

        /* Outputting data */
        append(stream, output(r4));
        append(stream, output(r5));

        append(stream, halt());
        
}


void build_load_val_test(Seq_T stream) 
{
        append(stream, loadval(r0, 'B'));
        append(stream, output(r0));
        append(stream, loadval(r1, 10));
        append(stream, output(r0));
        append(stream, loadval(r2, 9));
        append(stream, output(r2));
        append(stream, loadval(r3, 122));
        append(stream, output(r3));
}
void build_input_test(Seq_T stream) 
{
        append(stream, input(r0));
        append(stream, output(r0));
        append(stream, halt());
}

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
       
}

void build_print_a_digit(Seq_T stream)
{
        // • Add r1 and r2, placing the resulting value into r3.
        // • Output the result (r3) which should print the digit.
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));

        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_add_test(Seq_T stream)
{
        append(stream, loadval(r2, 30));
        append(stream, output(r2));
        
        append(stream, loadval(r3, 25));
        append(stream, output(r3));

        append(stream, add(r1, r2, r3));
        append(stream, output(r1));

        /* addition with negative numbers */

        append(stream, nand(r4, r2, r2));
        // append(stream, output(r4));
        
        append(stream, loadval(r3, 63));
        append(stream, output(r3));

        append(stream, add(r1, r3, r4));
        append(stream, output(r1));


        append(stream, halt());

}

// void build_add-verbose_test(Seq_T stream)
// {
        
// }

void build_mult_test(Seq_T stream)
{
        append(stream, loadval(r2, 25));
        // append(stream, output(r2));
        
        append(stream, loadval(r3, 4));
        // append(stream, output(r3));

        append(stream, mult(r1, r2, r3));
        append(stream, output(r1));

        append(stream, halt());

}

void build_nand_test(Seq_T stream)
{
        append(stream, loadval(r2, 0));
        append(stream, output(r2));
        append(stream, loadval(r1, 0));
        append(stream, output(r1));
        
        append(stream, nand(r3, r2, r1));
        append(stream, halt());

}





