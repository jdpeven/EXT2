#ifndef UTIL_C
#define UTIL_C

#include "global.c"
#include "type.h"

/*
Name: 
Args:  -  -
       -  -
Return: -
Description:
SampleRun:
*/


/*
Name: getino
Args: dev - int * -
      pathname - char * -
Return:
Description:
SampleRun:
*/
int getino(int *dev, char *pathname)
{
    int i, ino, blk, disp;
    char buf[BLKSIZE];
    INODE *ip;
    MINODE *mip;

    printf("getino: pathname=%s\n", pathname);
    if (strcmp(pathname, "/")==0)
        return 2;

    if (pathname[0]=='/')
        mip = iget(*dev, 2);
    else
        mip = iget(running->cwd->dev, running->cwd->ino);

    strcpy(buf, pathname);
    tokenize(buf); // n = number of token strings

    for (i=0; i < n; i++){
        printf("===========================================\n");
        printf("getino: i=%d name[%d]=%s\n", i, i, kcwname[i]);
    
        ino = search(mip, name[i]);

        if (ino==0){
            iput(mip);
            printf("name %s does not exist\n", name[i]);
            return 0;
        }
        iput(mip);
        mip = iget(*dev, ino);
    }
    return ino;
    
}

/*
Name: 
Args:  -  -
       -  -
Return: -
Description:
SampleRun:
*/
int iput(MINODE *mip)  // dispose of a minode[] pointed by mip
{
    return 0;
}

/*
Name: iget
Args: dev - int - 
      get - ino - 
Return: MINODE *
Description:
SampleRun:
*/
MINODE * iget(dev, ino)
{
    /*search minode[] for an item with same (dev, ino)
    if(found){
       mip->refCount++;
       return mip
    }
    search minode[] array for an item pointed by mip whose refCount == 0;
        This means that you can rewrite over ones that aren't in use anymore
    mip->refCount = 1; //makr it in Useful
    Assign it to (dev, ino);
    initialize dirty = 0; mounted = 0; mountPtr = 0;
    Use mailman to load it into memory, then copy it to mip
    return mip;

    */
    int i, blk, disp;
    char buf[BLKSIZE];
    MINODE *mip;
    INODE *ip;
    for (i=0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->dev == dev && mip->ino == ino)
        {
            mip->refCount++;
            printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
            return mip;
        }
    }

    for (i=0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount == 0)
        {
            printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
            mip->refCount = 1;
            mip->dev = dev; mip->ino = ino;  // assing to (dev, ino)
            mip->dirty = mip->mounted = mip->mptr = 0;
            // get INODE of ino into buf[ ]      
            blk  = (ino-1)/8 + inode_start;  // iblock = Inodes start block #
            disp = (ino-1) % 8;
            //printf("iget: ino=%d blk=%d disp=%d\n", ino, blk, disp);
            get_block(dev, blk, buf);
            ip = (INODE *)buf + disp;
            // copy INODE to mp->INODE
            mip->INODE = *ip;
            return mip;
        }
    }   
    printf("PANIC: no more free minodes\n");
    return 0;
}



/*
Name: decompose
Args: input - char* - for input to be decomposed
      output - char** - string array to be returned
      count - int * - number of elements in output
      delimeter - char * - what you want to split on
Return = void (all done with pointers)
Description: Will split a string based on a delimeter
        Useful for decomposing a path /a/b/c -> [a, b, c]
Sample run:
    path = "/a/b/c"
    decompose(path, pathArr, &count, "/");
    pathArr = ["a", "b", "c"]
    count = 3
*/
void decompose(char * input, char ** output, int * count, char * delimeter)
{
    char * token;
    //char * output[80];
    int i;
    token = strtok(input, delimeter);
    i = 0;
    while(token != NULL)
    {
        output[i] = token;
        token = strtok(NULL, delimeter);
        i++;
    }
    *count = i;
    output[i] = NULL;
}

/*
Name: search
Args: mip - MINODE * -
      name - char * -
Return: int -
Description:
SampleRun: 
*/
int search(MINODE *mip, char *name)
{
    // YOUR search function !!!
}

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

#endif