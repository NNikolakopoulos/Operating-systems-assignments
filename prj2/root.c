#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <fcntl.h>
#include <ctype.h>

#include "functions.h"
#include "listInt.h"


#define SIZE_OF_BUF 500

int sig_counter=0;

void sigfun(int signo)//, siginfo_t *info, void *extra)
{
       //void *ptr_val = info->si_value.sival_ptr;
      // int int_val = info->si_value.sival_int;
       sig_counter++;
       //printf("Signal: %d, value: [%d] %s\n", signo, int_val, (char*)ptr_val);

}
void createChildren(int lb, int ub, int n)
{
    
    int  i,j, interval=ub-lb, newlb, newub,modulo;
    modulo=interval%n;
    interval=interval/n;
    newlb=lb;
    newub=lb+interval;
    printf("%d\n",getpid());
    struct pollfd fds[n];


    
    int **fd = (int **)malloc(n * sizeof(int *));    
    char **primearr=malloc(n*sizeof(char*));   // in this array we will save the primes collected from all the internal nodes

    for (i=0; i<n; i++) 
        fd[i] = (int *)malloc(2*sizeof(int)); 
     for (i=0; i<n; i++) 
     {
        primearr[i]=NULL;
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

    pid_t pid=getpid();
    spawnChildren(newlb,newub,interval,modulo,n,fd,false,0,pid);     // create internal nodes, so we set the last argument as false to do the exec for internal

    int doneCounter=0,bufsiz=500;
    item bufpipe[SIZE_OF_BUF];

    char *done="done",*buf=malloc(SIZE_OF_BUF*sizeof(char)),*temp=NULL, *bufstart=NULL, *token=malloc(15*sizeof(char));

    double **Wtimes=(double **) malloc(n*sizeof(double*));
    for(i=0; i<n; i++)
        Wtimes[i]=(double *) malloc(n*sizeof(double));


    //struct sigaction action;s

   // action.sa_flags = SA_SIGINFO;
    //action.sa_sigaction = &sigfun;
    //sigemptyset(&action.sa_mask);
   // action.sa_flags =SA_RESTART; 
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &sigfun;
    if(sigaction(SIGUSR1, &sa, NULL))
            perror("fail\n");
    
    //sigaction(SIGUSR1, &sa, NULL);

    while(1)
    {
        //printf("katse\n");
        //if(sigaction(SIGUSR1, &action, NULL)==0)
            //printf("%d\n",sig_counter);
        
        int retpol=poll(fds,n,0);
        if( retpol == -1 && errno == EINTR)
            continue;
        else if( retpol == 0) 
        {
            //printf("ROOT:Poll timed - out .\n ");
            continue;
        }
        else if(retpol > 0)  //if some revent changed
        {
            for(i=0; i<n; i++)    //check every element in fd array
            {
                //printf("%d\n",fds[i].revents );
                if( fds[i].revents & POLLIN )   //if the child written something
                {
                    //printf("TZET\n");
                    //if(fds[i].fd == fd[i][0]) 
                    //{
                        bufsiz=safe_read(fds[i].fd,buf,SIZE_OF_BUF);
                        //printf("%s\n",buf);

                        if( buf[0] == 'd' ) //if the internal node is done sending all primes
                        {
                            doneCounter++;   // if doneCounter==branch factor , then all internal nodes finished their jobs
                            token = strtok(buf,",");
                            token= strtok(NULL,",");
                            j=0;
                            while(token!=NULL)
                            {
                                sscanf(token, "%lf", &(Wtimes[i][j]));
                                token=strtok(NULL,",");
                                j++;
                            }
                               // printf("%d\n",doneCounter);
                        } 
                        else
                        {
                            if(primearr[i]==NULL)
                            {
                                temp=malloc(10*(sizeof(char)));
                            }
                            else
                            {
                                temp=primearr[i];
                            }
                            //printf("OPA\n");
                            temp=realloc(temp,strlen(temp)+strlen(buf)+2);
                            primearr[i]=temp;
                            strcat(primearr[i],buf);
                            //printf("%s\n",primearr[i]);
                        }
                             
                   // }
                        /*
                        //for(i=0; i<SIZE_OF_BUF; i++)
                           // printf("%d %f\n",bufpipe[i].prime,bufpipe[i].time);
                        for(j=0; j<SIZE_OF_BUF; j++)
                        {
                            //printf("%d\n",j);
                            if( bufpipe[j].prime == -1 ) //if the internal node is done sending all primes
                            {
                                doneCounter++;
                                Wtimes[i][j]=bufpipe[j].time;  //store the time for the i-th worker
                                //printf("%d\n",doneCounter);
                            } 
                            else if( bufpipe[j].prime == 0)
                            {
                                break;
                            }
                            else if( bufpipe[j].prime > 0)
                            {
                                listinsert(arr_end[i],bufpipe[j].prime,bufpipe[j].time);  // add the prime with its time, to the last position in the list for the i-th internal node
                                arr_end[i]=arr_end[i]->next; //dont forget to set the this pointer to the last element
                            }*/
                            
                        
                           //printf("%d %f\n",bufpipe[i].prime, bufpipe[i].time);
                            
                        //printf("%s\n",primearr[i]);
                        //token=strtok_r(buf," ",&bufstart);/*
                }
                fds[i].fd = fd[i][0];
                fds[i].events = POLLIN;
                if(doneCounter == n)
                    break; 
            }  
              
        }    
        if(doneCounter == n)
            break;
    }

    double minTime=1000.0,maxTime=-1.0;
    for(i=0; i<n; i++)
    {
        printf("%s\n",primearr[i]);
        for(j=0; j<n; j++)
        {
            if(Wtimes[i][j] > maxTime)  
                maxTime=Wtimes[i][j];
            if(Wtimes[i][j] < minTime) 
                minTime=Wtimes[i][j];
        }
    }
    printf("min time=%f\nmax time=%f\n",minTime,maxTime);
    int counter=0;
    for(i=0; i<n; i++)
    {
        for(j=0; j<n; j++)
        {
            printf("W%d time=%f\n",counter,Wtimes[i][j]);
            counter++;
        }
    }
    for(i=0; i<n; i++)
    {
        close(fd[i][0]);
        free(primearr[i]);
        free(Wtimes[i]);
        free(fd[i]);
    }
    free(fd); 
    free(Wtimes);
    free(primearr);
    free(buf);
    free(token);
}


void root(int lb,int ub,int n)
{
    pid_t pid;
    createChildren(lb,ub,n);
    printf("signals received=%d\n",sig_counter)
    return;
        
}
