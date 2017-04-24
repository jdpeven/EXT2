#ifndef LS_C
#define LS_C


#include "global.c"
#include "util.c"
#include "type.h"
#include "time.h"


void printPermissions(int ino)
{
    MINODE * mip = iget(running->cwd->dev, ino);
    //printf( (S_ISDIR(mip->INODE.i_mode)) ? "d" : "-");
    if (S_ISDIR(mip->INODE.i_mode))
    {
        printf("d");
    }
    else if (S_ISREG(mip->INODE.i_mode))
    {
        printf("-");
    }
    else if (S_ISLNK(mip->INODE.i_mode))
    {
        printf("l");
    }
    else
    {
        printf("-");
    }
    printf( (mip->INODE.i_mode & S_IRUSR) ? "r" : "-");
    printf( (mip->INODE.i_mode & S_IWUSR) ? "w" : "-");
    printf( (mip->INODE.i_mode & S_IXUSR) ? "x" : "-");
    printf( (mip->INODE.i_mode & S_IRGRP) ? "r" : "-");
    printf( (mip->INODE.i_mode & S_IWGRP) ? "w" : "-");
    printf( (mip->INODE.i_mode & S_IXGRP) ? "x" : "-");
    printf( (mip->INODE.i_mode & S_IROTH) ? "r" : "-");
    printf( (mip->INODE.i_mode & S_IWOTH) ? "w" : "-");
    printf( (mip->INODE.i_mode & S_IXOTH) ? "x" : "-");
    iput(mip);
}


int ls (char * pathname)
{
    int inodeToFind, block0, i;
    char* cp, * linkpath;
    char dbuf[BLKSIZE], sbuf[BLKSIZE], linkname[60];
    INODE* ip;
    DIR* dp;
    DIR* linkdp;
    MINODE * temp;                              //will be set to the MINODE from pathname
    //printf("This is being ls-ed\n");
    if(strcmp(pathname, "")==0){                //ls this file
        temp = iget(running->cwd->dev, running->cwd->ino);
    }

    else if(strcmp(pathname, "/")==0){
        dev = root->dev;
        inodeToFind = getino(&dev, "/");
        temp = iget(dev, inodeToFind);
    }
    else{
        inodeToFind = getino(&(running->cwd->dev),pathname);
        if (inodeToFind == 0)
        {
            printf("DIR %s not found\n", pathname);
            return;
        }
        temp = iget((running->cwd->dev), inodeToFind);
    }

    i = 0;
    while (temp->INODE.i_block[i] != 0)
    {
        block0 = temp->INODE.i_block[i];
        get_block(temp->dev, block0, dbuf);
        dp = (DIR*)dbuf;
        cp = dbuf;

        while (cp < &dbuf[BLKSIZE])
        {
            //printPermissions(dp->inode); NOT WORKING idk why.
            strncpy(sbuf, dp->name, dp->name_len);
            sbuf[dp->name_len] = 0;
            ip = iget(dev, dp->inode);

            printPermissions(dp->inode);

            if (S_ISLNK(ip->i_mode))
            {
                readlinkUtil(sbuf, linkname);
                //printf("nice\n");
                strcat(sbuf," -> ");
                strcat(sbuf, linkname);
            }

            printf("   %d   %.13s\t%d\t%d\t%d\t%s\n", ip->i_links_count, 
                ctime(&ip->i_ctime), ip->i_size, dp->rec_len, dp->inode,
                sbuf);

            cp += dp->rec_len;
            dp = (DIR*)cp;
            iput(ip);
        }
        i++;
    }

    iput(temp);

    printf("\n");
    

}


int readlinkUtil(char * filename, char * buffer)
{
    int dev, ino;
    MINODE * mip = malloc(sizeof(MINODE));
    if(strlen(filename) == 0){
        //printf("Filename not provided\n");
        return;
    }
    //printf("readlink filename = %s\n", filename);

    if(filename[0] == '/')                       //absolute path
    {
        dev = root->dev;
        ino = getino(&dev, filename);               //will get the ino of the first argument
    }
    else
    {
        dev = running->cwd->dev;
        ino = getino(&dev, filename);               //will get the ino of the first argument
    }
    if(ino == 0)                       //file not found
    {
        //printf("file not found\n");
        return -1;
    }
    //printf("file Ino = %d\n", ino);

    mip = iget(dev,ino);

    if(mip->INODE.i_mode != 0xa1ff){                //not a symlink file
        //printf("File provided is not a symlink file\n");
        iput(mip);
        return 0;
    }

    //2. copy target filename in INODE.i_block into a buffer;
    strcpy(buffer, (char *)mip->INODE.i_block);
    //3. return strlen((char *)mip ->INODE.i_block);

    /*free(omip);
    free(nmip);
    free(pmip);*/
    iput(mip);
    return(strlen((char *)mip->INODE.i_block));
}

#endif