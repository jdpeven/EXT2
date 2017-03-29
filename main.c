#include "type.h"
#include "util.c"
#include "global.c"
#include "mount_root.c"
#include "init.c"
#include "ls.c"
#include "cd.c"
#include "quit.c"
#include "statFile.c"

int main(int argc, char*argv[])
{
    int cmdIndex, error;
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
        
        sscanf(command, "%s %s %s", cmd, path);

        if(strcmp(command, "ls") == 0)
            ls(path);
        if(strcmp(command, "cd") == 0)
            cd(path);
        if(strcmp(command, "stat") == 0){
            struct stat mystat;
            stat(path, &mystat);
        }
        if(strcmp(command, "quit") == 0){
            quit(path);
            break;
        }

        /*cmdIndex = findCmd(command);
        if(cmdIndex == -1)
            {printf("Invalid command\n");}
        else
            {error = fptr[cmdIndex](path);}*/

        //Get commands
        //Use sscanf to break it into cmd an pathname
        //use function pointers to call the specific functions
        //if there is a path, decompose it using decompose()
        //execute (usually by forking a child)
    }

    //printf("Hello\n");
    return 0;
}