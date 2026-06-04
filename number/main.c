/*	$OpenBSD: number.c,v 1.20 2016/03/07 12:07:56 mestre Exp $	*/

/*
 * Copyright (c) 1988, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
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
 * Port of OpenBSD number.c to ZX Spectrum Next (z88dk).
 * Reads numbers from argv only; scientific-notation (e/E) input is not
 * supported in this port.
 */

#define __ZXNEXT 1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/zxn.h>

#define	MAXNUM		65		/* Biggest number we handle. */

static const char	*const name1[] = {
	"",		"one",		"two",		"three",
	"four",		"five",		"six",		"seven",
	"eight",	"nine",		"ten",		"eleven",
	"twelve",	"thirteen",	"fourteen",	"fifteen",
	"sixteen",	"seventeen",	"eighteen",	"nineteen",
},
		*const name2[] = {
	"",		"ten",		"twenty",	"thirty",
	"forty",	"fifty",	"sixty",	"seventy",
	"eighty",	"ninety",
},
		*const name3[] = {
	"hundred",	"thousand",	"million",	"billion",
	"trillion",	"quadrillion",	"quintillion",	"sextillion",
	"septillion",	"octillion",	"nonillion",	"decillion",
	"undecillion",	"duodecillion",	"tredecillion",	"quattuordecillion",
	"quindecillion",		"sexdecillion",
	"septendecillion",		"octodecillion",
	"novemdecillion",		"vigintillion",
};

void	convert(char *);
int	number(const char *, int);
void	pfract(int);
int	unit(int, const char *);
void	usage(void);
void	help(void);

static int lflag;

int main(int argc, char *argv[]) {
	int i, first;

	/* parse leading options (no getopt in the clib) */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			help();
			return 0;
		}
		if (strcmp(argv[i], "-l") == 0) {
			lflag = 1;
			continue;
		}
		break;
	}

	argc -= i;
	argv += i;

	if (argc <= 0) {
		usage();
		return 1;
	}

	for (first = 1; *argv != NULL; first = 0, ++argv) {
		if (!first)
			(void)printf("...\n");
		convert(*argv);
		if (lflag)
			(void)printf("\n");
	}
	return 0;
}

void convert(char *line) {
	int flen = 0, len, rval;
	char *p, *fraction;

	if (*line == '\0')
		return;

	/* strip trailing and leading whitespace */
	len = strlen(line) - 1;
	while ((isblank((unsigned char)line[len])) || (line[len] == '\n'))
		line[len--] = '\0';
	while ((isblank((unsigned char)line[0])) || (line[0] == '\n'))
		line++;

	fraction = NULL;
	for (p = line; *p != '\0' && *p != '\n'; ++p) {
		if (isblank((unsigned char)*p))
			goto badnum;
		if (isdigit((unsigned char)*p))
			continue;
		switch (*p) {
		case '.':
			if (fraction != NULL)
				goto badnum;
			fraction = p + 1;
			*p = '\0';
			break;
		case '-':
		case '+':
			if (p == line)
				break;
			/* FALLTHROUGH */
		default:
badnum:			(void)printf("illegal number: %s\n", line);
			exit(1);
		}
	}
	*p = '\0';

	if ((len = strlen(line)) > MAXNUM ||
	    ((fraction != NULL) && (flen = strlen(fraction)) > MAXNUM)) {
		(void)printf("number too long (max %d digits).\n", MAXNUM);
		exit(1);
	}

	if (*line == '-') {
		(void)printf("minus%s", lflag ? " " : "\n");
		++line;
		--len;
	}
	if (*line == '+') {
		(void)printf("plus%s", lflag ? " " : "\n");
		++line;
		--len;
	}

	rval = len > 0 ? unit(len, line) : 0;
	if (fraction != NULL && flen != 0)
		for (p = fraction; *p != '\0'; ++p)
			if (*p != '0') {
				if (rval)
					(void)printf("%sand%s",
					    lflag ? " " : "",
					    lflag ? " " : "\n");
				if (unit(flen, fraction)) {
					if (lflag)
						(void)printf(" ");
					pfract(flen);
					rval = 1;
				}
				break;
			}
	if (!rval)
		(void)printf("zero%s", lflag ? "" : ".\n");
}

int unit(int len, const char *p) {
	int off, rval;

	rval = 0;
	if (len > 3) {
		if (len % 3) {
			off = len % 3;
			len -= off;
			if (number(p, off)) {
				rval = 1;
				(void)printf(" %s%s",
				    name3[len / 3], lflag ? " " : ".\n");
			}
			p += off;
		}
		for (; len > 3; p += 3) {
			len -= 3;
			if (number(p, 3)) {
				rval = 1;
				(void)printf(" %s%s",
				    name3[len / 3], lflag ? " " : ".\n");
			}
		}
	}
	if (number(p, len)) {
		if (!lflag)
			(void)printf(".\n");
		rval = 1;
	}
	return (rval);
}

int number(const char *p, int len) {
	int val, rval;

	rval = 0;
	switch (len) {
	case 3:
		if (*p != '0') {
			rval = 1;
			(void)printf("%s hundred", name1[*p - '0']);
		}
		++p;
		/* FALLTHROUGH */
	case 2:
		val = (p[1] - '0') + (p[0] - '0') * 10;
		if (val) {
			if (rval)
				(void)printf(" ");
			if (val < 20)
				(void)printf("%s", name1[val]);
			else {
				(void)printf("%s", name2[val / 10]);
				if (val % 10)
					(void)printf("-%s", name1[val % 10]);
			}
			rval = 1;
		}
		break;
	case 1:
		if (*p != '0') {
			rval = 1;
			(void)printf("%s", name1[*p - '0']);
		}
	}
	return (rval);
}

void pfract(int len) {
	static const char *const pref[] = { "", "ten-", "hundred-" };

	switch(len) {
	case 1:
		(void)printf("tenths%s", lflag ? "" : ".\n");
		break;
	case 2:
		(void)printf("hundredths%s", lflag ? "" : ".\n");
		break;
	default:
		(void)printf("%s%sths%s", pref[len % 3], name3[len / 3],
		    lflag ? "" : ".\n");
		break;
	}
}

void usage(void) {
	puts("usage: .number [-l] number ...");
}

void help(void) {
	puts("number - convert numbers to English\n");
	usage();
	puts("\n"
	     "DESCRIPTION\n"
	     "\n"
	     "-h, --help\n\tshow this help\n"
	     "\n"
	     "-l\n\tprint each number on a single line\n"
	     "\t(default prints each magnitude group on its own line).\n"
	     "\n"
	     "This is a port of OpenBSD number.c to ZXNext.\n"
	     "Scientific notation (e/E) is not supported.");
}

