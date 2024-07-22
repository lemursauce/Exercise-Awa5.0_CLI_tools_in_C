#ifndef _AWA_COMMON_H_
#define _AWA_COMMON_H_

#define Filetype_Awatalk 2
#define Filetype_Awatism 1
#define Filetype_Awobject 0

#define Filetype_max 2

#define Token_no_op         0x00
#define Token_print         0x01
#define Token_print_num     0x02
#define Token_read          0x03
#define Token_read_num      0x04
#define Token_blow          0x05
#define Token_submerge      0x06
#define Token_pop           0x07
#define Token_duplicate     0x08
#define Token_surround      0x09
#define Token_merge         0x0A
#define Token_add           0x0B
#define Token_subtract      0x0C
#define Token_multiply      0x0D
#define Token_divide        0x0E
#define Token_count         0x0F
#define Token_label         0x10
#define Token_jump          0x11
#define Token_equal_to      0x12
#define Token_less_than     0x13
#define Token_greater_than  0x14
#define Token_terminate     0x1F

#define Awatism_no_op         "nop"
#define Awatism_print         "prn"
#define Awatism_print_num     "pr1"
#define Awatism_read          "red"
#define Awatism_read_num      "r3d"
#define Awatism_blow          "blo"
#define Awatism_submerge      "sbm"
#define Awatism_pop           "pop"
#define Awatism_duplicate     "dpl"
#define Awatism_surround      "srn"
#define Awatism_merge         "mrg"
#define Awatism_add           "4dd"
#define Awatism_subtract      "sub"
#define Awatism_multiply      "mul"
#define Awatism_divide        "div"
#define Awatism_count         "cnt"
#define Awatism_label         "lbl"
#define Awatism_jump          "jmp"
#define Awatism_equal_to      "eql"
#define Awatism_less_than     "lss"
#define Awatism_greater_than  "gr8"
#define Awatism_terminate     "trm"

#define Token_quick_range_max 0x14

#define Awatalk_command_bits 5

//////////

/// `".awa"`
#define FileExtensionLen_Awatalk 4
extern const char * awatalkExtension;
/// `".awasm"`
#define FileExtensionLen_Awatism 6
extern const char * awatismExtension;
/// `".o"`
#define FileExtensionLen_Awobject 2
extern const char * awobjectExtension;

#define AWA_SCII_len 64
extern const char * AWA_SCII;

extern const char * ASCII_to_AWASCII;

#define LEADING_CHAR_len 13
extern const char * leadingChars;

//////////

/**
 * @brief Provides a check/mode of a file given its file name.
 * 
 * @param __fileName name of the file to check the extension of 
 * @return `int` describing the AWA mode of the file extension, `-1` if invalid.
 * The modes include `AWATALK`, `AWATISM`, and `AWOBJECT`, which can all be found
 * in the `common.h` file.
 */
int checkExtension(char * __fileName);

/**
 * @brief Adds/replaces extension of filename to match compile mode
 * 
 * @param __fileName name of the file being changed to match the extension mode
 * @param mode an `int` that matches the AWA mode desired
 */
void replaceExtension(char * __fileName, int mode);

int isAWASCII(char _c);

const char * commandToAwatism(int i);
int AwatismToCommand(char * s);

int isCommand(int _command_val);
int requiresbits(int _command_val);


#endif