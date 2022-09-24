#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <poll.h>
#include <fcntl.h>
#include <ctype.h>

#include "functions.h"

#define SIZE_OF_BUF 500

int main(int argc,char *argv[])
{ 
    struct pollfd fd[1];
    int lb,ub,n,pipew,piper,primeAlgo,rootpid;
    double t1, t2,sumtime=0, ticspersec,start_time,end_time;
    struct tms tb1, tb2;
    bool flag;

    start_time = (double) times(&tb2);
    lb=atoi(argv[1]);  
    ub=atoi(argv[2]);
    n=atoi(argv[3]);
    pipew=atoi(argv[4]);
    primeAlgo=atoi(argv[5]);
    rootpid=atoi(argv[6]);
    //printf("%d %d %d %d \n",lb,ub,n,pipew);
    int  i,j;
    char *buf=malloc(SIZE_OF_BUF*sizeof(char));  
    fd[0].fd=pipew;
    fd[0].events=POLLOUT;

    ticspersec = (double) sysconf(_SC_CLK_TCK);

    flag=true;
    switch(primeAlgo)
    {
        case 0:
            for(i=lb; i<ub; i++)  //check for primes
            {
                if(flag)
                {
                    t1 = (double) times(&tb1);
                    flag=false;
                }
                   
                if(prime1(i))
                { 
                    t2 = (double) times(&tb1);                        
                    sprintf(buf,"%d,%f",i,((t2-t1)/ticspersec));
                    sumtime+=((t2-t1)/ticspersec);
                    //printf("write:%s\n",buf); 
                    /*fd[0].fd=pipew;
                    fd[0].events=POLLOUT; 
                    int retpol=poll(fd,1,0);
                    if( retpol == -1)
                        perror("POLL");
                    else if( retpol == 0) 
                    {
                        printf("PRIME 1:Poll timed - out .\n ");
                        exit(1);
                    }
                    else if(retpol > 0)  //if some revent changed
                    {
                        if( fd[0].revents & POLLOUT )   //if the child has written something 
                        {
                            printf("%s\n",buf); */
                            safe_write(pipew,buf,SIZE_OF_BUF);
                        
                          
                          
                    flag=true; 
                }
                else
                    flag=false;
            }
            break;
        case 1:
            for(i=lb; i<ub; i++)  //check for primes
            {
                if(flag)
                    t1 = (double) times(&tb1);
                if(prime2(i))
                {
                    t2 = (double) times(&tb1);
                    sprintf(buf,"%d,%f",i,((t2-t1)/ticspersec));
                    sumtime+=((t2-t1)/ticspersec);
                    //printf("%d\n",i);
                    //sprintf(buf,"%d ",i); 
                    //printf("write:%s\n",buf);
                    /*fd[0].fd=pipew;
                    fd[0].events=POLLOUT;
                    int retpol=poll(fd,1,0);
                    if( retpol == -1)
                        perror("POLL");
                    else if( retpol == 0) 
                    {
                        printf("PRIME2:Poll timed - out .\n ");
                        exit(1);
                    }
                    else if(retpol > 0)  //if some revent changed
                    {
                        if( fd[0].revents == POLLOUT )   //if the child has written something
                        {
                            printf("%s\n",buf); */
                            safe_write(pipew,buf,SIZE_OF_BUF);
                        
                        
                    
                    flag=true;
                }
                else    
                    flag=false;
            } 
            break;
        case 2:
            for(i=lb; i<ub; i++)  //check for primess
            {
                if(flag)
                    t1 = (double) times(&tb1);
                if(prime3(i))
                {
                    t2 = (double) times(&tb1);
                    sprintf(buf,"%d,%f",i,((t2-t1)/ticspersec));
                    sumtime+=((t2-t1)/ticspersec);
                    //printf("%d\n",i);
                    //sprintf(buf,"%d ",i);
                    //printf("write:%s\n",buf); 
                    /*fd[0].fd=pipew;
                    fd[0].events=POLLOUT;
                    int retpol=poll(fd,1,0);
                    if( retpol == -1)
                        perror("POLL");
                    else if( retpol == 0) 
                    {
                        printf("PRIME3:Poll timed - out .\n ");
                        exit(1);
                    }
                    else if(retpol > 0)  //if some revent changed
                    {
                        if( fd[0].revents == POLLOUT )   //if the child has written something 
                        {
                            printf("%s\n",buf); */
                            safe_write(pipew,buf,SIZE_OF_BUF);
                        
                    
                    flag=true;
                }
                else
                    flag=false;
            }  
            break; 
    }
    char *done=malloc(15*sizeof(char)), *sumtimestr=malloc(10*sizeof(char));
    end_time = (double) times(&tb2);
    strcpy(done,"done,");
    sprintf(sumtimestr,"%f",sumtime);
    strcat(done,sumtimestr);
    strcpy(buf,done);
    //printf("%s\n",done);
    /*fd[0].fd=pipew;
    fd[0].events=POLLOUT;
    int retpol=poll(fd,1,0);
    if( retpol == -1)
        perror("POLL");
    else if( retpol == 0) 
    {
        printf("Poll timed - out .\n ");
            exit(1);
    }
    else if(retpol > 0)  //if some revent changed
    {
         if( fd[0].revents & POLLOUT )   //if the child has written something 
         { */
             //printf("%s\n",buf);
               safe_write(pipew,buf,SIZE_OF_BUF);
         
                
    //printf("%d\n",rootpid);
    kill(rootpid,SIGUSR1);
    //{
    //    exit(1);
    //}
    /*
    union sigval value;
    value.sival_int=0;
    value.sival_ptr=NULL;
    if(sigqueue(rootpid, SIGUSR1, value) == 0) 
    {
        printf("signal sent successfully!!\n");
    } */


    close(pipew);
    free(buf);
    free(done);
    free(sumtimestr);
    //printf("%f\n",sumtime);
    //printf("termatisan ola\n");
    return 0; 
}