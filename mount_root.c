#include "global.c"
#include "type.h"
#include "util.c"

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



}