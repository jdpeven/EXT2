#ifndef ALLOCATE_DEALLOCATE_C
#define ALLOCATE_DEALLOCATE_C

#include "global.c"
#include "util.c"
#include "type.h"


int ialloc()
{
  int  i;
  char buf[BLKSIZE];
  get_block(running->cwd->dev, imap, buf);

  for (i=0; i < ninodes; i++){
    if (tst_bit(buf, i)==0){                        //find an empty inode
       set_bit(buf,i);                              //say that it isn't empty anymore
       decFreeInodes(running->cwd->dev);                          //tell the fd that there is one less free inode
                                                    //This function will get the superBlock/GroupDescriptor and decriment free inodes count in both
                                                    //function is in helpers.c
      
       put_block(running->cwd->dev, imap, buf);                //dev = fd, imapNum = gp->bg_inode_bitmap, buf = new edited imap 
       
       //get_block(fd, imapNum, buf);               //we're getting the bitmap we just "put"
       //printBitMap(buf);    
       
       //printf("Availabe INodes: %d\n", getAvailINode()); //This should be on less than before
       return i+1;                                  
    }
  }
  printf("ialloc(): no more free inodes\n");        //it's gone through all the inodes and all are 1.
  return 0;
}

int balloc(int dev)
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(dev, bmap, buf);
  //printf("The file we're messing with is %d\n", fd);

  for (i=0; i < nblocks; i++){
    if (tst_bit(buf, i)==0){                        
       set_bit(buf,i);                              //say that it isn't empty anymore
       decFreeBlocks(dev);                          //tell the fd that there is one less free inode
                                                    //This function will get the superBlock/GroupDescriptor and decriment free inodes count in both
                                                    //function is in helpers.c
      
       put_block(dev, bmap, buf);                //dev = fd, imapNum = gp->bg_inode_bitmap, buf = new edited imap 
       return i+1;                                  
    }
  }
  printf("ialloc(): no more free inodes\n");        //it's gone through all the inodes and all are 1.
  return 0;
}

idealloc(int dev, int ino)
{
    char buf[BLKSIZE];
    get_block(dev, imap, buf); //this will get the inode bitmap from the dev
    clr_bit(buf, (ino-1));           //this may be an off by one error
    incFreeInodes(dev);
    put_block(dev, imap, buf);
    printf("Inode #%d deallocated\n", ino);
}

bdealloc(int dev, int block)
{
    char buf[BLKSIZE];
    get_block(dev, bmap, buf); //this will get the inode bitmap from the dev
    clr_bit(buf, (block-1));           //this may be an off by one error
    incFreeBlocks(dev);
    put_block(dev, bmap, buf);
    printf("Block #%d deallocated\n", block);
}

#endif