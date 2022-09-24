#include "invTypes.h"

invertedptr invertedcreate();
void invertedinsert(invertedptr list,HThash hash);
void invertedelete(invertedptr list);
void invertedprint(invertedptr list);
void deletelistinvert(listptr list);
void invertedinserthelp(invertedptr inv,listptr studptr);