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
	MINODE *mip = malloc(sizeof(MINODE));
	char buf[20], fdbuf[2];

	ino = getino(&(running->cwd->dev), path);
	mip = iget(running->cwd->dev, ino);
	size = mip->INODE.i_size;

	fd = openFile(path, "r");
	snprintf(fdbuf, 2, "%d", fd);

	printf("fd = %d\n", fd);
	snprintf(buf, 20, "%d", size);
	printf("File size: %s\n", buf);

	myread(path, buf);

	closeFile(fdbuf);
	return 1;
}

#endif