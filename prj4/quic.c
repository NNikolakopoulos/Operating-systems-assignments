#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
# include <sys/times.h>

#include "fun.h"
#include "globalVars.h"



int main(int argc, char **argv)
{
    char *src, *dest;
    double t1, t2, ticspersec;
    //struct tms tb1 , tb2 ;
    

    t1 = ( double ) times (NULL) ;
    for(int i=1; i<argc; i++)
    {
        if( argv[i][0]!='-')   //if its not flag, then its source path and destination path
        {
            src = strdup(argv[i]);
            dest = strdup(argv[i+1]);
            i++;
        }
        if(!strcmp(argv[i],"-v"))     
        {
            verbose_flag = true;
        }
        if(!strcmp(argv[i],"-l"))     
        {
            links_flag = true;
        }
        if(!strcmp(argv[i],"-d"))     
        {
            delete_flag = true;
        }
    }
    traverse_dir(src,dest,0777);   // we insert a random mode, so if dest path does not exists, it will create it with the given mode.
    if(delete_flag)
        delete_routine(src,dest);
    t2 = ( double ) times (NULL) ;
    print_statistics(t2-t1);
    free(src);
    free(dest);

}
