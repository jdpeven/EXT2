#include "type.h"
#include "util.c"
#include "global.c"
#include "mount_root.c"
#include "init.c"

int main(int argc, char*argv[])
{
    init();
    printf("Initialization complete\n");
    if(argc < 1)       //"a.out diskimage"
        devName = argv[1];
    mount_root();       //devName is a global so it doesn't need to be passed in

    //Loop for commands
    while(1)
    {
        strcpy(command, "");
        printf("P0 running: input command : ");
        fgets(command, 128, stdin);
        command[strcspn(command, "\n")] = 0;        //removes /n
        sscanf("%s %s", )
        //Get commands
        //Use sscanf to break it into cmd an pathname
        //use function pointers to call the specific functions
        //if there is a path, decompose it using decompose()
        //execute (usually by forking a child)
    }

    printf("Hello\n");
    return 0;
}