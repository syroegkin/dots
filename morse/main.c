/*	$OpenBSD: morse.c,v 1.22 2016/03/07 12:07:56 mestre Exp $	*/

/*
 * Copyright (c) 1988, 1993
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

#define __ZXNEXT 1


#include <ctype.h>
// #include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/zxn.h>

#define PROGRAM_NAME morse

static char *options[] = {
  "-h",
  "--help",
  "-d",
  "-s"
};
#define OP_H options[0]
#define OP_HELP options[1]
#define OP_D options[2]
#define OP_S options[3]

static char
  *digit[] = {
  "-----",
  ".----",
  "..---",
  "...--",
  "....-",
  ".....",
  "-....",
  "--...",
  "---..",
  "----.",
},
  *alph[] = {
  ".-",
  "-...",
  "-.-.",
  "-..",
  ".",
  "..-.",
  "--.",
  "....",
  "..",
  ".---",
  "-.-",
  ".-..",
  "--",
  "-.",
  "---",
  ".--.",
  "--.-",
  ".-.",
  "...",
  "-",
  "..-",
  "...-",
  ".--",
  "-..-",
  "-.--",
  "--..",
};

struct punc {
  char c;
  char *morse;
} other[] = {
  { 'e', "..-.." },   /* accented e - only decodes */
  { ',', "--..--" },
  { '.', ".-.-.-" },
  { '?', "..--.." },
  { '/', "-..-." },
  { '-', "-....-" },
  { ':', "---..." },
  { ';', "-.-.-." },
  { '(', "-.--." },   /* KN */
  { ')', "-.--.-" },
  { '"', ".-..-." },
  { '`', ".-..-." },
  { '\'', ".----." },
  { '+', ".-.-." },   /* AR \n\n\n */
  { '=', "-...-" },   /* BT \n\n */
  { '@', ".--.-." },
  { '\n', ".-.-" },   /* AA (will only decode) */
  { '\0', NULL }
};

struct prosign {
  char *c;
  char *morse;
} ps[] = {
  { "<AS>", ".-..." },      /* wait */
  { "<CL>", "-.-..-.." },
  { "<CT>", "-.-.-" },      /* start */
  { "<EE5>", "......" },    /* error */
  { "<EE5>", "......." },
  { "<EE5>", "........" },
  { "<SK>", "...-.-" },
  { "<SN>", "...-." },      /* understood */
  { "<SOS>", "...---..." },
  { NULL, NULL }
};


void morse(int);
void decode(char *);
void show(char *);

static uint8_t sflag = 0;
static uint8_t dflag = 0;

void usage() {
  puts("usage: .morse [-d | -s] string ...");
}

uint8_t error() {
  usage();
  return 1;
}

uint8_t help() {
  puts("morse - reformat input as morse code\n(produces the words by default)");
  puts("");
  usage();
  puts("");
  puts("DESCRIPTION");
  puts("");
  puts("-h, --help\n\tshow this help");
  puts("");
  puts("-s\n\tThe -s option for morse produces dots and dashes rather than words.");
  puts("");
  puts("-d\n\tDecode morse output consisting of dots and slashes (as generated by using the -s option).");
  puts("");
  puts("This is a port of OpenBSD morse.c to ZXNext");
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return error();
  }

  uint8_t i;

  for (i = 1; i < argc; i++) {
    if (
        strcmp(argv[i], OP_H)
      && strcmp(argv[i], OP_HELP)
      && strcmp(argv[i], OP_D)
      && strcmp(argv[i], OP_S)
    ) {
      break;
    }

    if (strcmp(argv[i], OP_H) == 0 || strcmp(argv[i], OP_HELP) == 0) {
      return help();
    }
    if (strcmp(argv[i], OP_D) == 0) {
      dflag = 1;
    }
    if (strcmp(argv[i], OP_S) == 0) {
      sflag = 1;
    }

    if ((dflag == 1 && sflag == 1) || (dflag == 0 && sflag == 0)) {
      return error();
    }
  }

  argc -= i;
  argv += i;

  // int ch;
  char *p;

  if (dflag) {
    if (*argv) {
      do {
        decode(*argv);
      } while (*++argv);
    }
    putchar('\n');
  } else {
    if (*argv)
      do {
        for (p = *argv; *p; ++p)
          morse((int)*p);
        show("");
      } while (*++argv);
    // honestly, I have no idea why this is here
    show("...-.-"); /* SK */
  }
  return 0;
}

void morse(int c)
{
  int i;

  if (isalpha(c))
    show(alph[c - (isupper(c) ? 'A' : 'a')]);
  else if (isdigit(c))
    show(digit[c - '0']);
  else if (isspace(c))
    show("");  /* could show BT for a pause */
  else {
    i = 0;
    while (other[i].c) {
      if (other[i].c == c) {
        show(other[i].morse);
        break;
      }
      i++;
    }
  }
}

void decode(char *s)
{
  int i;
  
  for (i = 0; i < 10; i++)
    if (strcmp(digit[i], s) == 0) {
      putchar('0' + i);
      return;
    }
  
  for (i = 0; i < 26; i++)
    if (strcmp(alph[i], s) == 0) {
      putchar('A' + i);
      return;
    }
  i = 0;
  while (other[i].c) {
    if (strcmp(other[i].morse, s) == 0) {
      putchar(other[i].c);
      return;
    }
    i++;
  }
  i = 0;
  while (ps[i].c) {
    /* put whitespace around prosigns */
    if (strcmp(ps[i].morse, s) == 0) {
      printf(" %s ", ps[i].c);
      return;
    }
    i++;
  }
  putchar('x'); /* line noise */
}



void show(char *s)
{
  if (sflag)
    printf(" %s", s);
  else for (; *s; ++s)
    printf(" %s", *s == '.' ? "dit" : "daw");
  printf("\n");
}