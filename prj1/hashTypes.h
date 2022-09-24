#ifndef HEADER_HASH
#define HEADER_HASH

typedef int HTkey;


typedef struct studenttag                // list to save info about the student
{
    int studentID,zip,year;
    char *firstname;
    char *lastname;
    float gpa;
}student;

typedef student* HTitem;                 // we can use this name to make it easier in hash implementation, because the "item" is a pointer to a student.

typedef struct listtag            // node of a list used in the hashtable
{    
    student* item;                // pointer to student == HTitem  
    struct listtag *next;
} listnode;

typedef listnode* listptr;   //pointer to list

typedef struct HThashtag
{
    listptr* array;     //hashtable
    int size;           //size of hashtable
}HThash;


#endif


