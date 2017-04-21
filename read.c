#ifndef READ_C
#define READ_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "iget_iput_getino.c"

int myread (char* filename, char* bytesToRead)
{
	MINODE *mip;
	int i, inoToFind = 0, curfd = -1;
	char buf[BLKSIZE];
	int btoread = atoi(bytesToRead);

	strcpy(buf, "");
	inoToFind = getino(&(running->cwd->dev), filename);
	printf("we are looking for ino #%d\n", inoToFind);

	if (running->fd[0] == NULL)
	{
		printf("No fd's have been allocated, try opening a file...\n");
		return;
	}

	i = 0;
	while (i < 16)
	{
		if (running->fd[i]->mptr->ino == inoToFind)
		{
			printf("Reading from fd %d\n", i);
			mip = running->fd[i]->mptr;
			curfd = i;
			break;
		}
		i++;
	}
	if (curfd == -1)
	{
		printf("No fd was found with the name [%s], try opening that file\n", filename);
	}

	printf("i_blocks: %d\n", mip->INODE.i_blocks);
	i = 0;
	while (mip->INODE.i_block[i] != 0)
	{
		read_block(mip->dev, mip->INODE.i_block[i], buf, btoread);
		printf("Read %d bytes\n%s\n", btoread, buf);
		buf[0] = 0;
		i++;
	}
}

int read_block(int fd, char buf[ ], int size)
{

}

#endif