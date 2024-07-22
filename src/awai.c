#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "awaparser.h"
#include "awainterpreter.h"

const char * help_str = 
"Usage: awai FILE [-s] [-x MAX_INST] \n\n"
"Tool to interpret an Awa5.0 file from one of the specified formats, whether that be \"Awatalk\" (.awa),\n"
"\"Awatisms\" (.awasm, also known as \"Awasembly\"), or \"Awobject\" (.o). The interpreter will run the\n"
"code and take inputs / print outputs via the console. If any inputs are required, they will be taken as\n"
"specified by the original documentation.\n\n"
"The following describes the usage of the various options / flags:\n"
"  FILE                   the required Awa5.0 file to interpret\n"
"  -s                     print the running stats\n"
"  -x                     set the maximum amount of instructions to run (to avoid infinite loops)\n"
"      MAX_INST               the maximum amount of instructions specified\n\n"
"  -h                     display this help and exit";


int main(int argc, char *argv[]) {
    int opt, _optind, i = 1;

    int inCount = 0;
    char inFileStr[1024];
    memset(inFileStr, 0, sizeof(inFileStr));

    
    int stats = 0;
    long long max_inst = -1;

    while ((opt = getopt(argc, argv, "hsx:")) != -1) {
        _optind = optind;
        --_optind;

        if (opt == 'x') {
            --_optind;
            if ((max_inst = atoll(optarg)) == 0) {
                fprintf(stderr, "Invalid input for max_inst\n");
                exit(EXIT_FAILURE);
            }
        } else if (opt == 's') {
            stats = 1;
        } else if (opt == 'h') {
            puts(help_str);
            exit(EXIT_SUCCESS);
        } else {
            exit(EXIT_FAILURE);
        }

        while (i < _optind) {
            if (inCount) {
                fprintf(stderr, "Cannot declare multiple inputs\n");
                exit(EXIT_FAILURE);
            }
            ++inCount;
            memcpy(inFileStr, argv[i], sizeof(inFileStr));
            ++i;
        }
        i = optind;
    }

    // get remaing inputs if need be
    while (i < argc) {
        if (inCount) {
            fprintf(stderr, "Cannot declare multiple inputs\n");
            exit(EXIT_FAILURE);
        }
        ++inCount;
        memcpy(inFileStr, argv[i], sizeof(inFileStr));
        ++i;
    }

    FILE * inFile;
    int inExt = checkExtension(inFileStr);
    if (inExt == -1) {
        fprintf(stderr, "Cannot compile a non AWA5.0 file type\n");
        exit(EXIT_FAILURE);
    } if (inExt == Filetype_Awobject) {
        if ((inFile = fopen(inFileStr, "rb")) == NULL) {
            fprintf(stderr, "File does not exist\n");
            exit(EXIT_FAILURE);
        }
    } else {
        if ((inFile = fopen(inFileStr, "r")) == NULL) {
            fprintf(stderr, "File does not exist\n");
            exit(EXIT_FAILURE);
        }
    }

    Parser p;
    make_parser(&p, inFile, NULL, inExt, 0);
    run_parser(&p);
    interpretParser(&p, max_inst, stats);
    
    free_parser(&p);
    fclose(inFile);
    return EXIT_SUCCESS;
}