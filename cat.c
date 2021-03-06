#ifndef CAT_C
#define CAT_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "time.h"
#include "open_close_lseek.c"
#include "read.c"


int mycat (char* path)
{
	int ino, size, fd;
	MINODE *mip;// = malloc(sizeof(MINODE));
	char buf[20], fdbuf[2], *nice;
	char readbuf[BLKSIZE];
	int n, total = 0;

	if(strcmp(path, "") == 0)
	{
		printf("Filename not provided\n");
		return 0;
	}

	ino = getino(&(running->cwd->dev), path);
	if(ino == 0)
	{
		printf("Invalid path\n");
		return 0;
	}
	mip = iget(running->cwd->dev, ino);
	size = mip->INODE.i_size;

	fd = openFile(path, "r");
	snprintf(fdbuf, 2, "%d", fd);
	strcpy(nice, fdbuf);	//fdbuf is losing its info idk why so I had to cpy it.

	printf("========================================\n");
	while(n = read_block(fd, readbuf, BLKSIZE))
    {
        printf("%s", readbuf);
		total+=n;
    }
	printf("\n========================================\n");
	printf("Printed %d bytes\n", total);
	
	closeFile(nice);
	return 1;
}

#endif