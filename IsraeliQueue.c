
#include "IsraeliQueue.h"
#include <stdbool.h>
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define FRIEND_QUOTA 5
#define RIVAL_QUOTA 3

typedef struct List {
    void* item;
    struct List* next;
    int friends_counter;
    int rivals_counter;
} *List;

struct IsraeliQueue_t{
    List list ;
    FriendshipFunction* friendshipFunctions;
    ComparisonFunction comparisonFunction;
    int friendshipThreshold;
    int rivalryThreshold;
};

IsraeliQueueError Aux_IsraeliQueueEnqueue(IsraeliQueue currentQ,int enemy_count,int friends_count, void* item) ;
List rightPlace(IsraeliQueue currentQ, void * item);
int average(void* item1,void* item2,IsraeliQueue currentQueue);
int Isfriend(void* item1,void* item2,IsraeliQueue currentQueue);

IsraeliQueue IsraeliQueueCreate(FriendshipFunction* friendshipFunctions, ComparisonFunction comparisonFunction, int friendshipThreshold, int rivalryThreshold)
{
    IsraeliQueue currentQ = (IsraeliQueue)malloc(sizeof(IsraeliQueue));
    if (currentQ == NULL)
    {
        return NULL;  // Failed to allocate memory
    }

    // Set the parameters
    currentQ->list = NULL;
    currentQ->friendshipFunctions = friendshipFunctions;
    currentQ->comparisonFunction = comparisonFunction;
    currentQ->friendshipThreshold = friendshipThreshold;
    currentQ->rivalryThreshold = rivalryThreshold;
    return currentQ;
}

IsraeliQueueError IsraeliQueueAddFriendshipMeasure(IsraeliQueue q, FriendshipFunction friendships_function)
{
   int len = 0;
    while (q->friendshipFunctions[len] != NULL) {
        len++;
    }
    q->friendshipFunctions = realloc(*q->friendshipFunctions, sizeof(FriendshipFunction*) * (len + 1));
    if (q->friendshipFunctions == NULL) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    q->friendshipFunctions[len] = friendships_function;
    q->friendshipFunctions[len+1] = NULL;
    return ISRAELIQUEUE_SUCCESS;
}

IsraeliQueueError IsraeliQueueUpdateRivalryThreshold(IsraeliQueue q, int n_thresh)
{
    q->rivalryThreshold = n_thresh ;
    return ISRAELIQUEUE_SUCCESS ;
}

void* IsraeliQueueDequeue(IsraeliQueue q)
{
    if (!q||q->list == NULL) {  // List is empty
        return NULL;
    }
    void* tmpitem = q->list->item;
    void* item_copy = malloc(sizeof(void*));  // Allocate memory for the item copy
    q->list->item = NULL;

    memcpy(item_copy, tmpitem, sizeof(void*));  // Copy the item pointer

    if(IsraeliQueueSize(q)==1){
        List tmphead = q->list;
        q->list=NULL;
        free(tmphead);
}
    else {
        List tmphead = q->list;
        q->list = q->list->next;
        free(tmphead);  // Free the old head of the list
    }
    return item_copy;
}

bool IsraeliQueueContains(IsraeliQueue q, void* item)
{
    List current = q->list;
    while (current != NULL) {

        if ( q->comparisonFunction(current->item,item)) {
            return true; // Item found in list

            current = current->next;
        }
        current=current->next;
    }
    return false; // Item not found in list
}

////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////


// Clone an existing IsraeliQueue
IsraeliQueue IsraeliQueueClone(IsraeliQueue q)
{
    IsraeliQueue newq = IsraeliQueueCreate(q->friendshipFunctions, q->comparisonFunction, q->friendshipThreshold, q->rivalryThreshold);
    if (newq == NULL)
    {
        return NULL; // Failed to allocate memory
    }

    // Copy each item in the queue
    List tmp = q->list;
    while (tmp != NULL)
    {
        List copyListTmp = malloc(sizeof(*newq->list));
        if (copyListTmp == NULL)
        {
            IsraeliQueueDestroy(newq);
            return NULL; // Handle allocation failure
        }
        copyListTmp->item = malloc(sizeof(*(tmp->item)));
        if (copyListTmp->item == NULL)
        {
            free(copyListTmp);
            IsraeliQueueDestroy(newq);
            return NULL; // Handle allocation failure
        }
        memcpy(copyListTmp->item, tmp->item, sizeof(int)); // Copy the item pointer
        copyListTmp->friends_counter = tmp->friends_counter;
        copyListTmp->rivals_counter = tmp->rivals_counter;
        copyListTmp->next = NULL;

        // Add the new item to the queue
        if (newq->list == NULL)
        {
            newq->list = copyListTmp;
        }
        else
        {
            List last = newq->list;
            while (last->next != NULL)
            {
                last = last->next;
            }
            last->next = copyListTmp;
        }

        tmp = tmp->next;
    }

    return newq;
}
/////////////////////////////////////////////
/*void printList( IsraeliQueue l)
{
     List list=l->list;
    while (list!=NULL)
    {
        printf("%d",*(int*)(list->item));
        list=list->next;
    }

}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////

int Isfriend(void* item1,void* item2,IsraeliQueue currentQueue)
{
    int i=0;
    while(currentQueue->friendshipFunctions[i])
    {
        if(currentQueue->friendshipFunctions[i](item1,item2)>currentQueue->friendshipThreshold)
     {
         return 1;
     }
        i++;
    }
    return 0;
}

int average(void* item1,void* item2,IsraeliQueue currentQueue)
{
    int i=0,sum=0;
    while(currentQueue->friendshipFunctions[i])
    {
        sum+=currentQueue->friendshipFunctions[i](item1,item2);
        i++;
    }
    if(sum/i<currentQueue->rivalryThreshold)
    {
        return  1;
    }
    return  0;
}

List rightPlace(IsraeliQueue currentQ, void * item)
{
    List FriendPointer=currentQ->list;
    List EnemyPointer=currentQ->list->next;
    while(1)
    {
        while (!(Isfriend(FriendPointer->item,item,currentQ))||(FriendPointer->friends_counter>=FRIEND_QUOTA))
        {
            if(!FriendPointer->next)
            {
                return FriendPointer;
            }
            EnemyPointer=EnemyPointer->next;
            FriendPointer=FriendPointer->next;

        }

        if(!FriendPointer->next)
        {
            if(FriendPointer->friends_counter<FRIEND_QUOTA)
            {
                FriendPointer->friends_counter++;
            }
            return FriendPointer;
        }

        while(((Isfriend(EnemyPointer->item,item,currentQ)) || !(average(EnemyPointer->item,item,currentQ)))||(EnemyPointer->rivals_counter>=RIVAL_QUOTA))
        {

            if(!EnemyPointer->next)
            {
                if(FriendPointer->friends_counter<FRIEND_QUOTA)
                {
                    FriendPointer->friends_counter++;
                    return FriendPointer;
                }
                else{
                    break;
                }
            }

            EnemyPointer=EnemyPointer->next;
        }

        if(EnemyPointer->rivals_counter<RIVAL_QUOTA)
        {
            EnemyPointer->rivals_counter++;
            FriendPointer = EnemyPointer;
        }
    }
}

IsraeliQueueError Aux_IsraeliQueueEnqueue(IsraeliQueue currentQ,int enemy_count,int friends_count, void* item) {
    List temp=malloc(sizeof(*temp));
    if(!temp)
    {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    if (!currentQ->list) {

        temp->item = item;
        temp->rivals_counter = enemy_count;
        temp->friends_counter = friends_count;
        temp ->next = NULL;
        currentQ->list=temp;
        return ISRAELIQUEUE_SUCCESS;

    }

    List rightplace = rightPlace(currentQ, item);
    if(rightplace->next!=NULL){
        temp->next = rightplace->next;
    }
    else{
        temp->next = NULL;
    }
    temp->item = item;
    temp->friends_counter = friends_count;
    temp->rivals_counter = enemy_count;
    rightplace->next = temp;
    return ISRAELIQUEUE_SUCCESS;

}
IsraeliQueueError IsraeliQueueEnqueue(IsraeliQueue currentQ, void * item) {
   return Aux_IsraeliQueueEnqueue(currentQ,0,0,item);

}

IsraeliQueueError IsraeliQueueUpdateFriendshipThreshold(IsraeliQueue currentQ, int updated)
{
    currentQ->friendshipThreshold=updated;
    return  ISRAELIQUEUE_SUCCESS;

}

int IsraeliQueueSize(IsraeliQueue currentQ)
{
    List temp=currentQ->list;
    int count=0;
    if(temp==NULL)
    {
        return 0;
    }
    while(temp)
    {
        count++;
        temp=temp->next;
    }
    return count;
}

IsraeliQueue IsraeliQueueMerge(IsraeliQueue *qarr, ComparisonFunction compare_function)
{
    int i=0,friendsum=0,enemysum=1,m=0,totalItems=0;
   IsraeliQueue newQueue =IsraeliQueueCreate(qarr[0]->friendshipFunctions,compare_function,0,0);
   if(newQueue==NULL)
   {
       return NULL;
   }
    while (qarr[i])
    {
        totalItems+=IsraeliQueueSize(qarr[i]);
        friendsum+=qarr[i]->friendshipThreshold;
        enemysum=enemysum*qarr[i]->rivalryThreshold;
        while(qarr[i]->friendshipFunctions[m]&&i!=0)
        {
           if( IsraeliQueueAddFriendshipMeasure(newQueue,qarr[i]->friendshipFunctions[m])!=ISRAELIQUEUE_SUCCESS){//merge the friendshipfunctions
               return  NULL;
           }
            m++;
        }

        m=0;
        i++;
    }

    IsraeliQueueUpdateFriendshipThreshold(newQueue,friendsum/i);
    IsraeliQueueUpdateRivalryThreshold(newQueue,ceil(pow(abs(enemysum),1/i)));

    while(totalItems)
    {
        int index=0;
        while(qarr[index])
        {
            if(!IsraeliQueueSize(qarr[index])==0)
            {
                IsraeliQueueEnqueue(newQueue,IsraeliQueueDequeue(qarr[index]));
                totalItems--;
            }

            index++;
        }
}

    return newQueue;
}

void IsraeliQueueDestroy(IsraeliQueue currentQ){

    while(currentQ->list){
        List toDelete=currentQ->list;
        currentQ->list=currentQ->list->next;
        free(toDelete);
    }
        /*int i = 0;
       while (currentQ->friendshipFunctions[i] != NULL) {///////////with this part I get a seqmrntation fault(remember to check)
            free(currentQ->friendshipFunctions[i]);
            i++;}

        free(currentQ->friendshipFunctions);
      free(currentQ);*/
  }

List returnPrev_Node(IsraeliQueue currentQ, List check)
{
    List l=currentQ->list;

    while(l->next!=NULL){
    if((l->next->item==check->item)  && l->next->friends_counter==check->friends_counter && l->next->rivals_counter==check->rivals_counter )
    {
        return l;
    }
    l=l->next;
    }

    return NULL;
}

List reversedList(IsraeliQueue currentQ)
{ List saveList=currentQ->list;
    List temp= malloc(sizeof (temp));

    temp->item=currentQ->list->item;
    temp->next=NULL;
    temp->friends_counter=currentQ->list->friends_counter;
    temp->rivals_counter=currentQ->list->rivals_counter;
    saveList=saveList->next;
    while(saveList!=NULL)
    {
        List temp22= malloc(sizeof (temp));
        temp22->item=saveList->item;
        temp22->friends_counter=saveList->friends_counter;
        temp22->rivals_counter=saveList->rivals_counter;
        temp22->next=temp;
        temp=temp22;
        saveList=saveList->next;
    }
    return temp;
}

IsraeliQueueError IsraeliQueueImprovePositions(IsraeliQueue currentQ){
    IsraeliQueueError error;
    List temp= reversedList(currentQ);
    List Prev_wanted;
    List wanted;
    List saveList=currentQ->list;
    while(temp->next!=NULL)
    {
        Prev_wanted= returnPrev_Node(currentQ,temp);
        if(!Prev_wanted){
               while(saveList->next->next!=NULL)
               {
                   saveList=saveList->next;
               }
               wanted=saveList->next;
               Prev_wanted=saveList;
               Prev_wanted->next=NULL;

           }else {
               wanted = Prev_wanted->next;
               if (Prev_wanted->next != NULL) {
                   Prev_wanted->next = wanted->next;
               } else {
                   Prev_wanted->next = NULL;
               }
           }
        void* item= malloc(sizeof (item));
        if(item==NULL)
           {return ISRAELIQUEUE_ALLOC_FAILED;

           }
        memcpy(item,wanted->item, sizeof(void*));
        if(Aux_IsraeliQueueEnqueue(currentQ,wanted->rivals_counter,wanted->friends_counter,item)!=ISRAELIQUEUE_SUCCESS)
        {
            return error;
        }
        free(wanted);
        temp = temp->next;
    }
    void* item= malloc(sizeof (item));
    memcpy(item,currentQ->list->item, sizeof(void*));
    if(Aux_IsraeliQueueEnqueue(currentQ,currentQ->list->rivals_counter,currentQ->list->friends_counter,item)!=ISRAELIQUEUE_SUCCESS)
    {
        return error;
    }
    IsraeliQueueDequeue(currentQ);

    return ISRAELIQUEUE_SUCCESS;
}
