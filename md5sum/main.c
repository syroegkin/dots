#define __ZXNEXT 1

#include <stdio.h>
#include <strings.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include "lib/cli/cli.h"
#include "lib/md5/md5.h"

// Debug
#define DEBUG 1

// Missing 28mhz variable
#ifndef __RTM_28MHZ
    #define __RTM_28MHZ  0x03
#endif

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

// Print actual hash value
void print_hash(uint8_t *p){
    for(unsigned int i = 0; i < 16; ++i){
        printf("%02x", p[i]);
    }
    puts("");
}

int main(int argc, char *argv[]) {
    uint32_t len = 0;           // File length
    uint8_t result[16];         // MD5 result

    CommandLineOptions* args = getCommandLineOptions(argc, argv);

    // In case of errors, terminate now
    if (args == CLI_ERROR_STATUS) {
        return 1;
    }

    // Show help and do not go any furhter
    if (args->help) {
        help();
        return 0;
    }

    #ifdef DEBUG
    printf("file: %d\n", args->file);
    if (args->file != 0) {
        printf("filename: %s\n", args->filename);
    }
    printf("output: %d\n", args->output);
    if (args->outputFilename != NULL) {
        printf("output filename: %s\n", args->outputFilename);
    }
    printf("progress: %d\n", args->progress);
    printf("compare: %d\n", args->compare);
    #endif

    // Prepare
    old_cpu_speed = ZXN_READ_REG(REG_TURBO_MODE);
    ZXN_NEXTREG(REG_TURBO_MODE, __RTM_28MHZ);
    atexit(cleanup);

    if (args->file == 0) {
        // We use input as a source
        #ifdef DEBUG
        printf("We use input as a source\n");
        #endif
        md5String("Hello next!", result);
    } else if (args->file == 1) {
        // We use file as a source
        #ifdef DEBUG
        printf("We use file as a source\n");
        #endif
        // Try to open the file
        errno = 0;
        fin = esx_f_open(args->filename, ESXDOS_MODE_R);
        if (errno != 0) {
            printf("Error opening file:  %s\n", args->filename);
            // exit(1);
            return 1;
        }

        md5File(&fin, result, args->progress);
        
        // Close the file
        esx_f_close(fin);
    }

    if (args->output == 0) {
        // Result to the screen
        print_hash(result);
    } else {
        // Result to the file

    }
    
    // example:
    // /home/syr/work/zx-spectrum/z88dk/src/z80asm/t/1451/hexdump.c
    
    return 0;
}