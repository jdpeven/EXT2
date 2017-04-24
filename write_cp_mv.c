#ifndef WRITE_CP_MV_C
#define WRITE_CP_MV_C

#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "levelTwoHelper.c"
#include "open_close_lseek.c"
#include "mkdir_creat.c"

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
    char wbuf[BLKSIZE], directbuf[BLKSIZE], doublebuf[BLKSIZE];
    char * cp, *cq;
    int remain;
    int indirectOffset;
    int doubleOffset;
    int * intp;
    char firstHalf[BLKSIZE], secondHalf[BLKSIZE];

    oftp = running->fd[fd];
    lbk = oftp->offset / BLKSIZE;
    startByte = oftp->offset % BLKSIZE;
    
    mip = oftp->mptr;

    switch(oftp->mode)              //for determining how to change the size
    {
        case 1:                     //write
            mip->INODE.i_size = 0;
            break;
        case 2:                     //readwrite. SHOULD BE SAME AS WRITE???
            mip->INODE.i_size = 0;
            break;
        /*case 3:                     //append
            mip->INODE.i_size += nbytes;        //different!
            break;*/
    }
    
    
    while(nbytes > 0)
    {
        //What's important to remember is that lbk is which actual block we want, but we only
        //have the first 12 easily. So the first if statement is pretty, then the next two aren't

        if(lbk < 12)            //writing to direct blocks
        {
            blk = mip->INODE.i_block[lbk];                  //blk is now the block at that offset
        }
        else if(lbk >= 12 && lbk < 268)      //indirect 268 = 256 + 12
        {
            printf("Indirect blocks, need block #[%d]\n", lbk);
            indirectOffset = lbk - 12;                  //this will be the block in the indirect block
            get_block(mip->dev, mip->INODE.i_block[12], directbuf);         //gets the indirect block into directbuf
            intp = &directbuf;                          //points to the start of the buffer
            intp += indirectOffset;                     //moving it to the offset
            blk = *intp;                                //now the block has the ACTUAL block that is necessary
        }
        else                    //double indirect
        {
            printf("Double indirect blocks, need block #[%d]\n", lbk);
            doubleOffset = (lbk - 12 - 256) / 256;
            indirectOffset = (lbk - 12 - 256) % 256;
            get_block(mip->dev, mip->INODE.i_block[13], doublebuf);
            intp = &doublebuf;
            intp += doubleOffset;
            get_block(mip->dev, *intp, directbuf);
            intp = &directbuf;
            intp += indirectOffset;
            blk = *intp;
        }
        if(blk == 0)
        {
            printf("The required block has not been allocated yet\n");
            blk = increaseSize(lbk, mip);
            printf("lbk [%d] has been allocated to blk [%d]\n", lbk, blk);
        }
        printf("Need to write %d bytes to i_block[%d] = BNUM [%d] at startByte [%d]\n", nbytes, lbk, blk, startByte);

        //NOW FOR THE REAL STUFF

        get_block(mip->dev, blk, wbuf);             //wbuf now holds this block. Might or might not be empty
        //VERY VERY IMPORTANT
        memset(&wbuf[startByte], 0, BLKSIZE - startByte);                   //clearing it out
        cp = wbuf + startByte;
        cq = buf;
        remain = BLKSIZE - startByte;

        if(nbytes < remain)         //copying less bytes than there is room available
        {
            strcat(wbuf, buf);
            oftp->offset += nbytes;
            oftp->mptr->INODE.i_size+=nbytes;
            put_block(mip->dev, blk, wbuf);
            break;
        }
        //now otherwise we need to copy only the first part over
        strncpy(firstHalf, buf, remain);//now firstHalf has the first part of the message
        firstHalf[remain] = '\0';       //MAYBE MAYBE MAYBE
        strcat(wbuf, firstHalf);        //first half is added to the buf
        nbytes -= remain;
        memcpy(secondHalf, &buf[remain], nbytes);
        strncpy(buf, secondHalf, nbytes);
        buf[nbytes] = '\0';
        oftp->mptr->INODE.i_size+=remain;
        oftp->offset+=remain;           //oftp->offset should be a multiple of 1024 now
        startByte = 0;                  //for the next set of writing
        lbk++;                          //dealing with the next block

        put_block(mip->dev, blk, wbuf);
    }
}

int mymv(char * src, char * dest)
{
    int srcino,destino;
    int srcdev,destdev;

    if(strcmp(src, "") == 0 || strcmp(dest, "") == 0 || strcmp(src, dest) == 0)
    {
        printf("Error on inputs\n");
        return 0;
    }

    if(src[0] == '/')           //absolute
        srcdev = root->dev;
    else
        srcdev = running->cwd->dev;

    if(dest[0] == '/')           //absolute
        destdev = root->dev;
    else
        destdev = running->cwd->dev;

    srcino = getino(&srcdev,src);
    if(srcino == 0)
    {
        printf("Source file does not exist\n");
        return 0;
    }
    if(srcdev == destdev)
    {
        mylink(src, dest);
        unlink(src);
    }
    else
    {
        //Problem for level three
    }
}

int mycp(char * src, char * dest)
{
    int srcino,destino;
    int srcdev,destdev;
    MINODE * srcminode;
    int fd, gd;
    char buf[BLKSIZE];
    int n;

    if(src[0] == '/')           //absolute
        srcdev = root->dev;
    else
        srcdev = running->cwd->dev;

    if(dest[0] == '/')           //absolute
        destdev = root->dev;
    else
        destdev = running->cwd->dev;

    srcino = getino(&srcdev,src);
    if(srcino == 0)
    {
        printf("Source file does not exist\n");
        return 0;
    }
    
    fd = openFile(src, "r");

    destino = getino(&destdev,dest);
    if(destino == 0)
    {
        printf("Destination file does not exist yet, creating\n");
        mymkdirCreat(dest, "creat");
        printf("File created successfully\n");
    }
    destino = getino(&destdev, dest); //now it should exists

    gd = openFile(dest, "w");

    while(n = read_block(fd, buf, BLKSIZE))
    {
        mywrite(gd, buf, n);
    }
    closeFile(src);
    closeFile(dest); 
}

#endif