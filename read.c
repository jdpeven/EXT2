#ifndef READ_C
#define READ_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "iget_iput_getino.c"

int myread (char* filename, char* bytesToRead)
{
	MINODE *mip;
	int i, inoToFind = 0;

	inoToFind = getino(&(running->cwd->dev), filename);
	printf("we are looking for ino #%d\n", inoToFind);

	if (running->fd[0] == NULL)
	{
		printf("No fd's have been allocated, try opening a file...\n");
		return;
	}

	for (i = 0; i < 16; i++)
	{
		if (running->fd[i]->mptr->ino == inoToFind)
		{
			printf("going into fd %d\n", i);
			return;
		}
	}
}

#endif