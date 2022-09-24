#include "hashTypes.h"
#include "invTypes.h"


// this list will be used for the function p(ostal code)
typedef struct listzipnode
{
    int zip;                          //just hold the postal code
    int count;                        //and how students have this postal code
    struct listzipnode* next;         

}listzip;

typedef listzip* listzipptr;