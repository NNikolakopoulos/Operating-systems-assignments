
#ifndef HEADER_LIST
#define HEADER_LIST

typedef struct itemtag
{
    int prime;
    double time;
}item;

typedef struct listtag            // node of a list used in the hashtable
{    
    item *data;                // pointer to student == HTitem  
    struct listtag *next;
} listnode;

typedef listnode* listptr;   //pointer to list


#endif