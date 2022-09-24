#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>

#include "functions.h"
#include "listInt.h"

#define SIZE_OF_BUF 500


int main(int argc,char *argv[])
{
    int lb,ub,n,pipew,primeAlgo,pid;

    lb=atoi(argv[1]);  
    ub=atoi(argv[2]);
    n=atoi(argv[3]);
    pipew=atoi(argv[4]);
    primeAlgo=atoi(argv[5]);
    pid=atoi(argv[6]);
    //printf("%d %d %d %d\n",lb,ub,n,pipew);
    
    int  i,j,interval=ub-lb,newlb,newub,modulo;
    modulo=interval%n;
    interval=interval/n;
    
    newlb=lb;
    newub=lb+interval;

    struct pollfd fds[n];
    //int *fd=malloc(2*n*sizeof(int));
    int **fd = (int **)malloc(n * sizeof(int *));   

    for (i=0; i<n; i++)                     // allocate the memory for the pipes
       fd[i] = (int *)malloc(2*sizeof(int)); 
    for (i=0; i<n; i++)          // initialize
    {
        fd[i][0] = 0;
        fd[i][1] = 0;
    }


    for(i=0; i< n; i++)
    {
        if(pipe(fd[i]) < 0) 
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        else 
        {
            // first slot for read polled for input
            //fds[i].fd = fd[2*i];
            fds[i].fd = fd[i][0];
            fds[i].events = POLLIN;
            //fds[i][0].revents = 0; 
            /*
            // second slot with write polled for output
            fds[i][1].fd = fd[i][1];
            fds[i][1].events = POLLOUT;
            //fds[i][1].revents = 0;  */
        }
    }
           
    

    // now we must spawn n-leaf nodes
    spawnChildren(newlb,newub,interval,modulo,n,fd,true,0,pid);    

    // we will need a buffer to store temporarily the info collected by pipes, from the lead nodes
    char *buf=malloc(SIZE_OF_BUF*sizeof(char)), *bufstart, *done=malloc(15*sizeof(char)), *token=malloc(20*sizeof(char)), *primestr=malloc(20*sizeof(char));//, *mintime=malloc(20*sizeof(char));
    int prime,bufsiz,primecounter=0;
    double t,worker_time;
    double *Wtimes=malloc(n*sizeof(double));

    strcpy(done,"done");

    listptr list=listcreate();

    

    // in the following loop we must collect 
    // all the primes and their executions times from leaf nodes 
    int doneCounter=0;
    while(1)    
    {   
        int retpol=poll(fds,n,0);
        //printf("%d\n",retpol);
        
        if( retpol == -1 && errno == EINTR)
            continue;
        else if( retpol == 0) 
        {
            continue;
        }
        else if(retpol > 0)  //if some revent changed
        {
            for(i=0; i<n; i++)    //check every element in fd array
            {
                //printf("opa\n");
                if( fds[i].revents & POLLIN )   //if the child has written something
                {
                    if(fds[i].fd == fd[i][0]) {
                    bufsiz=safe_read(fds[i].fd,buf,SIZE_OF_BUF);   //read the prime from the worker
                    //printf("%s\n",buf);
                    bufstart=buf;
                    token=strtok_r(buf,",",&bufstart);

                    if(strcmp(token,done) )  //if its NOT the "done" message(that means it is [prime,time_to_find_prime])
                    {
                        //strcpy(primestr,token);   //store the prime
                        //strcat(primestr,",");
                        //printf("%s\n",primestr);
                        prime=atoi(token);   // convert the prime
                        token=strtok_r(NULL,",",&bufstart);
                        sscanf(token, "%lf", &t);
                        //strcat(primestr,token);
                        //printf("%s\n",primestr);
                        insertlistsorted(list,prime,t);   // insert the prime in the list with O(1) complexity
                        primecounter++;           // keep track of how many primes are saved into the list
                    }
                    if( !strcmp(token,done) )  //if the worker is done finding all primes, it sends "done" message , and time for Wi
                    {
                        token=strtok_r(NULL,",",&bufstart);
                        doneCounter++;       // then increase done counter
                        sscanf(token, "%lf", &worker_time);
                        Wtimes[i]=worker_time;
                    }
                    
                           
                    //printf("TOKEN=%s ",token);
                    //printf("%s\n",token);
                    //printf("%s\n",time);
                    }
                    
                }
                fds[i].fd = fd[i][0];
                fds[i].events = POLLIN;
                if(doneCounter == n)        // if all the workers are done with finding all primes
                    break;              // then stop the "for" loop
            }
                      
        }    
           
        if(doneCounter == n)      //and the "while" loop
            break; 
    }
    int tempbufsize;
    i=0;
    buf[0]='\0';
    while (i<primecounter)
    {
        tempbufsize=0;
        while( tempbufsize < (SIZE_OF_BUF-SIZE_OF_BUF/2) && i<primecounter)
        {
            //sprintf(token,"%d ",primearr[i]);
            token[0]='\0';
            token=removelist(list);
            tempbufsize=tempbufsize+strlen(token);
            strcat(buf,token);
            free(token);
            i++;
        }/*
        fds[0].fd=pipew;
        fds[0].events=POLLOUT;
        int retpol=poll(fds,n,5000);
        if( retpol == -1)
           perror("POLL");
        else if( retpol == 0) 
        {
            printf("INTERNAL:Poll timed - out .\n ");
            exit(1);
        }
        else if(retpol > 0)  //if some revent changed
        {
            if( fds[0].revents & POLLOUT )   //if the child has written something 
            {
                //printf("%s\n",buf);*/
                safe_write(pipew,buf,SIZE_OF_BUF);
               
               
        buf[0]='\0'; 
    } 
    //printf("%d %d\n",i, primecounter);
    strcpy(buf,"done");
    for(i=0; i<n; i++)
    {
        sprintf(token,",%f",Wtimes[i]);
        strcat(buf,token);
    }
    safe_write(pipew,buf,SIZE_OF_BUF);
    //listprint(list);/*
    /*item **primearr=malloc(primecounter*sizeof(item*)); // now we transfer the primes from list to an array to sort them faster with qsort
    for(i=0; i<n; i++)
        primearr[i]=NULL;

    listptr prevnode;
    i=0;
    while(list!=NULL)
    {
        prevnode=list;
        if(list->data == NULL)
        {
            prevnode=list;
            list=list->next;
            free(prevnode);
        }
        primearr[i]=list->data;
        prevnode=list;
        list=list->next;
        free(prevnode);
        i++;
    } 
    qsort(primearr,primecounter,sizeof(item*),cmpfunc); */
    //bufsiz=500;
    /*for(i=0; i<primecounter; i++)
        printf("%d %d %f\n",i,primearr[i]->prime, primearr[i]->time);
    int tempbufsize=0;
    token[0]='\0';
    buf[0]='\0';
    item bufpipe[SIZE_OF_BUF];
    for(i=0; i<SIZE_OF_BUF; i++)  //initialize the values of the buffer that will be parsed into the parent-root
    {
        bufpipe[i].prime=0;
        bufpipe[i].time=0.0;
    }
    i=0;
    while (i<primecounter)
    {
        tempbufsize=0;
        while( tempbufsize < (SIZE_OF_BUF) && i<primecounter)
        {
            bufpipe[tempbufsize].prime=primearr[i]->prime;
            bufpipe[tempbufsize].time=primearr[i]->time;
            //printf("%d %f\n",bufpipe[tempbufsize].prime,bufpipe[tempbufsize].time);
            tempbufsize++;
            i++;
        }
        if(tempbufsize<SIZE_OF_BUF)   // if the buffer is not full, set as zeros the remaining elements, because it may have some primes from the previous loop
        {
           while(tempbufsize < (SIZE_OF_BUF))
            {
                bufpipe[tempbufsize].prime=0;
                bufpipe[tempbufsize].time=0.0;
                tempbufsize++;
            } 
        }
        fds[0].fd=pipew;
        fds[0].events=POLLOUT;
        int retpol=poll(fds,n,300);
        if( retpol == -1)
           perror("POLL");
        else if( retpol == 0) 
        {
            printf("PRIME 1:Poll timed - out .\n ");
            exit(1);
        }
        else if(retpol > 0)  //if some revent changed
        {
            if( fds[0].revents & POLLOUT )   //if the child has written something 
            {
                //printf("%s\n",buf);
                //for(i=0; i<SIZE_OF_BUF; i++)
                   // printf("%d %f\n",bufpipe[i].prime,bufpipe[i].time);
                write(pipew,&bufpipe,sizeof(bufpipe));
            }   
        }        
        buf[0]='\0'; 
    }
    for(i=0; i<n; i++)
    {
        bufpipe[i].prime=-1;
        bufpipe[i].time=Wtimes[i];
    }
    write(pipew,&bufpipe,sizeof(bufpipe));
    /*
    while(i<n)  //do the same to parse all the times for workers
    {
        tempbufsize=0;
        while( tempbufsize < (SIZE_OF_BUF) && i<n)
        {
            bufpipe[tempbufsize].prime=-1;
            bufpipe[tempbufsize].time=Wtimes[i];
            tempbufsize++;
            i++;
        }
        if(tempbufsize<SIZE_OF_BUF)   // if the buffer is not full, set as zeros the remaining elements, because it may have some primes from the previous loop
        {
           while(tempbufsize < (SIZE_OF_BUF))
            {
                bufpipe[tempbufsize].prime=0;
                bufpipe[tempbufsize].time=0.0;
                tempbufsize++;
            } 
        } 
        
    }*/
    

    
    for(i=0; i<n; i++)
    {
        close(fd[i][0]);
        free(fd[i]);
    }
    free(buf);
    free(token);
    free(done);
    free(primestr);
    free(Wtimes);
    free(fd);
    //deletelist(list);
    /*
    free(fd);
    //free(primearr);
    free(buf);
    free(token);
    free(done); */
    return 0;   
}