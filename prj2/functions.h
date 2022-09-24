void spawnChildren(int newlb,int newub,int interval,int modulo,int n,int **fd,bool isWorker,int primeAlgo,int pid);
int safe_write(int fd,char* buf,size_t size);
int safe_read(int fd,char* buf,size_t size);
int prime1(int n);
int prime2(int n);
int prime3(int n); 
int cmpfunc (const void * a, const void * b); 
//void sigfun(int signo);