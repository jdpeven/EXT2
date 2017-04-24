#ifndef TYPE_H
#define TYPE_H

#include <ext2fs/ext2_fs.h>
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define FREE          0
#define READY         1

#define BLKSIZE     1024
#define BLOCK_SIZE  1024                //not sure why this is defined twice

#define NMINODE      100                
#define NFD           16                //Max number of opened files allowed
#define NPROC          4                //Max number of running Processes allowed
#define NMOUNT        10                //randomly chose 10

typedef struct minode{
  INODE INODE;                         
  int dev, ino;
  int refCount;                         //initialized to 0
  int dirty;
  int mounted;
  struct MOUNT *mptr;
}MINODE;

typedef struct mount{
  char name[128];
  MINODE *mntroot;
  int ninodes;
  int nblocks;
  int bmap;
  int imap;
  int inode_start;
  int dev;
}MOUNT;


typedef struct oft{             //open file descriptors
  int  mode;                    //Permissions?
  int  refCount;
  MINODE *mptr;                 //pointer to the referenced minode
  int  offset;                  //This will be changed with lseek
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid;
  MINODE      *cwd;
  OFT         *fd[NFD];     //16       
}PROC;

#endif