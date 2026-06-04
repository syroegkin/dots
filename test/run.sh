#!/bin/sh
#
# Host-based test runner for the ZX Spectrum Next dot commands.
#
# The real binaries are z80 built with z88dk, but the command *logic* is
# portable C; the only ZX-specific include, <arch/zxn.h> (and <arch/zxn/esxdos.h>
# for md5), is satisfied by the stubs in test/stub. So we compile the logic with
# the host compiler and check behaviour:
#
#   * golden stdout tests for the argv->stdout commands (number, morse)
#   * RFC-1321 vector + wrapper tests for the md5 library
#
# This validates logic, not the z80 codegen -- that needs an emulator smoke test.
#
# Usage:
#   ./test/run.sh            run all tests
#   ./test/run.sh --record   (re)generate the golden .out files from current output
#
# Override the compiler with CC=... (defaults to cc).

set -eu

ROOT=$(CDPATH= cd "$(dirname "$0")/.." && pwd)
CC=${CC:-cc}
CFLAGS="-std=c99 -Wall"
STUB="$ROOT/test/stub"

RECORD=0
[ "${1:-}" = "--record" ] && RECORD=1

BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

pass=0
fail=0

# golden <command-name>: compile $name/main.c (plus test/shim/$name.c if present,
# for commands that need esxdos backed by stdio), then run every
# test/golden/$name/<case>.args and diff stdout against <case>.out. If a
# <case>.in file exists it is passed as the command's final argument (the file
# to read), so <case>.args holds options only.
golden() {
	name=$1
	shim=""
	[ -f "$ROOT/test/shim/$name.c" ] && shim="$ROOT/test/shim/$name.c"
	# shellcheck disable=SC2086
	$CC $CFLAGS -I "$STUB" "$ROOT/$name/main.c" $shim -o "$BUILD/$name"
	dir="$ROOT/test/golden/$name"
	for argf in "$dir"/*.args; do
		[ -e "$argf" ] || continue
		case=$(basename "$argf" .args)
		outf="$dir/$case.out"
		infile=""
		if [ -f "$dir/$case.in" ]; then
			cp "$dir/$case.in" "$BUILD/in"
			infile="$BUILD/in"
		fi
		# args are intentionally word-split; errors print to stdout, and a
		# non-zero exit (e.g. illegal input) is expected for some cases.
		# shellcheck disable=SC2046
		"$BUILD/$name" $(cat "$argf") $infile > "$BUILD/got" 2>&1 || true
		if [ "$RECORD" = 1 ]; then
			cp "$BUILD/got" "$outf"
			echo "  rec  $name/$case"
			continue
		fi
		if [ -f "$outf" ] && diff -u "$outf" "$BUILD/got" > "$BUILD/diff" 2>&1; then
			echo "  ok   $name/$case"
			pass=$((pass + 1))
		else
			echo "  FAIL $name/$case"
			sed 's/^/        /' "$BUILD/diff" 2>/dev/null || true
			fail=$((fail + 1))
		fi
	done
}

echo "== golden: number =="
golden number
echo "== golden: morse =="
golden morse
echo "== golden: fold =="
golden fold

echo "== md5 library =="
if [ "$RECORD" = 1 ]; then
	echo "  (skipped: md5 vectors are fixed, nothing to record)"
else
	# shellcheck disable=SC2086
	$CC $CFLAGS -I "$STUB" \
		"$ROOT/md5sum/lib/md5/md5.c" "$ROOT/test/md5/test_md5.c" \
		-o "$BUILD/test_md5"
	if "$BUILD/test_md5"; then
		pass=$((pass + 1))
	else
		fail=$((fail + 1))
	fi
fi

echo
if [ "$RECORD" = 1 ]; then
	echo "recorded golden outputs."
	exit 0
fi
echo "passed: $pass  failed: $fail"
[ "$fail" -eq 0 ]
