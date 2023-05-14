
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
    int friendshipFucnsCounter;

    ComparisonFunction comparisonFunction;

    int friendshipThreshold;
    int rivalryThreshold;
    int listSize;

};

IsraeliQueueError Aux_IsraeliQueueEnqueue(IsraeliQueue currentQ,int enemy_count,int friends_count, void* item) ;
List rightPlace(IsraeliQueue currentQ, void * item);
int average(void* item1,void* item2,IsraeliQueue currentQueue);
int Isfriend(void* item1,void* item2,IsraeliQueue currentQueue);

/** Finished **/
IsraeliQueue IsraeliQueueCreate(FriendshipFunction* friendshipFunctions, ComparisonFunction comparisonFunction, int friendshipThreshold, int rivalryThreshold)
{
    if(friendshipFunctions == NULL || comparisonFunction == NULL)
    {
        return NULL;
    }
    IsraeliQueue currentQ = (IsraeliQueue)malloc(sizeof(*currentQ));
    if (currentQ == NULL)
    {
        return NULL;  // Failed to allocate memory
    }

    //count the number of friendship funcs to create list for it
    currentQ->friendshipFucnsCounter = 0;
    for(int i = 0 ; friendshipFunctions[i] != NULL ; ++i)
    {
        ++(currentQ->friendshipFucnsCounter);
    }
    currentQ->friendshipFunctions = malloc((sizeof (FriendshipFunction)) * (currentQ->friendshipFucnsCounter + 1));
    if(currentQ->friendshipFunctions == NULL)
    {
        return NULL;
    }

    //set the friendship functions to the list
    for(int i = 0 ; i <= currentQ->friendshipFucnsCounter ; ++i)
    {
        if(i != currentQ->friendshipFucnsCounter)
        {
            currentQ->friendshipFunctions[i] = friendshipFunctions[i];
        }
        else
        {
            currentQ->friendshipFunctions[i] = NULL;
        }
    }

    // Set the parameters
    currentQ->listSize = 0;
    currentQ->list = NULL;
    currentQ->comparisonFunction = comparisonFunction;
    currentQ->friendshipThreshold = friendshipThreshold;
    currentQ->rivalryThreshold = rivalryThreshold;
    return currentQ;
}

/** Finished **/
int IsraeliQueueSize(IsraeliQueue currentQ)
{
    if(currentQ->list == NULL)
    {
        return 0;
    }

    //run on the list and count
    List temp = currentQ->list;
    int count=0;
    while(temp)
    {
        ++count;
        temp = temp->next;
    }
    return count;
}

/** Finished **/
bool IsraeliQueueContains(IsraeliQueue q, void* item)
{
    if(q == NULL || item == NULL)
    {
        return false;
    }
    // read all the list and search for comparison match
    List current = q->list;
    while (current)
    {
        if (q->comparisonFunction(current->item , item) == 0)
        {
            return true; // Item found in list
        }
        current = current->next;
    }
    // Item not found in list
    return false;
}

/** Finished **/
void IsraeliQueueDestroy(IsraeliQueue currentQ)
{
    if(currentQ == NULL)
    {
        return;
    }
    //iterate over the list and delete each node in it
    while(currentQ->list)
    {
        List toDelete = currentQ->list;
        currentQ->list = currentQ->list->next;
        free(toDelete);
    }
    free(currentQ->friendshipFunctions);
    free(currentQ);
}

/** Finished **/
void* IsraeliQueueDequeue(IsraeliQueue q)
{
    // List is empty or NULL parameter
    if (q == NULL || q->list == NULL)
    {
        return NULL;
    }
    void* toReturnItem = q->list->item;
    List tmpHead = q->list;
    if(IsraeliQueueSize(q) == 1)
    {
        q->list = NULL;
    }
    else
    {
        q->list = q->list->next;
    }
    free(tmpHead);
    --(q->listSize);
    return toReturnItem;
}

IsraeliQueueError IsraeliQueueAddFriendshipMeasure(IsraeliQueue q, FriendshipFunction friendships_function)
{
    int len = 0;
    while (q->friendshipFunctions[len] != NULL) {
        len++;
    }
    q->friendshipFunctions = realloc(*(q->friendshipFunctions), sizeof(FriendshipFunction*) * (len + 1));
    if (q->friendshipFunctions == NULL) {
        return ISRAELIQUEUE_ALLOC_FAILED;
    }
    q->friendshipFunctions[len] = friendships_function;
    q->friendshipFunctions[len+1] = NULL;
    return ISRAELIQUEUE_SUCCESS;
}

/** Finished **/
IsraeliQueueError IsraeliQueueUpdateRivalryThreshold(IsraeliQueue q, int n_thresh)
{
    if(q == NULL)
    {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    q->rivalryThreshold = n_thresh ;
    return ISRAELIQUEUE_SUCCESS ;
}

/** Finished **/
IsraeliQueueError IsraeliQueueUpdateFriendshipThreshold(IsraeliQueue currentQ, int updated)
{
    if(currentQ == NULL)
    {
        return ISRAELIQUEUE_BAD_PARAM;
    }
    currentQ->friendshipThreshold = updated;
    return  ISRAELIQUEUE_SUCCESS;
}





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
