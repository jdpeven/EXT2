#include "global.c"
#include "util.c"
#include "type.h"

int mymkdir(char * pathname)
{
    MINODE *parent = malloc(sizeof(MINODE));
    char * decomp[80];
    char newDirName[128];
    int size, i, parentIno = 0, childIno = 0;
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

    strcpy(pathSacrifice, pathname);                    //pathSacrifice = "/a/b/c"
    strcpy(shortPath, "");                              //Clears out shortpath
    decompose(pathname, decomp, &size, "/");            //decomp = ["a","b","c"]
    strcpy(newDirName, decomp[size-1]);                 //newDirName = "c"

    if(size == 1 && pathname[0] != '/' ){                                        //creating a dir relative to cwd
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
    }

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

    kmkdir(parent, newDirName);

    parent->dirty = 1;
    iput(parent);
    return 0;

}

kmkdir(MINODE * pmip, char * basename)
{
    int ino, bno, i;
    char * buf[BLKSIZE];
    char * cp;
    DIR *mdp = malloc(sizeof(DIR));

    MINODE * mip = malloc(sizeof(MINODE));
    DIR * newdp = malloc(sizeof(DIR));
    
    ino = ialloc(pmip->cwd->dev);
    bno = balloc(pmip->cwd->dev);
    mip = iget(pmip->cwd->dev, ino);

    INODE * ip = &mip->INODE;
    ip->i_mode = 0x41ED;
    ip->i_uid = running->uid;
    ip->i_gid = running->gid;
    ip->i_size = BLKSIZE;
    ip->i_links_count = 2;
    ip->i_atime = i_ctime = i_mtime = time(0L);            //THIS IS RELEVANT FOR TOUCH
    ip->i_blocks = 2;
    ip->i_block[0] = bno;
    for(i = 0; i < 15; i++)
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
    mdp->rec_len = 4*((8+strlen("..")+3)/4);

    putblock(pmip->cwd->dev, bno, buf);

    enterChild(pmip, ino, basename);
}

enterChild(MINODE * pmip, int ino, char * basename)
{
    int needLength;
    int parentBlockNum = 0;
    char bbuf[BLKSIZE];
    char * cp
    needLength = 4*((8+strlen(basename)+3)/4);
    //Now actually adding the dir to the block
    while(parent->INODE.i_block[parentBlockNum+1] != 0)             //finding last non-empty block
        parentBlockNum++;
    printf("First available block is at index [%d] with Block # [%d]\n",parentBlockNum, parent->INODE.i_block[parentBlockNum]);

    getblock(parent->dev, parent->INODE.i_block[parentBlockNum],bbuf);

    dp = (DIR *)bbuf;
    cp = bbuf;

    if(dp->inode != 0)                              //not the first entry in that dirblock
    {
        printf("Step to last data entry in block: [%d]\n", parent->INODE.i_block[parentBlockNum]);

        while(cp + dp->rec_len < bbuf[BLKSIZE])
        {
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
                    break;
                    //TODO
                    //that
                }
                //otherwise it will fit into this block
                remainLength = dp->rec_len - (4*((8+dp->name_len+3)/4));
                dp->rec_len = (4*((8+dp->name_len+3)/4));
                break;
            }
        }
    }
    //else, first entry in datablock, dp is already pointing to the right location
    strcpy(dp->name, basename);
    dp->inode = ino;
    dp->rec_len = remainLength;
    dp->name_len = strlen(basename);
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