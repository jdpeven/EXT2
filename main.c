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
#include "rmdir_rm.c"
#include "link_unlink.c"
#include "symlink_readlink.c"
#include "touch.c"

int main(int argc, char*argv[])
{
    int cmdIndex, error;
    char linkName[128];
    int linkSize;
    system("./shcopy");            //sh copy syscall
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
        strcpy(path2, "");
        printf("\nPrinting cwd details\n");
        printMinode(proc[0].cwd);
        printf("P0 running: input command [ls,cd,stat,pwd,mkdir,creat,link,unlink,symlink,readlink,touch,quit]: ");
        fgets(command, 128, stdin);
        
        command[strcspn(command, "\n")] = 0;        //removes /n
        
        sscanf(command, "%s %s %s %s", cmd, path, path2);

        printf("cmd = %s, path = %s, path2 = %s\n", cmd, path, path2);

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
            mymkdirCreat(path, "mkdir");
        }
        else if(strcmp(cmd, "creat") == 0){
            mymkdirCreat(path, "creat");
        }
        else if(strcmp(cmd, "link") == 0){
            mylink(path, path2);
        }
        else if(strcmp(cmd, "unlink") == 0){
            unlink(path);
        }
        else if(strcmp(cmd, "symlink") == 0){
            symlink(path, path2);
        }
        else if(strcmp(cmd, "readlink") == 0){
            linkSize = readlink(path, linkName);            //maybe &linkName, arrays are weird
        }
        else if(strcmp(cmd, "touch") == 0){
            touch(path);
        }
        else if(strcmp(cmd, "rmdir") == 0){
            myrmdir(path);
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