#include "global.c"
#include "util.c"
#include "type.h"




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
    printf(" ");
    iput(mip);
}


int ls (char * pathname)
{
    int inodeToFind;
    int block0, i;
    char* cp;
    char dbuf[BLKSIZE], sbuf[BLKSIZE];
    DIR* dp;
    MINODE * temp;                              //will be set to the MINODE from pathname
    printf("This is being ls-ed\n");
    if(strcmp(pathname, "")==0){                //ls this file
        return;
    }
    if(pathname[0] == '/')
    {
        dev = root->dev;
        inodeToFind = getino(&dev, "/");
        temp = iget(dev, inodeToFind);
        
        block0 = temp->INODE.i_block[0];
        get_block(temp->dev, block0, dbuf);
        dp = (DIR*)dbuf;
        cp = dbuf;
        while (cp < &dbuf[BLKSIZE])
        {
            //printPermissions(dp->inode); NOT WORKING idk why.
            strncpy(sbuf, dp->name, dp->name_len);
            sbuf[dp->name_len] = 0;
            printf("%d|%s|\n", dp->inode, sbuf);
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }

        //printPermissions(inodeToFind);
        printf("\n");
    }
    else
        dev = running->cwd->dev;
    

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