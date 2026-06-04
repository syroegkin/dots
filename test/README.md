# Tests

Host-based tests for the dot commands. The shipped binaries are z80 (built with
z88dk), but the command *logic* is portable C, so these tests compile that logic
with the **host** compiler and check behaviour. The only ZX-specific includes
(`<arch/zxn.h>`, `<arch/zxn/esxdos.h>`) are satisfied by the stubs in
[`stub/`](stub/).

> Note: this validates logic, not the z80 code generation. Verifying the actual
> built binary still needs an emulator smoke test.

## Running

```
make test          # from the repo root
# or directly:
./test/run.sh
CC=clang ./test/run.sh   # pick a compiler
```

## What it covers

- **Golden stdout tests** for the argv→stdout commands (`number`, `morse`).
  Each case is a pair of files in `golden/<command>/`:
  - `<case>.args` — the command-line arguments (word-split)
  - `<case>.out`  — the expected stdout

  Add a case by dropping in a new `.args` file and recording its output:

  ```
  ./test/run.sh --record   # (re)generates every .out from current output
  ```

  Recorded output is seeded from the (verified-correct) implementation, so it
  locks in current behaviour against regressions — review new `.out` files
  before committing.

- **MD5 unit tests** ([`md5/test_md5.c`](md5/test_md5.c)): the RFC 1321 vectors
  checked via `md5Init`/`md5Update`/`md5Finalize`, plus the `md5File()` wrapper
  driven through a mock `esx_f_read()` (including a 2 KB input that crosses the
  1 KB read chunk).
