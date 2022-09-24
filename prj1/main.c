#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "hashInt.h"
#include "invInt.h"
#include "funInt.h"

void main(int argc,char **argv)
{ 
    HThash hash;
    bool nofile=true;
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i],"-i"))     //if we find the flag
        {
            hash=HTcreate(argv[i+1]);       // the next in line argument is the file name
            HTinsert(hash,argv[i+1]);
            nofile=false;
        }
        if(!strcmp(argv[i],"-c"))
            configfun(argv[i+1]);            
    }
    if(nofile)
        hash=HTcreateNofile();
    invertedptr inv=invertedcreate();
    invertedinsert(inv,hash);
    
    char *str=malloc(200*sizeof(char)),*flag;
    while(1)
    {
        printf("\nPlease insert command.\n");
        scanf(" %[^\n]s", str);
        if(!strcmp(str,"exit"))
        {
            printf("- exit programm\n");
            invertedelete(inv);
            HTdelete(hash);
            free(str);
            return;
        }

        int id,z,y;
        char *lname,*fname;
        float degree;

        if(str[0] == 'i')
        {
            lname=malloc(20*sizeof(char));
            fname=malloc(20*sizeof(char));

            sscanf(&str[1],"%d %s %s %d %d %f\n",&id,lname,fname,&z,&y,&degree);
            insert(hash,inv,id,y,z,lname,fname,degree);

            free(lname);
            free(fname);
        }
        else if(str[0] == 'l')
        {
            sscanf(&str[1],"%d\n",&id);
            look(hash,id);
        }
        else if(str[0] == 'd')
        {
            sscanf(&str[1],"%d\n",&id);
            delete(hash,inv,id);
        }
        else if(str[0] == 'n')
        {
            sscanf(&str[1],"%d\n",&y);
            number(inv,y);
        }
        else if(str[0] == 't')
        {
            int num;
            sscanf(&str[1],"%d %d\n",&num, &y);
            top(inv,num,y);
        }
        else if(str[0] == 'a')
        {
            sscanf(&str[1],"%d\n",&y);
            average(inv,y);
        }
        else if(str[0] == 'm')
        {
            sscanf(&str[1],"%d\n",&y);
            minimum(inv,y);
        }
        else if(str[0] == 'c')
        {
            count(inv);
        }
        else if(str[0] == 'p')
        {
            int rank;
            sscanf(&str[1],"%d\n",&rank);
            postalcode(inv,rank);
        }
        strcpy(str,"");  //initialize the buffer

    } 
}