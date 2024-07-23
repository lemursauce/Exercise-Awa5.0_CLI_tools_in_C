#include <stdio.h>
#include <stdint.h>

#include "vec.h"
#include "common.h"
#include "awacompiler.h"

void _Awatalk_compileBit(Parser * p, int bit) {
    if (bit) fprintf(p->outFile, "wa");
    else fprintf(p->outFile, "awa");
}

void _Awatalk_compileNBits(Parser * p, intptr_t v, int N, int details) {
    if (N <= 0) return;
    if (N != 5 && N != 8) fprintf(stderr, "WTH IS GOING ON???\n");
    intptr_t flag = ((intptr_t) 1) << (N-1);
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

    intptr_t val;
    int _withno, _req;
    for (size_t i = 0; i < VECTOR_SIZE(p->tokens); ++i) {
        val = (intptr_t) VECTOR_GET(p->tokens, i);
        _withno = (requiresbits(val) > 0);
        _req = 5;

        if (details) {
            fprintf(p->outFile, "\n%3s", commandToAwatism(val));
            if (_withno) {
                fprintf(p->outFile, " %4ld: ", (intptr_t) VECTOR_GET(p->tokens, i+1));
            } else {
                fprintf(p->outFile, ":%5s ", "");
            }
        }

        _Awatalk_compileNBits(p, val, _req, details);

        if (_withno) {
            _req = (val == Token_blow) ? 8 : 5;
            ++i;
            val = (intptr_t) VECTOR_GET(p->tokens, i);
            if (details) {
                fprintf(p->outFile, "   ");
            }
            _Awatalk_compileNBits(p, val, _req, details);
        }
    }
}

/////

//
void _compileAwatism(Parser * p, int details) {
    intptr_t val, _prevVal = -1;
    int _withno, _req;
    for (size_t i = 0; i < VECTOR_SIZE(p->tokens); ++i) {
        val = (intptr_t) VECTOR_GET(p->tokens, i);
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
                fprintf(p->outFile, " %4ld   ", (intptr_t) VECTOR_GET(p->tokens, i));
            } else {
                fprintf(p->outFile, " %ld", (intptr_t) VECTOR_GET(p->tokens, i));
            }
        } else if (details) {
            fprintf(p->outFile, "        ");
        }

        if (details) {
            fprintf(p->outFile, "# ");
            _Awatalk_compileNBits(p, val, 5, 1);
            if (_withno) {
                fprintf(p->outFile, "   ");
                _Awatalk_compileNBits(p, (intptr_t) VECTOR_GET(p->tokens, i), _req, 1);
            }
        }

        _prevVal = val;
    }
}

/////

void _compileAwobject(Parser * p) {
    for (size_t i = 0; i < VECTOR_SIZE(p->tokens); ++i) {
        putc((char) (intptr_t) VECTOR_GET(p->tokens, i), p->outFile);
    }
}

void compileFromParser(Parser * p, int details) {
    if (p->outMode == Filetype_Awatalk) _compileAwatalk(p, details);
    if (p->outMode == Filetype_Awatism) _compileAwatism(p, details);
    if (p->outMode == Filetype_Awobject) _compileAwobject(p);
}