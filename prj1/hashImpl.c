#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashInt.h"

int hashcode(int key,HThash hash)  {  return key % hash.size; }  // a simple hash function 

int countfilelines(char* filename)                 
{
    FILE *fp = fopen(filename,"r"); //open the file we want
    char c;   
    int lines=1;   // initialize number of lines (we check the possibility of being 0)

    if(fp == NULL)
        return 0;
    for(c=getc(fp); c != EOF; c = getc(fp))            // we are getting numbers from the file
        if( c == '\n')                                 //when we find an '\n' , we increase the number of lines
            lines++;
    fclose(fp);
    return lines;
}

///////////    list functions
listptr listcreate()           //create a list , by creating a dummy node 
{
    listptr dummy;
    dummy = (listptr)malloc(sizeof(listnode));
    dummy->next=NULL;                //only dummy node exists in list for now
    dummy->item=NULL;                //dummy node has no item
    return dummy;
}
 /////////////////////

void listinsert(listptr list,student* item)         //the memory for student is already allocated. thats why we take pointer.
{
    listptr new=NULL;        
    new=(listptr)malloc(sizeof(listnode));      
    new->item=item;             
    if(list == NULL)            // if the list is empty , 
        list=listcreate();      //   create a dummy node
    if (list->next==NULL)       // if only dummy node exists , 
    {
        list->next=new;         //   then simply add the new node
        new->next=NULL;
    }
    else                         // if there are more than 2 nodes in the list :    -|current node| --- |next node|      >>>>>      -|current node| --- |new node| --- |next node|
    {                            //                                                                                                                     ^^^^^^^^^^
        new->next=list->next;       //we insert the new node in between the previous nodes 
        list->next=new; 
    } 
}

//////////////////////


void deletelist(listptr list)      //function to delete all the nodes and the items of the nodes , in a list
{
    listptr currnode,nextnode;
    currnode=list;
    while (currnode != NULL)         //while list is not empty
    {  
        nextnode = currnode->next;    //save the next node
        if(currnode->item!=NULL)      //if the current node has item
        {
            free(currnode->item->firstname);
            free(currnode->item->lastname);
            free(currnode->item);     // then delete the item.
        }
        free(currnode);               //delete the current node
        currnode = nextnode;          //go to next node
    }

} 
void deletelistinvert(listptr list)    //this functions only deletes the nodes(not the items) { it will be used later in inverted index structure}
{
    listptr currnode,nextnode;
    currnode=list;
    while (currnode != NULL)  
    {  
        nextnode = currnode->next;  
        free(currnode);  
        currnode = nextnode;  
    }

}

//////// hastable functions


HThash HTcreate(char* filename)
{
    HThash hash;
    int i;
    hash.size=(countfilelines(filename));           //count the lines of the file
    hash.array=malloc((hash.size)*sizeof(listptr));         //create hash table with size=n( n = number of students = number of lines in the input file)
    for(i=0; i<hash.size; i++)                        // initialize the hash table
        hash.array[i]=NULL;
    return hash;
}

HThash HTcreateNofile()
{
    HThash hash;
    hash.size=100;
    hash.array=malloc(hash.size*sizeof(listptr));
    for(int i=0; i<hash.size; i++)                        // initialize the hash table
        hash.array[i]=NULL;
    return hash;
}


void HTinsertitem(HThash hash,HTitem item)     //insert an item(=student) to the hash table
{
    int i,index=hashcode(item->studentID,hash);
    listptr newlist;
    if(hash.array[index] == NULL)         //if the this cell of the array is NULL , then create a list and insert the student .
    {
        newlist=listcreate();
        listinsert(newlist,item);
        hash.array[index]=newlist;
    }
    else
    {
        listinsert(hash.array[index],item);    //if a list already exists , then just insert the student in the list
    }
         
}

void HTinsert(HThash hash,char *filename)     //insert all the students in the array. All we need is the name of the input file and the hashtable.
{
    FILE *fp = fopen(filename,"r");
    HTitem item;

    for(int i=0; i < hash.size; i++)   // hash.size == number of lines in the file( we use this, so we dont count them again)
    {
        item=(HTitem)malloc(sizeof(student));
        item->firstname=malloc(20*sizeof(char));
        item->lastname=malloc(20*sizeof(char));
        fscanf(fp,"%d %s %s %d %d %f\n", &(item->studentID) ,item->lastname , item->firstname, &(item->zip) , &(item->year), &(item->gpa));   //insert info in the item(=student)
        HTinsertitem(hash,item);   // insert the item to hash table
    }
    
    fclose(fp);
}

void HTprint(HThash hash)        // just to print all the students inside the hashtable.
{
    listptr currnode;
    for(int i=0; i<hash.size; i++)
    {
        if(hash.array[i] == NULL)
            continue;
        currnode=hash.array[i]->next; //skip dummy node
        while (currnode != NULL)  
        {  
            if(currnode->item!=NULL)
            {
                printf("%d %s %s %d %d %.2f\n",currnode->item->studentID, currnode->item->firstname , currnode->item->lastname,currnode->item->zip,currnode->item->year,currnode->item->gpa);   
                currnode = currnode->next;  
            }
        }
    }
}

void HTdelete(HThash hash)     //free all the memory for the hashtable AND the students
{
    int i;
    listptr templist;
    for(i=0; i < hash.size; i++)
        deletelist(hash.array[i]);
    free(hash.array);

}
