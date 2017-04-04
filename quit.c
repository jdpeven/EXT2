#include "global.c"
#include "util.c"
#include "type.h"

int quit()
{
    int i, dirty = 0;
    //MINODE *mip;
    for (i=0; i < 100; i++){
        if(minode[i].dirty > 0){
            iput(&minode[i]);
            dirty++;
        }
    }
    printf("Wrote %d dirty files\n", dirty);
    return 0;
}