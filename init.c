#include "global.c"
#include "type.h"
#include "util.c"

int init()
{
    int i;

    PROC sUser;   //creating a new super user process.
    //sUser->next = ????
    sUser.pid = 0;
    sUser.uid = 0;
    sUser.cwd = NULL;
    //sUser.fd[NFD] = {}; ??????
    PROC nUser;
    nUser.pid = 1;
    nUser.uid = 1;
    nUser.cwd = NULL;

    proc[0] = sUser;  //updating our proc array
    proc[1] = nUser;

    MINODE temp;      //creating new empty minode since refcount is 0 they will be seen as empty
    temp.refCount = 0;

    for(i = 0; i < NMOUNT; i++)
    {
      mnttable[i] = NULL;
    }
    for(i = 0; i < NMINODE; i ++) //assigning every (100) minode to temp
    {
      minode[i] = temp;           //This shouldn't matter that they're all the same, because
    }                             //When searching for an empty spot, it will just check to see if the ref count
                                  //Is 0 and will replace it immediately
    root = malloc(sizeof(MINODE));
    running = &proc[0]; //assigning our running process to the first process AKA superuser

    return 0;

}