#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "ls.c"

DIR* openDir (char* nameOfDir, MINODE* curINODE)
{
    DIR* foundDir = malloc(sizeof(DIR));
    char blockBuf[1024];
    get_block(running->cwd->dev, curINODE->INODE.i_block[0], blockBuf);

    foundDir = (DIR*)blockBuf;
    if (strcmp(foundDir->name, nameOfDir))
    {
        foundDir = (DIR*)(blockBuf + foundDir->rec_len);
    }

    return foundDir;
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
    if (lastDir->rec_len == 12)                 
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