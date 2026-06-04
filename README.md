# ZX Spectrum Next Dot Commands

## Introduction

This repository provides a list of dot commands for the ZX Spectrum Next.

## Dot Commands

Below is a list of dot commands available for use with ZX Spectrum Next:

- [**.morse**](morse/README.md): _Bring Morse code to your ZX Spectrum Next._
- [**.md5sum**](md5sum/README.md): _Calculate the MD5 hash of a file._
- [**.number**](number/README.md): _Convert numbers to their English-language equivalent._
- [**.fold**](fold/README.md): _Wrap input lines to fit within a given width._

## How to Use

To use these dot commands in your ZX Spectrum Next programs, follow these steps:

1. Download the dot command from the "build" folder to the "dot" folder on your ZX Spectrum Next.
2. Use it.

## Development

To send a freshly built command to a real ZX Spectrum Next over the network:

1. On the host, from a command's directory, run `make upload`. This builds the
   command and pushes it to the machine using `nextsync.py`.
2. On the Next, run `.sync` to receive the transferred files.

Run `make test` from the repo root to execute the host-based test suite (see
[test/README.md](test/README.md)).

z88dk v2.3 is used as a compiler.

## Contribution

Feel free to contribute additional dot commands or improvements to existing ones. Follow these steps:

1. Fork the repository.
2. Create a new branch: `git checkout -b feature/new-command`.
3. Make changes and commit: `git commit -am 'Add new command'`.
4. Push the branch: `git push origin feature/new-command`.
5. Submit a pull request.
