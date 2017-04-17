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
            continue;
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

dup(char * strFD)
{
    int fd;
    int newIndex;
    fd = atoi(strFD);

    if(fd > NFD || fd < 0){
        printf("fd #[%d] not in range\n", fd);
        return 0;
    }
    if(running->fd[fd] == NULL){
        printf("fd #[%d] has not been allocated\n", fd);
        return 0;
    }

    newIndex = fdAlloc();
    running->fd[newIndex] = running->fd[fd];
    running->fd[fd]->refCount++;
  /*verify fd is an opened descriptor;
  duplicates (copy) fd[fd] into FIRST empty fd[ ] slot;
  increment OFT's refCount by 1;*/
}

dup2(char * strFD, char * strGD)
{
    int fd, gd;
    fd = atoi(strFD);
    gd = atoi(strGD);

    if(fd > NFD || fd < 0){
        printf("fd #[%d] not in range\n", fd);
        return 0;
    }
    if(running->fd[fd] == NULL){
        printf("fd #[%d] has not been allocated\n", fd);
        return 0;
    }
    if(gd > NFD || gd < 0){
        printf("gd #[%d] not in range\n", gd);
        return 0;
    }
    if(running->fd[gd] == NULL){
        printf("gd #[%d] has not been allocated\n", gd);
        return 0;
    }

    closeFile(strGD);
    running->fd[gd] = running->fd[fd];
  /*CLOSE gd fisrt if it's already opened;
  duplicates fd[fd] into fd[gd]; */
}




#endif