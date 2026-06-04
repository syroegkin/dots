/*	$OpenBSD: fold.c,v 1.18 2016/05/23 10:31:42 schwarze Exp $	*/
/*	$NetBSD: fold.c,v 1.6 1995/09/01 01:42:44 jtc Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kevin Ruddy.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Port of OpenBSD fold.c to ZX Spectrum Next (z88dk).
 *
 * Reads file(s) named on the command line via esxdos and writes the folded
 * text to stdout. This is a single-byte port: the upstream UTF-8/locale
 * handling (wchar_t/mbtowc/wcwidth) is dropped, but the column semantics
 * (tab -> next multiple of 8, backspace, carriage return) and the -b/-s/-w
 * behaviour are preserved.
 */

#define __ZXNEXT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>

#define DEFWIDTH	80
#define MAXBUF		256		/* pending-line buffer */
#define MAXWIDTH	(MAXBUF - 1)	/* widest line we fold to */

static uint8_t bflag;	/* -b: count bytes, not display columns */
static uint8_t sflag;	/* -s: break at the last blank within the width */

/* Folding state: buf holds the current not-yet-emitted line. */
static char	 buf[MAXBUF];
static unsigned	 bi;	/* bytes buffered */
static unsigned	 ci;	/* scan position; col counts buf[0..ci) */
static unsigned	 col;	/* display column of buf[0..ci) */

static void out(const char *p, unsigned n)
{
	while (n--)
		putchar(*p++);
}

/*
 * Feed one input byte through the folder. On entry the buffer is always
 * drained (ci == bi); we append the byte and then process as far as we can,
 * emitting completed lines and breaking over-width ones.
 */
static void fold_byte(int c, unsigned max_width)
{
	char ch;
	unsigned w, brk, sp;

	if (bi == MAXBUF) {		/* pathological line: flush, keep col */
		out(buf, bi);
		bi = ci = 0;
	}
	buf[bi++] = (char)c;

	while (ci < bi) {
		ch = buf[ci];

		/* End of line (and, unless -b, a carriage return) flushes. */
		if (ch == '\n' || (ch == '\r' && !bflag)) {
			ci++;
			out(buf, ci);
			memmove(buf, buf + ci, bi - ci);
			bi -= ci;
			ci = 0;
			col = 0;
			continue;
		}
		if (ch == '\b' && !bflag) {
			if (col)
				col--;
			ci++;
			continue;
		}

		/* Measure display width and accept the char if it fits. */
		w = (ch == '\t' && !bflag) ? (8 - (col & 7)) : 1;
		col += w;
		if (col <= max_width || ci == 0) {
			ci++;
			continue;
		}

		/* Line break required before buf[ci]. */
		brk = ci;
		if (sflag) {
			for (sp = ci; sp > 0; sp--) {
				if (buf[sp - 1] == ' ') {
					brk = sp;
					break;
				}
			}
		}
		out(buf, brk);
		putchar('\n');
		memmove(buf, buf + brk, bi - brk);
		bi -= brk;
		ci = 0;
		col = 0;	/* remainder's column is recomputed as we rescan */
	}
}

static unsigned char fin = 0xff;

static int fold_file(char *name, unsigned max_width)
{
	static unsigned char chunk[512];
	uint16_t n, i;

	errno = 0;
	fin = esx_f_open(name, ESXDOS_MODE_R);
	if (errno) {
		printf("error opening file: %s\n", name);
		fin = 0xff;
		return 1;
	}

	bi = ci = col = 0;
	do {
		n = esx_f_read(fin, chunk, sizeof(chunk));
		for (i = 0; i < n; i++)
			fold_byte(chunk[i], max_width);
	} while (n > 0);

	if (bi)			/* flush the final, unterminated line */
		out(buf, bi);

	esx_f_close(fin);
	fin = 0xff;
	return 0;
}

static void usage(void)
{
	puts("usage: .fold [-b] [-s] [-w width] file ...");
}

static void help(void)
{
	puts(".fold - wrap each input line to fit in width columns\n");
	usage();
	puts("\n"
	     "DESCRIPTION\n"
	     "\n"
	     "-h, --help\n\tshow this help\n"
	     "\n"
	     "-b\n\tcount bytes rather than display columns (tab, backspace\n"
	     "\tand carriage return lose their special width).\n"
	     "\n"
	     "-s\n\tbreak lines at the last blank within the width.\n"
	     "\n"
	     "-w width\n\tuse width columns instead of 80 (1..255).\n"
	     "\n"
	     "This is a port of OpenBSD fold.c to ZXNext (single-byte; no UTF-8).");
}

int main(int argc, char *argv[])
{
	int i;
	unsigned width = DEFWIDTH;
	int rc = 0;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			help();
			return 0;
		}
		if (strcmp(argv[i], "-b") == 0) {
			bflag = 1;
			continue;
		}
		if (strcmp(argv[i], "-s") == 0) {
			sflag = 1;
			continue;
		}
		if (strcmp(argv[i], "-w") == 0) {
			if (++i >= argc) {
				usage();
				return 1;
			}
			width = (unsigned)atoi(argv[i]);
			if (width < 1 || width > MAXWIDTH) {
				printf("illegal width value: %s\n", argv[i]);
				return 1;
			}
			continue;
		}
		break;	/* first non-option is a filename */
	}

	if (i >= argc) {	/* no files given */
		usage();
		return 1;
	}

	for (; i < argc; i++)
		if (fold_file(argv[i], width))
			rc = 1;

	return rc;
}
