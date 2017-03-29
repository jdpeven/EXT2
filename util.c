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
    printf("%s decomposed into: [", input);
    for(i = 0; i < *count; i++){
        printf("%s][", output[i]);
    }
    printf("'/0']\n");
}
/*
Name: iput
Args: mip - MINODE * - a pointer to an MINODE
Return: int - Not really used
Description: This function releases and unlocks a minode point- ed by mip. 
            If the process is the last one to use the minode (refCount = 0), 
            the INODE is written back to disk if it is dirty (modified).
SampleRun:
*/

int iput(MINODE *mip)  // dispose of a minode[] pointed by mip
{
    int ino, blk, offset;
    char ibuf[BLKSIZE];
    mip->refCount--;
    if(mip->refCount > 0){           //Still being referenced
        printf("dev=%d ino=%d refCount = %d\n", mip->refCount);
        return;
    }
    if(!mip->dirty){                 //it hasn't been changed
        printf("dev=%d ino=%d has not been changed, no need to rewrite\n");
        return;
    }

    printf("iput: dev=%d ino=%d\n", mip->dev, mip->ino);
    ino = mip->ino;
    blk = inode_start + (ino-1)/8;
    offset = (ino-1)%8;

    get_block(mip->dev, blk, ibuf);
    ip = (INODE *)ibuf + offset;

    *ip = mip->INODE;

    put_block(mip->dev, blk, ibuf);

    /*
    (1). mip->refCount--;
 
    (2). if (mip->refCount > 0) return;
        if (!mip->dirty)       return;
 
    (3).   write INODE back to disk 

    printf("iput: dev=%d ino=%d\n", mip->dev, mip->ino); 

    Use mip->ino to compute 

        blk containing this INODE
        disp of INODE in blk

        get_block(mip->dev, block, buf);

        ip = (INODE *)buf + disp;
        *ip = mip->INODE;

        put_block(mip->dev, block, buf);
    */
    return 0;
}

/*
Name: iget
Args: dev - int - 
      get - ino - 
Return: MINODE * -
Description: This function returns a pointer to the in- memory INODE of (dev, ino). 
            The returned minode is unique, i.e. only one copy of the INODE exists in 
            memory. In addition, the minode is locked for exclusive use until it is 
            either released or unlocked. (KCW, pg 324)
SampleRun:
*/
MINODE * iget(int dev, int ino)
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

    //checking if we already have the inode in our MINODES
    for (i=0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->dev == dev && mip->ino == ino)
        {
            mip->refCount++;
            printf("found [%d %d] as minode[%d] in the minode array\n", dev, ino, i);
            return mip;
        }
    }

    //If not... then we look for the first unuse Inode within MINODES and use that
    for (i=0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount == 0)
        {
            printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
            mip->refCount = 1;
            mip->dev = dev;
            mip->ino = ino;  // assing to (dev, ino)
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
Name: getino
Args: dev - int * - Final device number (mounting purposes)
      pathname - char * - The pathname of the file/directory "a/b/c"
Return: int - The inode number of a pathname
Description: getino() returns the inode number of a pathname. While traversing a 
                pathname the device number may change if the pathname crosses mounting 
                point(s). The parameter dev is used to record the final device number. 
                Thus, getino() essentially returns the (dev, ino) of a pathname. 
                The function uses token() to break up pathname into component strings. 
                Then it calls search() to search for the component strings in successive 
                directory minodes. (KCW, pg 324)
SampleRun:
*/

int getino(int *dev, char *pathname)
{
    int i, ino, blk, disp, n;
    char buff[128];
    char *name[80];
    INODE *ip;
    MINODE *mip;                        //This will be the "current" inode for traversal

    printf("getino: dev = %d pathname=%s\n",*dev, pathname);
    //printf("Dev = %d\n",*dev);
    //printf("pls no crash\n");
    if (strcmp(pathname, "/")==0){
        printf("Searching for root, returning 2\n");
        return 2;
    }

    if (pathname[0]=='/'){               //absolute pathname
        printf("Absolute pathname, mip = root\n");
        mip = iget(*dev, 2);            //Set the mip to the root
    }
    else{
        printf("Relative to CWD, getting its MINODE\n");
        printf("CWD: dev = %d, ino = %d\n", running->cwd->dev, running->cwd->ino);
        mip = iget(running->cwd->dev, running->cwd->ino);       //Gets cwd's MINODE
    }
    
    //buff = 

    strcpy(buff, pathname);  
    //printf("buff = %s\n", buff);                  
    decompose(buff, name, &n, "/");                              //JP changed n->&n
    //printf("pls no crash\n");
    /*for(i = 0; i < n; i++)
    {
        printf("%d: %s", i, name[i]);
    }*/

    for (i=0; i < n; i++){
        printf("===========================================\n");
        printf("getino: i=%d name[%d]=%s\n", i, i, name[i]);
    
        ino = search(mip, name[i]);

        if (ino==0){
            iput(mip);                                          //put it back
            printf("name %s does not exist\n", name[i]);
            return 0;
        }
        iput(mip);                                              //This is the "close parenthesis" of the iget from the check 4 lines above the for loop
        mip = iget(*dev, ino);                                  //This is the open parenthesis for the next loops of the for so the iput will put back this one
    }
    return ino;
}

/*
Name: search
Args: mip - MINODE * - the current MINODE
      name - char * - the name we're looking for in that MINODE
Return: int - found inode value
Description: will search through the given inode for the name, and return the names inode
SampleRun: int ino = search(cwd, "X");
*/
int search(MINODE *mip, char *name)
{
    char* cp;
    int block0;
    char dbuf[BLKSIZE], sbuf[BLKSIZE];
    DIR* dp;
    int i;
    int ino = 0;

    //printf("Searching for %s in dir %s", name, mip->INODE.name);

    for(i = 0; i < 12; i++){            //might be deep in the file
        block0 = mip->INODE.i_block[i];
        get_block(mip->dev, block0, dbuf);
        dp = (DIR*)dbuf;
        cp = dbuf;
        while(cp < &dbuf[BLKSIZE]){
            strncpy(sbuf, dp->name, dp->name_len);                  //similar to strcpy but will stop based on third argument
            sbuf[dp->name_len] = 0;
            printf("%4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, sbuf);
            if(strcmp(name, sbuf) == 0){
                ino = dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        if(ino){
            printf("Found '%s' with Ino [%d]\n", name, ino);
        }
        else
            printf("Did not find '%s'", name);
        return ino;
    }
}

/*
Name: get_block
Args: fd - int - The open file descriptor
      blk - int - The block number that you want
      buf - char[] - What you're reading the block into
Return: int - Maybe just an error code.
Description: Will get the block (blk) and read it into buf.
SampleRun: get_block(dev, 2, buf)
            reads the GD into buf.
*/

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);  
}

/*
Name: put_block
Args: fd - int - The open file descriptor
      blk - int - The block number that you want to write to
      buf - char[] - What you're writing to the block
Return: int - Maybe just an error code.
Description: Will get the block (blk) and write buf to it.
SampleRun: put_block(dev, 2, buf)
            write buf into GD.
*/

int put_block(int fd, int blk, char buf[ ])
{
    lseek(fd, (long)blk*BLKSIZE, 0);
    write(fd, buf, BLKSIZE);
}


/*
Name: findCmd
Args: command - char* - the command we're looking for
Return: int - the index of the command for the function pointer array
Description: Will fix the if() else() or switch statement
SampleRun: cmd = findCmd("ls");
*/

int findCmd(char * command)
{
    int i = 0;
    while(cmd[i]){
        if (strcmp(command, cmd[i]) == 0)
            return i;
        i++;
    }
    return -1;
}

void copyMinodeptr(MINODE * src, MINODE **dest)
{
    memcpy(&(*(*dest)).INODE, &(*src).INODE, sizeof(INODE));
    memcpy(&(*(*dest)).dev, &(*src).dev, sizeof(int));
    memcpy(&(*(*dest)).ino, &(*src).ino, sizeof(int));
    memcpy(&(*(*dest)).refCount, &(*src).refCount, sizeof(int));
    memcpy(&(*(*dest)).dirty, &(*src).dirty, sizeof(int));
    memcpy(&(*(*dest)).mounted, &(*src).mounted, sizeof(int));
   // memcpy(*dest->);

    /*typedef struct minode{
    INODE INODE;                         
    int dev, ino;
    int refCount;                         //initialized to 0
    int dirty;
    int mounted;
    struct mntable *mptr;
    }MINODE;*/
}

void printMinode(MINODE* mip)
{
    printf("dev = %d ", mip->dev);
    printf("ino = %d ",mip->ino);
    printf("refCount = %d ",mip->refCount);
    printf("dirty = %d ",mip->dirty);
    printf("mounted = %d\n",mip->mounted);

}


#endif