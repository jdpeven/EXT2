#include "global.c"
#include "util.c"
#include "type.h"

int quit()
{
    int i, dirty = 0;
    for (i=0; i < 100; i++){            //for each element in the array
        if(minode[i].dirty > 0){        //if we've changed it
            minode[i].refCount = 1;            //in case some reference got messed up, but we still want to write it
            iput(&minode[i]);                   //because at the end obviously nothing is referencing it
            dirty++;
        }
    }
    closeFile();                        //This will close all open fd
    printf("Wrote %d dirty files\n", dirty);
    return 0;
}