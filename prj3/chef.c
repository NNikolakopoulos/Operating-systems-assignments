#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>


#include "chef.h"

int create_chef(int n,int mantime)
{
    int id, retval, val1,val2,val3,sum;
    Info* mem;
    int food1,food2,prevfood1,prevfood2;
    sem_t sp;
    FILE *fp=fopen("log.txt","a+");
    struct timeval now;
    struct tm* t;
    pid_t pid=getpid();


    srand((unsigned) time(NULL));

    id = shmget(IPC_PRIVATE, sizeof(Info), IPC_CREAT | IPC_EXCL | 0666);  //create shared memory segment
    if( id < 0) {
        perror("shmget error\n");
        exit(1);
    }

    printf("shared memory id is: %d\n",id);
    
    
    mem = (Info *) shmat(id, (void*) 0, 0);  //attach

    // when everybody detaches, the shared memory segment will be destroyed
    if(shmctl (id ,IPC_RMID ,0) <0) {
        perror("shmctl error\n");
        exit(1);
    }



    //initialization of shared mem
    if ( sem_init (&(mem->log) ,1 ,1) != 0) {
        perror ( " Couldn ’t initialize ." ); 
        exit (3) ; 
    }  
    if ( sem_init (&(mem->mutex) ,1 ,1) != 0) {
        perror ( " Couldn ’t initialize ." ); 
        exit (3) ; 
    }  
    if ( sem_init (&(mem->chefReady) ,1 ,0) != 0) {
        perror ( " Couldn ’t initialize ." ); 
        exit (3) ; 
    }  
    if ( sem_init (&(mem->saladmaker1) ,1 ,0) != 0) {
        perror ( " Couldn ’t initialize ." ); 
        exit (3) ; 
    }  
    if ( sem_init (&(mem->saladmaker2) ,1 ,0) != 0) {
        perror ( " Couldn ’t initialize ." ); 
        exit (3) ; 
    }  
    if ( sem_init (&(mem->saladmaker3) ,1 ,0) != 0) {
        perror ( " Couldn ’t initialize ." ); 
        exit (3) ; 
    }  
    
    sem_wait(&(mem->mutex));
    mem -> numSalads = n;
    mem -> salads_maker1 = 0;
    mem -> salads_maker2 = 0;
    mem -> salads_maker3 = 0;
    mem -> done = 0;
    sem_post(&(mem->mutex));

    food1=rand()%3;   // choose a random food1,food2
    food2=rand()%3;

    prevfood1=-1;   // initialize as -1 the previous foods 
    prevfood2=-1;
    while(1)
    {
        //printf("%d\n",n);
       /* sem_wait(&(mem->mutex));
        n = mem -> numSalads;
        sem_post(&(mem->mutex));
        if( n <= 0)
        {
            sem_post(&(mem->saladmaker1));
            sem_post(&(mem->saladmaker2));
            sem_post(&(mem->saladmaker3));
            break;
            
        }*/
         // dont let the chef choose the same foods as before 
         // or if the food1,food2 are the same , choose again different foods
        if((food1 == prevfood1 && food2 == prevfood2) || (food1 == food2))  
        {
            food1=rand()%3;
            food2=rand()%3;
            continue;
        }

        
        //food = 0(for onion), =1(for pepper), =2(for tomato)

        if((food1 == 0 && food2 == 1) || (food1 == 1 && food2 == 0)) {
            sem_wait(&(mem->log));
            fseek(fp, 0L, SEEK_END);
            gettimeofday(&now, NULL);
            t = localtime(&now.tv_sec);
            fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [Selecting ingredients kremmidi piperi]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
            fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [Notify saladmaker 3]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
            fflush(fp);
            sem_post(&(mem->log));
            sem_post(&(mem -> saladmaker3));
            
        }
        if((food1 == 0 && food2 == 2) || (food1 == 2 && food2 == 0)) {
            sem_wait(&(mem->log));
            fseek(fp, 0L, SEEK_END);
            gettimeofday(&now, NULL);
            t = localtime(&now.tv_sec);
            fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [Selecting ingredients kremmidi ntomata]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
            fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [Notify saladmaker 2]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
            fflush(fp);
            sem_post(&(mem->log));
            sem_post(&(mem -> saladmaker2));
        }
        if((food1 == 1 && food2 == 2)|| (food1 == 2 && food2 == 1)) {
            sem_wait(&(mem->log));
            fseek(fp, 0L, SEEK_END);
            gettimeofday(&now, NULL);
            t = localtime(&now.tv_sec);
            fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [Selecting ingredients piperi ntomata]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
            fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [Notify saladmaker 1]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
            fflush(fp);
            sem_post(&(mem->log));
            sem_post(&(mem -> saladmaker1));
        }

        sem_wait(&(mem->mutex));
        n = mem -> numSalads;
        sem_post(&(mem->mutex));

        if( n <= 0)
        {
            sem_post(&(mem->saladmaker1));
            sem_post(&(mem->saladmaker2));
            sem_post(&(mem->saladmaker3));
            break;
        }
        sem_wait(&(mem -> chefReady));
        
        prevfood1=food1;
        prevfood2=food2;

        //print message in log file , that is time to sleep
        sem_wait(&(mem->log));
        fseek(fp, 0L, SEEK_END);
        gettimeofday(&now, NULL);
        t = localtime(&now.tv_sec);
        fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [man time for resting]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
        fflush(fp);
        sem_post(&(mem->log));

        sleep(mantime);
        
    }
    
    while(1) {
        sem_wait(&(mem->mutex));
        n = mem -> done;
        sem_post(&(mem->mutex));
        if(n == 3)
            break;
    }
        

    sum = mem->salads_maker1+mem->salads_maker2;
    sum+=mem->salads_maker3;
    printf("number of salads =%d\n",sum);
    printf("number of salads of salad_maker1 [%d] : [%d]\n",mem->maker1pid,mem->salads_maker1);
    printf("number of salads of salad_maker2 [%d] : [%d]\n",mem->maker2pid,mem->salads_maker2);
    printf("number of salads of salad_maker3 [%d] : [%d]\n",mem->maker3pid,mem->salads_maker3);

    sem_destroy(&(mem -> mutex));
    sem_destroy(&(mem -> chefReady));
    sem_destroy(&(mem -> saladmaker1));
    sem_destroy(&(mem -> saladmaker2));
    sem_destroy(&(mem -> saladmaker3)); 
    if( shmdt((void *) mem) == -1)
        perror("detatchment");

        // print end message in file
    fseek(fp, 0L, SEEK_END);
    gettimeofday(&now, NULL);
    t = localtime(&now.tv_sec);
    fprintf(fp,"[%02d:%02d:%02d:%02ld] [%d] [Chef] [End]\n", t->tm_hour, t->tm_min, t->tm_sec, now.tv_usec / 1000,pid);
    fflush(fp);

    char *temp_date=malloc(20*sizeof(char));
    char *status=malloc(30*sizeof(char));
    char * temp_pid=malloc(100*sizeof(char));
    char * proffesion=malloc(100*sizeof(char));
    int concurrent_saladmakers=0;
    size_t opa=sizeof(temp_pid);
    fclose(fp);
    fp=fopen("log.txt","r");


        
    while(1) {

        fscanf(fp,"[%15[^]]] [%7[^]]] [%15[^]]] [%50[^]]]\n", temp_date,proffesion,temp_pid,status);

        if(!strcmp(status,"End"))
            break;

        if(!strcmp(status,"Start making salad")) {
            concurrent_saladmakers++;
            if(concurrent_saladmakers==2) {     //if we have 2 saladmakers making salad concurrently,
                printf("[%s,",temp_date);
            }
        }
        else if(!strcmp(status,"End making salad")) {
            concurrent_saladmakers--;
            if(concurrent_saladmakers==1) {     //if we have 2 saladmakers making salad concurrently,
                printf("%s]\n",temp_date);
            }
        }
        //else
          //  break;
    }
    fclose(fp);

    free(temp_date);
    free(status);
    free(temp_pid);
    free(proffesion);

}


int main(int argc,char** argv)
{
    int n,mantime;
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i],"-m"))   
        {
            mantime=atoi(argv[i+1]);
        }
        if(!strcmp(argv[i],"-n"))     
        {
            n=atoi(argv[i+1]);
        }
    }
    create_chef(n,mantime);
    return 0;
}

