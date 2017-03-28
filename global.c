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

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

int dev;                        //init
int nblocks;                    //init
int ninodes;                    //init
int bmap;                       //init
int imap;                       //init
int inode_start;                //init

char * devName = "diskimage"; //will be used as default for mounting the root
char command[128];
char cmd[64];
char path[64];
//There should be a command array like in our old homework

#endif