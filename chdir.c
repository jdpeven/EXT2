#include "global.c"
#include "util.c"
#include "type.h"

int chdir(char * pathname)
{
    MINODE *temp;// = malloc(sizeof(MINODE));
    int dev;
    int ino;
    int mntparentino;
    int mntparentdev;
    //MOUNT * mou;

    if(strcmp(pathname, ".") == 0){
        printf("CD to the same directory, kinda pointless but hey man you do you\n");
        return 0;
    }

    if(strcmp(pathname, "..") == 0){
        //LEVEL3
        if(2 == nameToIno(running->cwd, ".") && running->cwd->dev != root->dev)
        //if you're at A root, but not THE root
        {
            printf("Need to jump mount points to Ino = %d, Dev = %d\n", running->cwd->mptr->mntloc->ino, running->cwd->mptr->mntloc->dev);
            //temp = iget(running->cwd->mptr->mntloc->dev, running->cwd->mptr->mntloc->ino);
            //^this is making things go into an inifinte loop
            //I'm worried that running->cwd = running->cwd->mntloc will screw up refcounts
            temp = running->cwd->mptr->mntloc;
            mntparentino = nameToIno(temp, "..");
            mntparentdev = temp->dev;
            temp = iget(mntparentdev, mntparentino);
            iput(running->cwd);
            running->cwd = temp;
            return 0;
        }
        printf("CD to the parent directory, getting ino of '..'\n");
        ino = nameToIno(running->cwd, "..");
        //ino = getino(&(running->cwd->dev), ".."); //it might not be the same device
        printf("'..' Ino = %d\n", ino);
        temp = iget(running->cwd->dev, ino);

        iput(running->cwd);
        //iput(temp);                             //(4)
        running->cwd = temp;

        printf("running->cwd = %s\n", pathname);
        return 0;
    }

    if(strcmp(pathname, "") == 0){          // cd to the root
        printf("No argument given, cd to root\n");
        iput(running->cwd);
        running->cwd = iget(root->dev, 2);
        return 0;
    }

    printf("Attempting to CD into %s\n", pathname);
    dev = running->cwd->dev;                //this might change
    //LEVEL THREE STUFF SHOULD BE TAKEN CARE OF IN GETINO
    ino = getino(&dev, pathname);
    if(ino == 0)
    {
        printf("File not found, cannot CD\n");
        //no need to iput() because never really called iget()
        return -1;
    }
    printf("Now ino = %d, dev = %d\n", ino, dev);
    temp = iget(dev, ino);             //ASK KC IS THIS RIGHT  (1)

    if(S_ISREG(temp->INODE.i_mode)){                      //(2)
        printf("Cannot cd into non-dir file\n");
        iput(temp);                     //putting it back
        return 0;
    }
    //otherwise
    //IF THERE ARE ISSUE WITH LVL3, THEY LIVE HERE
    iput(running->cwd);                             //(4)
    running->cwd = temp;                            //(3)                      
    printf("running->cwd = %s\n", pathname);
    return 0;
}