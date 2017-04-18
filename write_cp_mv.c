#ifndef WRITE_CP_MV_C
#define WRITE_CP_MV_C

#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "levelTwoHelper.c"

int writeFile()
{
    int fd;
    char input[BLKSIZE];
    int nlen;

    printf("Printing open fd\n");
    pfd();
    printf("Please enter a fd\n");
    scanf("%d", &fd);

    if(fd > NFD || fd < 0){
        printf("fd #[%d] not in range\n", fd);
        return 0;
    }
    if(running->fd[fd] == NULL){
        printf("fd #[%d] has not been allocated\n", fd);
        return 0;
    }

    if(running->fd[fd]->mode == 0)
    {
        printf("Cannot write to a file open for read only\n");
    }

    printf("Enter the string you'd like to write to the file\n");
    scanf("%s", &input);
    printf("You entered: %s\n", input);
    nlen = strlen(input);
    return mywrite(fd, input, nlen);
}

int mywrite(int fd, char buf[], int nbytes)
{
    OFT * oftp;
    MINODE * mip;
    int lbk, startByte, blk;
    char wbuf[BLKSIZE];
    char * cp, *cq;
    int remain;

    oftp = running->fd[fd];
    lbk = oftp->offset / BLKSIZE;
    startByte = oftp->offset % BLKSIZE;

    mip = oftp->mptr;

    switch(oftp->mode)              //for determining how to change the size
    {
        case 1:                     //write
            mip->INODE.i_size = nbytes;
            break;
        case 2:                     //readwrite. SHOULD BE SAME AS WRITE???
            mip->INODE.i_size = nbytes;
            break;
        case 3:                     //append
            mip->INODE.i_size += nbytes;        //different!
            break;
    }
    

    if(lbk < 12)            //writing to direct blocks
    {
        if(mip->INODE.i_block[lbk] == 0)                //incase you need to start a new block
            mip->INODE.i_block[lbk] = balloc(mip->dev); //allocates a new one
        blk = mip->INODE.i_block[lbk];                  //blk is now the newly allocated block
    }
    else if(lbk == 12)      //indirect
    {

    }
    else                    //double indirect
    {

    }
    printf("Need to write %d bytes to i_block[%d] = BNUM [%d] at startByte [%d]\n", nbytes, lbk, blk, startByte);

    while(nbytes > 0)
    {
        get_block(mip->dev, blk, wbuf);             //wbuf now holds this block. Might or might not be empty
        //VERY VERY IMPORTANT
        memset(&wbuf[startByte], 0, BLKSIZE - startByte);                   //clearing it out
        cp = wbuf + startByte;
        cq = buf;
        remain = BLKSIZE - startByte;

        while(remain > 0){
            *cp = *cq;
            cp++; cq++;
            nbytes--;
            remain--;
            oftp->offset++;
            if(oftp->offset > mip->INODE.i_size)            //I don't get this
                mip->INODE.i_size++;
            if(nbytes <= 0)
                break;
        }
        put_block(mip->dev, blk, wbuf);

        //This is in case you need to write to more than one block
        startByte = 0;              //going to start on a new block
        lbk++;                      //if we were doing block 3, now it's block 4
        if(lbk < 12)            //writing to direct blocks
        {
            if(mip->INODE.i_block[lbk] == 0)
                mip->INODE.i_block[lbk] = balloc(mip->dev);
            blk = mip->INODE.i_block[lbk];                  //blk is now the newly allocated block
        }
        else if(lbk == 12)      //indirect
        {

        }
        else                    //double indirect
        {

        }
        //now blk is the block number of the next block to write to
    }

}

#endif