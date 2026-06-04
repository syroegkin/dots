/*
 * Host shim for the fold command's esxdos file calls.
 *
 * On the ZX Spectrum Next, fold reads input files through esxdos
 * (esx_f_open/esx_f_read/esx_f_close). For host tests we back those with
 * stdio, mapping the small integer "file handle" to a FILE*. Linked into the
 * host build of fold/main.c by test/run.sh (the generic test/shim/<cmd>.c hook).
 */

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define MAXFD 8

static FILE *fdtab[MAXFD];

unsigned char esx_f_open(char *filename, unsigned char mode)
{
	int i;

	(void)mode;
	for (i = 1; i < MAXFD; i++)
		if (fdtab[i] == NULL)
			break;
	if (i == MAXFD) {
		errno = EMFILE;
		return 0xff;
	}
	fdtab[i] = fopen(filename, "rb");
	if (fdtab[i] == NULL)
		return 0xff;	/* errno set by fopen */
	return (unsigned char)i;
}

uint16_t esx_f_read(unsigned char handle, void *buffer, uint16_t bytes)
{
	if (handle >= MAXFD || fdtab[handle] == NULL)
		return 0;
	return (uint16_t)fread(buffer, 1, bytes, fdtab[handle]);
}

unsigned char esx_f_close(unsigned char handle)
{
	if (handle < MAXFD && fdtab[handle] != NULL) {
		fclose(fdtab[handle]);
		fdtab[handle] = NULL;
	}
	return 0;
}
