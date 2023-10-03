/**************************************************************
 *
 *                     uarray2.h
 *
 *     Assignment: iii
 *     Authors: Liam Smith (lsmith26) and Yoda Ermias (yermia01)
 *     Date: Sep 28, 2023
 * 
 *     Functions used in the creation of a UArray2_T based on Hanson's UArray
 *     interface.
 *
 *
 **************************************************************/

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;


extern T UArray2_new(int col_dim, int row_dim, int elemSize);

extern void UArray2_free(T *uArr_p);

extern int UArray2_width(T uArr);

extern int UArray2_height(T uArr);

extern int UArray2_size(T uArr);

extern void *UArray2_at(T uArr, int col, int row);

extern void UArray2_map_row_major(T uArr, void apply(int i, int j, 
                T a, void *p1, void *p2), void *cl);

extern void UArray2_map_col_major(T uArr, void apply(int i, int j, 
                T a, void *p1, void *p2), void *cl);


#undef T
#endif