#ifndef _AWA_INTERPRETER_H_
#define _AWA_INTERPRETER_H_

#include "awaparser.h"

char * _interpreter_getline();
void interpretParser(Parser * p, long long max_inst, int stats);

#endif