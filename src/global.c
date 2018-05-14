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

char * devName = "./diskimages/diskimage"; //will be used as default for mounting the root
char command[128];
char cmd[64];
char path[64];
char path2[64];

#endif