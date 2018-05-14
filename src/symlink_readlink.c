#ifndef SYMLINK_READLINK_C
#define SYMLINK_READLINK_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "mkdir_creat.c"

int symlink(char * oldFile, char * newFile)
{
    int oino, nino, pino;                           //pino is newFile parent inode
    int odev, ndev, pdev;
    int nbno;                                       //block number for newly allocated INODE

    
    MINODE * omip = malloc(sizeof(MINODE));
    MINODE * pmip = malloc(sizeof(MINODE));             //for the parent of the new file
    MINODE * nmip = malloc(sizeof(MINODE));             //because we're actually creating a file this time
    char pathSacrifice[128];


    if(strlen(oldFile) == 0 || strlen(newFile) == 0){
        printf("Incorrect number of filenames provided\n");
        return;
    }
    printf("Symlink Old = %s, New = %s\n", oldFile, newFile);

    if(oldFile[0] == '/')                       //absolute path
    {
        odev = root->dev;
        oino = getino(&odev, oldFile);               //will get the ino of the first argument
        omip = iget(root->dev,oino);
    }
    else
    {
        odev = running->cwd->dev;
        oino = getino(&odev, oldFile);               //will get the ino of the first argument
        omip = iget(running->cwd->dev,oino);
    }
    if(oino == 0)                       //file not found
    {
        printf("Old file not found\n");
        return -1;
    }
    printf("Old file Ino = %d\n", oino);

    /*if(S_ISDIR(omip->INODE.i_mode)){                      
        printf("Cannot link a dir\n");
        return -1;
    }*/

    //now dealing with the new file

    strcpy(pathSacrifice, newFile);             //saving a copy for dirname()/basename() calls
    
    if(newFile[0] == '/')                       //absolute path
    {
        ndev = root->dev;
        pdev = root->dev;
        nino = getino(&ndev, newFile);               //looks for that filename
        pino = getino(&pdev, dirname(pathSacrifice));
    }
    else
    {
        ndev = running->cwd->dev;
        pdev = running->cwd->dev;
        nino = getino(&ndev, newFile);               //will get the ino of the first argument
        pino = getino(&pdev, dirname(pathSacrifice));
    }
    if(nino != 0)
    {
        printf("The new file already exists\n");
        return;
    }

    printf("Now ready to symlink ofile [%s] to nfile [%s]\n", oldFile, newFile);

    pmip = iget(pdev, pino);                        //now we have the MINODE of the newFile's parent 

    //(2)
    //allocating new file
    nino = ialloc(pmip->dev);
    //bno = balloc(pmip->dev);
    nmip = iget(pmip->dev, nino);

    INODE * ip = &nmip->INODE;
    ip->i_mode = 0xa1ff;                    //stole this from KC by creating a symlink file in his and then stating it                                //This is symlink I think
    ip->i_uid = running->uid;
    ip->i_gid = running->pid;                               //MAYBE WRONG
    ip->i_size = strlen(oldFile);                           // "tiny" -> 4
    ip->i_links_count = 1;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);            //THIS IS RELEVANT FOR TOUCH
    ip->i_blocks = 0;                           //I'd imagine this is 0
    
    strcpy((char*)ip->i_block, oldFile);

    nmip->dirty = 1;
    iput(nmip);

    enterChild(pmip, nino, basename(newFile));
    pmip->dirty = 1;
    iput(pmip);
}

int readlink(char * filename, char * buffer)
{
    int dev, ino;
    MINODE * mip = malloc(sizeof(MINODE));
    if(strlen(filename) == 0){
        printf("Filename not provided\n");
        return;
    }
    printf("readlink filename = %s\n", filename);

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
        printf("file not found\n");
        return -1;
    }
    printf("file Ino = %d\n", ino);

    mip = iget(dev,ino);

    if(mip->INODE.i_mode != 0xa1ff){                //not a symlink file
        printf("File provided is not a symlink file\n");
        iput(mip);
        return 0;
    }

    //2. copy target filename in INODE.i_block into a buffer;
    strcpy(buffer, (char *)mip->INODE.i_block);
    //3. return strlen((char *)mip ->INODE.i_block);

    iput(mip);
    return(strlen((char *)mip->INODE.i_block));
}

#endif