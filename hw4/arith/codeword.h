#include <bitpack.h>
#include "vcs_conversion.h"
#include "image_manipulation.h"
#include <stdint.h>

extern void encodeImage(A2_and_Methods dct_compress, int width, int height);

extern A2_and_Methods decodeImage(FILE *input);