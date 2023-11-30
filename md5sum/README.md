# MD5SUM

Print MD5 (128-bit) checksums.

## Installation

Download the `MD5SUM` file from the "build" folder and place it in the "dot" folder on your ZX Spectrum Next.

## Usage

- `-h, --help`: Show help.
- `-f`: Specify the input file (Required).
- `-p`: Show progress.
- `-o`: Specify the output file. If omitted, the result is printed to stdout. If specified without a file, a .md5 file will be created at the same path as the original.

## Examples

```
.md5sum -f /license.md
e23a8aac56a30ab95d480606572e504d
```

```
.md5sum -f /license.md -o
```
will create `/license.md.md5` file with the md5sum

Usage with BASIC:
```basic
10 DIM d$(32)
20 OPEN #2, "v>d$"
30 .md5sum -f /license.md
40 CLOSE #2
50 PRINT d$
60 REM will print e23a8aac56a30ab95d480606572e504d
```

Derived from RSA Data Security, ported to ZXNext

Feel free to use, contribute, and enjoy!