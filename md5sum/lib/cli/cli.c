#include "cli.h"

// Global variables
CommandLineOptions CLO;

// CLI arguments
static char *cliOptions[] = {
    // Show help
    "-h",
    "--help",
    // filename
    "-f",
    // compare
    "-s",
    // Show progress
    "-p",
    // Output file
    "-o",
};

#define OP_H cliOptions[0]
#define OP_HELP cliOptions[1]
#define OP_F cliOptions[2]
#define OP_S cliOptions[3]
#define OP_P cliOptions[4]
#define OP_O cliOptions[5]

void usage(void) {
    puts("usage: .md5sum [-p | -s | -f | -h]");
}

void help() {
  puts("md5sum - reformat input as morse code\n(produces the words by default)");
  puts("");
  usage();
  puts("");
  puts("DESCRIPTION");
  puts("");
  puts("-h, --help\n\tshow this help");
  puts("");
  puts("-s\n\tThe -s option for morse produces dots and dashes rather than words.");
  puts("");
  puts("-d\n\tDecode morse output consisting of dots and slashes (as generated by using the -s option).");
  puts("");
  puts("This is a port of OpenBSD morse.c to ZXNext");
}

/**
 * @brief 
 * 
 * @param str - The string
 * @param prefix - The prefix to match against
 * @return int 
 */
int startsWith(const char *str, const char *prefix) {
    // Compare the first strlen(prefix) characters of str with prefix
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

CommandLineOptions* getCommandLineOptions(int argc, char *argv[]) {
    // Init the structure with the list of empty values
    CLO.compare = 0;
    CLO.file = 0;
    CLO.help = 0;
    CLO.progress = 0;
    CLO.output = 0;

    if (argc < 2) {
        usage();
        return CLI_ERROR_STATUS;
    }

    #ifdef DEBUG
    for (unsigned char j = 1; j < argc; j++) {
        printf("Pos: %d, val: %s\n", j, argv[j]);
    }
    #endif

    for (unsigned char i = 1; i < argc; i++) {
        // Check for the help
        if (strcmp(argv[i], OP_H) == 0 || strcmp(argv[i], OP_HELP) == 0) {
            CLO.help = 1;
        }

        if (strcmp(argv[i], OP_F) == 0) {
            CLO.file = 1;
            // increase iterator, as the filename parameter
            // must be placed exactly after the `-f` option
            i++;
            // Save the reference
            CLO.filename = argv[i];
        }

        // Compare existing file against its md5 checksum
        if (strcmp(argv[i], OP_S) == 0) {
            CLO.compare = 1;
        }

        // To show progress
        if (strcmp(argv[i], OP_P) == 0) {
            CLO.progress = 1;
        }

        // To generate output file
        if (strcmp(argv[i], OP_O) == 0) {
            CLO.output = 1;
            // Check if the next argument is an option
            if (startsWith(argv[i + 1], "-") != 1) {
                i++;
                // if not, save the reference
                CLO.outputFilename = argv[i];
            }
        }
    }
    return &CLO;
}