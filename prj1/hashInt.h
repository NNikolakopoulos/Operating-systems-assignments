#include "hashTypes.h"

int countfilelines(char* filename);
int hashcode(int key,HThash hash);

listptr listcreate();
void listinsert(listptr list,student* item);
void deletelist(listptr list);
void listprint(listptr list);
void insertlistsorted(listptr list,listptr studptr);

HThash HTcreate(char* filename);
HThash HTcreateNofile();
void HTinsertitem(HThash hash,HTitem item);
void HTinsert(HThash hash,char *fp);
void HTdelete(HThash hash);
void HTprint(HThash hash);









