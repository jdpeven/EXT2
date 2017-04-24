#ifndef MOUNT_UNMOUNT_C
#define MOUNT_UNMOUNT_C

#include "global.c"
#include "util.c"
#include "type.h"

mymount(char * mountpoint, char * filesys)
{
    int i;
    int mdev, mino;
    MINODE *mip;
    MOUNT * newMT = malloc(sizeof(MOUNT));
    
    if(strcmp(mountpoint, "") == 0 && strcmp(filesys, "") == 0)
    {
        printf("No mountpoint or filysys provided, printing mount details\n");
        pmount();
        return;
    } 
    for(i = 0; i < NMOUNT; i++)
    {
        if(mnttable[i] == NULL)
            continue;
        if(strcmp(filesys, mnttable[i]->name) == 0)
        {
            printf("Filesys already mounted at index %d\n", i);
            return 0;
        }
    }

    mino = getino(&running->cwd->dev, mountpoint);      //this might need to be changed
    if(mino == 0)
    {
        printf("Mountpoint not found\n");
        return 0;
    }
    mip = iget(running->cwd->dev, mino);
    if(!S_ISDIR(mip->INODE.i_mode))
    {
        printf("Cannot mount onto a non-dir entry\n");
        return 0;
    }
    if(mip->refCount > 1)
    {
        printf("MINODE is busy, cannot mount\n");
        return 0;
    }
    printf("Attempting to mount filesys %s to mountpoint %s\n", filesys, mountpoint);
    
    addMountEntry(filesys, &newMT);
    if(newMT->dev == 0)
    {
        return 0;
    }
    mip->mounted = 1;
    mip->mptr = newMT;
}

pmount()
{
    int i;
    for(int i = 0; i < NMOUNT; i++)
    {
        printf("MNT Index = %d\n",i);
        if(mnttable[i] == NULL)
            continue;
        printf("Name = %s, Dev = %d, NINODE = %d, NBLOCKS = %d, bmap = %d, imap = %d, inode_start = %d\n",
                    mnttable[i]->name, mnttable[i]->dev, mnttable[i]->ninodes, mnttable[i]->nblocks,
                    mnttable[i]->bmap, mnttable[i]->imap, mnttable[i]->inode_start);
    }
}

myunmount(char * filesys)
{
    int i;
    if(strcmp(filesys, "") == 0)
    {
        printf("name not provided\n");
        return 0;
    }
    /*for(i = 0; i < NMOUNT; i++)
    {

    }*/
}


//(*newMTptr)->dev != *newMT->dev
//So many errors
void addMountEntry(char * filesys, MOUNT ** newMTptr)
{
    int i;
    int newDev;
    char superBuf[BLKSIZE], groupBuf[BLKSIZE];
    SUPER *newsp;
    GD *newgp;
    MINODE *newroot = malloc(sizeof(MINODE));

    newDev = open(filesys, O_RDWR);
    if(newDev < 0)
    {
        printf("filesys opened unsuccessfully\n");
        return;
    }
    get_block(newDev, 1, superBuf);
    newsp = (SUPER *)superBuf;
    
    printf("s_magic = %x\n", newsp->s_magic);
    if (newsp->s_magic != 0xEF53){
        printf("NOT an EXT2 FS\n");
        return;
    }
    get_block(newDev, 2, groupBuf);
    newgp = (GD *)groupBuf;
    //now it's legit so we can add it
    //newMT = (MOUNT *)malloc(sizeof(MOUNT));
    (*newMTptr)->dev = newDev;
    strcpy((*newMTptr)->name, filesys);
    (*newMTptr)->ninodes = newsp->s_inodes_count;
    (*newMTptr)->nblocks = newsp->s_blocks_count;
    (*newMTptr)->bmap = newgp->bg_block_bitmap;
    (*newMTptr)->imap = newgp->bg_inode_bitmap;
    (*newMTptr)->inode_start = newgp->bg_inode_table;

    newroot = iget(newDev, 2);
    newroot->dev = newDev;        
    newroot->ino = 2;          
    newroot->refCount = 1;             
    newroot->dirty = 0;        
    newroot->mounted = 0;  

    (*newMTptr)->mntroot = newroot;

    for(i = 0; i < NMOUNT; i++)
    {
        if(mnttable[i] == NULL)
        {
            printf("New entry put at mnttable[%d]", i);
            mnttable[i] = (*newMTptr);
            return;
        }
    }
    printf("No free space for more Mount points\n");

    return;
}

#endif