#include "global.c"
#include "util.c"
#include "allocate_deallocate.c"
#include "type.h"
#include "ls.c"

childls(MINODE * mip)
{
    char * cp;
    char dbuf[1024], sbuf[1024];
    int block0;
    DIR *ddp;

    printf("Printing file details for the newly allocated directory\n");
    //ADD OTHER DETAILS

    block0 = mip->INODE.i_block[0];
    get_block(mip->dev, block0, dbuf);
    ddp = (DIR*)dbuf;
    cp = dbuf;
    while (cp < &dbuf[BLKSIZE])
    {
        //printPermissions(dp->inode); NOT WORKING idk why.
        strncpy(sbuf, ddp->name, ddp->name_len);
        sbuf[ddp->name_len] = 0;
        ip = iget(dev, ddp->inode);

        printPermissions(ddp->inode);
        printf("   %d   %.13s\t%d\t%s\n", ip->i_links_count, 
            ctime(&ip->i_ctime), ip->i_size, sbuf);
        cp += ddp->rec_len;
        ddp = (DIR*)cp;
        iput(ip);
    }
}

enterChild(MINODE * pmip, int ino, char * basename)
{
    int needLength, remainLength;
    int parentBlockNum = 0;
    char bbuf[BLKSIZE], sbuf[BLKSIZE];
    char * cp;

    printf("Starting enterChild with name %s\n", basename);

    needLength = 4*((8+strlen(basename)+3)/4);
    //Now actually adding the dir to the block
    while(pmip->INODE.i_block[parentBlockNum+1] != 0)             //finding last non-empty block
        parentBlockNum++;
    printf("First available block is at index [%d] with Block # [%d]\n",parentBlockNum, pmip->INODE.i_block[parentBlockNum]);

    get_block(pmip->dev, pmip->INODE.i_block[parentBlockNum],bbuf);

    dp = (DIR *)bbuf;
    cp = bbuf;

    if(dp->inode != 0)                              //not the first entry in that dirblock
    {
        printf("Step to last data entry in block: [%d]\n", pmip->INODE.i_block[parentBlockNum]);

        while(cp < &bbuf[BLKSIZE])
        {
            strncpy(sbuf, dp->name, dp->name_len);                  //similar to strcpy but will stop based on third argument
            sbuf[dp->name_len] = 0;
            printf("%4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, sbuf);//printing info
            if(dp->rec_len == 4*((8+dp->name_len+3)/4))             //normal entries
            {
                cp+=dp->rec_len;
                dp = (DIR *)cp;
            }
            else                                        //we've reached the end
            {
                if((4*((8+dp->name_len+3)/4)) + needLength > dp->rec_len)   //if the actual size of the current last inode is greater than
                                                                            //what it needs AND our new file, then we need to allocate a new
                                                                            //dirblock
                {
                    printf("Need to allocate new dir block\n");
                    parentBlockNum++;
                    pmip->INODE.i_block[parentBlockNum] = balloc();
                    pmip->INODE.i_size += 1024;
                    pmip->INODE.i_blocks += 2;
                    get_block(pmip->dev,pmip->INODE.i_block[parentBlockNum], bbuf);
                    dp = (DIR *)bbuf;
                    break;
                }
                //otherwise it will fit into this block
                remainLength = dp->rec_len - (4*((8+dp->name_len+3)/4));
                dp->rec_len = (4*((8+dp->name_len+3)/4));
                
                cp+=dp->rec_len;        //getting ready to add the new entry
                dp = (DIR *)cp;
                break;
            }
            if(cp + dp->rec_len > &bbuf[BLKSIZE])
            {
                printf("Need to allocate new block\n");
                parentBlockNum++;
                pmip->INODE.i_block[parentBlockNum] = balloc();
                pmip->INODE.i_size += 1024;
                pmip->INODE.i_blocks += 2;
                get_block(pmip->dev,pmip->INODE.i_block[parentBlockNum], bbuf);
                dp = (DIR *)bbuf;
                break;
            }
        }
    }
    //else, first entry in datablock, dp is already pointing to the right location
    strcpy(dp->name, basename);
    dp->inode = ino;
    dp->rec_len = remainLength;
    dp->name_len = strlen(basename);
    put_block(pmip->dev, pmip->INODE.i_block[parentBlockNum],bbuf);
}

kmkdir(MINODE * pmip, char * basename)
{
    int ino, bno, i;
    char * buf[BLKSIZE];
    char * cp;
    DIR *mdp = malloc(sizeof(DIR));

    MINODE * mip = malloc(sizeof(MINODE));
    DIR * newdp = malloc(sizeof(DIR));
    
    printf("Entering kmkdir\n");

    ino = ialloc(pmip->dev);
    bno = balloc(pmip->dev);
    mip = iget(pmip->dev, ino);

    INODE * ip = &mip->INODE;
    ip->i_mode = 0x41ED;
    ip->i_uid = running->uid;
    ip->i_gid = running->pid;                               //MAYBE WRONG
    ip->i_size = BLKSIZE;
    ip->i_links_count = 2;
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);            //THIS IS RELEVANT FOR TOUCH
    ip->i_blocks = 2;                           //for some reason counts every 512 bytes
    ip->i_block[0] = bno;
    for(i = 1; i < 15; i++)
        ip->i_block[i] = 0;
    mip->dirty = 1;
    iput(mip);

    cp = &buf;
    mdp = (DIR * ) buf;
    strcpy(mdp->name, ".");
    mdp->inode = ino;
    mdp->name_len = 1;
    mdp->rec_len = 4*((8+strlen(".")+3)/4);
    
    cp += mdp->rec_len;
    mdp = (DIR *)cp;
    strcpy(mdp->name, "..");
    mdp->inode = pmip->ino;
    mdp->name_len = 2;
    mdp->rec_len = BLKSIZE - 4*((8+strlen(".")+3)/4);           //whatever is left over after the "."
    
    put_block(pmip->dev, bno, buf);

    childls(mip);

    enterChild(pmip, ino, basename);
}

int mymkdir(char * pathname)
{
    MINODE *parent = malloc(sizeof(MINODE));
    char * parentPath = malloc(sizeof(char)*128); 
    char * childName = malloc(sizeof(char)*128);

    //char * decomp[80];
    //char newDirName[128];
    int size, i, parentIno = 0, childIno = 0;
    int dev;
    char shortPath[128];
    char pathSacrifice[128];                   //making a copy of pathname which will be detroyed in "decompose"
    int needLength = 0;
    int parentBlockNum;
    char bbuf[BLKSIZE];             //block buf
    char * cp;
    int remainLength;               //for the new length remaining in a dir block

    if(strcmp(pathname, "")==0){
        printf("Filename not provided, returning -1\n");
        return -1;
    }
    if(pathname[0] == '/')
        dev = root->dev;
    else
        dev = running->cwd->dev;
    

    //strcpy(pathSacrifice, pathname);                    //pathSacrifice = "/a/b/c"
    //strcpy(shortPath, "");                              //Clears out shortpath
    //decompose(pathname, decomp, &size, "/");            //decomp = ["a","b","c"]
    //strcpy(newDirName, decomp[size-1]);                 //newDirName = "c"

    strcpy(pathSacrifice, pathname); 
    parentPath = dirname(pathSacrifice);
    strcpy(pathSacrifice, pathname); 
    childName = basename(pathSacrifice);

    printf("DirName = %s, Pathname = %s\n", parentPath, childName);

    /*if(size == 1 && pathname[0] != '/' ){                                        //creating a dir relative to cwd
        parent = iget(running->cwd->dev, running->cwd->ino);
    }
    else{                                               //Not relative to cwd
        if(pathname[0] == '/'){                          //Absolute pathname, would have been removed in decompose
            strcat(shortPath, "/"); 
        }                    
        for(i = 0; i < size - 1; i++)                   //Recreates a path with all but the last element
        {
            strcat(shortPath, decomp[i]);
            strcat(shortPath, "/");
        }                                               //shortPath = "/a/b/". I don't think the trailing "/" will matter
        parentIno = getino(&(running->cwd->dev), shortPath);
        
        if(parentIno <= 0){
            printf("Parent directory not found\n");
            return -1;
        }
        parent = iget(running->cwd->dev, parentIno);
    }*/

    if(S_ISREG(parent->INODE.i_mode)){                      
        printf("Cannot mkdir in non-dir file\n");
        return -1;
    }

    childIno = search(parent, newDirName);                  //sees if it already exists
    if(childIno != 0){
        printf("A file/directory of this name already exists, cannot create new one\n");
        return -1;
    }

    printf("Ready to make directory [%s], Parent path [%s]\n", newDirName, shortPath);
    printf("Parent MINODE loaded into 'parent'\n");

    printf("Printing parents block information\n");
    printBlocks(&(parent->INODE));

    kmkdir(parent, newDirName);

    return 0;
}

/************* Algorithm of Insert_dir_entry ***************** 
(1). need_len = 4*((8+name_len+3)/4); // new entry need length 
(2). for each existing data block do {
if (block has only one entry with inode number==0)
   enter new entry as first entry in block;
else{
(3) go to last entry in block;
ideal_len = 4*((8+last_entry’s name_len+3)/4); remain = last entry's rec_len - ideal_len;
if (remain >= need_len){
trim last entry's rec_len to ideal_len;
} enter new entry as last entry with rec_len = remain;
   else{
(4)   allocate a new data block;
      enter new entry as first entry in the data block;
}  }  increase DIR's size by BLKSSIZE;
     }  write block to disk;
(5). mark DIR's minode modified for write back;*/