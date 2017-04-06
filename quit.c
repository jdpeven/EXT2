#include "global.c"
#include "util.c"
#include "type.h"

int quit()
{
    int i, dirty = 0;
    //MINODE *mip;
    for (i=0; i < 100; i++){
        if(minode[i].dirty > 0){
            minode[i].refCount = 0;            //in case some reference got messed up, but we still want to write it
            iput(&minode[i]);                   //because at the end obviously nothing is referencing it
            dirty++;
        }
    }
    printf("Wrote %d dirty files\n", dirty);
    return 0;
}