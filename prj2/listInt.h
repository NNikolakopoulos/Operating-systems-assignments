#include "listTypes.h"

listptr listcreate();
void listinsert(listptr list,int prime,double time);  
void insertlistsorted(listptr list,int prime,double time); 
void deletelist(listptr list);
int listremove(listptr list);
void listprint(listptr list);
char *removelist(listptr list);
