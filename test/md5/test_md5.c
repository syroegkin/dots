/*
 * Host unit tests for md5sum/lib/md5.
 *
 * Two layers are exercised:
 *   1. The algorithm (md5Init/md5Update/md5Finalize) against the canonical
 *      RFC 1321 test-suite vectors -- independent ground truth.
 *   2. The md5File() wrapper, driven through a mock esx_f_read() that serves
 *      bytes from a memory buffer (including an input larger than the 1024-byte
 *      read chunk, to cover the chunking loop).
 *
 * Built and run by test/run.sh on the host compiler.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../../md5sum/lib/md5/md5.h"

/* --- mock esxdos file read: serves bytes from a memory buffer --------------- */

static const uint8_t *g_src;
static uint16_t       g_len;
static uint16_t       g_pos;

uint16_t esx_f_read(unsigned char handle, void *buffer, uint16_t bytes)
{
	uint16_t n = g_len - g_pos;
	(void)handle;
	if (n > bytes)
		n = bytes;
	memcpy(buffer, g_src + g_pos, n);
	g_pos += n;
	return n;
}

/* --- helpers ---------------------------------------------------------------- */

static int failures;

static void hash_to_string(const uint8_t *p, char *out)
{
	unsigned int i;
	for (i = 0; i < 16; ++i)
		sprintf(out + i * 2, "%02x", p[i]);
}

static void check(const char *label, const char *got, const char *want)
{
	if (strcmp(got, want) == 0) {
		printf("  ok   %s\n", label);
	} else {
		printf("  FAIL %s: got %s want %s\n", label, got, want);
		failures++;
	}
}

static void md5_string(const char *in, char *out)
{
	MD5Context ctx;
	md5Init(&ctx);
	md5Update(&ctx, (uint8_t *)in, strlen(in));
	md5Finalize(&ctx);
	hash_to_string(ctx.digest, out);
}

/* --- tests ------------------------------------------------------------------ */

int main(void)
{
	static const struct {
		const char *in;
		const char *want;
	} vectors[] = {
		{ "",                                                                 "d41d8cd98f00b204e9800998ecf8427e" },
		{ "a",                                                                "0cc175b9c0f1b6a831c399e269772661" },
		{ "abc",                                                              "900150983cd24fb0d6963f7d28e17f72" },
		{ "message digest",                                                   "f96b697d7cb7938d525a2f31aaf161d0" },
		{ "abcdefghijklmnopqrstuvwxyz",                                       "c3fcd3d76192e4007dfb496cca67e13b" },
		{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",   "d174ab98d277d9f5a5611c2c9f419d9f" },
		{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		                                                                      "57edf4a22be3c955ac49da2e2107b67a" },
	};
	size_t i;
	char got[33];
	uint8_t result[16];
	unsigned char fd = 0;
	static uint8_t big[2048];

	puts("RFC-1321 vectors (md5Init/md5Update/md5Finalize):");
	for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); ++i) {
		md5_string(vectors[i].in, got);
		check(vectors[i].in[0] ? vectors[i].in : "(empty)", got, vectors[i].want);
	}

	puts("md5File() wrapper (via mock esx_f_read):");

	/* short input: single read */
	g_src = (const uint8_t *)"abc";
	g_len = 3;
	g_pos = 0;
	md5File(&fd, result, 0);
	hash_to_string(result, got);
	check("\"abc\"", got, "900150983cd24fb0d6963f7d28e17f72");

	/* 2048 bytes: crosses the 1024-byte read chunk (multiple reads) */
	memset(big, 'a', sizeof(big));
	g_src = big;
	g_len = (uint16_t)sizeof(big);
	g_pos = 0;
	md5File(&fd, result, 0);
	hash_to_string(result, got);
	check("2048 x 'a' (chunked)", got, "b7ea2d21ad2ef3e28085d30247603e0b");

	if (failures) {
		printf("\n%d MD5 test(s) failed\n", failures);
		return 1;
	}
	puts("\nAll MD5 tests passed.");
	return 0;
}
