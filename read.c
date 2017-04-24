#ifndef READ_C
#define READ_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "iget_iput_getino.c"

int myread (char* filename, char* bytesToRead)
{
	MINODE *mip;
	int i, inoToFind = 0, curfd = -1, bytesRead;
	char *buf;
	int btoread = atoi(bytesToRead);

	strcpy(buf, "");
	inoToFind = getino(&(running->cwd->dev), filename);

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
			//printf("Reading from fd %d\n", i);
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

	printf("~~~~~~~~~~~~~~TEXT~~~~~~~~~~~~~\n");
	bytesRead = read_block(curfd, buf, btoread);
	printf("~~~~~~~~~~~~~~/TXT~~~~~~~~~~~~~\n");
	printf("~~~~~~~~~~~~~~INFO~~~~~~~~~~~~~\n");
	printf("BYTES READ %d\n", bytesRead);
	printf("~~~~~~~~~~~~~~/NFO~~~~~~~~~~~~~\n");
	
	//I DONT KNOW WHY but it crashes if I remove this while...
	i = 0;
	while (mip->INODE.i_block[i] != 0)
	{
		i++;
	}
	return 0;
}


int read_block(int fd, char *buf, int nbytes)
{
	int offset, mode, refCount, avil, lbk, startByte, blk, remain, bytesRead = 0;
	char *cq, readbuf[BLKSIZE], *cp;
	int indirectOffset, doubleOffset;
	char directbuf[BLKSIZE], doublebuf[BLKSIZE];
	int *intp;

	cq = buf;
	offset = running->fd[fd]->offset;
	mode = running->fd[fd]->mode;
	refCount = running->fd[fd]->refCount;
	avil = running->fd[fd]->mptr->INODE.i_size - offset;

	while (nbytes && avil)
	{
		lbk = offset / BLKSIZE;
		startByte = offset % BLKSIZE;

		if (lbk < 12)
		{
			blk = running->fd[fd]->mptr->INODE.i_block[lbk];
		}
		else if (lbk >= 12 && lbk < 256 + 12)
		{
			indirectOffset = lbk - 12;
			get_block(running->fd[fd]->mptr->dev,
				running->fd[fd]->mptr->INODE.i_block[12], directbuf);
			intp = &directbuf;
			intp += indirectOffset;
			blk = *intp;
		}
		else
		{
			doubleOffset = (lbk - 12 - 256) / 256;
			indirectOffset = (lbk - 12 - 256) % 256;
			get_block(running->fd[fd]->mptr->dev,
				running->fd[fd]->mptr->INODE.i_block[13], doublebuf);
			intp = &doublebuf;
			intp += doubleOffset;
			get_block(running->fd[fd]->mptr->dev, *intp, directbuf);
			intp = &directbuf;
			intp += indirectOffset;
			blk = *intp;
		}

		//printf("bock: %d\n%d\n", blk, running->fd[fd]->mptr->INODE.i_block[12]);
		get_block(running->fd[fd]->mptr->dev, blk, readbuf);

		cp = readbuf + startByte;
		remain = BLKSIZE - startByte;
		while (remain > 0)
		{
			*cq++ = *cp++;
			running->fd[fd]->offset++;
			bytesRead++;
			avil--;
			nbytes--;
			remain--;
			if (nbytes <= 0 || avil <= 0)
			{
				*cq++ = 0;
				break;
			}
		}

		printf("%s", buf);
		strcpy(buf, "");
		cq = buf;
		//printf("emtpy??? %s\n", buf);
		offset += BLKSIZE;
	}
	
	*cq++ = 0;
	printf("\n");
	return bytesRead;
}

#endif
























/*#ifndef READ_C
#define READ_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "iget_iput_getino.c"

int myread (char* filename, char* bytesToRead)
{
	MINODE *mip;

	int i, inoToFind = 0, curfd = -1, bytesRead;
	char *buf;

	int btoread = atoi(bytesToRead);

	strcpy(buf, "");
	inoToFind = getino(&(running->cwd->dev), filename);
	//printf("we are looking for ino #%d\n", inoToFind);

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
			//printf("Reading from fd %d\n", i);
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

	bytesRead = read_block(curfd, buf, btoread);
	printf("%s\n", buf);
	printf("~~~~~~~~~~~~~~/TXT~~~~~~~~~~~~~\n");
	printf("%d BYTES READ %d\n", i, bytesRead);
	
	//I DONT KNOW WHY but it crashes if I remove this while...
	i = 0;
	while (mip->INODE.i_block[i] != 0)
	{
		i++;
	}
}


int read_block(int fd, char *buf, int nbytes)
{
	int offset, mode, refCount, avil, lbk, startByte, blk, remain, bytesRead = 0;
	char *cq, readbuf[BLKSIZE], *cp;
	int indirectOffset, doubleOffset;
	char directbuf[BLKSIZE], doublebuf[BLKSIZE];
	int *intp;

	//printf("entered read_block\n");

	cq = buf;
	offset = running->fd[fd]->offset;
	mode = running->fd[fd]->mode;
	refCount = running->fd[fd]->refCount;
	avil = running->fd[fd]->mptr->INODE.i_size - offset;

	//printf("offset: %d\nmode: %d\nrefCount: %d\navil: %d\nnbytes: %d\n", offset, mode, refCount, avil, nbytes);

	lbk = offset / BLKSIZE;
	startByte = offset % BLKSIZE;

	while (nbytes && avil)
	{
		if (lbk < 12)
		{
			printf("> reading from a DIRECT block: %d\n", lbk);
			blk = running->fd[fd]->mptr->INODE.i_block[lbk];
		}
		else if (lbk >= 12 && lbk < 256 + 12)
		{
			printf("> reading from a INDIRECT block\n");

			//indirect
		}
		else
		{
			printf("> reading from a 2xINDIRECT block\n");
			//2x indirect
		}

		get_block(running->fd[fd]->mptr->dev, blk, readbuf);

		cp = readbuf + startByte;
		remain = BLKSIZE - startByte;
		while (remain > 0)
		{
			*cq++ = *cp++;
			running->fd[fd]->offset++;
			bytesRead++;
			avil--;
			nbytes--;
			remain--;
			if (nbytes <= 0 || avil <= 0)
			{
				break;
			}
		}
		lbk++;
	}
	printf("~~~~~~~~~~~~~~INFO~~~~~~~~~~~~~\n");
	printf("read_block: %d bytes | fd: %d\n", bytesRead, fd);
	printf("~~~~~~~~~~~~~~TEXT~~~~~~~~~~~~~\n");
	
	*cq++ = 0;
	return bytesRead;
}

#endif*/


//working on indirect stuff idk what he really wants here. Need to ask jackson
/*#ifndef READ_C
#define READ_C

#include "global.c"
#include "util.c"
#include "type.h"
#include "iget_iput_getino.c"

int myread (char* filename, char* bytesToRead)
{
	MINODE *mip;

	int i, inoToFind = 0, curfd = -1, bytesRead;
	char *buf;

	int btoread = atoi(bytesToRead);

	strcpy(buf, "");
	inoToFind = getino(&(running->cwd->dev), filename);
	//printf("we are looking for ino #%d\n", inoToFind);

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
			//printf("Reading from fd %d\n", i);
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

	bytesRead = read_block(curfd, buf, btoread);
	printf("%s\n", buf);
	printf("~~~~~~~~~~~~~~/TXT~~~~~~~~~~~~~\n");
	printf("%d BYTES READ %d\n", i, bytesRead);
	
	//I DONT KNOW WHY but it crashes if I remove this while...
	i = 0;
	while (mip->INODE.i_block[i] != 0)
	{
		i++;
	}
}


int read_block(int fd, char *buf, int nbytes)
{
	int offset, mode, refCount, avil, lbk, startByte, blk, remain, bytesRead = 0;
	char *cq = (char*) malloc(BLKSIZE), readbuf[BLKSIZE], *cp = (char*) malloc(BLKSIZE);
	strcpy(cp, "");
	strcpy(cq, "");
	int i = 0;
	//printf("entered read_block\n");

	cq = buf;
	offset = running->fd[fd]->offset;
	mode = running->fd[fd]->mode;
	refCount = running->fd[fd]->refCount;
	avil = running->fd[fd]->mptr->INODE.i_size - offset;

	//printf("offset: %d\nmode: %d\nrefCount: %d\navil: %d\nnbytes: %d\n", offset, mode, refCount, avil, nbytes);

	lbk = offset / BLKSIZE;
	startByte = offset % BLKSIZE;

	while (nbytes && avil)
	{
		if (lbk < 12)
		{
			printf("> reading from a DIRECT block: %d\n", lbk);
			blk = running->fd[fd]->mptr->INODE.i_block[lbk];
		}
		else if (lbk >= 12 && lbk < 256 + 12)
		{
			printf("> reading from a INDIRECT block\n");
			//indirect
		}
		else
		{
			printf("> reading from a 2xINDIRECT block\n");
			//2x indirect
		}

		get_block(running->fd[fd]->mptr->dev, blk, readbuf);

		//cp = readbuf + startByte;

		strcpy(cp, readbuf);
		remain = BLKSIZE - startByte;
		while (remain > 0)
		{
			//printf("i: %d avil: %d nbytes: %d remain: %d\n", bytesRead, avil, nbytes, remain);
			//printf("startByte: %d, cplen = %d, cqlen = %d", startByte, strlen(cp), strlen(cq));
			*cq++ = *cp++;
			//printf("\tcheck\n");
			running->fd[fd]->offset++;
			bytesRead++;
			avil--;
			nbytes--;
			remain--;
			if (bytesRead % BLKSIZE == 0)	//char* is at its limit! need to flush
			{
				printf("\n%s", buf); //buf is not getting reset.
				cq[0] = '\0';
				buf[0] = '\0';
				printf("\nWWWWWWWWWWWWWWWWWWW\nWWWWWWWW\nWWWWWWWWWWW\nWWWWWWww\n%s\n", buf);
			}
			if (nbytes <= 0 || avil <= 0)
			{
				break;
			}
			i = 0;
		}
		lbk++;
	}
	
	*cq++ = 0;
	return bytesRead;
}

#endif*/