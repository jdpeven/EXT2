#ifndef LS_C
#define LS_C


#include "global.c"
#include "util.c"
#include "type.h"
#include "time.h"


void printPermissions(int ino, int dev)
{
    MINODE * mip = iget(dev, ino);
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
    //if we ls /a/b, directory will be the MINODE for /a/b,
    //element will be the MINODE for each entry in the directory
    //This is important for size, links, etc
    MINODE * directory, * element;
    int pathIno, pathDev, elementIno;
    int blockNum, actualBlock;
    char buf[BLKSIZE], sbuf[BLKSIZE];
    char * cp;
    DIR * dp;
    int linksCount, gid, uid, size;
    char * createtime;

    if(strcmp(pathname, "") == 0)
    {
        //So I can iput it later and be fine
        pathDev = running->cwd->dev;
        pathIno = running->cwd->ino;
        directory = iget(pathDev, pathIno);//// running->cwd;
    }
    else
    {
        if(pathname[0] == '/')
        {
            pathDev = root->dev;
            pathIno = search(root, pathname);
        }
        else
        {
            pathDev = running->cwd->dev;
            pathIno = search(running->cwd, pathname);
        }
        if(pathIno == 0)                //not found
        {
            printf("Path not found\n");
            return 0;
        }
        directory = iget(pathDev, pathIno);
    }

    if(S_ISREG(directory->INODE.i_mode))
    {
        printf("Trying to ls a file, calling stat\n");
        statFile(pathname);
        return 0;
    }

    //will iterate through the block of the inode
    //Only [0,12) because we will assume directories do not have indirect blocks
    for(blockNum = 0; blockNum < 12; blockNum++)
    {
        actualBlock = directory->INODE.i_block[blockNum];
        if(actualBlock == 0)
            break;
        get_block(pathDev, actualBlock, buf);
        dp = (DIR *)buf;
        cp = buf;
        
        printf("Permissions\tLinks\tGID\tUID\tCreation Time\tSize\tRec_len\tInode\tName\n");
        while(cp < &buf[BLKSIZE])
        {
            printPermissions(pathIno, pathDev);
            //strcpy(sbuf, "");                       //clearing it out
            strncpy(sbuf, dp->name, dp->name_len);
            sbuf[dp->name_len] = 0;
            element = iget(pathDev, dp->inode);
            size = element->INODE.i_size;
            linksCount = element->INODE.i_links_count;
            gid = element->INODE.i_gid;
            uid = element->INODE.i_uid;

            printf("\t%d\t%d\t%d\t%.13s\t%d\t%d\t%d\t%s", linksCount, gid, uid,
                    ctime(&(element->INODE.i_ctime)),size,dp->rec_len,dp->inode, sbuf); 

            if(S_ISLNK(element->INODE.i_mode))
            {
                //The name of what is being linked is stored in the inode's i_block
                printf("->%s", (char*)element->INODE.i_block);
            }

            printf("\n");
            cp+=dp->rec_len;
            dp = (DIR *)cp;
            iput(element);
        }
    }
}

/*
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
    int dev;
    
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

    if(S_ISREG(temp->INODE.i_mode))
        //regular file
    {
        printf("Trying to ls a file, calling stat\n");
        statFile(pathname);
        return 0;
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
            ip = iget(running->cwd->dev, dp->inode);        //dev to running->cwd->dev

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

    //free(omip);
    //free(nmip);
    //free(pmip);
    iput(mip);
    return(strlen((char *)mip->INODE.i_block));
}
*/

#endif