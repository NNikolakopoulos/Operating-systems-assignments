
char *newpath(char *path1,char *path2);
void add_total_bytes(char *path);
void copyfile(char *src,char *dest,mode_t fdmode);
int delete_routine(char *src,char *dest);
int cycle_detect(char *src,char *dest);
int traverse_dir(char *src,char *dest,mode_t mode);
void print_statistics(double t);