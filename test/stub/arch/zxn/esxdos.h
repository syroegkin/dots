/*
 * Host-test stub for <arch/zxn/esxdos.h>.
 *
 * Declares the esxdos file calls used by the commands. The definitions are
 * provided by the test harness: test/md5/test_md5.c mocks esx_f_read() over a
 * memory buffer, and test/shim/fold.c backs open/read/close with stdio.
 */
#ifndef DOTS_TEST_STUB_ESXDOS_H
#define DOTS_TEST_STUB_ESXDOS_H

#include <stdint.h>

#define ESXDOS_MODE_R 0x01

unsigned char esx_f_open(char *filename, unsigned char mode);
uint16_t      esx_f_read(unsigned char handle, void *buffer, uint16_t bytes);
unsigned char esx_f_close(unsigned char handle);

#endif
