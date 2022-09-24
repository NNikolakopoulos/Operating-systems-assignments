#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "listInt.h"

//int sig_counter=0;

int safe_read(int fd,char* buf,size_t size)
{
    char *tempbuf=buf;
    size_t len=0;
    int n;

    while( len < size )    // while not exceeding size of bytes must be read
    {
        if((n = read( fd, tempbuf, size-len)) == 0)     
        {
            return 0;                      
        }
        if( n < 0 && errno == EINTR)  // if a signal occured while read was in progress
            continue;                 //try reading again
        if( n < 0 )                   // if an error occured
            return n;

        len=len+n;          // by increasing this len, it means that we are decreasing the size of bytes we want to read
        tempbuf=tempbuf+n;  //but we must continue 
    }
    return len;  //number of bytes read
}

int safe_write(int fd,char* buf,size_t size)
{
    char *tempbuf=buf;
    size_t len=0;
    int n;

    while( len < size )
    {
        if( (n = write( fd, tempbuf, size-len)) >= 0 )
        {
            len=len+n;    
            buf=buf+n;
        }
        else if( errno == EINTR)    //if interrupted by signal
            continue;               //continue
        else
            return n;               //if error occured, return bytes writeen
    }
    return len;   //bytes written
}


void spawnChildren(int newlb,int newub,int interval,int modulo,int n,int **fd,bool isWorker,int primeAlgo,int processid)
{
    pid_t pid;
    int i,j;
    int lb=newlb,ub=newub; //these are the fathers lb,ub

    for(i=0; i<n; i++)   // spawn n-children
    {

        pid=fork();

        if(  pid == -1 )
        {
            perror("failed to fork");
            //exit(2);
        }
        if( pid == 0 )   //if its child(= Internal node)
        {
            char *newlbstr=malloc(20*sizeof(char));
            char *newubstr=malloc(20*sizeof(char));
            char *bfstr=malloc(10*sizeof(char));   //bf stands for branch factor
            char *pipewritestr=malloc(50*sizeof(char));
            char *primeAlgostr=malloc(20*sizeof(char));   
            char *pidstr=malloc(15*sizeof(char));

            // some internal-leaf node arithmetic to find which one prime finding algorithm to use in leaf node
            if(!isWorker)  // if its internal node 
                primeAlgo=i%n;    // we save the number of the 1st child's prime finding algorithm
            if(isWorker)   //if its child
            {
                primeAlgo=primeAlgo+i;    // its the number saved before + the i-th child (its something like offset)
                primeAlgo=primeAlgo%3;    // dont forget to modulo
            }


            //we need to pass as argument lower bound and upper bound of interval to the child. 
            //Also we need to pass as argument the pipe write end to child.
            sprintf(newlbstr,"%d",newlb);
            sprintf(newubstr,"%d",newub);
            sprintf(pipewritestr,"%d",fd[i][1]);  
            sprintf(bfstr,"%d",n);
            sprintf(primeAlgostr,"%d",primeAlgo);
            sprintf(pidstr,"%d",processid);
            
            close(fd[i][0]);     // close read end in child

            if( isWorker == false)  //if its internal node, give the right path for the executable of internal
            {
                
                if(execlp("internal","./internal",newlbstr,newubstr,bfstr,pipewritestr,primeAlgostr,pidstr,NULL) == -1)
                    perror("execl");
            }
                
            else    //if its leaf node, then  give the executable path for the leaf node
            {
                  // we will need the pid of root for the signals in leaf nodes
                if(execlp("leaf","./leaf",newlbstr,newubstr,bfstr,pipewritestr,primeAlgostr,pidstr,NULL) == -1)
                    perror("execl");
            }
        }
        else
        {
            close(fd[i][1]);   //close write in parent
            newlb=newlb+interval;
            newub=newub+interval;
            if(i == n-2)
                newub=newub+modulo;  
        }
    }
}

int prime1(int n)
{
    int i;
    if (n==1) return 0;
    for (i=2 ; i<n ; i++)
            if ( n % i == 0) return 0;
    return 1;
}

int prime2(int n)
{
    int i=0, limitup=0;
    limitup = (int)(sqrt((float)n));

    if (n==1) return 0;
    for (i=2 ; i <= limitup ; i++)
            if ( n % i == 0) return 0;
    return 1;
}

int prime3(int n)    //
{
	if (n % 2 == 0 || n % 3 == 0)   // if the number is divided by 2 or 3 then it isn't prime  
		return 0;              
	int div=5;      //  initialize divisor = 5
	while (div * div <= n)   //while  divisor< square root of number
	{
		if (n % div == 0 || n % (div + 2) == 0)   //we check if this number is divided by div or div +2 for example 5 or 7 , 11 or 13 , ...   
            return 0;
	    div=div+6;                  // we check only this specific divisors
	}
    return 1;  //if we are here, that means that this number is prime
}



int cmpfunc (const void * a, const void * b) 
{

    item *a1=(item *) a;
    item *b1=(item *) b;
   return ( a1->prime - b1->prime );
}
/*
void sigfun(int signo)//, siginfo_t *info, void *extra)
{
       //void *ptr_val = info->si_value.sival_ptr;
      // int int_val = info->si_value.sival_int;
       sig_counter++;
       //printf("Signal: %d, value: [%d] %s\n", signo, int_val, (char*)ptr_val);

}

/*
int cmpfunc (const void *stringOne, const void *stringTwo) 
{
    char i;
    while( i != ',')
    {

    }
    return strcmp(*(const char **)stringOne, *(const char **)stringTwo);
} */