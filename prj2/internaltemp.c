
int main(int argc,char *argv[])
{
    int lb,ub,n,pipew,primeAlgo;

    lb=atoi(argv[1]);  
    ub=atoi(argv[2]);
    n=atoi(argv[3]);
    pipew=atoi(argv[4]);
    primeAlgo=atoi(argv[5]);
    //printf("%d %d %d %d\n",lb,ub,n,pipew);
    
    int  i,j,interval=ub-lb,newlb,newub,modulo;
    modulo=interval%n;
    interval=interval/n;
    
    newlb=lb;
    newub=lb+interval;

    pid_t pid;
    struct pollfd fds[n];
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
    spawnChildren(newlb,newub,interval,modulo,n,fd,true,primeAlgo);    

    // we will need a buffer to store temporarily the info collected by pipes, from the lead nodes
    char *buf=malloc(30*sizeof(char)), *bufstart, *done="done",*token=malloc(30*sizeof(char));
    int prime,bufsiz;

    listptr list=listcreate();

    // in the following loop we must collect 
    // all the primes and their executions times from leaf nodes 
    int doneCounter=0;
    while(1)    
    {   
        int retpol=poll(fds,n,300);
        if( retpol == -1)
            perror("POLL");
        else if( retpol == 0) 
        {
            printf("INTERNAL:Poll timed - out .\n ");
            exit(1);
        }
        else if(retpol > 0)  //if some revent changed
        {
            for(i=0; i<n; i++)    //check every element in fd array
            {
                if( fds[i].revents == POLLIN )   //if the child has written something
                {
                    if(fds[i].fd == fd[i][0]) {
                    bufsiz=read(fds[0].fd,buf,sizeof(buf));
                    if(strcmp(buf,done) != 0)
                        buf[bufsiz]='\0';
                    printf("read:%s\n",buf);
                    //bufstart=buf;
                    //token=strtok_r(buf," ",&bufstart);/*
                    if( !strcmp(buf,done) )
                    {
                        doneCounter++;
                       // printf("%d\n",doneCounter);
                    } 
                    }
                        
                    //printf("%d\n",doneCounter);
                    //prime=atoi(token);
                    //prime=atoi(buf);
                    //insertlistsorted(list,prime);
                }
                //if(fds[i].revents & POLLHUP)
                    //doneCounter++; 
                fds[i].fd = fd[i][0];
                fds[i].events = POLLIN;
                if(doneCounter == n)
                    break; 
            }    
        }   
        //printf("%d\n",doneCounter);
        if(doneCounter == n)
            break; 
        
        
    }
    //wait(NULL);
    /*
    //for(i=0; i<n; i++)
    //{
    //    close(fd[i][0]);
    //    free(fd[i]);
    //} */
    //free(fd);
    //printf("tzet\n");
    strcpy(buf,done);
    write(pipew,buf,sizeof(buf));
    return 0; 
}