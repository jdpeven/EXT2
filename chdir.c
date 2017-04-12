#include "global.c"
#include "util.c"
#include "type.h"

int chdir(char * pathname)
{
    MINODE *temp = malloc(sizeof(MINODE));
    int dev;
    int ino;

    if(strcmp(pathname, ".") == 0){
        printf("CD to the same directory, kinda pointless but hey man you do you\n");
        return 0;
    }

    if(strcmp(pathname, "..") == 0){
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
    /*
    if(pathname[0] == '/'){                  //absolute path
        printf("Absolute pathname, running->cwd = root\n");
        temp = iget(root->dev, root->ino);
        iput(root);                                         //will just decriment the refCount
        //copyMinodeptr(root, &temp);
        //temp = root;
        //memcpy(&(*temp), &(*root), sizeof(MINODE));
        //*running->cwd = *root;
    }
    else{
        temp = iget(running->cwd->dev, running->cwd->ino);
        iput(running->cwd);                                 //will just decriment the refCount
    }
        //temp = running->cwd;
    printf("Now printing temps details\n");
    printMinode(temp);
    */

    printf("Attempting to CD into %s\n", pathname);
    ino = getino(&(running->cwd->dev), pathname);
    if(ino == 0)
    {
        printf("File not found, cannot CD\n");
        //no need to iput() because never really called iget()
        return -1;
    }
    printf("Now ino = %d\n", ino);
    temp = iget(running->cwd->dev, ino);             //ASK KC IS THIS RIGHT  (1)

    if(S_ISREG(temp->INODE.i_mode)){                      //(2)
        printf("Cannot cd into non-dir file\n");
        iput(temp);                     //putting it back
        return 0;
    }
    //otherwise
    iput(running->cwd);                             //(4)
    running->cwd = temp;                            //(3)                      
    printf("running->cwd = %s\n", pathname);
    return 0;
    /*
    (1) Get inode of pathname into a minode             done
    (2) verify it is a DIR
    (3) Change running proc pwd to that pathname
    (4) iput(old cwd);

    if (no pathname)
         cd to root;
      else
         cd to pathname;
    */
    
}