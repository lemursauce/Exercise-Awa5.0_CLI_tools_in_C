#include "awaparser.h"

#include <string.h>
#include <ctype.h>

#include "common.h"

void make_parser(Parser * p, FILE * inFile, FILE * outFile, int inMode, int outMode) {
    p->inFile = inFile;
    p->outFile = outFile;
    p->inMode = inMode;
    p->outMode = outMode;
    p->index = 0;
    p->line = 1;
    p->col = 0;
    p->commands = 1;
    memset(p->labels, -1, 32*sizeof(int));
    VECTOR_INIT(p->tokens);
}

void free_parser(Parser * p) {
   VECTOR_FREE(p->tokens);
}

void _print_parser_stderr(Parser * p) {
    if (p->inMode) fprintf(stderr, "Error at line %d, col %d: ", p->line, p->col);
    else fprintf(stderr, "Error at index %d: ", p->index);
}

int _getc(Parser * p) {
    int fc = fgetc(p->inFile);
    ++p->index;
    ++p->col;
    if (fc == '\n') {
        ++p->line;
        p->col = 0;
    }
    if (feof(p->inFile)) return -1;
    return fc;
}

////////

int _Awatalk_getNextValid(Parser * p, int * nonValPrev) {
    int fc;
    while (1) {
        fc = _getc(p);
        if (fc == -1) return -1;
        if (fc == 'a' || fc == 'A') {
            int retval = *nonValPrev ? 0 : 2;
            *nonValPrev = 0;
            return retval;
        } else if (fc == 'w' || fc == 'W') {
            *nonValPrev = 0;
            return 1;
        } else {
            *nonValPrev = 1;
        }
    }
    return -1;
}

int _Awatalk_getNextBit(Parser * p, int * nonValPrev) {
    int val = _Awatalk_getNextValid(p,nonValPrev);

    if (val == -1) return -1;
    else if (val == 0) {
        if (_Awatalk_getNextValid(p,nonValPrev) != 1) {
            _print_parser_stderr(p);
            fprintf(stderr, "could not finish reading \"awa\", missing the sequential 'w'\n");
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        if (_Awatalk_getNextValid(p,nonValPrev) != 2) {
            _print_parser_stderr(p);
            fprintf(stderr, "could not finish reading \"awa\", missing the sequential 'a'\n");
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        return 0;
    } else if (val == 1) {
        if (_Awatalk_getNextValid(p,nonValPrev) != 2) {
            _print_parser_stderr(p);
            fprintf(stderr, "could not finish reading \"wa\", missing the sequential 'a'\n");
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        return 1;
    } else {
        _print_parser_stderr(p);
        fprintf(stderr, "Error on accepting \"valid\" character when parsing bit\n");
        free_parser(p);
        exit(EXIT_FAILURE);
    }

    return -1;
}

int _Awatalk_getNextNBits(Parser * p, int * nonValPrev, int N, int _iscommand) {
    int val = 0, any = 0, bit;
    while (N --> 0) {
        bit = _Awatalk_getNextBit(p, nonValPrev);
        if (bit == -1) {
            if (_iscommand && any == 0 && feof(p->inFile)) return -1;
            _print_parser_stderr(p);
            fprintf(stderr, "could not finish reading set of bits, missing %d bits\n", N+1);
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        any = 1;
        val = (val<<1) | bit;
    }
    return val;
}

void _parseAwatalk(Parser * p) {
    ssize_t val;
    int nonValPrev = 1;
    if ((val = _Awatalk_getNextBit(p, &nonValPrev)) != 0) {
        _print_parser_stderr(p);
        fprintf(stderr, "Awatalk program must start with initial \"awa\" (not case sensitive)\n");
        free_parser(p);
        exit(EXIT_FAILURE);
    }
    while (1) {
        val = _Awatalk_getNextNBits(p, &nonValPrev, 5, 1);
        if (val < 0) {
            break;
        }
        if (!isCommand(val)) {
            _print_parser_stderr(p);
            fprintf(stderr, "%02lx is not a valid command\n", val);
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        VECTOR_PUSH(p->tokens,val);
        if (val != Token_label) ++p->commands;
        int reqbits = requiresbits(val);
        if (reqbits) {
            int _signed = (val == Token_blow);
            int _tolabel = (val == Token_label);
            val = _Awatalk_getNextNBits(p, &nonValPrev, reqbits, 0);
            if (_signed && val & (1 << (reqbits-1))) {
                val = -(val ^ (1 << (reqbits-1)));
            }
            if (_tolabel) {
                if (p->labels[val] != -1) {
                    _print_parser_stderr(p);
                    fprintf(stderr, "Cannot have repeat labels\n");
                    free_parser(p);
                    exit(EXIT_FAILURE);
                }
                p->labels[val] = VECTOR_SIZE(p->tokens)+1;
            }
            VECTOR_PUSH(p->tokens,val);
        }
    }
    
}

////////

int _Awatism_getNextImportChar(Parser * p) {
    int fc;
    while (1) {
        fc = _getc(p);
        if (fc == -1) break;
        if (isalnum(fc) || fc == '#' || fc == ';' || fc == '\n') return fc;
    }
    return -1;
}

int _Awatism_getNextSep(Parser * p, int commentMode) {
    int fc;
    while (1) {
        fc = _getc(p);
        if (fc == -1 || fc == '\n' || (commentMode == 0 && fc == ';')) return 1;
        else if (fc == '#') commentMode = 1;
        else if (commentMode == 0 && !isspace(fc)) {
            _print_parser_stderr(p);
            fprintf(stderr, "Invalid characters after command prior to separator ('%c')\n", (char) fc);
            free_parser(p);
            exit(EXIT_FAILURE);
        }
    }
    return -1;
}

int _Awatism_parseCommand(Parser * p, char * buf) {
    int fc;
    // buf[0] is already assumed to be filled in and is of size 4 with buf[3] = 0
    for (int i = 1; i < 3; ++i) {
        fc = _getc(p);
        if (fc == -1 || !isalnum(fc)) {
            _print_parser_stderr(p);
            fprintf(stderr, "Invalid characters encountered when parsing command\n");
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        buf[i] = (char) fc;
    }
    int command = AwatismToCommand(buf);
    if (!isCommand(command)) {
        _print_parser_stderr(p);
        fprintf(stderr, "Invalid command \"%s\"\n", buf);
        free_parser(p);
        exit(EXIT_FAILURE);
    }
    return command;
}

int _Awatism_parseReqWS(Parser * p) {
    int fc = _getc(p);
    if (fc == -1) {
        _print_parser_stderr(p);
        fprintf(stderr, "Reached end of file before parsing required whitespace\n");
        free_parser(p);
        exit(EXIT_FAILURE);
    }
    if (fc != ' ' && fc != '\t') {
        _print_parser_stderr(p);
        fprintf(stderr, "Encountered non-whitespace characters when whitespace is required\n");
        free_parser(p);
        exit(EXIT_FAILURE);
    }

    while (1) {
        fc = _getc(p);
        if (fc != ' ' && fc != '\t') break;
    }
    return fc;
}

int _Awatism_parseNumber(Parser * p, ssize_t * hold) {
    int fc = _Awatism_parseReqWS(p), firstdigit = -1, negate = 0;
    if (fc == -1) {
        _print_parser_stderr(p);
        fprintf(stderr, "Reached end of file before parsing required number\n");
        free_parser(p);
        exit(EXIT_FAILURE);
    }
    if (fc == '-') {
        negate = 1;
        *hold = 0;
    } else {
        if (!isdigit(fc)) {
            _print_parser_stderr(p);
            fprintf(stderr, "Encountered non-numeric character when trying to parse number\n");
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        *hold = fc - '0';
        firstdigit = fc;
    }

    while (isdigit(fc = _getc(p))) {
        if (firstdigit == -1) firstdigit = fc;
        else if (firstdigit == '0') {
            _print_parser_stderr(p);
            fprintf(stderr, "Non-zero number cannot start with '0' character\n");
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        *hold = (10 * (*hold)) + (fc - '0');
    }
    if (firstdigit < 0) {
        _print_parser_stderr(p);
        fprintf(stderr, "Encountered non-numeric character when trying to parse number\n");
        free_parser(p);
        exit(EXIT_FAILURE);
    }
    if (fc != -1 && !isspace(fc) && fc != '#' && fc != ';') {
        _print_parser_stderr(p);
        fprintf(stderr, "expected number to be standalone and not end in an invalid character \"%c\"\n", (char) fc);
        free_parser(p);
        exit(EXIT_FAILURE);
    }
    if (negate) *hold *= -1;
    return fc;
}

void _parseAwatism(Parser * p) {
    ssize_t val, command;
    char buf[4];
    buf[3] = 0;
    while (1) {
        val = _Awatism_getNextImportChar(p);
        if (val == -1) break;
        if (isalnum(val)) {
            buf[0] = val;
            command = _Awatism_parseCommand(p,buf);
            if (command < 0) break;
            VECTOR_PUSH(p->tokens,command);
            if (command != Token_label) ++p->commands;
            int reqbits = requiresbits(command);
            if (reqbits) {
                int _signed = (command == Token_blow);
                int _tolabel = (command == Token_label);
                ssize_t hold;
                val = _Awatism_parseNumber(p, &hold);
                if (!_signed && hold < 0) {
                    _print_parser_stderr(p);
                    fprintf(stderr, "Expected unsigned number, received a signed number\n");
                    free_parser(p);
                    exit(EXIT_FAILURE);
                }
                if (_signed) --reqbits;
                // check if number fits in bits???
                if (_tolabel) {
                    if (p->labels[hold] != -1) {
                        _print_parser_stderr(p);
                        fprintf(stderr, "Cannot have repeat labels\n");
                        free_parser(p);
                        exit(EXIT_FAILURE);
                    }
                    p->labels[hold] = VECTOR_SIZE(p->tokens)+1;
                }
                VECTOR_PUSH(p->tokens, hold);
            }
            if (val == '#') {
                _Awatism_getNextSep(p,1);
            } else if (val != -1 && val != '\n') _Awatism_getNextSep(p,0);
        } else if (val == '#') {
            _Awatism_getNextSep(p,1);
        }
    }
}

////////

void _parseAwobject(Parser * p) {
    ssize_t val;
    int _command = 1, _signed = 0, _label = 0, _req = 5;
    while (1) {
        val = (unsigned char) _getc(p);
        if (feof(p->inFile)) break;
        if (_command && !isCommand(val)) {
            _print_parser_stderr(p);
            fprintf(stderr, "%02lx is not a valid command\n", val);
            free_parser(p);
            exit(EXIT_FAILURE);
        }
        if (_command && val != Token_label) ++p->commands;
        if (_signed && val & (1 << (_req-1))) {
            val = -(val ^ (1 << (_req-1)));
        }
        if (_label) {
            if (p->labels[val] != -1) {
                _print_parser_stderr(p);
                fprintf(stderr, "Cannot have repeat labels\n");
                free_parser(p);
                exit(EXIT_FAILURE);
            }
            p->labels[val] = VECTOR_SIZE(p->tokens)+1;
        }

        if (_command) _command = !requiresbits(val);
        else _command = 1;

        _signed = (val == Token_blow);
        _req    = (val == Token_blow) ? 8 : 5;
        _label  = (val == Token_label);
        VECTOR_PUSH(p->tokens,val);
    }
}

////////

void run_parser(Parser * p) {
    if (p->tokens.size > 0) {
        VECTOR_FREE(p->tokens);
        VECTOR_INIT(p->tokens);
    }
    if (p->inMode == Filetype_Awatalk) _parseAwatalk(p);
    if (p->inMode == Filetype_Awatism) _parseAwatism(p);
    if (p->inMode == Filetype_Awobject) _parseAwobject(p);
}