#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "ls.c"

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

void rmChild (MINODE* parent, char* dirToRemove)
{
    DIR* foundDir = malloc(sizeof(DIR));
    DIR* prevDir = malloc(sizeof(DIR));
    char buf[BLKSIZE];
    char* cp;
    int i = 0, foundflag = 0, ithBlock;

    printf("Entering rmChild\n");

    //Getting the first block, and scanning for dirToRemove
    while (i < 12 && foundflag == 0)
    {
        get_block(running->cwd->dev, parent->INODE.i_block[0], buf);
        cp = buf;
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

    //check to see if the rec_len > ideal length. If so, it is the last dirent
    if (foundDir->rec_len > 4*((8+strlen(foundDir->name)+4)/4))
    {
        printf("number of blocks %d\n", parent->INODE.i_blocks);
        //increasing the 2nd to last dir's rec_len by the last dir's rec_len
        prevDir->rec_len += foundDir->rec_len;
        printf("number of blocks %d\n", parent->INODE.i_blocks);
        printf("prevdir: NAME: %s, rec_len: %d\n", prevDir->name, prevDir->rec_len);
        ////PROBLEM i am not removing the dir from the parent block
        foundDir = 0;
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
    
}

krmdir (MINODE* parent, char* dirToRemove, int inoToRemove)
{
    uid_t uid = getuid(), euid = geteuid();
    MINODE* inodeToRemove = malloc(sizeof(MINODE));
    DIR* lastDir = malloc(sizeof(DIR));
    int i;

    printf("# Entering krmdir\n");

    //Permission check
    if (uid < 0 || uid != euid)                 
    {
        printf("# Incorrect priveledges\n");
        return -1;
    }

    inodeToRemove = iget(running->cwd->dev, inoToRemove);
    
    if (inodeToRemove->INODE.i_links_count > 2)
    {
        printf("# DIR is not empty\n");
        return -1;
    }
    //TODO: Check if busy... no fucking idea what that means...

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
    rmChild(parent, dirToRemove);

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
}