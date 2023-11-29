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
    // Show progress
    "-p",
    // Output file
    "-o",
};

#define OP_H cliOptions[0]
#define OP_HELP cliOptions[1]
#define OP_F cliOptions[2]
#define OP_P cliOptions[3]
#define OP_O cliOptions[4]

void usage(void) {
    puts("usage: .md5sum -f [-p | -o | -h]");
}
void help() {
    puts("v1.0.0 by Sergii Skorokhodov");
    puts("md5sum - Print MD5 (128-bit) checksums.");
    puts("");
    usage();
    puts("");
    puts("DESCRIPTION");
    puts("\n-h, --help\tShow this help");
    puts("-f\t\tSpecify input file (Required)");
    puts("-o\t\tSpecify output file");
    puts("\t\tIf omitted, result printed to stdout");
    puts("\t\tIf specified without a file, a .md5 file will be created at the same path as the original");
    puts("\nDerived from RSA Data Security, ported to ZXNext");
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
    CLO.file = 0;
    CLO.help = 0;
    CLO.progress = 0;
    CLO.output = 0;

    if (argc < 2) {
        usage();
        return CLI_ERROR_STATUS;
    }

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