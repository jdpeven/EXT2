#include "global.c"
#include "util.c"
#include "type.h"

int chdir(char * pathname)
{
    MINODE *temp = malloc(sizeof(MINODE));
    int dev;
    //printf("My device is currently %d\n", temp->dev);

    int ino;

    if(strcmp(pathname, "") == 0){          // cd to the root
        printf("No argument given, cd to root\n");
        running->cwd = iget(root->dev, 2);
        iput(root);                         //balancing the iget
        //copyMinodeptr(root, &running->cwd);
        //memcpy(&(*temp), &(*root), sizeof(MINODE));
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
    //printf("Now ino = %d\n", ino);
    temp = iget(temp->dev, ino);             //ASK KC IS THIS RIGHT  (1)

    if(S_ISREG(temp->INODE.i_mode)){                      //(2)
        printf("Cannot cd into non-dir file\n");
        return 0;
    }
    //otherwise
    iput(running->cwd);                             //(4)
    running->cwd = temp;                            //(3)

    printf("running->cwd = %s", pathname);
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