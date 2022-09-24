#include "funTypes.h"
#include "invTypes.h"


void insert(HThash hash,invertedptr inv,int id,int y, int z, char *lname,char* fname,float degree);
void look(HThash hash,int id);
void delete(HThash hash,invertedptr inv,int id);
void number(invertedptr inv,int y);
void top(invertedptr inv,int number,int y);
void average(invertedptr inv,int y);
void minimum(invertedptr inv,int y);
void count(invertedptr inv);
void postalcode(invertedptr inv,int rank);

int searchlistzip(listzipptr list,int z);
listzipptr sortziplist(listzipptr list); 
void configfun(char *filename);