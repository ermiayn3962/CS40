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

#ifndef ARRAY2_INCLUDED
#define ARRAY2_INCLUDED
#define T UArray2_T
typedef struct T *T;

typedef void UArray2_applyfun(int i, int j, T array2, void *elem, void *cl);
typedef void UArray2_mapfun(T array2, UArray2_applyfun apply, void *cl);

extern T     UArray2_new   (int width, int height, int size);
extern void  UArray2_free  (T *array2);
extern int   UArray2_width (T array2);
extern int   UArray2_height(T array2);
extern int   UArray2_size  (T array2);
extern void *UArray2_at    (T array2, int i, int j);
extern void  UArray2_map_row_major(T array2, UArray2_applyfun apply, void *cl);
extern void  UArray2_map_col_major(T array2, UArray2_applyfun apply, void *cl);
#undef T
#endif