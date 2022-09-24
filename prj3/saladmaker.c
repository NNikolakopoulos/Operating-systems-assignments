#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include "saladmaker.h"

int create_saladmaker(int lb,int ub,int id, int w)
{
    char *str;
    int val1,val2,sleeptime,n;
    Info* mem;
    FILE *fp=fopen("log.txt","a+");
    struct timeval now;
    struct tm* t;
    pid_t pid = getpid();

    srand(time(NULL));

    mem = (Info *) shmat(id, (void*) 0, 0);//attach

    //save the pid of each saladmaker in shared memory segment 
    switch(w) {
        case 0:
            str = strdup("saladmaker1");
            sem_wait(&(mem->mutex));
            mem -> maker1pid = pid;
            n = mem -> numSalads;
            sem_post(&(mem->mutex));
            break;
        case 1:
            str = strdup("saladmaker2");
            sem_wait(&(mem->mutex));
            mem -> maker2pid = pid;
            n = mem -> numSalads;
            sem_post(&(mem->mutex));
            break;
        case 2:
            str = strdup("saladmaker3");
            sem_wait(&(mem->mutex));
            mem -> maker3pid = pid;
            n = mem -> numSalads;
            sem_post(&(mem->mutex));
            break;
    }
    

    while(n > 0)
    {
        

       
        switch(w) {
            case 0:
                sem_wait(&(mem->log));
                fseek(fp, 0L, SEEK_END);
                gettimeofday(&now, NULL);
                t = localtime(&now.tv_sec);
                fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [%s] [Waiting for ingredients]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid,str);
                fflush(fp);
                sem_post(&(mem->log));
                sem_wait(&(mem -> saladmaker1));
            break;
            case 1:
                sem_wait(&(mem->log));
                fseek(fp, 0L, SEEK_END);
                gettimeofday(&now, NULL);
                t = localtime(&now.tv_sec);
                fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [%s] [Waiting for ingredients]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid,str);
                fflush(fp);
                sem_post(&(mem->log));
                sem_wait(&(mem -> saladmaker2));
            break;
            case 2:
                sem_wait(&(mem->log));
                fseek(fp, 0L, SEEK_END);
                gettimeofday(&now, NULL);
                t = localtime(&now.tv_sec);
                fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [%s] [Waiting for ingredients]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid,str);
                fflush(fp);
                sem_post(&(mem->log));
                sem_wait(&(mem -> saladmaker3));
            break;

        }
        sem_wait(&(mem->log));
        fseek(fp, 0L, SEEK_END);
        gettimeofday(&now, NULL);
        t = localtime(&now.tv_sec);
        fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [%s] [Get ingredients]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid,str);
        fflush(fp);
        sem_post(&(mem->log));

        sem_wait(&(mem->mutex));
        n = mem -> numSalads;
        sem_post(&(mem->mutex));
        if( mem -> numSalads <= 0)
            break;


        sem_post(&(mem -> chefReady));  // send message to chef that he received all the ulika
        


        sem_wait(&(mem->log));
        fseek(fp, 0L, SEEK_END);
        gettimeofday(&now, NULL);
        t = localtime(&now.tv_sec);
        fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [%s] [Start making salad]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid,str);
        fflush(fp);
        sem_post(&(mem->log));

        //now we are creating salad
        sleeptime = (rand() % (ub - lb + 1)) + lb;
        sleep(sleeptime);

        sem_wait(&(mem->log));
        fseek(fp, 0L, SEEK_END);
        gettimeofday(&now, NULL);
        t = localtime(&now.tv_sec);
        fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [%s] [End making salad]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid,str);
        fflush(fp);
        sem_post(&(mem->log));
        

        sem_wait(&(mem->mutex));
        (mem -> numSalads)--;
        n = mem -> numSalads;
        if(w==0) 
            (mem -> salads_maker1)++;
        else if (w==1)
            (mem -> salads_maker2)++;
        else if (w==2)
            (mem -> salads_maker3)++;
        sem_post(&(mem->mutex));

    }
    sem_post(&(mem->chefReady));
    sem_wait(&(mem->mutex));
    (mem->done)++;
    sem_post(&(mem->mutex));
    if( shmdt((void *) mem) == -1)
        perror("detatchment");
    free(str);
    fclose(fp);

}


int main(int argc, char** argv)
{

    int t1,t2,s,w;
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i],"-t1"))   
        {
            t1=atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-t2"))     
        {
            t2=atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-s"))     
        {
            s=atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-w"))     
        {
            w=atoi(argv[i+1]);
        }
    }
    create_saladmaker(t1,t2,s,w);
    return 0;
}

