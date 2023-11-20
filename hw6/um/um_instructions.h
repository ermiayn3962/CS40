/**************************************************************
 *
 *                     um_instructions.h
 *
 *     Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *     This is the interface for the processing of instructions.
 *
 **************************************************************/

#ifndef UM_INSTRUCTIONS_H
#define UM_INSTRUCTIONS_H

#include "memory.h"

void process_instructions(UM_Memory memory);
void segmented_load(UM_Memory memory, UArray_T registers, int a, int b, int c);


#endif