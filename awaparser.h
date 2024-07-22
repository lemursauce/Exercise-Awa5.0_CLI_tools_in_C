#ifndef _AWAPARSER_H_
#define _AWAPARSER_H_

#include <stdio.h>
#include "vec.h"

typedef struct {
    FILE * inFile;
    FILE * outFile;
    int inMode;
    int outMode;
    int index;
    int line;
    int col;
    int commands;
    vector tokens;
    int labels[32];
} Parser;

void make_parser(Parser * p, FILE * inFile, FILE * outFile, int inMode, int outMode);
void free_parser(Parser * p);

void run_parser(Parser * p);

#endif