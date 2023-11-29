#define __ZXNEXT 1

#include <stdio.h>
#include <strings.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include "lib/cli/cli.h"
#include "lib/md5/md5.h"

// Missing 28mhz variable
#ifndef __RTM_28MHZ
    #define __RTM_28MHZ  0x03
#endif

// global variables
unsigned char fin  = 0xff;   // file descriptor
unsigned char fout = 0xff;   // file descriptor

char errorMessage[] = "Error opening file: ";

// clean up at exit
static unsigned char old_cpu_speed;
void cleanup(void) {
    // Ensure all files are closed
    if (fin != 0xff) esx_f_close(fin);
    if (fout != 0xff) esx_f_close(fout);
    ZXN_NEXTREGA(REG_TURBO_MODE, old_cpu_speed);
}

// hash value to string
void hashToString(uint8_t *p, char *hashString){
    for(unsigned int i = 0; i < 16; ++i){
        sprintf(hashString + i * 2, "%02x", p[i]);
    }
}

int main(int argc, char *argv[]) {
    uint32_t len = 0;           // File length
    uint8_t result[16];         // MD5 result
    char outputFilename[128];   // Auto generated output filename (original filename + .md5)
    char hashString[32];

    CommandLineOptions* args = getCommandLineOptions(argc, argv);

    // In case of errors, terminate now
    if (args == CLI_ERROR_STATUS) {
        return 2;
    }

    // Show help and do not go any furhter
    if (args->help) {
        help();
        return 0;
    }

    // Prepare
    old_cpu_speed = ZXN_READ_REG(REG_TURBO_MODE);
    ZXN_NEXTREG(REG_TURBO_MODE, __RTM_28MHZ);
    atexit(cleanup);

    if (args->file == 0) {
        // error!
        puts("Please specify the file name!");
        return 1;
    } else if (args->file == 1) {
        // We use file as a source
        // Try to open the file
        fin = esx_f_open(args->filename, ESXDOS_MODE_R);
        if (errno) {
            printf("%s%s\n", errorMessage, args->filename);
            return 2;
        }

        md5File(&fin, result, args->progress);
        
        // Close the file
        esx_f_close(fin);
    }

    // Turn a hash to the string
    hashToString(result, hashString);

    if (args->output == 0) {
        // Result to the screen
        printf("%s", hashString);
    } else if (args->output == 1) {
        // Result to the file
        if (args->outputFilename != NULL) {
            // Try to open the file
            fout = esx_f_open(args->outputFilename, ESXDOS_MODE_CT | ESXDOS_MODE_W);
        } else {
            strcpy(outputFilename, args->filename);
            strcat(outputFilename, ".md5");
            // Try to open the file
            fout = esx_f_open(outputFilename, ESXDOS_MODE_CT | ESXDOS_MODE_W);
        }
        if (errno) {
            printf("%s%s\n", errorMessage, args->outputFilename);
            return 2;
        }

        // Save result to the file        
        esx_f_write(
            fout,
            hashString,
            32 // MD5 hash is always 32 characters long, hence 32 bytes
        );
        esx_f_close(fout);
    }

    return 0;
}