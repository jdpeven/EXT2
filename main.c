#include "type.h"
#include "util.c"
#include "global.c"
#include "mount_root.c"

int main(int argc, char*argv[])
{
    if(argc < 1)       //"a.out diskimage"
        devName = argv[1];
    mount_root();       //devName is a global so it doesn't need to be passed in



    printf("Hello\n");
    return 0;
}