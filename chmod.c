#ifndef CHMOD_C
#define CHMOD_C

#include "global.c"
#include "util.c"
#include "type.h"

int mychmod(char * path, char * path2)
{
	char filepath[64];
	int newMode = 0;
	int ino, dev;
	MINODE * mip = malloc(sizeof(MINODE));


	strcpy(filepath, path);
	if (strlen(path2) == 0)
	{
		newMode = 0;
	}
	else
	{
		newMode = atoi(path2);
	}

	if (strlen(filepath) == 0)
	{
		printf("No filename provided");
		return 0;
	}

	printf("Changing the mode of <%s> to \"%d\"", filepath, newMode);

	if (filepath[0] == '/')
	{
		//we have an absolute path
		dev = root->dev;
		ino = getino(&dev, filepath);
		mip = iget(root->dev, ino);
	}
	else
	{
		//relative path (use cwd)
		dev = running->cwd->dev;
		ino = getino(&dev, filepath);
		mip = iget(running->cwd->dev, ino);
	}

	if (ino ==0)
	{
		printf("File not found\n");
		return 0;
	}
	printf("File INO = %d\n", ino);
	printf("ip->mode = %d", mip->INODE.i_mode);

	mip->INODE.i_mode = newMode;
	mip->INODE.i_atime = time(0L);
    mip->INODE.i_ctime = time(0L);
    mip->dirty = 1;
	iput(mip);
}

#endif