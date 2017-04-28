#ifndef CHMOD_C
#define CHMOD_C

#include "global.c"
#include "util.c"
#include "type.h"

int mychmod(char * type, char * path)
{
	char filepath[64];
	int newmode = 0;
	int ino, dev;
	int user, group, all;
	MINODE * mip;// = malloc(sizeof(MINODE));

	if(strcmp(type, "") == 0 || strcmp(path, "") == 0)
	{
		printf("Invalid number argument's provided\n");
		return 0;
	}
	if(strlen(type) != 4)
	{
		printf("Invalid type, must be 4 digits\n");
		return 0;
	}

	user = type[1] - '0';
	group = type[2] - '0';
	all = type[3] - '0';
	printf("User = %d, Group = %d, All = %d\n", user, group, all);
	
	if(user > 7 || group > 7 || all > 7)
	{
		printf("Invalid input, each digit must be < 8\n");
	}

	if (path[0] == '/')
	{
		//we have an absolute path
		dev = root->dev;
		ino = getino(&dev, path);
		mip = iget(root->dev, ino);
	}
	else
	{
		//relative path (use cwd)
		dev = running->cwd->dev;
		ino = getino(&dev, path);
		mip = iget(running->cwd->dev, ino);
	}

	if (ino ==0)
	{
		printf("File not found\n");
		return 0;
	}
	printf("File INO = %d\n", ino);
	printf("ip->mode = %d", mip->INODE.i_mode);

	newmode = mip->INODE.i_mode & ((-1)<<9);	//if this works it is the single greatest line of code I've every written
	newmode |= (user << 6);
	newmode |= (group << 3);
	newmode |= (all);
	/*
		So I realized that when just doing the above three lines, it looked good but
		when I tried to open the file it would break because it wasn't a dir anymore
		I'm not sure where but I'm guessing the dir/reg bits are further down than the
		one's for permissions. So I originally set newmode as the original mode and
		negative one bit shifted over 9 times. -1 in two's compement is all ones, and
		then bit shifting it over 9 times puts 9 0s before the 1s. So anything to the left
		of the permission bits is fine, whereas everything else is cleared out. Then after 
		that I just bit shift the permissions to the correct location and we're good to go
	*/

	printf("New mode is now %d\n", newmode);

	printf("Changing the mode of <%s> to \"%d\"\n", path, newmode);

	mip->INODE.i_mode = newmode;
	mip->INODE.i_atime = time(0L);
    mip->INODE.i_ctime = time(0L);
    mip->dirty = 1;
	iput(mip);
}

#endif