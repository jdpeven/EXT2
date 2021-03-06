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
#include "link_unlink.c"
#include "symlink_readlink.c"
#include "touch.c"
#include "open_close_lseek.c"
#include "levelTwoHelper.c"
#include "write_cp_mv.c"
#include "chmod.c"
#include "read.c"
#include "mount_unmount.c"
#include "cat.c"


int main(int argc, char*argv[])
{
    int cmdIndex, error;
    char linkName[60];
    int linkSize = 0;
    system("./shcopy");            //sh copy syscall

    init();
    printf("Initialization complete\n");
    if(argc < 1)       //"a.out diskimage"
        devName = argv[1];  //null
    mount_root();       //devName is a global so it doesn't need to be passed in

    //Loop for commands
    while(1)
    {
        strcpy(command, "");
        strcpy(cmd, "");
        strcpy(path, "");
        strcpy(path2, "");
        printf("\nPrinting cwd details\n");
        printMinode(running->cwd);
        printf("P0 running: input command [ls,cd,stat,pwd,mkdir,creat,link,unlink,symlink,readlink,touch,quit, open, close, mount, etc]: ");
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
            //struct stat mystat;
            statFile(path);
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
            if(linkSize != 0){
                printf("\n[%d] bytes read into buffer = [%s]\n", linkSize, linkName);
            }
            else{
                printf("Error with readlink\n");
            }
        }
        else if(strcmp(cmd, "touch") == 0){
            touch(path);
        }
        else if(strcmp(cmd, "chmod") == 0){
            mychmod(path, path2);
        }
        else if(strcmp(cmd, "rmdir") == 0){
            myrmdir(path);
        }
        else if(strcmp(cmd, "rm") == 0){
            unlink(path);
            //myrm(path);
        }
        else if(strcmp(cmd, "open") == 0){
              openFile(path, path2);
        }
        else if(strcmp(cmd, "close") == 0){
              closeFile(path);
        }
        else if(strcmp(cmd, "read") == 0){
            myread(path, path2);
        }   
        else if(strcmp(cmd, "write") == 0){
            writeFile();
        }
        else if(strcmp(cmd, "lseek") == 0){
            mylseek(path, path2);
        }
        else if(strcmp(cmd, "cat") == 0){
            mycat(path);
            //printf("made it\n");
        }
        else if(strcmp(cmd, "cp") == 0){
            mycp(path, path2);
        }
        else if(strcmp(cmd, "mv") == 0){
            mymv(path, path2);
        }
        else if(strcmp(cmd, "pfd") == 0){
            pfd();
        }
        else if(strcmp(cmd, "mount") == 0)
        {
            mymount(path, path2);
        }
        else if(strcmp(cmd, "unmount") == 0)
        {
            myunmount(path);
        }
        else if(strcmp(cmd, "fork") == 0)
        {
            fork(path);
        }
        else{
            printf("COMMAND NOT FOUND\n");
        }
    }

    //printf("Hello\n");
    return 0;
}