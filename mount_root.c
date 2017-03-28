#include "global.c"
#include "type.h"
#include "util.c"

int super()
{
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    ninodes = sp->s_inodes_count;
    nblocks = sp->s_blocks_count;
    if(isEXT2())
        printf("Valid EXT2FS\n");
}

int isEXT2()
{
    printf("s_magic = %x\n", sp->s_magic);
    if (sp->s_magic != 0xEF53){
        printf("NOT an EXT2 FS\n");
        exit(1);
    }
}

int gd()
{
    char buf[BLKSIZE];
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;
    inode_start = gp->bg_inode_table;
    printf("bmap = %d imap = %d iblock = %d\n", bmap, imap, inode_start);
}


int mount_root()
{
    /*
    open device for RW (get a file descriptor dev for the opened device)
      read SUPER block to verify it's an EXT2 FS
   
      root = iget(dev, 2);     //get root inode 
    
      Let cwd of both P0 and P1 point at the root minode (refCount=3)
          P0.cwd = iget(dev, 2); 
          P1.cwd = iget(dev, 2);
    */
    dev = open(devName, O_RDWR);
    if(dev < 0){
        printf("Error in opening file\n");
        return -1;
    }
    super();
    gd();
    printf("mount : %s  mounted on /", devName);
    printf("nblocks = %d  bfree = %d   ninodes = %d\n", sp->s_blocks_count, sp->s_free_blocks_count, sp->s_inodes_count);
    root = iget(dev, 2);
    printf("Mounted root\n");
    printf("Creating P0, P1");
    proc[0].cwd = iget(dev, 2);             //p0
    proc[1].cwd = iget(dev, 2);             //p1

    return 0;
}