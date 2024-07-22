#include <stdio.h>

#include "vec.h"
#include "common.h"
#include "awacompiler.h"

void _Awatalk_compileBit(Parser * p, int bit) {
    if (bit) fprintf(p->outFile, "wa");
    else fprintf(p->outFile, "awa");
}

void _Awatalk_compileNBits(Parser * p, ssize_t v, int N, int details) {
    if (N <= 0) return;
    ssize_t flag = ((ssize_t) 1) << (N-1);
    int first = 1;
    while (flag) {
        int bit = ((v&flag)>0);
        if ((details && !first) || (!details && !bit)) putc(' ', p->outFile);
        if (details && bit) putc(' ', p->outFile);
        _Awatalk_compileBit(p, bit);
        flag >>= 1;
        first = 0;
    }
}

void _compileAwatalk(Parser * p, int details) {
    if (details) fprintf(p->outFile, "init:    ");
    fprintf(p->outFile, "Awa");

    ssize_t val;
    int _command = 1, _req = 5, _withno;
    for (size_t i = 0; i < VECTOR_SIZE(p->tokens); ++i) {
        val = (ssize_t) VECTOR_GET(p->tokens, i);
        if (_command) _withno = (requiresbits(val) > 0);
        else _withno = 0;

        if (details) {
            if (_command) {
                fprintf(p->outFile, "\n%3s", commandToAwatism(val));
                if (_withno) {
                    fprintf(p->outFile, " %4ld: ", (ssize_t) VECTOR_GET(p->tokens, i+1));
                } else {
                    fprintf(p->outFile, ":%5s ", "");
                }
            } else {
                fprintf(p->outFile, "   ");
            }
        }
        
        _Awatalk_compileNBits(p, val, _req, details);
        

        _command = !_withno;
        _req = (val == Token_blow) ? 8 : 5;
    }
}

/////

//
void _compileAwatism(Parser * p, int details) {
    ssize_t val, _prevVal = -1;
    int _withno, _req;
    for (size_t i = 0; i < VECTOR_SIZE(p->tokens); ++i) {
        val = (ssize_t) VECTOR_GET(p->tokens, i);
        _withno = (requiresbits(val) > 0);
        _req = (val == Token_blow) ? 8 : 5;

        if (!details && ((val == Token_duplicate && _prevVal == Token_duplicate)
        || _prevVal == Token_equal_to || _prevVal == Token_less_than || _prevVal == Token_greater_than)) {
            fprintf(p->outFile, "; ");
        } else if (i>0) {
            putc('\n',p->outFile);
        }

        fprintf(p->outFile, "%s", commandToAwatism(val));

        if (_withno) {
            ++i;
            if (details) {
                fprintf(p->outFile, " %4ld   ", (ssize_t) VECTOR_GET(p->tokens, i));
            } else {
                fprintf(p->outFile, " %ld", (ssize_t) VECTOR_GET(p->tokens, i));
            }
        } else if (details) {
            fprintf(p->outFile, "        ");
        }

        if (details) {
            fprintf(p->outFile, "# ");
            _Awatalk_compileNBits(p, val, 5, 1);
            if (_withno) {
                fprintf(p->outFile, "   ");
                _Awatalk_compileNBits(p, (ssize_t) VECTOR_GET(p->tokens, i), _req, 1);
            }
        }

        _prevVal = val;
    }
}

/////

void _compileAwobject(Parser * p) {
    for (size_t i = 0; i < VECTOR_SIZE(p->tokens); ++i) {
        putc((char) (ssize_t) VECTOR_GET(p->tokens, i), p->outFile);
    }
}

void compileFromParser(Parser * p, int details) {
    if (p->outMode == Filetype_Awatalk) _compileAwatalk(p, details);
    if (p->outMode == Filetype_Awatism) _compileAwatism(p, details);
    if (p->outMode == Filetype_Awobject) _compileAwobject(p);
}