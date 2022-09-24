#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "invInt.h"
#include "hashInt.h"

invertedptr invertedcreate()
{
    invertedptr dummy;
    dummy = (invertedptr)malloc(sizeof(inverted));
    dummy->next=NULL;
    dummy->year=0;
    dummy->stud=listcreate();
    return dummy;
}
void insertlistsorted(listptr list,listptr studptr)   //insert students with descending gpa order in the list.
{
    listptr current=list->next;       
    while(current!=NULL)
    {
        if(current->next==NULL || current->next->item->gpa <= studptr->item->gpa)
        {
            listinsert(current,studptr->item);
            break;
        }
        current=current->next;    
    }
}
void invertedinserthelp(invertedptr inv,listptr studptr)
{
    bool flag;
    invertedptr currentinverted=inv,newinverted=NULL;
    listptr templist;             

    if(currentinverted->next == NULL )                       //if only dummy node exists
    {
        newinverted=(invertedptr)malloc(sizeof(inverted));     //we create the inverted index list node 

        newinverted->year=-1;
        newinverted->next=NULL;
        newinverted->stud=listcreate();
        listinsert(newinverted->stud,studptr->item);                 // and we insert in that node the student
        currentinverted->next = newinverted;      // now dummy node is pointing to the 1st actual node of the inverted index list
        newinverted->year=newinverted->stud->next->item->year;     
    }
    else
    {
        currentinverted=currentinverted->next;    //skip dummy node
        flag=true;                                             // we use a flag to see if exists a node with the student's year of study 
        while(currentinverted != NULL)  
        {
            if(currentinverted->year == studptr->item->year)        //if we find the student's year of study in the inverted index list 
            {
                
                insertlistsorted(currentinverted->stud,studptr);
                //listinsert(currentinverted->stud,studptr->item);           // just add the pointer to student in the existing list
                flag=false;                                          // we inserted the student pointer in the inverted index list , so we skip the following if(...) statement.
                break;  
            }
            currentinverted=currentinverted->next;            //we skip the dummy node and then keep going
        }

        if(flag)         // if it's true then we must create a new node in the inverted index list
        {    
            newinverted=(invertedptr)malloc(sizeof(inverted));     //we create the inverted index list node 

            newinverted->stud=listcreate();
            listinsert(newinverted->stud,studptr->item); 
            newinverted->next=inv->next;
            inv->next=newinverted;
            newinverted->year=newinverted->stud->next->item->year;
        }
    }

}

void invertedinsert(invertedptr list,HThash hash)
{
    listptr studptr;

    for(int i=0; i<hash.size; i++)                          //for every i in the hashtable  = for every student in the hashtable
    {        
        studptr=hash.array[i];    
        if(studptr != NULL)
        {
            studptr=studptr->next;                       
            while( studptr != NULL)                                //while a node exist in this particular array[i]                                  
            {           
                invertedinserthelp(list,studptr);
                studptr=studptr->next; 
            }
            
        }
    }
}

//////////////////////


void invertedelete(invertedptr list)
{
    invertedptr currnode,nextnode;
    currnode=list;
    while (currnode != NULL)  
    {  
        nextnode = currnode->next;  
        if(currnode->stud!=NULL)
            deletelistinvert(currnode->stud);
        free(currnode);  
        currnode = nextnode;  
    }
}

void invertedprint(invertedptr list)
{
    invertedptr currnode;
    currnode=list->next;   //skip dummy node 
    while (currnode != NULL)  
    {
        if(currnode->stud!=NULL)   
        {
            listprint(currnode->stud);
        }
        currnode = currnode->next;  
    }
}

void listprint(listptr list)
{
    listptr currnode=list->next;
    while (currnode != NULL)  
    {
        if(currnode->item != NULL)
            printf("%d   %d    %.2f\n",currnode->item->studentID,currnode->item->year,currnode->item->gpa);
        currnode = currnode->next;  
    }

}