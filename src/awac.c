#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "common.h"
#include "awaparser.h"
#include "awacompiler.h"

const char * help_str = 
"Usage: awac INPUT_FILE [-t | -s | -c] [-o OUTPUT_FILE]\n\n"
"Tool to compile an Awa5.0 file into a different format, whether that be \"Awatalk\" (.awa),\n"
"\"Awatisms\" (.awasm, also known as \"Awasembly\"), or \"Awobject\" (.o). The output will\n"
"contain all the same logic as the source file but in a different format.\n\n"
"The following describes the usage of the various options / flags:\n"
"  INPUT_FILE             the required Awa5.0 file to compile\n"
"  -o                     set the output file when finished compiling\n"
"      OUTPUT_FILE            the file being set as the output file when -o is called\n\n"
"  -t                     set the compile mode to \"Awatalk\"\n"
"  -s                     set the compile mode to \"Awatisms\"\n"
"  -c                     set the compile mode to \"Awobject\"\n\n"
"  -d                     compile with \"details\" on (only with -s or -t)\n\n"
"  -h                     display this help and exit";


int main(int argc, char *argv[]) {
    int opt, _optind, i = 1;
    
    int inCount = 0;
    int outCount = 0;
    char inFileStr[1024];
    memset(inFileStr, 0, sizeof(inFileStr));
    char outFileStr[1024];
    memset(outFileStr, 0, sizeof(outFileStr));
    strcpy(outFileStr, "out");

    int mode = Filetype_Awobject;
    int modeSet = 0;
    int detail = 0;

    while ((opt = getopt(argc, argv, "o:tscdh")) != -1) {
        _optind = optind;
        --_optind;
        
        if (opt == 'o') {
            --_optind;
            if (outCount) {
                fprintf(stderr, "Cannot declare multiple outputs\n");
                exit(EXIT_FAILURE);
            }
            ++outCount;
            memcpy(outFileStr, optarg, sizeof(outFileStr));
        } else if (opt == 't' || opt == 's' || opt == 'c') {
            if (modeSet) {
                fprintf(stderr, "Cannot set multiple output types\n");
                exit(EXIT_FAILURE);
            }
            if      (opt == 't') mode = Filetype_Awatalk;
            else if (opt == 's') mode = Filetype_Awatism;
            // else mode = 0;
        } else if (opt == 'd') {
            detail = 1;
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
    FILE * outFile;
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

    replaceExtension(outFileStr, mode);
    if (mode == Filetype_Awobject) {
        if ((outFile = fopen(outFileStr, "wb")) == NULL) {
            fprintf(stderr, "Failed to create file to write to\n");
            fclose(inFile);
            exit(EXIT_FAILURE);
        }
    } else {
        if ((outFile = fopen(outFileStr, "w")) == NULL) {
            fprintf(stderr, "Failed to create file to write to\n");
            fclose(inFile);
            exit(EXIT_FAILURE);
        }
    }

    Parser p;
    make_parser(&p, inFile, outFile, inExt, mode);
    run_parser(&p);
    compileFromParser(&p, detail);
    
    free_parser(&p);
    fclose(inFile);
    fclose(outFile);
    return EXIT_SUCCESS;
}