#include "global.c"
#include "type.h"
#include "util.c"

int init()
{
    int i;

    PROC sUser;
    //sUser->next = ????
    sUser.pid = 0;
    sUser.uid = 0;
    sUser.cwd = NULL;
    //sUser.fd[NFD] = {}; ??????
    PROC nUser;
    nUser.pid = 1;
    nUser.uid = 1;
    nUser.cwd = NULL;

    proc[0] = sUser;
    proc[1] = nUser;

    MINODE temp;
    temp.refCount = 0;
    for(i = 0; i < NMINODE; i ++)
    {
      minode[i] = temp;           //This shouldn't matter that they're all the same, because
    }                             //When searching for an empty spot, it will just check to see if the ref count
                                  //Is 0 and will replace it immediately
    root = malloc(sizeof(MINODE));
    running = &proc[0];
    /*(1). 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
     (2). MINODE minode[100]; all with refCount=0
     (3). MINODE *root = 0;
   */

    return 0;

}