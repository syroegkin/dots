#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <string.h>

typedef struct {
    unsigned char help;         // Show help
    unsigned char progress;     // Show progress
    unsigned char file;         // Use file for input
    unsigned char output;       // To produce the ouptut file with the hash result
    char *filename;             // The filename to operate with
    char *outputFilename;       // Output filename
} CommandLineOptions;

#define CLI_ERROR_STATUS 0

CommandLineOptions* getCommandLineOptions(int argc, char *argv[]);
void usage(void);
void help(void);

#endif