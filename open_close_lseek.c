#ifndef OPEN_CLOSE_LSEEK_C
#define OPEN_CLOSE_LSEEK_C

#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "levelTwoHelper.c"

int openFile(char * pathname, char * type)
{
    int mode, ino, dev;
    MINODE * mip;
    int newEntry;

    if(strcmp(pathname, "") == 0 || strcmp(type, "") == 0){
        printf("Error on inputs\n");
        return;
    }
    
    if(strcmp(type, "r") == 0){
        mode = 0;
    }
    else if(strcmp(type, "w") == 0){
        mode = 1;
    }
    else if(strcmp(type, "rw") == 0){
        mode = 2;
    }
    else if(strcmp(type, "append") == 0){
        mode = 3;
    }
    else{
        printf("invalid type\n Only 'r','w','rw','append' allowed\n");
        return;
    }

    if(pathname[0] == '/')
        ino = getino(&(root->dev), pathname);
    else
        ino = getino(&(running->cwd->dev), pathname);

    printf("Inode of this file = %d\n", ino);
    if(pathname[0] == '/')
        mip = iget(root->dev, ino);
    else
        mip = iget(running->cwd->dev, ino);
    printf("MINODE loaded into memory\n");

    if(!S_ISREG(mip->INODE.i_mode)){
        printf("The requested file is not regular\n");
        return 0;
    }
    //PERMISSIONS CHECKING

    OFT *newFD = malloc(sizeof(OFT));
    newEntry = fdAlloc();
    newFD->mode = mode;
    newFD->refCount = 1;
    newFD->mptr = mip;

    switch(mode){
        case 0://reading
            newFD->offset = 0;
            break;
        case 1:
            truncate(newFD->mptr);
            newFD->offset = 0;
            break;
        case 2:
            newFD->offset = 0;
            break;
        case 3:
            newFD->offset = mip->INODE.i_size;
            break;
        default:
            printf("Invalid mode\n");
            return -1;
    }
    switch(mode){
        case 0:
            newFD->mptr->INODE.i_atime = time(0L);
            break;
        case 1:
            newFD->mptr->INODE.i_atime = time(0L);
            newFD->mptr->INODE.i_mtime = time(0L);
            break;
        case 2:
            newFD->mptr->INODE.i_atime = time(0L);
            newFD->mptr->INODE.i_mtime = time(0L);
            break;
        case 3:
            newFD->mptr->INODE.i_atime = time(0L);
            newFD->mptr->INODE.i_mtime = time(0L);
            break;
        default:
            printf("Invalid mode\n");
            return -1;
    }

    newFD->mptr->dirty = 1;
    running->fd[newEntry] = newFD;
    //IPUT mip????
    return newEntry;
}

int closeFile(char * strFD)
{
    OFT *oftp;
    MINODE *mip;
    int fd;

    fd = atoi(strFD);

    if(fd > NFD || fd < 0){
        printf("fd #[%d] not in range\n", fd);
        return 0;
    }
    if(running->fd[fd] == NULL){
        printf("fd #[%d] has not been allocated\n", fd);
        return 0;
    }
    oftp = running->fd[fd];
    running->fd[fd] = NULL;
    oftp->refCount--;

    if(oftp->refCount > 0){         //still being referenced
        printf("This is still being referenced\n");
        return 0;
    }

    printf("Wrote back fd #[%d]\n", fd);
    //otherwise we need to write it back
    mip = oftp->mptr;
    iput(mip);                      //puts it back

    return 0;
}

int mylseek(char * strFD, char * pos)
{
    int fd, position;
    int originalPosition;

    fd = atoi(strFD);
    position = atoi(pos);

    if(fd > NFD || fd < 0){
        printf("fd #[%d] not in range\n", fd);
        return 0;
    }
    if(running->fd[fd] == NULL){
        printf("fd #[%d] has not been allocated\n", fd);
        return 0;
    }

    if(position < 0 || position > running->fd[fd]->mptr->INODE.i_size)
    {
        printf("Index [%d] not in range\n", position);
        return 0;
    }
    originalPosition = running->fd[fd]->offset;
    running->fd[fd]->offset = position;
    printf("FD #[%d] offset changed from %d to %d\n", fd, originalPosition, position);
    return originalPosition;

  //From fd, find the OFT entry. 
  //change OFT entry's offset to position but make sure NOT to over run either end
  //of the file.
  //return originalPosition
}

#endif