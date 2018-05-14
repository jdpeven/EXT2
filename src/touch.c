#ifndef TOUCH_C
#define TOUCH_C

#include "global.c"
#include "util.c"
#include "type.h"

touch(char * filename)
{
    int ino, dev;
    MINODE * mip = malloc(sizeof(MINODE));

    if(strlen(filename) == 0){
        printf("Filename not provided\n");
        return;
    }
    printf("touch = %s\n", filename);

    if(filename[0] == '/')                       //absolute path
    {
        dev = root->dev;
        ino = getino(&dev, filename);               //will get the ino of the first argument
        mip = iget(root->dev,ino);
    }
    else
    {
        dev = running->cwd->dev;
        ino = getino(&dev, filename);               //will get the ino of the first argument
        mip = iget(running->cwd->dev,ino);
    }
    if(ino == 0)                       //file not found
    {
        printf("File not found\n");
        return -1;
    }
    printf("File Ino = %d\n", ino);

    mip->INODE.i_atime = time(0L);
    mip->INODE.i_ctime = time(0L);
    mip->dirty = 1;
    iput(mip);
}


#endif