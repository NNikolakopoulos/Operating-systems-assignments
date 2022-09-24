#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "funInt.h"
#include "hashInt.h"
#include "invInt.h"

void configfun(char *filename)
{
    FILE *fptr;
    char c;
    fptr = fopen(filename, "r"); 
    c = fgetc(fptr); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(fptr); 
    } 
        
    fclose(fptr); 
}

student *insertstudent(int id,int y, int z, char *lname,char* fname,float degree)
{
    student* stud=malloc(sizeof(student));
    stud->lastname=malloc(20*sizeof(char));
    stud->firstname=malloc(20*sizeof(char));
    stud->studentID=id;
    stud->year=y;
    stud->zip=z;
    strcpy(stud->lastname,lname);
    strcpy(stud->firstname,fname);
    stud->gpa=degree;
}

void insert(HThash hash,invertedptr inv,int id,int y, int z, char *lname,char* fname,float degree)
{
    bool flag;
    student *stud;
    listptr ptr,newlist;
    int i=hashcode(id,hash);
    ptr=hash.array[i];

    if(ptr==NULL)      //if it doesnt exist in the hash table , then create a new list in hash.array[i] and insert him
    {
        stud=insertstudent(id,y,z,lname,fname,degree);
        newlist=listcreate();
        listinsert(newlist,stud);
        hash.array[i]=newlist;
        invertedinserthelp(inv,newlist->next);
        printf("- Student |%d| inserted\n",id);
    }
    else
    {
        ptr=ptr->next;      //skip dummy node
        flag=true;
        while(ptr!=NULL)
        {
            if(ptr->item->studentID==id)
            {
                printf("- Student |%d| exists\n",id);
                flag=false;
                break;
            }
            ptr=ptr->next;
        }
        if(flag == true)             //if we did not find the student in this particular list in the hash table , then insert him into this list
        {
            stud=insertstudent(id,y,z,lname,fname,degree);
            listinsert(newlist,stud);
            hash.array[i]=newlist;
            newlist=listcreate();
            listinsert(newlist,stud);
            invertedinserthelp(inv,newlist->next);
            printf("- Student |%d| inserted\n",id);
        }
    }
}


void look(HThash hash,int id)
{
    int i=hashcode(id,hash);
    listptr list=hash.array[i];
    if(list == NULL)
        printf("- Student |%d| does not exist\n",id);
    else
    {
        list=list->next;      //skip dummy node
        while(list!=NULL)
        {
            if(list->item->studentID == id)
            {
                printf("- |%d| |%s| |%s| |%d| |%d| |%.2f|\n",list->item->studentID, list->item->firstname , list->item->lastname,list->item->zip,list->item->year,list->item->gpa);   
                return;
            }
        }
        printf("- Student |%d| does not exist\n",id);
    }
}


void deletelistnode(listptr prevnode)     //we need the previous node in order to delete the the current node.
{
    listptr currnode=prevnode->next;
    if(currnode!=NULL)         
    {
        prevnode->next=currnode->next;
        free(currnode);
    }
        
    
}

void delete(HThash hash,invertedptr inv,int id)
{
    int i=hashcode(id,hash),y;
    listptr list=hash.array[i],previousnode;
    invertedptr currinv;

    bool flag=false;
    if(list!=NULL)
    {
        previousnode=list;
        list=list->next;
        while(list!=NULL)
        {
            if(list->item->studentID == id)
            {
                flag=true;                     // the student we are looking for exists!!
                y=list->item->year;
                deletelistnode(previousnode);
                break;
            }
            previousnode=list;    
            list=list->next;
        }
    }    
    if(!flag)                                  //if student doesnt exist, then print that
    {
        printf("- Student |%d| does not exists\n",id);
        return;
    }
    else
    {
        currinv=inv->next;           // initialize and skip the dummy node
        while(currinv!=NULL)
        {
            if(currinv->year==y)   //if we found the students year  in the inverted index list , then search in this particular node for the student
            {
                list=currinv->stud;
                previousnode=list;
                list=list->next;    //skip dummy node
                while(list!=NULL)
                {
                    if(list->item->studentID == id)
                    {
                        free(list->item);
                        deletelistnode(previousnode);
                        break;
                    }
                    previousnode=list;
                    list=list->next;
                }
            }
            currinv=currinv->next;
        }
    }
    printf("- Student |%d| deleted\n",id);
}


void number(invertedptr inv,int y)
{
    invertedptr currinv=inv->next;   //initialize and skipp dummy node
    listptr list;
    int counter=0,studyear;
    bool flag=true;

    while(currinv!=NULL)
    {
        studyear=currinv->year;
        if(studyear==y)
        {
            printf("fsd\n");
            flag=false;
            list=currinv->stud;    //initialize the pointer 
            list=list->next;       //skip the dummy node
            while(list!=NULL)
            {
                counter++;
                list=list->next;
            }
            break;
        }
        currinv=currinv->next;
    }
    if(flag)
        printf("- No students enrolled in year:%d\n",y);
    else
        printf("- %d students in year:%d\n",counter,y);

}


void top(invertedptr inv,int number,int y)
{
    invertedptr currinv;
    listptr studptr;
    currinv=inv->next;
    int i;
    float gpaDuplicate;

    while(currinv != NULL)
    {
        if(currinv->year == y)
        {
            studptr=currinv->stud->next;
            i=1;
            gpaDuplicate=studptr->item->gpa;
            while(studptr!=NULL)
            {
                if(gpaDuplicate!=studptr->item->gpa)      //if there is no duplicate 
                {
                    i++;                                 
                    gpaDuplicate=studptr->item->gpa;      //initialize gpaDuplicate.
                }
                if(i==number)      //if we found the rank of the student we wanted, then print him
                {
                    printf("-Student :|%d| |%s| |%s| |%d| |%d| |%.2f|\n",studptr->item->studentID, studptr->item->firstname , studptr->item->lastname,studptr->item->zip,studptr->item->year,studptr->item->gpa);
                }
                if(i>number)      // if we printed already the top number-students , then stop 
                    break;
                studptr=studptr->next;      //continue for the next student
            }
            return;    // we found the students , so stop the function.
        }
        currinv=currinv->next;
    }
    printf("-No students enrolled in year:%d\n",y);
}


void average(invertedptr inv,int y)
{
    invertedptr currinv;
    listptr studptr;
    currinv=inv->next;
    int i=0;
    float x=0.0;

    while(currinv != NULL)
    {
        if(currinv->year == y)
        {
            studptr=currinv->stud->next;
            while(studptr!=NULL)
            {
                i++;
                x=x+studptr->item->gpa;
                studptr=studptr->next;
            }
            x=x/i;
            printf("- Average gpa:%.2f\n",x);
            return;
        }
        currinv=currinv->next;
    }
    printf("-No students enrolled in year:%d\n",y);
}

void minimum(invertedptr inv,int y)
{
    invertedptr currinv;
    listptr studptr;
    currinv=inv->next;
    float mingpa;
    int minid;

    while(currinv != NULL)
    {
        if(currinv->year == y)
        {
            studptr=currinv->stud->next;
            while(studptr->next!=NULL)
            {
                studptr=studptr->next;
            }
            printf("-Student :|%d| |%s| |%s| |%d| |%d| |%.2f|\n",studptr->item->studentID, studptr->item->firstname , studptr->item->lastname,studptr->item->zip,studptr->item->year,studptr->item->gpa);
            mingpa=studptr->item->gpa;
            minid=studptr->item->studentID;

            //we search again for other students with the minimun gpa (search for duplicates)
            studptr=currinv->stud->next;
            while(studptr->next!=NULL)
            {
                if(mingpa==studptr->item->gpa && minid!=studptr->item->studentID)
                    printf("-Student :|%d| |%s| |%s| |%d| |%d| |%.2f|\n",studptr->item->studentID, studptr->item->firstname , studptr->item->lastname,studptr->item->zip,studptr->item->year,studptr->item->gpa);
                studptr=studptr->next;
            }
            return;
        }
        currinv=currinv->next;
    }
    printf("-No students enrolled in year:%d\n",y);
}
    

void count(invertedptr inv)
{
    invertedptr currinv=inv->next; //skip dummy node
    listptr studptr;
    int counter=0;

    if(inv == NULL || inv->next==NULL)                  //if the inverted index list is empty or it has only dummy node
    {
        printf("- No students are enrolled\n");
        return;
    }
        
    while(currinv != NULL)
    {

        studptr=currinv->stud->next;
        while(studptr!=NULL)
        {
            counter++;
            studptr=studptr->next;
        }
        printf("- year:%d  number of students:%d\n",currinv->year,counter);
        counter=0;
        currinv=currinv->next;
    }
}



void postalcode(invertedptr inv,int rank)
{
    invertedptr currinv=inv->next;      //initialize and skip dummy node
    listptr studptr;
    listzipptr zipptr,tempzip,newnodezip;
    int i,counter;

    if(inv == NULL || inv->next==NULL)       //if the inverted index list is empty or it has only dummy node
    {
        printf("- No students are enrolled\n");
        return;
    } 

    zipptr=malloc(sizeof(listzip));  //create dummy node
    zipptr->next=NULL;                    //and initialize next  

    while(currinv != NULL)
    {

        studptr=currinv->stud->next;    //initialize the student pointer (skip dummy node)
        while(studptr!=NULL)
        {
            if(!searchlistzip(zipptr,studptr->item->zip))    //if we did not find the postal code of this student in the "postal code list" , 
            {
                newnodezip=malloc(sizeof(listzip));            //then insert a node in this list , with this postal code
                newnodezip->zip=studptr->item->zip;
                newnodezip->count=1;
                if(zipptr->next == NULL)   //if only dummy node exists
                {
                    newnodezip->next = NULL;
                    zipptr->next = newnodezip; 
                }
                else
                {
                    newnodezip->next=zipptr->next;
                    zipptr->next=newnodezip;
                }
            }
            studptr=studptr->next;
        }
        currinv=currinv->next;
    }
    zipptr=sortziplist(zipptr);//sort the zip list.

    tempzip=zipptr->next;
    i=1;
    counter=tempzip->count;
    while(tempzip!=NULL)
    {
        if(counter!=tempzip->count)
        {
            i++;
            counter=tempzip->count;
        }
        if(i==rank)
            printf("- postal code:%d is the %d most popular %d\n",tempzip->zip,rank,tempzip->count);
        if(i>rank)
            break;
        tempzip=tempzip->next;    
    }
    while(zipptr!=NULL)              ////free the memory that was allocated for the zip list
    {
        tempzip=zipptr->next;
        free(zipptr);
        zipptr=tempzip;
    }

}

listzipptr sortziplist(listzipptr list)     // we delete the list and create a new sorted list.
{
    listzipptr temp;
    listzipptr newlist,current,newnode;

    newlist=malloc(sizeof(listzip)); //create a new  list , that will be sorted. The first node will be the dummy node
    newlist->next=NULL;          


        //we do this to free the dummy node
    temp=list->next;
    free(list);
    list=temp;


    while(list!=NULL)
    {
        temp=list->next;
        

        newnode=malloc(sizeof(listzip));   // create a new node in the new sorted list

        //insert into that node , the zip and the number of students with that zip code , of the old (unsorted) list
        newnode->zip=list->zip;             
        newnode->count=list->count;

        free(list);     //we dont need the old node now
        list=temp;       //go to next node of the old list


        //insert the new node sorted.
        current=newlist;       
        while(current!=NULL)
        {
            if(current->next==NULL)
            {
                newnode->next = NULL;
                current->next = newnode; 
                break;
            }
            else if(current->next->count <= newnode->count)
            {
                newnode->next=current->next;
                current->next=newnode;
                break;
            }
            current=current->next;    
        }
    }
    return newlist;
}
int searchlistzip(listzipptr list,int z)   
{
    listzipptr listzip=list;
    if(listzip == NULL)                 //if the list with the postal codes  is empty
        return 0;
    else if(listzip->next == NULL)       //or if it has only dummy node
        return 0;
    else 
    {
        listzip=listzip->next; //skip dummy
        while(listzip!=NULL)
        {
            if(listzip->zip == z)        //if we find the zipcode already inside the list , then
            {
                listzip->count++;            //         just add 1 to the counter of this postal code
                return 1;
            }
            listzip=listzip->next;
        }
        return 0;
    }
}