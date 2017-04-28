#ifndef RMDIR_RM_C 
#define RMDIR_RM_C

#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "ls.c"

//will search through the directory to see if it's empty
int emptyDir(MINODE *mip)
{
    char buf[BLKSIZE];
    char sbuf[BLKSIZE];
    char * cp;
    DIR * dp;
    int i;
    for(i = 1; i < 15; i++)
    {
        if(mip->INODE.i_block[i] != 0)
            return 0;
    }
    get_block(mip->dev, mip->INODE.i_block[0], buf);

    cp = &buf;
    dp = (DIR *)cp;

    while(cp < &buf[BLKSIZE])
    {
        strncpy(sbuf, dp->name, dp->name_len);
        sbuf[dp->name_len] = 0;
        if(strcmp(sbuf, ".") != 0 && strcmp(sbuf, "..") != 0)
        {
            return 0;
        }
        cp+=dp->rec_len;
        dp = (DIR *)cp;
    }
    return 1;
}


int myrmdir(char * pathname)
{
    int cdev, cino;
    int pdev, pino;
    MINODE * pmip, *cmip;
    char pathSacrifice[128];
    int basenameIndex;
    char * bname[128];

    strcpy(pathSacrifice, pathname);

    if(strcmp(pathname, "") == 0)
    {
        printf("Pathname not provided\n");
        return 0;
    }

    if(pathname[0] == '/')
        cdev = root->dev;
    else
        cdev = running->cwd->dev;

    cino = getino(&cdev, pathname);
    if(cino == 0)
    {
        printf("File not found, returning\n");
        return 0;
    }
    cmip = iget(cdev, cino);

    //check ownernship

    if(!S_ISDIR(cmip->INODE.i_mode))
    {
        printf("Cannot rmdir a non dir file\n");
        iput(cmip);
        return 0;
    }
    if(cmip->INODE.i_links_count > 2)
    {
        printf("Cannot rmdir a directory that isn't empty\n");
        iput(cmip);
        return 0;
    }
    if(cmip->INODE.i_uid != running->uid)
    {
        printf("Invalid permissions. uid = %d, INODE.i_uid = %d\n",running->uid,cmip->INODE.i_uid);
        iput(cmip);
        return 0;
    }

    if(!emptyDir(cmip))
    {
        printf("Cannot rmdir a directory that isn't empty\n");
        iput(cmip);
        return 0;
    }

    if (cmip->refCount > 1)
    {
        printf("# DIR is busy\n");
        iput(cmip);
        return -1;
    }

    truncate(cmip);
    idealloc(cdev, cino);
    iput(cmip);

    if(pathname[0] == '/')
        pdev = root->dev;
    else
        pdev = running->cwd->dev;

    pino = getino(&pdev, dirname(pathSacrifice));
    if(pino == 0)
    {
        printf("I have no idea how this happened\n");
        return 0;
    }
    pmip = iget(pdev,pino);

    decompose(pathname, bname, &basenameIndex, "/");

    printf("Now removing %s from the parent\n", bname[basenameIndex-1]);

    rmChild(pmip, bname[basenameIndex-1]);

    pmip->INODE.i_links_count--;
    pmip->INODE.i_atime = time(0L);
    pmip->INODE.i_mtime = time(0L);
    pmip->dirty = 1;
    iput(pmip);
    return 0;
}

int rmChild(MINODE *pmip, char *name)
{
    char buf[BLKSIZE];
    char sbuf[BLKSIZE];
    int blk = 0;
    char * cp;
    DIR * dp;
    char * cq;      //to find the second to last entry
    DIR * dq;       //to find the second to last entry
    int found = 0;
    int removedLen; //for when we remove the entry from the middle
    while(blk < 12 && found == 0)             //can assume only direct blocks
    {
        get_block(pmip->dev, pmip->INODE.i_block[blk], buf);
        cp = &buf;
        dp = (DIR *)cp;
        while(cp < &buf[BLKSIZE])
        {
            strncpy(sbuf, dp->name, dp->name_len);
            sbuf[dp->name_len] = 0;
            if(strcmp(name, sbuf) == 0)
            {
                found = 1;
                break;
            }
            else
            {
                cp+= dp->rec_len;
                dp = (DIR *)cp;
            }
        }
        blk++;
    }

    blk--; //dumb off by one error
    //now blk is the block of the entry
    //cp is the pointer to the beginning of the entry
    //dp is the pointer to the DIR entry
    //sbuf is the name of the dir
    ////testing = 4*((8+dp->name_len + 3)/4);
    if(dp->rec_len == BLKSIZE)
        //The only entry
    {
        bdealloc(pmip->dev, pmip->INODE.i_block[blk]);   //deeallocating the block
        pmip->INODE.i_block[blk] = 0;
        pmip->INODE.i_size--;                            //decreasing the size
    }
    else if(dp->rec_len == 4*((8+dp->name_len +3)/4))
        //In the middle
    {
        removedLen = dp->rec_len;
        mymemcpy(cp, cp+dp->rec_len, buf);
        cq = &buf;
        dq = (DIR *)cq;
        while(dq->rec_len == 4*((8+dq->name_len + 3)/4))
        {
            cq += dq->rec_len;
            dq = (DIR*)cq;
        }
        //now cq/dq are referencing the last entry
        dq->rec_len+=removedLen;
    }
    
    else if(dp->rec_len > 4*((8+dp->name_len + 3)/4))
        //last entry in the set
    {
        //Need to find the SECOND to last entry
        cq = &buf;
        dq = (DIR *)cq;
        while(cq + dq->rec_len < cp)
        {
            cq+= dq->rec_len;
            dq = (DIR *)cq;
        }
        //now cq/dq are the second to last entries of the list
        dq->rec_len += dp->rec_len;             //This should write over the last entry
    }
    put_block(pmip->dev, pmip->INODE.i_block[blk], buf);
}

int mymemcpy(char * src, char * dest, char * arr)
{
    while(dest < &arr[BLKSIZE])
    {
        *src = *dest;
        src++;
        dest++;
    }
}




#endif