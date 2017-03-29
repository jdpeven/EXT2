#include "global.c"
#include "util.c"
#include "type.h"

int statFile(char * pathname, struct stat *mystat)
{
    MINODE * temp;
    int ino;
    if(strcmp(pathname, "")==0){                    //stat pwd
        printf("\n********** stat **********\n");
        printf("No pathname provided, stat pwd\n");
        printf("dev= %d   ino=%d   mod=%4x\n",running->cwd->dev,running->cwd->ino,running->cwd->INODE.i_mode);
        printf("uid=%d   gid=%d   nlink=%d\n",running->cwd->INODE.i_uid, running->cwd->INODE.i_gid, running->cwd->INODE.i_links_count);
        printf("size=%d time= %s\n",running->cwd->INODE.i_size, ctime(&running->cwd->INODE.i_ctime));
        return 0;
    }
    ino = getino(&(running->cwd->dev),pathname);
    temp = iget((running->cwd->dev), ino);

    printf("\n********** stat **********\n");
    printf("stat %s\n", pathname);
    printf("dev= %d   ino=%d   mod=%4x\n",temp->dev,temp->ino,temp->INODE.i_mode);
    printf("uid=%d   gid=%d   nlink=%d\n",temp->INODE.i_uid, temp->INODE.i_gid, temp->INODE.i_links_count);
    printf("size=%d time= %s\n",temp->INODE.i_size, ctime(&temp->INODE.i_ctime));


    return 0;
}