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

## Versioning

Each command is versioned independently with [semantic versioning](https://semver.org).
The source of truth is the `VERSION` file in the command's directory; its value is
stamped into that command's `<name>.zxpkg.toml` manifest and the version badge in its
`README.md`.

Run these from inside a command's directory (e.g. `cd morse`):

| Command | Effect |
|---|---|
| `make version` | Print the current version. |
| `make bump-patch` | `1.0.0 → 1.0.1` (bug fixes), then sync the manifest + README. |
| `make bump-minor` | `1.0.0 → 1.1.0` (new features), then sync. |
| `make bump-major` | `1.0.0 → 2.0.0` (breaking changes), then sync. |
| `make sync-version` | Re-stamp `VERSION` into the manifest + README without bumping. |

`make` (the default build) runs `sync-version` automatically, so the manifest and
README never drift from `VERSION`. Bumping is always a deliberate, separate step —
builds never change the version on their own.

## Publishing to pkg.zx.in.net

Each command ships a `<name>.zxpkg.toml` manifest so it can be indexed by the
[ZXPkg registry](https://pkg.zx.in.net). One repository can ship several packages —
the registry indexes every `*.zxpkg.toml` it finds.

To list this repository on the registry, submit it once at
<https://pkg.zx.in.net/new>. After that the portal re-crawls the default branch on a
schedule: whenever a command's manifest `version` changes (via a `bump-*` build), the
portal cuts and archives a new version of that package automatically.

The manifest's `[[artifact]].src` points at the committed binary under `build/`
(e.g. `morse/build/MORSE`), so make sure the freshly built binary is committed before
pushing a release.

## Contribution

Feel free to contribute additional dot commands or improvements to existing ones. Follow these steps:

1. Fork the repository.
2. Create a new branch: `git checkout -b feature/new-command`.
3. Make changes and commit: `git commit -am 'Add new command'`.
4. Push the branch: `git push origin feature/new-command`.
5. Submit a pull request.
