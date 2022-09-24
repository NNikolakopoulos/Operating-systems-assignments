#ifndef HEADER_TYPES
#define HEADER_TYPES


typedef struct Infotag
{
    pid_t maker1pid,maker2pid,maker3pid;
    int numSalads,salads_maker1,salads_maker2,salads_maker3,done;
    sem_t mutex,chefReady,saladmaker1,saladmaker2,saladmaker3,log;

}Info;



#endif
