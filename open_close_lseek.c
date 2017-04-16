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

    newEntry = fdAlloc();
    running->fd[newEntry]->mode = mode;
    running->fd[newEntry]->refCount = 1;
    running->fd[newEntry]->mptr = mip;

    switch(mode){
        case 0://reading
            running->fd[newEntry]->offset = 0;
            break;
        case 1:
            truncate(running->fd[newEntry]->mptr);
            running->fd[newEntry]->offset = 0;
            break;
        case 2:
            running->fd[newEntry]->offset = 0;
            break;
        case 3:
            running->fd[newEntry]->offset = mip->INODE.i_size;
            break;
        default:
            printf("Invalid mode\n");
            return -1;
    }
    switch(mode){
        case 0:
            running->fd[newEntry]->mptr->INODE.i_atime = time(0L);
            break;
        case 1:
            running->fd[newEntry]->mptr->INODE.i_atime = time(0L);
            running->fd[newEntry]->mptr->INODE.i_mtime = time(0L);
            break;
        case 2:
            running->fd[newEntry]->mptr->INODE.i_atime = time(0L);
            running->fd[newEntry]->mptr->INODE.i_mtime = time(0L);
            break;
        case 3:
            running->fd[newEntry]->mptr->INODE.i_atime = time(0L);
            running->fd[newEntry]->mptr->INODE.i_mtime = time(0L);
            break;
        default:
            printf("Invalid mode\n");
            return -1;
    }

    running->fd[newEntry]->mptr->dirty = 1;
    //IPUT mip????
    return newEntry;
}


#endif