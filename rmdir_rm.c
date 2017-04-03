#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "ls.c"

myrmdir(char * pathname)
{
    printf("removing %s\n", pathname);
    //to see if a dir is empty, check if it's link_count == 2;
}