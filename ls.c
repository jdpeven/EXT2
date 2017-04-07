#ifndef LS_C
#define LS_C


#include "global.c"
#include "util.c"
#include "type.h"
#include "time.h"


void printPermissions(int ino)
{
    MINODE * mip = iget(running->cwd->dev, ino);
    printf( (S_ISDIR(mip->INODE.i_mode)) ? "d" : "-");
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
    char* cp;
    char dbuf[BLKSIZE], sbuf[BLKSIZE];
    INODE* ip;
    DIR* dp;
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
            printf("   %d   %.13s\t%d\t%d\t%s\n", ip->i_links_count, 
                ctime(&ip->i_ctime), ip->i_size, dp->rec_len, sbuf);
            cp += dp->rec_len;
            dp = (DIR*)cp;
            iput(ip);
        }
        i++;
    }

    iput(temp);

    printf("\n");
    

}


/*
int main(int argc, char **argv)
{
    if(argc != 2)    
        return 1;
 
    struct stat fileStat;
    if(stat(argv[1],&fileStat) < 0)    
        return 1;
 
    printf("Information for %s\n",argv[1]);
    printf("---------------------------\n");
    printf("File Size: \t\t%d bytes\n",fileStat.st_size);
    printf("Number of Links: \t%d\n",fileStat.st_nlink);
    printf("File inode: \t\t%d\n",fileStat.st_ino);
    printf("Ctime: \t\t%s\n", ctime(&fileStat.st_ctime));
 
    printf("st_mode: %x\n", fileStat.st_mode);
    printf("File Permissions: \t");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");
 
    printf("The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");
 
    return 0;
}
*/

#endif