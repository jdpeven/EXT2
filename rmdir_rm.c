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
        return 0;
    }
    if(cmip->INODE.i_links_count > 2)
    {
        printf("Cannot rmdir a directory that isn't empty\n");
        return 0;
    }

    if(!emptyDir(cmip))
    {
        printf("Cannot rmdir a directory that isn't empty\n");
        return 0;
    }

    if (inodeToRemove->refCount > 1)
    {
        printf("# DIR is busy\n");
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


/*

DIR* openDir (char* nameOfDir, MINODE* curINODE)
{
    DIR* foundDir = malloc(sizeof(DIR));
    char blockBuf[BLKSIZE];
    get_block(running->cwd->dev, curINODE->INODE.i_block[0], blockBuf);

    foundDir = (DIR*)blockBuf;
    if (strcmp(foundDir->name, nameOfDir))
    {
        foundDir = (DIR*)(blockBuf + foundDir->rec_len);
    }

    return foundDir;
}

MINODE* rmChild (MINODE* parent, char* dirToRemove)
{
    DIR* foundDir = malloc(sizeof(DIR));
    DIR* prevDir = malloc(sizeof(DIR));
    DIR* nextDir = malloc(sizeof(DIR));
    DIR* curDir = malloc(sizeof(DIR));
    char buf[BLKSIZE];
    char* cp;
    char* finalcp;
    int i = 0, foundflag = 0, ithBlock, removedRL, sumRL = 0, adhelp = 0;
    int foundino;

    printf("Entering rmChild\n");

    //Getting the first block, and scanning for dirToRemove
    while (i < 12 && foundflag == 0)
    {
        get_block(running->cwd->dev, parent->INODE.i_block[i], buf);
        cp = buf;
        finalcp = buf;
        foundDir = (DIR*)buf;
        
        while (cp < &buf[BLKSIZE])
        {
            if (strcmp(foundDir->name, dirToRemove))
            {
                prevDir = foundDir;
                foundDir = (DIR*)(cp += foundDir->rec_len);
            }
            else
            {
                foundflag = 1;
                ithBlock = i;
                break;
            }
        }
        i++;
    }

    printf("NAME: %s, REC: %d\n", foundDir->name, foundDir->rec_len);
    printf("SUMRL = %d\n", sumRL);
    printf("estimated %d\n",  4*((8+strlen(foundDir->name)+3)/4));

    //check to see if the rec_len > ideal length. If so, it is the last dirent
    if (foundDir->rec_len > 4*((8+strlen(foundDir->name)+3)/4))
    {
        printf("number of blocks %d\n", parent->INODE.i_blocks);
        //increasing the 2nd to last dir's rec_len by the last dir's rec_len
        prevDir->rec_len += foundDir->rec_len;
        printf("number of blocks %d\n", parent->INODE.i_blocks);
        printf("prevdir: NAME: %s, rec_len: %d\n", prevDir->name, prevDir->rec_len);
    }
    else if (foundDir->rec_len == 4*((8+strlen(foundDir->name)+3)/4))
    {
        printf("Removing from the middle\n");
        //Storing the rec_len of our dir, we will add it to the final dirent
        removedRL = foundDir->rec_len;
        
        printf("\nGoing through dirs...\n");
        nextDir = (DIR*)cp;
        printf("reclen: %d name: %s\n", nextDir->rec_len, nextDir->name);
        while(nextDir->rec_len == 4*((8+strlen(nextDir->name)+3)/4))
        {
            //cp = (DIR*)(cp + nextDir->rec_len);
            nextDir = (DIR*)(cp += nextDir->rec_len);
            printf("reclen: %d name: %s\n", nextDir->rec_len, nextDir->name);
        }

        printf("last dirs rec: %d -> ", nextDir->rec_len);
        //nextDir->rec_len += removedRL;
        printf("%d\n", nextDir->rec_len);

        printf("adhelp = %d, sumRL = %d\n", adhelp, sumRL);
        
        while (strcmp(((DIR*)(finalcp+sumRL))->name, dirToRemove))
        {
            printf("we are fucking with: %s\n", ((DIR*)(finalcp+sumRL))->name);
            sumRL += ((DIR*)(finalcp+sumRL))->rec_len;
        }
        printf("we are fucking with: %s\n", ((DIR*)(finalcp+sumRL))->name);

        adhelp = sumRL + ((DIR*)(finalcp+sumRL))->rec_len;
        
        curDir = ((DIR*)(finalcp+sumRL));
        nextDir = ((DIR*)(finalcp+adhelp));
        //printf("current dir: name: %s recLen: %d\n", curDir->name, curDir->rec_len);
        while (nextDir->rec_len <= 4*((8+strlen(((DIR*)(finalcp+adhelp))->name)+3)/4))
        {  
            printf("current dir: name: [%s] recLen: %d\n", curDir->name, curDir->rec_len);
            //printf("next dir: name: [%s] recLen: %d\n\n", nextDir->name, nextDir->rec_len);
            printf("hello");
            curDir->inode = nextDir->inode;
            curDir->rec_len = nextDir->rec_len;
            curDir->name_len = nextDir->name_len;
            curDir->file_type = nextDir->file_type;
            strcpy(curDir->name, nextDir->name);

            adhelp += ((DIR*)(finalcp+sumRL))->rec_len;
            curDir = ((DIR*)(finalcp+sumRL+curDir->rec_len));
            nextDir = ((DIR*)(finalcp+adhelp));
        }
        //printf("next dir: %d, equation: %d\n", nextDir->rec_len, 4*((8+strlen(nextDir->name)+3)/4));
        //printf("next dir-> %s, size = %d\n", nextDir->name, strlen(nextDir->name));
        //printf("OUT current dir: name: [%s] recLen: %d\n", curDir->name, curDir->rec_len);
        //printf("next dir: name: [%s] recLen: %d\n", nextDir->name, nextDir->rec_len);
        curDir->inode = nextDir->inode;
        curDir->rec_len = nextDir->rec_len + removedRL;
        curDir->name_len = nextDir->name_len;
        curDir->file_type = nextDir->file_type;
        strcpy(curDir->name, nextDir->name);
        
        
        put_block(running->cwd->dev, parent->INODE.i_block[ithBlock], finalcp);
        return parent;
    }
    else if (foundDir->rec_len == 1024)
    {
        //only dir in the dir, so we need to completely deallocate the block
        //Moving the nonzero blocks up
        for (i = ithBlock; i < 11; i++)
        {
            if (parent->INODE.i_block[i+1] != 0)
            {
                parent->INODE.i_block[i] = parent->INODE.i_block[i+1];
                parent->INODE.i_block[i+1] = 0;
            }
            else
            {
                //once we find the first 0 block, then we can stop
                break;
            }
        }

        //Make the size of the parent inode 1 block less
        parent->INODE.i_size -= 1024;
    }
    
    put_block(running->cwd->dev, parent->INODE.i_block[ithBlock], buf);
    return parent;
}

krmdir (MINODE* parent, char* dirToRemove, int inoToRemove)
{
    int uid = running->uid;
    MINODE* inodeToRemove = malloc(sizeof(MINODE));
    DIR* lastDir = malloc(sizeof(DIR));
    int i;

    printf("# Entering krmdir\n");

    //Permission check LEVEL 3
    if (uid < 0 || uid != euid)                 
    {
        printf("# Incorrect priveledges\n");
        return -1;
    }

    inodeToRemove = iget(running->cwd->dev, inoToRemove);

    if(S_ISREG(inodeToRemove->INODE.i_mode))
    {
        printf("> Cannot call rmdir on a file, try \"rm\".\n");
        return -1;
    }
    
    if (inodeToRemove->INODE.i_links_count > 2)
    {
        printf("# DIR is not empty\n");
        return -1;
    }
    //TODO: Check if busy... no fucking idea what that means...
    if (inodeToRemove->refCount > 1)
    {
        printf("# DIR is busy\n");
        return -1;
    }
    
    //getting the dir '..'
    lastDir = openDir("..", inodeToRemove);

    //if ..'s rec_len == 12, not empty
    if (strcmp(lastDir->name, ".."))                 
    {
        printf("# DIR is not empty\n");
        return -1;
    }

    printf("# %s is empty and not in use, continuing with removal\n", dirToRemove);

    //deallocating all blocks for this dir
    for (i = 0; i < 12; i++)                    
    {
        if (inodeToRemove->INODE.i_block[i] != 0)
        {
            bdealloc(running->cwd->dev, inodeToRemove->INODE.i_block[i]);
        }
    }
    //Deallocating the inode itself
    idealloc(running->cwd->dev, inoToRemove);
    //clears refcount
    iput(inodeToRemove);

    //remove the childs entry from the parent dir
    parent = rmChild(parent, dirToRemove);

    //decrementing link count, touching time fields, marking dirty
    parent->INODE.i_links_count--;
    parent->INODE.i_atime = time(0L);
    parent->INODE.i_mtime = time(0L);
    parent->dirty = 1;
    iput(parent);
    return 0;
}

myrmdir(char * pathname)
{
    MINODE * parent = malloc(sizeof(MINODE));
    char dirToRemove[128], shortPath[128];
    char * splitPath[128];
    int size, i, parentIno, inoToRemove, returnStatus;

    printf("> removing %s\n", pathname);

    //Checking to see if a pathname is given
    if (!strcmp(pathname, ""))
    {
        printf("> No pathname specified.\n");
        return -1;
    }
    if (!strcmp(pathname, "/"))
    {
        printf("> Can't remove root\n");
        return -1;
    }

    //splitting the path by '/'
    decompose(pathname, splitPath, &size, "/"); 
    strcpy(dirToRemove, splitPath[size-1]);

    //printing the returned array
    for (i = 0; i < size; i++)                 
    {
        printf("> splitPath[%d] = <%s>\n", i, splitPath[i]);
    }

    //we remove the dir from cwd
    if (size == 1 && pathname[0] != '/')        
    {
        //parent == pointer a MINODE storing cwd inode
        parent = iget(running->cwd->dev,        
            running->cwd->ino);
    }
    else
    {
        if (pathname[0] == '/')
        {
            strcpy(shortPath, "/");
        }
        
        //Reconstructing orig path but to PARENT dir
        for (i = 0; i < size-1; i++)            
        {
            strcat(shortPath, splitPath[i]);
            strcat(shortPath,"/");
        }
        parentIno = getino(&(running->cwd->dev), shortPath);
        printf("> shortPath AKA Parent DIR = <%s> with ino = <%d>\n", shortPath, parentIno);

        //DIR not found
        if (parentIno <= 0)                     
        {
            printf("> Parent DIR not found.\n");
            return -1;
        }

        //getting the parent MINODE pointer
        parent = iget(running->cwd->dev,        
            parentIno);
    }

    if (S_ISREG(parent->INODE.i_mode))
    {
        printf("> Cannot call rmdir on a file, try \"rm\".\n");
        return -1;
    }

    //looking for the ino of the dir to remove
    inoToRemove = search(parent, dirToRemove); 

    if (inoToRemove <= 0)
    {
        printf("> No such dir exists\n");
        return -1;
    }

    printf("Ready to remove dir <%s>, Parent path <%s>\n", dirToRemove, shortPath);
    returnStatus = krmdir(parent, dirToRemove, inoToRemove);
    if (returnStatus == 0)
    {
        printf("># DIR successfuly removed!\n");
    }
    return 0;
}*/
#endif