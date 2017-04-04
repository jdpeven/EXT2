#ifndef UTIL_C
#define UTIL_C

#include "global.c"
#include "type.h"
#include "iget_iput_getino.c"

/*
Name: 
Args:  -  -
       -  -
Return: -
Description: 
SampleRun:
*/

/*
Name: decompose
Args: input - char* - for input to be decomposed
      output - char** - string array to be returned
      count - int * - number of elements in output
      delimeter - char * - what you want to split on
Return = void (all done with pointers)
Description: Will split a string based on a delimeter
        Useful for decomposing a path /a/b/c -> [a, b, c]
Sample run:
    path = "/a/b/c"
    decompose(path, pathArr, &count, "/");
    pathArr = ["a", "b", "c"]
    count = 3
*/

void decompose(char * input, char ** output, int * count, char * delimeter)
{
    char * token;
    //char * output[80];
    int i;
    token = strtok(input, delimeter);
    i = 0;
    while(token != NULL)
    {
        output[i] = token;
        token = strtok(NULL, delimeter);
        i++;
    }
    *count = i;
    output[i] = NULL;
    printf("%s decomposed into: [", input);
    for(i = 0; i < *count; i++){
        printf("%s][", output[i]);
    }
    printf("'/0']\n");
}


/*
Name: search
Args: mip - MINODE * - the current MINODE
      name - char * - the name we're looking for in that MINODE
Return: int - found inode value
Description: will search through the given inode for the name, and return the names inode
SampleRun: int ino = search(cwd, "X");
*/
int search(MINODE *mip, char *name)
{
    char* cp;
    int block0;
    char dbuf[BLKSIZE], sbuf[BLKSIZE];
    DIR* dp;
    int i;
    int ino = 0;

    //printf("Searching for %s in dir %s", name, mip->INODE.name);

    for(i = 0; i < 12; i++){            //might be deep in the file
        block0 = mip->INODE.i_block[i];
        get_block(mip->dev, block0, dbuf);
        dp = (DIR*)dbuf;
        cp = dbuf;
        while(cp < &dbuf[BLKSIZE]){
            strncpy(sbuf, dp->name, dp->name_len);                  //similar to strcpy but will stop based on third argument
            sbuf[dp->name_len] = 0;
            printf("%4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, sbuf);
            if(strcmp(name, sbuf) == 0){
                ino = dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        if(ino){
            printf("Found '%s' with Ino [%d]\n", name, ino);
        }
        else
            printf("Did not find '%s'\n", name);
        return ino;
    }
}

/*
Name: get_block
Args: fd - int - The open file descriptor
      blk - int - The block number that you want
      buf - char[] - What you're reading the block into
Return: int - Maybe just an error code.
Description: Will get the block (blk) and read it into buf.
SampleRun: get_block(dev, 2, buf)
            reads the GD into buf.
*/

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);  
}

/*
Name: put_block
Args: fd - int - The open file descriptor
      blk - int - The block number that you want to write to
      buf - char[] - What you're writing to the block
Return: int - Maybe just an error code.
Description: Will get the block (blk) and write buf to it.
SampleRun: put_block(dev, 2, buf)
            write buf into GD.
*/

int put_block(int fd, int blk, char buf[ ])
{
    lseek(fd, (long)blk*BLKSIZE, 0);
    write(fd, buf, BLKSIZE);
}


/*
Name: findCmd
Args: command - char* - the command we're looking for
Return: int - the index of the command for the function pointer array
Description: Will fix the if() else() or switch statement
SampleRun: cmd = findCmd("ls");
*/

int findCmd(char * command)
{
    int i = 0;
    while(cmd[i]){
        if (strcmp(command, cmd[i]) == 0)
            return i;
        i++;
    }
    return -1;
}

void copyMinodeptr(MINODE * src, MINODE **dest)
{
    memcpy(&(*(*dest)).INODE, &(*src).INODE, sizeof(INODE));
    memcpy(&(*(*dest)).dev, &(*src).dev, sizeof(int));
    memcpy(&(*(*dest)).ino, &(*src).ino, sizeof(int));
    memcpy(&(*(*dest)).refCount, &(*src).refCount, sizeof(int));
    memcpy(&(*(*dest)).dirty, &(*src).dirty, sizeof(int));
    memcpy(&(*(*dest)).mounted, &(*src).mounted, sizeof(int));
   // memcpy(*dest->);

    /*typedef struct minode{
    INODE INODE;                         
    int dev, ino;
    int refCount;                         //initialized to 0
    int dirty;
    int mounted;
    struct mntable *mptr;
    }MINODE;*/
}

void printMinode(MINODE* mip)
{
    printf("dev = %d ", mip->dev);
    printf("ino = %d ",mip->ino);
    printf("refCount = %d ",mip->refCount);
    printf("dirty = %d ",mip->dirty);
    printf("mounted = %d\n",mip->mounted);
}

char * inoToName(MINODE*mip, int childIno, char **childname)
{
    char* cp;
    int block0;
    char dbuf[BLKSIZE], sbuf[BLKSIZE];
    DIR* dp;
    int i;
    int ino = 0;

    //printf("Searching for %s in dir %s", name, mip->INODE.name);

    for(i = 0; i < 12; i++){            //might be deep in the file
        block0 = mip->INODE.i_block[i];
        get_block(mip->dev, block0, dbuf);
        dp = (DIR*)dbuf;
        cp = dbuf;
        while(cp < &dbuf[BLKSIZE]){
            strncpy(sbuf, dp->name, dp->name_len);                  //similar to strcpy but will stop based on third argument
            sbuf[dp->name_len] = 0;
            //printf("%4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, sbuf);
            if(dp->inode == childIno){
                strncpy(*childname, dp->name, dp->name_len);
                //strcat(*childname, "\n");
                //*childname[dp->name_len] = 0;
                return;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        /*if(ino){
            printf("Found '%s' with Ino [%d]\n", name, ino);
        }
        else
            printf("Did not find '%s'", name);
        return ino;*/
    }
}


int tst_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;  j = bit % 8;
  if (buf[i] & (1 << j))
     return 1;
  return 0;
}

int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit/8; j=bit%8;
  buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit/8; j=bit%8;
  buf[i] &= ~(1 << j);
}

int decFreeInodes(int dev)
{
  char buff[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buff);
  sp = (SUPER *)buff;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buff);

  get_block(dev, 2, buff);
  gp = (GD *)buff;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buff);
}

int incFreeInodes(int dev)
{
  char buff[BLKSIZE];

  get_block(dev, 1, buff);
  sp = (SUPER *)buff;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buff);

  get_block(dev, 2, buff);
  gp = (GD *)buff;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buff);
}

int decFreeBlocks(int dev)
{
  char buff[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buff);
  sp = (SUPER *)buff;
  sp->s_free_blocks_count--;
  put_block(dev, 1, buff);

  get_block(dev, 2, buff);
  gp = (GD *)buff;
  gp->bg_free_blocks_count--;
  put_block(dev, 2, buff);
}

int incFreeBlocks(int dev)
{
  char buff[BLKSIZE];

  get_block(dev, 1, buff);
  sp = (SUPER *)buff;
  sp->s_free_blocks_count++;
  put_block(dev, 1, buff);

  get_block(dev, 2, buff);
  gp = (GD *)buff;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, buff);
}


int nameToIno(MINODE * mip, char * name)
{
    char* cp;
    int block0;
    char dbuf[BLKSIZE], sbuf[BLKSIZE];
    DIR* dp;
    int i;
    int ino = 0;

    //printf("Searching for %s in dir %s", name, mip->INODE.name);

    for(i = 0; i < 12; i++){            //might be deep in the file
        block0 = mip->INODE.i_block[i];
        get_block(mip->dev, block0, dbuf);
        dp = (DIR*)dbuf;
        cp = dbuf;
        while(cp < &dbuf[BLKSIZE]){
            strncpy(sbuf, dp->name, dp->name_len);                  //similar to strcpy but will stop based on third argument
            sbuf[dp->name_len] = 0;
            //printf("%4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, sbuf);
            if(strcmp(sbuf, name) == 0){
                return dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
}

#endif