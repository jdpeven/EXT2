#include "global.c"
#include "util.c"
#include "type.h"

int chdir(char * pathname)
{
    MINODE *temp;
    //printf("My device is currently %d\n", temp->dev);

    int ino;

    if(strcmp(pathname, "") == 0){          // cd to the root
        printf("No argument given, cd to root\n");
        running->cwd = root;
        //memcpy(&(*temp), &(*root), sizeof(MINODE));
        return 0;
    }

    if(pathname[0] == '/'){                  //absolute path
        printf("Absolute pathname, running->cwd = root\n");
        temp = root;
        //memcpy(&(*temp), &(*root), sizeof(MINODE));
        //*running->cwd = *root;
    }
    else
        temp = running->cwd;

    printf("Attempting to CD into %s\n", pathname);
    ino = getino(temp, pathname);
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