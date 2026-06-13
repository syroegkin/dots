# Number

`v1.0.0`

Convert numbers to their English-language equivalent (e.g. `2026` becomes
"two thousand twenty-six").

## Installation

Download the `NUMBER` file from the "build" folder and place it in the "dot"
folder on your ZX Spectrum Next.

## Usage

```
.number [-l] number ...
```

- `-h, --help`: Display the help message.
- `-l`: Print each number on a single line. By default each magnitude group is
  printed on its own line (which reads nicely on the narrow screen).

Numbers may be negative (`-`), positive (`+`), and may contain a decimal
fraction (e.g. `3.14`). Scientific notation (`1.5e3`) is not supported in this
port.

## Examples

```
.number 2026

two thousand.
twenty-six.
```

```
.number -l 2026

two thousand twenty-six
```

```
.number 0

zero.
```

```
.number -42

minus
forty-two.
```

```
.number -l 3.14

three and fourteen hundredths
```

```
.number 100 200

one hundred.
...
two hundred.
```

This is a port of OpenBSD number.c to ZXNext.

Feel free to use, contribute, and enjoy!
