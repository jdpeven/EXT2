#ifndef TYPE_H
#define TYPE_H

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

typedef struct minode{
  INODE* INODE;                         //it was yelling at me so I made this a pointer
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntable *mptr;
}MINODE;

typedef struct oft{             //open file descriptors
  int  mode;                    //Permissions?
  int  refCount;
  MINODE *mptr;                 //pointer to the referenced inode?
  int  offset;                  //This will be changed with lseek
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid;
  MINODE      *cwd;
  OFT         *fd[NFD];         
}PROC;

#endif