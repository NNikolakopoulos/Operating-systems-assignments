#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listInt.h"

listptr listcreate()           //create a list , by creating a dummy node 
{
    listptr dummy;
    dummy = (listptr)malloc(sizeof(listnode));
    dummy->next=NULL;                //only dummy node exists in list for now
    dummy->data=NULL;
    return dummy;
}
 /////////////////////

void listinsert(listptr list,int prime,double time)         //the memory for student is already allocated. thats why we take pointer.
{
    listptr new=NULL;        
    new=(listptr)malloc(sizeof(listnode));        
    new->data=malloc(sizeof(item));
    new->data->prime=prime;
    new->data->time=time;
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


void insertlistsorted(listptr list,int prime,double time)   //insert students with descending gpa order in the list.
{
    listptr new=NULL;        
    new=(listptr)malloc(sizeof(listnode));        
    new->data=malloc(sizeof(item));
    new->data->prime=prime;
    new->data->time=time;
    if(list == NULL)            // if the list is empty , 
        list=listcreate();      //   create a dummy node
    if (list->next==NULL)       // if only dummy node exists , 
    {
        list->next=new;         //   then simply add the new node
        new->next=NULL;
    }
    else
    {
        listptr temp = list->next;
        listptr prev = list;
        while(temp != NULL && temp->data->prime < prime) 
        {
            prev = temp;
            temp = temp->next;
        }
        new->next = temp;
        prev->next = new;
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
        if(currnode->data!=NULL)
            free(currnode->data);
        free(currnode);               //delete the current node
        currnode = nextnode;          //go to next node
    }

} 

char *removelist(listptr list)
{
    if (list!=NULL)
    {
        char *str=(char *) malloc(15*sizeof(char));
        listptr currnode=list->next,nextnode;
        if(currnode != NULL)  //if NOT only dumme node exists
        {
            sprintf(str,"%d,%f ",currnode->data->prime,currnode->data->time);
            if(currnode->next != NULL)
                list->next=currnode->next;
            if(currnode->data != NULL)
                free(currnode->data);
            free(currnode);
            return str;
        } 
    }
    else
        printf("list empty");
    return '\0';
    

}

void listprint(listptr list)
{
    listptr currnode=list->next;
    while(currnode!= NULL)
    {
        printf("PRIME=%d  %f\n",currnode->data->prime,currnode->data->time);
        currnode=currnode->next;
    }
}