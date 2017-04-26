#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"


MOUNT *mnttable[NMOUNT];
MINODE minode[NMINODE];		//minode array
MINODE *root;				//pointer to the root minode
PROC   proc[NPROC], *running;	//array of processes, and pointer to running process


//int dev;                        //init
int nblocks;                    //init
int ninodes;                    //init
int bmap;                       //init
int imap;                       //init
int inode_start;                //init

char * devName = "diskimage"; //will be used as default for mounting the root
char command[128];
char cmd[64];
char path[64];
char path2[64];

/*int mkdir(char * s){}
int rmdir(char * s){}
int cd(char * s){}              //we will make cd soon so delete
int pwd(char * s){}
int creat(char * s){}
int quit(char * s){}
int help(char * s){}
int link(char * s){}
int unlink(char * s){}
int symlink(char *s){}
int stat(char *s){}
int chmod(char *s){}
int touch(char *s){}*/



//char * CMD[] = {"mkdir","rmdir","ls", "cd", "pwd", "creat", "quit", "help", "link", "unlink", "symlink", "stat", "chmod", "touch", 0};
//int (*fptr[ ])(char *) = {(int (*) ())mkdir,rmdir,ls,cd,pwd,creat,quit,help,link,unlink,symlink,stat,chmod,touch};


#endif