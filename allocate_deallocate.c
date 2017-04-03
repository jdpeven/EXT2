#include "global.c"
#include "util.c"
#include "type.h"


int ialloc()
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(running->cwd->dev, imap, buf);
  //printf("The file we're messing with is %d\n", fd);

  for (i=0; i < ninodes; i++){
    if (tst_bit(buf, i)==0){                        //find an empty inode
      //printf("Inode # %d is free\n", i+1);
       //printf("Availabe INodes: %d\n", getAvailINode());
       //printBitMap(buf); 
       set_bit(buf,i);                              //say that it isn't empty anymore
       //printBitMap(buf);
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

/*iallocHelper()
{
// read SUPER block
  char buff[BLKSIZE];
  printf("**********STARTING IALLOC**********\n");
  int ino;
  get_block(fd, 1, buf);
  sp = (SUPER *)buf;

  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;
  nfreeInodes = sp->s_free_inodes_count;
  nfreeBlocks = sp->s_free_blocks_count;
  printf("ninodes=%d nblocks=%d nfreeInodes=%d nfreeBlocks=%d\n", 
	 ninodes, nblocks, nfreeInodes, nfreeBlocks);

  // read Group Descriptor 0
  get_block(fd, 2, buf);
  gp = (GD *)buf;

  imapNum = gp->bg_inode_bitmap;
  printf("imap = %d\n", imapNum);

    //ino = ialloc();
    //printf("allocated ino = %d\n", ino);
    //printf("\n");
    

  //getchar();
  //printf("The file we're messing with is %d\n", fd);
  for (i=0; i < 5; i++){  
    ino = ialloc();
    printf("allocated ino = %d\n", ino);              //k so all we did was mark that we can now potentially use This
                                                      //I'd imagine that the next step is taking this "ino" and adding stuff to it
                                                      //Because we know it's been recently allocated and is free
  }
  //get_block(fd,imapNum,buf);
  //printBitMap(buf, ninodes);
  for(i = 0; i < 5; i++){
    idealloc(fd,ino-i);
  }
  //printBitMap(buf, ninodes);
  
  printf("**********ENDING IALLOC**********\n");
}*/

int balloc()
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(running->cwd->dev, bmap, buf);
  //printf("The file we're messing with is %d\n", fd);

  for (i=0; i < nblocks; i++){
    if (tst_bit(buf, i)==0){                        //find an empty inode
      //printf("Inode # %d is free\n", i+1);
       //printf("Availabe INodes: %d\n", getAvailINode());
       //printBitMap(buf); 
       set_bit(buf,i);                              //say that it isn't empty anymore
       //printBitMap(buf);
       decFreeBlocks(running->cwd->dev);                          //tell the fd that there is one less free inode
                                                    //This function will get the superBlock/GroupDescriptor and decriment free inodes count in both
                                                    //function is in helpers.c
      
       put_block(running->cwd->dev, bmap, buf);                //dev = fd, imapNum = gp->bg_inode_bitmap, buf = new edited imap 
       
       //get_block(fd, imapNum, buf);               //we're getting the bitmap we just "put"
       //printBitMap(buf);    
       
       //printf("Availabe INodes: %d\n", getAvailINode()); //This should be on less than before
       return i+1;                                  
    }
  }
  printf("ialloc(): no more free inodes\n");        //it's gone through all the inodes and all are 1.
  return 0;
}
/*
ballocHelper()
{
// read SUPER block
  printf("**********STARTING BALLOC**********\n");
  int block;
  get_block(fd, 1, buf);
  sp = (SUPER *)buf;

  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;
  nfreeInodes = sp->s_free_inodes_count;
  nfreeBlocks = sp->s_free_blocks_count;
  printf("ninodes=%d nblocks=%d nfreeInodes=%d nfreeBlocks=%d\n", 
	 ninodes, nblocks, nfreeInodes, nfreeBlocks);

  // read Group Descriptor 0
  get_block(fd, 2, buf);
  gp = (GD *)buf;

  bmapNum = gp->bg_block_bitmap;
  printf("bmap = %d\n", bmapNum);

    /*ino = ialloc();
    printf("allocated ino = %d\n", ino);
    printf("\n");
    

  //getchar();
  //printf("The file we're messing with is %d\n", fd);
  for (i=0; i < 5; i++){  
    block = balloc();
    printf("allocated block = %d\n", block);              //k so all we did was mark that we can now potentially use This
                                                      //I'd imagine that the next step is taking this "ino" and adding stuff to it
                                                      //Because we know it's been recently allocated and is free
  }
  get_block(fd,bmapNum,buf);
  printBitMap(buf, nblocks);
  for (i = 0; i < 5; i++){
    bdealloc(fd,block-i);
  }
  printBitMap(buf, nblocks);
  
  printf("**********ENDING BALLOC**********\n");
}*/


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