#include "global.c"
#include "type.h"
#include "util.c"


int super(int dev)
{
    char buf[BLKSIZE];              //creating a new buf of blksize aka 1024
    get_block(dev, 1, buf);         //getting the block with our dev (global int) that we found in mount_root()
    sp = (SUPER *)buf;              //casting our buf to a super pointer. Assigning our global super pointer sp to it.
    ninodes = sp->s_inodes_count;   //getting the number of inodes
    nblocks = sp->s_blocks_count;   //getting the number of blocks
    if(isEXT2())                    //aux function to check to see if the diskimage has a valid EXT2 FS
        printf("Valid EXT2FS\n");
}

int isEXT2()
{
    printf("s_magic = %x\n", sp->s_magic);  //we print the magic number contained within the super struct. (ind: 1 block of the diskimage)
    if (sp->s_magic != 0xEF53){             //if the number matches this predetermined hex, than we are good to go!
        printf("NOT an EXT2 FS\n");
        exit(1);
    }
}

int gd(int dev)
{
    char buf[BLKSIZE];                  //new buf
    get_block(dev, 2, buf);             //get the block of the GD
    gp = (GD *)buf;                     //assign our GLOBAL group desc pointer (gp) to the casted buf
    //filling some global vars here with information from the gp
    bmap = gp->bg_block_bitmap;         //block number of the bitmap
    imap = gp->bg_inode_bitmap;         //Imap block number
    inode_start = gp->bg_inode_table;   //indoes begin block number
    printf("bmap = %d imap = %d iblock = %d\n", bmap, imap, inode_start);
}


int mount_root()
{
    int i, j;
    int dev;

    dev = open(devName, O_RDWR);    //opening our file, here devName is diskimage
    if(dev < 0){
        printf("Error in opening file\n");
        return -1;
    }
    super(dev);    //calling super function. This gets our sp pointer, num of blocks, num of inodes, and checks for EXT2 validity
    gd(dev);       //calling gd function. Gets our gp pointer, and some global vars.
    printf("mount : %s  mounted on /\n", devName);
    printf("nblocks = %d  bfree = %d   ninodes = %d\n", sp->s_blocks_count, gp->bg_free_blocks_count, sp->s_inodes_count);
    
    root = iget(dev, 2);    //setting root to the MINODE* that is returned by iget. passing it the dev global int, and 2 for 
    root->dev = dev;        //setting roots dev to our global dev
    root->ino = 2;          //setting the inode number
    root->refCount = 1;     
    root->dirty = 0;        
    root->mounted = 0;      
    //mnttable?

    printf("Mounted root\n");
    printf("Printing root values\n");
    printMinode(root);
    printf("Creating P0, P1\n");
    proc[0].cwd = iget(dev, 2);             //setting cwd of p0 and p1 to the root.
    proc[1].cwd = iget(dev, 2);             //p1

    for(i = 0; i < NFD; i++)    //Each entry in the table
    {
        for(j = 0; j < 4; j++)  //Each proc
        {
            proc[j].fd[i] = NULL;
        }
    }

    return 0;
}