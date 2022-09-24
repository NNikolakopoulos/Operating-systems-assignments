#include "hashTypes.h"
#ifndef HEADER_INV
#define HEADER_INV

typedef struct invertedtag
{
    int year;
    listptr stud;
    struct invertedtag *next;
    

}inverted;

typedef inverted *invertedptr;

#endif
