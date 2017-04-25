#ifndef LINK_UNLINK_C
#define LINK_UNLINK_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "mkdir_creat.c"
#include "rmdir_rm.c"

int mylink(char * oldFile, char * newFile)
{
    int oino, nino, pino;                           //pino is newFile parent inode
    int odev, ndev, pdev;

    MINODE * omip = malloc(sizeof(MINODE));
    MINODE * pmip = malloc(sizeof(MINODE));             //for the parent of the new file
    char pathSacrifice[128];


    if(strlen(oldFile) == 0 || strlen(newFile) == 0){
        printf("Incorrect number of filenames provided\n");
        return;
    }
    printf("Link Old = %s, New = %s\n", oldFile, newFile);

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
        printf("File not found\n");
        return -1;
    }
    printf("Old file Ino = %d\n", oino);

    if(S_ISDIR(omip->INODE.i_mode)){                      
        printf("Cannot link a dir\n");
        return -1;
    }

    //now dealing with the new file
    //we need to find the path to that file
    //and then make sure that file doesn't already exist  DONE
    //We also need to check to make sure that the devs are the same  DONE
    //Because hard links are't allowed across mounting


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
    if(odev != ndev)
    {
        printf("odev = %d, ndev = %d Cannot hard link across devices\n", odev, ndev);
        return;
    }

    printf("Now ready to hardlink ofile [%s] to nfile [%s]\n", oldFile, newFile);

    pmip = iget(pdev, pino);                        //now we have the MINODE of the newFile's parent 

    //I DON'T KNOW IF CREATE NEEDS TO BE CALLED
    enterChild(pmip, omip->ino, basename(newFile));

    omip->INODE.i_links_count++;
    omip->dirty = 1;

    iput(pmip);
    iput(omip);
    return;
}

int unlink(char * filename)
{
    int nino, ndev;
    int pino, pdev;
    MINODE * mip = malloc(sizeof(MINODE));
    MINODE * pmip = malloc(sizeof(MINODE));
    char pathSacrifice[128];

///(1)

    strcpy(pathSacrifice, filename);

    if(filename[0] == '/'){              //absolute
        pdev = root->dev;
        ndev = root->dev;
        nino = getino(&ndev, filename); 
        pino = getino(&pdev, dirname(pathSacrifice));
    }
    else{
        pdev = running->cwd->dev;
        ndev = running->cwd->dev;
        nino = getino(&ndev, filename); 
        pino = getino(&pdev, dirname(pathSacrifice));
    }
    
    if(nino == 0)
    {
        printf("File not found, returning\n");
        return -1;
    }

    mip = iget(ndev, nino);               //now mip has the file you want

    if(S_ISDIR(mip->INODE.i_mode)){                      
        printf("Cannot unlink a dir\n");
        return -1;
    }

    if(mip->INODE.i_uid != running->uid)
    {
        printf("Invalid permissions. uid = %d, INODE.i_uid = %d\n",running->uid,mip->INODE.i_uid);
        return 0;
    }

    printf("Ready to unlink [%s]\n", filename);

    //(2)
    mip = iget(ndev, nino);
    pmip = iget(pdev, pino);

    rmChild(pmip, basename(filename));
    //rm_child(pmip, mip->ino, basename(filename))  //needs to be added once rmdir is done
    pmip->dirty = 1;
    iput(pmip);

    //(3) decriment INODE's link count
    mip->INODE.i_links_count--;
    if(mip->INODE.i_links_count > 0)
    {
        mip->dirty = 1;
        iput(mip);
        return;
    }
    //(4) if it's not a symbolic link
    if(!S_ISLNK(mip->INODE.i_mode)){                //if it is just a normal file                 
        truncate(mip);
        idealloc(ndev, nino);
    }
    iput(mip);
    return;
}

/*
unlink(char *filename)
{ 
    1. get filenmae's minode:
        ino = getino(&dev, filename); mip = iget(dev, ino);
        check it's a REG or SLINK file
   2. remove basename from parent DIR
      rm_child(pmip, mip->ino, basename);
      pmip->dirty = 1;
      iput(pmip);
   3. // decrement INODE's link_count
      mip->INODE.i_links_count--;
      if (mip->INODE.i_links_count > 0){
      mip->dirty = 1; iput(mip);
}
4. if (!SLINK file) // assume:SLINK file has no data block
      truncate(mip); // deallocate all data blocks
      deallocate INODE;
      iput(mip);
}

*/



#endif