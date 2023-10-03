/**************************************************************
 *
 *                     bit2.h
 *
 *     Assignment: iii
 *     Authors: Liam Smith (lsmith26) and Yoda Ermias (yermia01)
 *     Date: Sep 28, 2023
 * 
 *     Functions used in the creation of a Bit2_T based on Hanson's Bit
 *     interface.
 *
 *
 **************************************************************/

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

#define T Bit2_T
typedef struct T *T;


extern T Bit2_new(int col_dim, int row_dim);

extern void Bit2_free(T *bArr_p);

extern int Bit2_width(T bArr);

extern int Bit2_height(T bArr);

extern int Bit2_put(T bArr, int col, int row, int value);

extern int Bit2_get(T bArr, int col, int row);

extern void Bit2_map_row_major(T bArr, void apply(int col, int row, 
                T a, int value, void *a_cl), void *cl);

extern void Bit2_map_col_major(T bArr, void apply(int col, int row, 
                T a, int value, void *a_cl), void *cl);

#undef T
#endif