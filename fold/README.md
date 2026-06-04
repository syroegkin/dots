# Fold

Wrap each input line of a file to fit within a given number of columns — handy
for reading PC-authored text (≈80-column lines) on the Next's narrow screen.

## Installation

Download the `FOLD` file from the "build" folder and place it in the "dot"
folder on your ZX Spectrum Next.

## Usage

```
.fold [-b] [-s] [-w width] file ...
```

- `-h, --help`: Display the help message.
- `-b`: Count bytes rather than display columns (tab, backspace and carriage
  return lose their special width).
- `-s`: Break lines at the last blank within the width, so words are not split.
- `-w width`: Wrap at `width` columns instead of the default 80 (1–255).

Output goes to the screen, or to a redirected stream (e.g. a NextBASIC channel).

## Examples

Reflow a readme to fit the 32-column screen, breaking on spaces:

```
.fold -s -w 32 /readme.txt
```

Hard-wrap (may split words) at 20 columns:

```
.fold -w 20 notes.txt
```

By default a tab advances to the next multiple of 8 columns; with `-b` every
byte (tabs included) counts as one.

This is a port of OpenBSD fold.c to ZXNext. It is single-byte only — there is no
UTF-8/wide-character handling, and the legacy `-<width>` digit shorthand is not
supported (use `-w`).

Feel free to use, contribute, and enjoy!
