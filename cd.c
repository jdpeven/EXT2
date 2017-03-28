#include "global.c"
#include "util.c"
#include "type.h"

int cd(char * pathname)
{
    /*
    (1) Get inode of pathname into a minode
    (2) verify it is a DIR
    (3) Change running proc pwd to that pathname
    (4) iput(old cwd);


    if (no pathname)
         cd to root;
      else
         cd to pathname;
    */

    return 0;
}