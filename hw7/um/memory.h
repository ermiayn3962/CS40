/**************************************************************
 *
 *                     memory.h
 *
 *     Assignment: UM
 *     Authors:  Yoda Ermias (yermia01) & Maiah Islam (mislam07)
 *     Date:     11/13/2023
 *
 *     This module contains the interface for the universal emulator's
 *     memory
 *
 **************************************************************/
#ifndef MEMORY_H
#define MEMORY_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <seq.h>
#include <uarray.h>

typedef struct UM_Memory *UM_Memory;


UM_Memory UM_Memory_New(FILE *file);
void clean_up_memory(UM_Memory *memory);

Seq_T get_segment(UM_Memory memory, int id);

void map_segment(UM_Memory memory, uint32_t *registers, int b, int c);
void unmap_segment(UM_Memory memory, uint32_t *registers, int c);

void segmented_load(UM_Memory memory, uint32_t *registers, int a, int b, int c);
void segmented_store(UM_Memory memory, uint32_t *registers, int a, int b, int c);

void load_program(UM_Memory memory, uint32_t *registers, uint32_t *counter,
                  int b, int c);


#endif