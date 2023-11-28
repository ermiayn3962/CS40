#include <stdbool.h>

enum tx { NONE, ROT90, ROT180, ROT270, FLIPH, FLIPV, TRANS };

bool changes_aspect(enum tx transformation) 
{
        switch (transformation) {
        case NONE:   return false;
        case ROT90:  return true;
        case ROT180: return false;
        case ROT270: return true;
        case FLIPH:  return false;
        case FLIPV:  return false;
        case TRANS:  return true;
        }
        return false;
}
