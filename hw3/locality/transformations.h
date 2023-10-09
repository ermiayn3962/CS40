/**************************************************************
 *
 *                     transformations.h
 *
 *     Assignment: locality
 *     Authors: Maiah Islam (mislam07) and Yoda Ermias (yermia01)
 *     Date: Oct 8, 2023
 * 
 *     Contains the declaration of the function to transform an image.
 *
 *
 **************************************************************/

#include "a2methods.h"
#include "pnm.h"
#include "cputiming.h"

extern void transform(Pnm_ppm source, int rotation, A2Methods_T methods, 
                      char *time_file_name, int mappingType);