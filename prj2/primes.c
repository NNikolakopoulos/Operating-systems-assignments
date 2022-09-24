#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "root.h"

int main(int argc, char*argv[])
{
    int lb,ub,n;
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i],"-l"))    
        {
            lb=atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-u"))   
        {
            ub=atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-w"))     
        {
            n=atoi(argv[i+1]);
        }
    }
    root(lb,ub,n);
}

