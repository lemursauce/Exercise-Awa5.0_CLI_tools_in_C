#include "common.h"
#include <string.h>

const char * awatalkExtension =  ".awa";
const char * awatismExtension = ".awasm";
const char * awobjectExtension = ".o";

const char * AWA_SCII = "AWawJELYHOSIUMjelyhosiumPCNTpcntBDFGRbdfgr0123456789 .,!'()~_/;\n";

const char _AWASCII_TABLE[128] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 63,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    52, 55,  0,  0,  0,  0,  0, 56, 57, 58,  0,  0, 54,  0, 53, 61,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0, 62,  0,  0,  0,  0,
     0,  0, 32, 25, 33,  5, 34, 35,  8, 11,  4,  0,  6, 13, 26,  9,
    24,  0, 36, 10, 27, 12,  0,  1,  0,  7,  0,  0,  0,  0,  0, 60,
     0,  2, 37, 29, 38, 15, 39, 40, 18, 21, 14,  0, 16, 23, 30, 19,
    28,  0, 41, 20, 31, 22,  0,  3,  0, 17,  0,  0,  0,  0, 59,  0
};
const char * ASCII_to_AWASCII = _AWASCII_TABLE;

const char * leadingChars = "slmpr4bcegjnt";

int checkExtension(char * __fileName) {
    int len = strlen(__fileName);
    if (!len) return -1;

    int i = len-1;
    if (__fileName[i] == awatalkExtension[FileExtensionLen_Awatalk-1]) {
        if (len < FileExtensionLen_Awatalk) return -1;
        if (strcmp(__fileName+len-FileExtensionLen_Awatalk , awatalkExtension) == 0)
            return Filetype_Awatalk;
    }
    if (__fileName[i] == awatismExtension[FileExtensionLen_Awatism-1]) {
        if (len < FileExtensionLen_Awatism) return -1;
        if (strcmp(__fileName+len-FileExtensionLen_Awatism , awatismExtension) == 0)
            return Filetype_Awatism;
    }
    if (__fileName[i] == awobjectExtension[FileExtensionLen_Awobject-1]) {
        if (len < FileExtensionLen_Awobject) return -1;
        if (strcmp(__fileName+len-FileExtensionLen_Awobject , awobjectExtension) == 0)
            return Filetype_Awobject;
    }
    return -1;
}

const char * __extensionFromMode(int mode) {
    if (mode == Filetype_Awobject) return awobjectExtension;
    if (mode == Filetype_Awatism)  return awatismExtension;
    if (mode == Filetype_Awatalk)  return awatalkExtension;
    return NULL;
}

int __extensionLenFromMode(int mode) {
    if (mode == Filetype_Awobject) return FileExtensionLen_Awobject;
    if (mode == Filetype_Awatism)  return FileExtensionLen_Awatism;
    if (mode == Filetype_Awatalk)  return FileExtensionLen_Awatalk;
    return -1;
}

void replaceExtension(char * __fileName, int mode) {
    if (mode < 0 || Filetype_max < mode) return;
    int ext = checkExtension(__fileName);
    if (ext == mode) return;

    if (ext != -1) {
        int len = strlen(__fileName);
        int lenExt = __extensionLenFromMode(ext);
        memset(__fileName+len-lenExt, 0, lenExt);
    }
    strcat(__fileName, __extensionFromMode(mode));
}

int isAWASCII(char _c) {
    if (_c < 0) return 0;
    return (int) ASCII_to_AWASCII[(int) _c];
}

const char * commandToAwatism(int i) {
    switch(i) {
        case (Token_no_op)          : return Awatism_no_op;
        case (Token_print)          : return Awatism_print;
        case (Token_print_num)      : return Awatism_print_num;
        case (Token_read)           : return Awatism_read;
        case (Token_read_num)       : return Awatism_read_num;
        case (Token_blow)           : return Awatism_blow;
        case (Token_submerge)       : return Awatism_submerge;
        case (Token_pop)            : return Awatism_pop;
        case (Token_duplicate)      : return Awatism_duplicate;
        case (Token_surround)       : return Awatism_surround;
        case (Token_merge)          : return Awatism_merge;
        case (Token_add)            : return Awatism_add;
        case (Token_subtract)       : return Awatism_subtract;
        case (Token_multiply)       : return Awatism_multiply;
        case (Token_divide)         : return Awatism_divide;
        case (Token_count)          : return Awatism_count;
        case (Token_label)          : return Awatism_label;
        case (Token_jump)           : return Awatism_jump;
        case (Token_equal_to)       : return Awatism_equal_to;
        case (Token_less_than)      : return Awatism_less_than;
        case (Token_greater_than)   : return Awatism_greater_than;
        case (Token_terminate)      : return Awatism_terminate;
    }
    return NULL;
}


int AwatismToCommand(char * s) {
    if      (strcmp(s, Awatism_no_op)        == 0) return Token_no_op;
    else if (strcmp(s, Awatism_print)        == 0) return Token_print;
    else if (strcmp(s, Awatism_print_num)    == 0) return Token_print_num;
    else if (strcmp(s, Awatism_read)         == 0) return Token_read;
    else if (strcmp(s, Awatism_read_num)     == 0) return Token_read_num;
    else if (strcmp(s, Awatism_blow)         == 0) return Token_blow;
    else if (strcmp(s, Awatism_submerge)     == 0) return Token_submerge;
    else if (strcmp(s, Awatism_pop)          == 0) return Token_pop;
    else if (strcmp(s, Awatism_duplicate)    == 0) return Token_duplicate;
    else if (strcmp(s, Awatism_surround)     == 0) return Token_surround;
    else if (strcmp(s, Awatism_merge)        == 0) return Token_merge;
    else if (strcmp(s, Awatism_add)          == 0) return Token_add;
    else if (strcmp(s, Awatism_subtract)     == 0) return Token_subtract;
    else if (strcmp(s, Awatism_multiply)     == 0) return Token_multiply;
    else if (strcmp(s, Awatism_divide)       == 0) return Token_divide;
    else if (strcmp(s, Awatism_count)        == 0) return Token_count;
    else if (strcmp(s, Awatism_label)        == 0) return Token_label;
    else if (strcmp(s, Awatism_jump)         == 0) return Token_jump;
    else if (strcmp(s, Awatism_equal_to)     == 0) return Token_equal_to;
    else if (strcmp(s, Awatism_less_than)    == 0) return Token_less_than;
    else if (strcmp(s, Awatism_greater_than) == 0) return Token_greater_than;
    else if (strcmp(s, Awatism_terminate)    == 0) return Token_terminate;
    else return -1;
}

int isCommand(int _command_val) {
    return (_command_val == Token_terminate || (0 <= _command_val && _command_val <= Token_quick_range_max));
}

int requiresbits(int _command_val) {
    switch(_command_val) {
        case Token_blow:
            return 8;
        case Token_submerge:
        case Token_surround:
        case Token_label:
        case Token_jump:
            return 5;
        default:
            return 0;
    }
}

