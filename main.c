#include "type.h"
#include "util.c"
#include "global.c"
#include "mount_root.c"
#include "init.c"
#include "pwd.c"
#include "ls.c"
#include "chdir.c"
#include "quit.c"
#include "statFile.c"
#include "iget_iput_getino.c"
#include "mkdir_creat.c"

int main(int argc, char*argv[])
{
    int cmdIndex, error;
    system("/home/parallels/Homework/cpts-360-lab7/shcopy");
    init();
    printf("Initialization complete\n");
    if(argc < 1)       //"a.out diskimage"
        devName = argv[1];
    mount_root();       //devName is a global so it doesn't need to be passed in

    //Loop for commands
    while(1)
    {
        strcpy(command, "");
        strcpy(cmd, "");
        strcpy(path, "");
        printf("Printing cwd details\n");
        printMinode(proc[0].cwd);
        printf("P0 running: input command : ");
        fgets(command, 128, stdin);
        
        command[strcspn(command, "\n")] = 0;        //removes /n
        
        sscanf(command, "%s %s %s", cmd, path);

        printf("cmd = %s, pathname = %s\n", cmd, path);

        if(strcmp(cmd, "ls") == 0)
            ls(path);
        else if(strcmp(cmd, "cd") == 0){
            chdir(path);
        }
        else if(strcmp(cmd, "stat") == 0){
            struct stat mystat;
            statFile(path, &mystat);
        }
        else if(strcmp(cmd, "pwd") == 0){
            pwd(path);
        }
        else if(strcmp(cmd, "quit") == 0){
            quit(path);
            break;
        }
        else if(strcmp(cmd, "mkdir") == 0){
            mymkdir(path);
        }
        else{
            printf("COMMAND NOT FOUND\n");
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