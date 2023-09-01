#define __ZXNEXT 1

#include <stdio.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include "md5.h"


void print_hash(uint8_t *p){
    for(unsigned int i = 0; i < 16; ++i){
        printf("%02x", p[i]);
    }
    printf("\n");
}

// global variables

unsigned char fin  = 0xff;   // file descriptor
unsigned char fout = 0xff;   // file descriptor

// clean up at exit

static unsigned char old_cpu_speed;

void cleanup(void)
{
   if (fin != 0xff) esx_f_close(fin);
   if (fout != 0xff) esx_f_close(fout);
   
   puts("    ");

   ZXN_NEXTREGA(REG_TURBO_MODE, old_cpu_speed);
}

int main()
{
  old_cpu_speed = ZXN_READ_REG(REG_TURBO_MODE);
  ZXN_NEXTREG(REG_TURBO_MODE, RTM_14MHZ);
   
  atexit(cleanup);

  puts("Hello next!");
  uint8_t result[16];
  md5String("Hello next!", result);
  print_hash(result);
  return 0;
}