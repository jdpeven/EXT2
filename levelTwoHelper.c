#ifndef lEVELTWOHELPER_C
#define lEVELTWOHELPER_C

#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"

//will print all open file table instances
void pfd()
{
    int i = 0;
    printf("=============  pid = %d ==========\n", running->pid);
    printf("fd  mode  count  offset  [dev, ino]\n");
    for(i = 0; i < NFD; i++)
    {
        if(running->fd[i] == NULL)
            break;
        printf("%d    %d     %d       %d       [%d, %d]\n", i, running->fd[i]->mode, running->fd[i]->refCount, 
                running->fd[i]->offset, running->fd[i]->mptr->dev, running->fd[i]->mptr->ino);
    }
}

//Finds the first open spot and returns the index. Maybe not totally necessary
int fdAlloc()
{
    int i;
    for(i = 0; i < NFD; i++)
    {
        if(running->fd[i] == NULL) //unused
        {
            printf("Index #[%d] is free\n", i);
            return i;
        }
    }
    printf("No more free spots open\n");
    return -1;
}



#endif