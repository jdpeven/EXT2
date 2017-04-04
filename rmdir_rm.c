#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "ls.c"

krmdir (MINODE* parent, char* dirToRemove)
{

}

myrmdir(char * pathname)
{
    MINODE * parent = malloc(sizeof(MINODE));
    char dirToRemove[128], shortPath[128];
    char * splitPath[128];
    int size, i, parentIno, inoToRemove;

    printf("> removing %s\n", pathname);

    if (!strcmp(pathname, ""))                  //Checking to see if a pathname is given
    {
        printf("> No pathname specified.\n");
        return -1;                              //returning error
    }

    decompose(pathname, splitPath, &size, "/"); //splitting the path by '/'
    strcpy(dirToRemove, splitPath[size-1];

    for (i = 0; i < size; i++)                  //printing the returned array
    {
        printf("> splitPath[%d] = <%s>\n", i, splitPath[i]);
    }

    if (size == 1 && pathname[0] != '/')        //we remove the dir from cwd
    {
        parent = iget(running->cwd->dev,        //parent == pointer a MINODE storing cwd inode
            running->cwd->ino);
    }
    else
    {
        if (pathname[0] == '/')
        {
            strcpy(shortPath, "/");
        }
        
        for (i = 0; i < size-1; i++)            //Reconstructing orig path but to PARENT dir
        {
            strcat(shortPath, splitPath[i]);
            strcat(shortPath,"/");
        }
        parentIno = getino(&(running->cwd->dev), shortPath);
        printf("> shortPath AKA Parent DIR = <%s> with ino = <%d>\n", shortPath, parentIno);

        if (parentIno <= 0)                     //DIR not found
        {
            printf("> Parent DIR not found.\n");
            return -1;
        }

        parent = iget(running->cwd->dev,        //getting the parent MINODE pointer
            parentIno);
    }

    if (S_ISREG(parent->INODE.i_mode))
    {
        printf("> Cannot call rmdir on a file, try \"rm\".\n");
        return -1;
    }

    inoToRemove = search(parent, dirToRemove); //looking for the ino of the dir to remove
    if (inoToRemove <= 0)
    {
        printf("> No such dir exists\n");
        return -1;
    }
    
    printf("Ready to remove dir <%s>, Parent path <%s>\n", dirToRemove, shortPath);
    krmdir(parent, dirToRemove);

    parent->INODE.i_links_count--;
    parent->INODE.i_atime = time(0L);
    parent->dirty = 1;
    iput(parent);                           //overwrite with new data
    return 0;                               //success
}