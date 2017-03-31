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



    iput(parent);
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