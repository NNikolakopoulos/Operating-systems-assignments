#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
# include <sys/times.h>

#include "fun.h"
#include "globalVars.h"

int total_bytes = 0;  // total bytes copied
bool verbose_flag = false;   //verbose flag
bool delete_flag = false;    // delete flag
bool links_flag = false;     // links flag

// returns string: path1/path2
char *newpath(char *path1,char *path2)
{
    char *newpath = (char *) malloc( strlen(path1) + strlen(path2) + 3 );
    strcpy(newpath,path1);
    strcat(newpath,"/");
    strcat(newpath,path2);
    return newpath;
}

void add_total_bytes(char *path) {
    struct stat statbuf;
    lstat(path,&statbuf);
    total_bytes+=statbuf.st_size;
}

// copy src to dest
void copyfile(char *src,char *dest,mode_t fdmode)
{
    char buf[BUFSIZ];
    ssize_t bytes_read;
    int fd1,fd2;

    if(( fd1 = open (src, O_RDONLY)) < 0 ) {
        perror ( " open source"); 
        exit (1); 
    }
    if (( fd2 = open (dest , O_WRONLY | O_CREAT | O_TRUNC , fdmode)) < 0 ) {     
        printf(" (%04o)\n", fdmode);
        perror ( " open destination"); 
        exit (1); 
    }

    while(( bytes_read = read ( fd1 , buf , sizeof ( buf ))) > 0 ) {
        write (fd2, buf, bytes_read);
        total_bytes += bytes_read;
    }
       
    close (fd1); 
    close (fd2); 
    if(verbose_flag)
        printf("%s\n",src);
}


int delete_routine(char *src,char *dest)
{
    DIR *dirp1, *dirp2;
    struct dirent *direntp1, *direntp2;
    struct stat statbuf1, statbuf2;
    char *newname1, *newname2, *abspath1, *abspath2;
    bool flag=true;

    if(((dirp1=opendir(src)) == NULL) &&(ENOENT != errno) ) {
        fprintf(stderr,"delete source:cannot open %s\n",src);
        exit(1);
    }
    if(((dirp2=opendir(dest)) == NULL)) { //if destination does not exists, 
        fprintf(stderr,"delete dest: cannot open %s\n",dest);
        exit(1);
    }    
    while( (direntp2 = readdir(dirp2)) != NULL) {
        flag=false; // initialize flag for every directory entry of source path
        if(direntp2->d_ino == 0 || !strcmp(direntp2->d_name, ".") || !strcmp(direntp2->d_name, "..")) 
            continue;

        newname2 = newpath(dest,direntp2->d_name);   //update the path and look for this particular directory entry:   ./dest/direntp2->d_name
        
        newname1 = newpath(src,direntp2->d_name); // we need to check if ./src/direntp2->d_name exists

        lstat(newname2,&statbuf2);

        // if it does not exists in source directory, then we must delete it from destination
        if ( (lstat(newname1,&statbuf1) < 0)){ //&& errno == ENOENT) { 
            switch(statbuf2.st_mode & S_IFMT) {
                case(S_IFREG):
                    remove(newname2);
                    printf("deleting: %s\n",newname2);
                    break;
                case(S_IFDIR):
                    delete_routine(newname1,newname2);
                    remove(newname2);
                    printf("deleting: %s\n",newname2);
                    free(newname2);
                    free(newname1);
                   continue;
                case(S_IFLNK):
                    if(links_flag) {
                        remove(newname2);
                        printf("deleting: %s\n",newname2);
                    }
                    break;
            }
        }
        if((statbuf2.st_mode & S_IFMT) == S_IFDIR) {
            delete_routine(newname1,newname2);
        }
        free(newname2);
        free(newname1);
    }
    closedir(dirp1);
    closedir(dirp2);
}

int cycle_detect(char *src,char *dest)
{
    char *abspath2 = realpath(dest,NULL), *abspath1 = realpath(src,NULL), *temp;
    int i,len = strlen(abspath2);
    //temp = strdup(abspath2);
    for( i=len-1; i>0; i--) {
        if(abspath2[i] == '/'){
            abspath2[i] = '\0';
            if( !strcmp(abspath2,abspath1)) {
                free(abspath1);
                free(abspath2);
                return true;
            }
                
            break;
        }
    }
    free(abspath1);
    free(abspath2);
    return false;

}

int traverse_dir(char *src,char *dest,mode_t mode)
{
    DIR *dirp1, *dirp2, *tempdir;
    struct dirent *direntp1, *direntp2;
    struct stat statbuf1, statbuf2;
    char *newname1, *newname2, *abspath1, *abspath2;
    bool flag=true;
    int len;

    if((dirp1=opendir(src)) == NULL) {
        fprintf(stderr,"cannot open %s\n",src);
        exit(1);
    }
    if(((dirp2=opendir(dest)) == NULL) && (ENOENT == errno)) { //if destination does not exists, \

        mkdir(dest,mode);       // create destination directory with given mode
        if(cycle_detect(src,dest)) {
            printf("cycle detected: src:%s    dest:%s\n",src,dest);
            return 0;
        }
            
        dirp2=opendir(dest); // open the new directory
        //dont forget to add the size of the directory( 1024 bytes==1 block) in total_bytes (copied)
        add_total_bytes(dest);

        if(verbose_flag)
            printf("%s\n",src);
    }
    else if(dirp2 == NULL) {
        fprintf(stderr,"cannot open %s\n",dest);
        exit(1);
    }    
    
    while( (direntp1 = readdir(dirp1)) != NULL) {
        flag=true; // initialize flag for every directory entry of source path
        if(direntp1->d_ino == 0 || !strcmp(direntp1->d_name, ".") || !strcmp(direntp1->d_name, "..")) 
            continue;

        newname1 = newpath(src,direntp1->d_name);   //update the path and look for this particular directory entry:   ./src/direntp1->d_name
        
        lstat(newname1,&statbuf1);   

        closedir(dirp2);
        dirp2=opendir(dest);
        
        while( (direntp2 = readdir(dirp2)) != NULL) {
            if(direntp2->d_ino == 0 || !strcmp(direntp2->d_name, ".") || !strcmp(direntp2->d_name, "..")) 
                continue;

            newname2 = newpath(dest,direntp2->d_name);

            lstat(newname2,&statbuf2);
        
            if(!(strcmp(direntp1->d_name,direntp2->d_name)))  // if they have the same name
            {
                // if one inode refers to file and the other to catalog, obviously they are not identical
                if( (((statbuf1.st_mode & S_IFMT) == S_IFREG) && ((statbuf2.st_mode & S_IFMT) == S_IFDIR)) \
                    || (((statbuf2.st_mode & S_IFMT) == S_IFREG) && ((statbuf1.st_mode & S_IFMT) == S_IFDIR)) ) {
                        //continue searching in this directory for a possible file or directory that is identical to source/file or source/directory
                        continue;
                }
                else if(((statbuf1.st_mode & S_IFMT) == S_IFDIR) && ((statbuf2.st_mode & S_IFMT) == S_IFDIR)) { //if both inodes refer to catalogs
                    traverse_dir(newname1,newname2,0644); //   we will recursively check their entries(the mode does not really matters, because we won't create new directory in destination)
                }
                else if(((statbuf1.st_mode & S_IFMT) == S_IFREG) && ((statbuf2.st_mode & S_IFMT) == S_IFREG)) {  //if both inodes refer to files
                    if( statbuf1.st_size != statbuf2.st_size)   // if they haven't the same size, they are not identical,
                        break;   // stop the loop, to copy the source/file in destination                        
                    // if they have the same size, we must check for the last modification date
                    else if( (statbuf1.st_size == statbuf2.st_size) && (statbuf1.st_mtime > statbuf2.st_mtime)) 
                        break;
                }
                else if(((statbuf1.st_mode & S_IFMT) == S_IFLNK) && ((statbuf2.st_mode & S_IFMT) == S_IFLNK)) {  //if they are symbolink links
                    abspath1 = (char *) malloc( PATH_MAX );
                    abspath2 = (char *) malloc( PATH_MAX );
                    len = readlink(newname1, abspath1, sizeof(abspath1)-1);
                    abspath1[len] = '\0';
                    len = readlink(newname2, abspath2, sizeof(abspath2)-1);
                    abspath2[len] = '\0';

                    //if they dont have the same path, we need to create it
                    if( strcmp(abspath1,abspath2) ) {
                        free(abspath1);
                        free(abspath2);
                        remove(newname2);
                        break;
                    }
                    free(abspath1);
                    free(abspath2);
                }
                flag=false;  //that means these entities are the same and there is no need to copy
            }
            
            free(newname2);
        }
        if(flag) {
            newname2 = newpath(dest,direntp1->d_name);
            switch((statbuf1.st_mode & S_IFMT)) {
                case(S_IFREG):
                    mode = statbuf1.st_mode & 0777; // save the mode for owner,group,others
                    copyfile(newname1,newname2,mode);  // just copy the file
                    break;
                case(S_IFDIR):
                    mode = statbuf1.st_mode & 0777;
                    traverse_dir(newname1,newname2,mode); // keep going deeper in the hierarchy 
                    break;
                case(S_IFLNK):
                    if(links_flag) {
                        abspath1 = (char *) malloc( PATH_MAX );
                        len = readlink(newname1, abspath1, PATH_MAX-1);
                        abspath1[len] = '\0';
                        symlink(abspath1,newname2);
                        free(abspath1);
                        add_total_bytes(newname2);
                        if(verbose_flag)
                            printf("%s\n",newname1);
                    }
                    break;
            }
            free(newname2);
        }
        //closedir(dirp2);
        free(newname1);
    }
    closedir(dirp1);
    closedir(dirp2);
}
void print_statistics(double t) 
{
    double   time,ticspersec = ( double ) sysconf ( _SC_CLK_TCK );
    time = t/ticspersec;
    printf("\ntotal bytes copied: %d  in %.4fsec at %.2f bytes/sec\n",total_bytes, time, total_bytes/time);
}

