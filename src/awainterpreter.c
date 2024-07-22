#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "vec.h"
#include "common.h"
#include "awaparser.h"
#include "awainterpreter.h"

typedef struct {
    vector content;
    int isdouble;
} Bubble;

typedef struct {
    Parser * p;
    Bubble * bubbles;
    long long max_inst;
    int cycles;
    size_t i;
    size_t end;
} Interpreter;

char * _interpreter_getline() {
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;
    
    if(line == NULL) return NULL;

    while(1) {
        c = fgetc(stdin);
        if(feof(stdin) || c == '\n') break;

        if(--len == 0) {
            len = lenmax;
            ptrdiff_t diff = line - linep;
            char * linen = realloc(linep, lenmax <<= 1);

            if(linen == NULL) {
                free(linep);
                return NULL;
            } else {
                line = linen + diff;
                linep = linen;
            }
        }

        *line++ = c;
    }
    *line = '\0';
    return linep;
}

/////

Bubble * make_singleBubble(intptr_t val) {
    Bubble * b = malloc(sizeof(Bubble));
    VECTOR_INIT(b->content);
    VECTOR_PUSH(b->content, val);
    b->isdouble = 0;
    return b;
}

Bubble * make_doubleBubble() {
    Bubble * b = malloc(sizeof(Bubble));
    VECTOR_INIT(b->content);
    b->isdouble = 1;
    return b;
}

Bubble * copy_bubble(Bubble * source) {
    Bubble * b = malloc(sizeof(Bubble));
    VECTOR_INIT(b->content);
    b->isdouble = source->isdouble;
    if (b->isdouble) {
        // b/source is double
        if (VECTOR_SIZE(source->content)) {
            VECTOR_RESIZE(b->content, VECTOR_SIZE(source->content));
        }
        for (size_t i = 0; i < VECTOR_SIZE(source->content); ++i) {
            Bubble * b2 = copy_bubble((Bubble*) VECTOR_GET(source->content, i));
            VECTOR_PUSH(b->content, b2);
        }
    } else {
        // b/source is single
        if (VECTOR_SIZE(source->content)) {
            VECTOR_PUSH(b->content, VECTOR_GET(source->content, 0));
        }
    }
    return b;
}

void free_bubble(Bubble * b) {
    if (b->isdouble) {
        for (size_t i = 0; i < VECTOR_SIZE(b->content); ++i) {
            Bubble * b2 = (Bubble *) VECTOR_GET(b->content, i);
            if (b2) free_bubble(b2);
        }
    }
    VECTOR_FREE(b->content);
    free(b);
}

void _singleBubble_set_number(Bubble * b, intptr_t val) {
    VECTOR_SET(b->content, 0, val);
}

intptr_t _singleBubble_get_number(Bubble * b) {
    return (intptr_t) VECTOR_GET(b->content, 0);
}

void _print_bubble(Bubble * b) {
    if (b->isdouble) {
        putchar('[');
        for (size_t i = 0; i < VECTOR_SIZE(b->content); ++i) {
            if (i) printf(", ");
            _print_bubble((Bubble *) VECTOR_GET(b->content, i));
        }
        putchar(']');
    } else if (VECTOR_SIZE(b->content) > 0) {
        printf("%ld", (intptr_t) VECTOR_GET(b->content, 0));
    }
}

/////

void make_interpreter(Interpreter * in, Parser * p, long long max_inst) {
    in->p = p;
    in->max_inst = max_inst;
    in->end = VECTOR_SIZE(p->tokens);
    in->bubbles = 0;
}

void stop_interpreter(Interpreter * in, int fail) {
    free_bubble(in->bubbles);
    if (fail) exit(EXIT_FAILURE);
}

/////

int _interpreter_print(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        fprintf(stderr, "Cannot call prn when bubble abyss is empty\n");
        stop_interpreter(in,1);
    }
    Bubble * b = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    if (b->isdouble) {
        for (intptr_t i = (intptr_t) VECTOR_SIZE(b->content)-1; i >= 0; --i) {
            Bubble * b2 = (Bubble*) VECTOR_GET(b->content, i);
            if (b2->isdouble) {
                fprintf(stderr, "Cannot print double bubble within double bubble\n");
                fprintf(stderr, "Tried to print The following bubble as an AWASCII character:\n");
                _print_bubble(b2);
                fprintf(stderr, "\n");
                stop_interpreter(in,1);
            }
            intptr_t val = _singleBubble_get_number(b2);
            if (val < 0 || AWA_SCII_len <= val) {
                fprintf(stderr, "Tried to print character (%ld) that does not exist in AWASCII\n", val);
                stop_interpreter(in,1);
            }
            putchar(AWA_SCII[val]);
        }
    } else {
        intptr_t val = _singleBubble_get_number(b);
        if (val < 0 || AWA_SCII_len <= val) {
            fprintf(stderr, "Tried to print character (%ld) that does not exist in AWASCII\n", val);
            stop_interpreter(in,1);
        }
        putchar(AWA_SCII[val]);
    }
    free_bubble(b);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    return 1;
}
int _interpreter_print_num(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        fprintf(stderr, "Cannot call pr1 when bubble abyss is empty\n");
        stop_interpreter(in,1);
    }
    Bubble * b = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    if (b->isdouble) {
        for (intptr_t i = (intptr_t) VECTOR_SIZE(b->content)-1; i >= 0; --i) {
            Bubble * b2 = (Bubble*) VECTOR_GET(b->content, i);
            if (b2->isdouble) {
                fprintf(stderr, "Cannot print double bubble within double bubble\n");
                fprintf(stderr, "Tried to print The following bubble as an integer:\n");
                _print_bubble(b2);
                fprintf(stderr, "\n");
                stop_interpreter(in,1);
            }
            intptr_t val = _singleBubble_get_number(b2);
            printf("%ld ", val);
        }
    } else {
        intptr_t val = _singleBubble_get_number(b);
        printf("%ld ", val);
    }
    free_bubble(b);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    return 1;
}
int _interpreter_read(Interpreter * in) {
    char * line;
    intptr_t len;
    while (1) {
        line = _interpreter_getline();
        if (line == NULL) {
            fprintf(stderr, "Error encountered when reading line\n");
            stop_interpreter(in,1);
        }
        len = strlen(line);
        if (len > 0) break;
        else free(line);
    }
    intptr_t c;
    Bubble * b = make_doubleBubble();
    for (intptr_t i = len-1; i >= 0; --i) {
        c = isAWASCII(line[i]);
        if (c) VECTOR_PUSH(b->content, make_singleBubble(c));
    }
    free(line);
    VECTOR_PUSH(in->bubbles->content, b);
    return 1;
}
int _interpreter_read_num(Interpreter * in) {
    char * line;
    size_t len;
    while (1) {
        line = _interpreter_getline();
        if (line == NULL) {
            fprintf(stderr, "Error encountered when reading line\n");
            stop_interpreter(in,1);
        }
        len = strlen(line);
        if (len > 0) break;
        else free(line);
    }
    size_t i;
    intptr_t c, prev = -1;
    int numfound = 0, neg = 0;
    for (i = 0; i < len; ++i) {
        c = line[i];
        if (isdigit(c)) {
            numfound = 1;
            if (prev == '-') neg = 1;
            break;
        }
        prev = c;
    }
    if (!numfound) {
        free(line);
        fprintf(stderr, "Could not find number within line\n");
        stop_interpreter(in,1);
    }
    intptr_t val = 0;
    for (; i < len; ++i) {
        c = line[i];
        if (!isdigit(c)) break;
        val = 10*val + (c-'0');
    }
    if (neg) val = -val;
    free(line);
    VECTOR_PUSH(in->bubbles->content, make_singleBubble(val));
    return 1;
}
int _interpreter_blow(Interpreter * in, intptr_t num) {
    Bubble * b = make_singleBubble(num);
    VECTOR_PUSH(in->bubbles->content, b);
    return 1;
}
int _interpreter_submerge(Interpreter * in, intptr_t num) {
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        fprintf(stderr, "Cannot call sbm when bubble abyss is empty\n");
        stop_interpreter(in,1);
    }
    if (num < 0) {
        fprintf(stderr, "Cannot submerge a negative depth (tried %ld)\n", num);
        stop_interpreter(in,1);
    }

    Bubble * b = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    intptr_t low = 0;
    if (num > 0 && (size_t) num < VECTOR_SIZE(in->bubbles->content)) {
        low = VECTOR_SIZE(in->bubbles->content) - num;
    }
    VECTOR_INSERT(in->bubbles->content, low, b);
    return 1;
}
int _interpreter_pop(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        fprintf(stderr, "Cannot call pop when bubble abyss is empty\n");
        stop_interpreter(in,1);
    }
    Bubble * b = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    if (b->isdouble) {
        for (size_t i = 0; i < VECTOR_SIZE(b->content); ++i) {
            VECTOR_PUSH(in->bubbles->content, VECTOR_GET(b->content, i));
            VECTOR_SET(b->content, i, NULL);
        }
    }
    free_bubble(b);
    return 1;
}
int _interpreter_duplicate(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        fprintf(stderr, "Cannot call dpl when bubble abyss is empty\n");
        stop_interpreter(in,1);
    }
    Bubble * b = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    Bubble * dpl = copy_bubble(b);
    VECTOR_PUSH(in->bubbles->content, dpl);
    return 1;
}
int _interpreter_surround(Interpreter * in, intptr_t num) {
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        fprintf(stderr, "Cannot call srn when bubble abyss is empty\n");
        stop_interpreter(in,1);
    }

    size_t lower = 0;
    if (num <= 0) {
        fprintf(stderr, "Must surround at least 1 element\n");
        stop_interpreter(in,1);
    }
    if ((size_t) num < VECTOR_SIZE(in->bubbles->content)) {
        lower = VECTOR_SIZE(in->bubbles->content) - num;
    }

    Bubble * b = make_doubleBubble();
    for (size_t i = lower; i < VECTOR_SIZE(in->bubbles->content); ++i) {
        VECTOR_PUSH(b->content, VECTOR_GET(in->bubbles->content, i));
        VECTOR_SET(in->bubbles->content, i, NULL);
    }

    VECTOR_RESIZE(in->bubbles->content, lower+1);
    VECTOR_SET(in->bubbles->content, lower, b);
    return 1;
}
int _interpreter_merge(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling mrg\n");
        stop_interpreter(in,1);
    }

    Bubble * b1, *b2, *bm;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    VECTOR_POP(in->bubbles->content);
    bm = make_doubleBubble();

    if (b1->isdouble) {
        for (size_t i = 0; i < VECTOR_SIZE(b1->content); ++i) {
            VECTOR_PUSH(bm->content, VECTOR_GET(b1->content, i));
            VECTOR_SET(b1->content, i, 0);
        }
        free_bubble(b1);
    } else VECTOR_PUSH(bm->content, b1);
    
    if (b2->isdouble) {
        for (size_t i = 0; i < VECTOR_SIZE(b2->content); ++i) {
            VECTOR_PUSH(bm->content, VECTOR_GET(b2->content, i));
            VECTOR_SET(b2->content, i, 0);
        }
        free_bubble(b2);
    } else VECTOR_PUSH(bm->content, b2);
    
    VECTOR_PUSH(in->bubbles->content, bm);
    return 1;
}
int _interpreter_add(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling 4dd\n");
        stop_interpreter(in,1);
    }

    //
    Bubble * b1, *b2, *bm;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    VECTOR_POP(in->bubbles->content);
    
    if (b1->isdouble) {
        // swap with bm as a temporary hold
        bm = b1;
        b1 = b2;
        b2 = bm;
        // now b2 is guaranteed to be a double if at least one is a double
    }

    if (b1->isdouble) {
        // both are double
        bm = make_doubleBubble();
        size_t len = VECTOR_SIZE(b2->content);
        if (len > VECTOR_SIZE(b1->content)) len = VECTOR_SIZE(b1->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }

        for (size_t i = 1; i <= len; ++i) {
            Bubble * temp1 = VECTOR_GET(b1->content, VECTOR_SIZE(b1->content)-i);
            Bubble * temp2 = VECTOR_GET(b2->content, VECTOR_SIZE(b2->content)-i);
            if (temp1->isdouble || temp2->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val1 = _singleBubble_get_number(temp1);
            intptr_t val2 = _singleBubble_get_number(temp2);
            VECTOR_SET(temp1->content, 0, (val1+val2));
            VECTOR_PUSH(bm->content, temp1);
            VECTOR_SET(b1->content, VECTOR_SIZE(b1->content)-i, 0);
        }
    } else if (b2->isdouble) {
        // just b2 is double, b1 is single
        bm = make_doubleBubble();
        intptr_t val = _singleBubble_get_number(b1);
        size_t len = VECTOR_SIZE(b2->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }

        for (size_t i = 0; i < len; ++i) {
            Bubble * temp = VECTOR_GET(b2->content, i);
            if (temp->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val2 = _singleBubble_get_number(temp);
            VECTOR_SET(temp->content, 0, (val+val2));
            VECTOR_PUSH(bm->content, temp);
            VECTOR_SET(b2->content, i, 0);
        }
    } else {
        // both are single
        bm = make_singleBubble(_singleBubble_get_number(b1) +
                               _singleBubble_get_number(b2));
        
    }
    free_bubble(b1);
    free_bubble(b2);
    VECTOR_PUSH(in->bubbles->content, bm);

    return 1;
}
int _interpreter_subtract(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling 4dd\n");
        stop_interpreter(in,1);
    }

    //
    Bubble * b1, *b2, *bm;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    VECTOR_POP(in->bubbles->content);
    
    int rev = 0;
    if (b1->isdouble) {
        // swap with bm as a temporary hold
        bm = b1;
        b1 = b2;
        b2 = bm;
        rev = 1;
        // now b2 is guaranteed to be a double if at least one is a double
    }

    if (b1->isdouble) {
        // both are double, rev = 1
        bm = make_doubleBubble();
        size_t len = VECTOR_SIZE(b2->content);
        if (len > VECTOR_SIZE(b1->content)) len = VECTOR_SIZE(b1->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }

        for (size_t i = 1; i <= len; ++i) {
            Bubble * temp1 = VECTOR_GET(b1->content, VECTOR_SIZE(b1->content)-i);
            Bubble * temp2 = VECTOR_GET(b2->content, VECTOR_SIZE(b2->content)-i);
            if (temp1->isdouble || temp2->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val1 = _singleBubble_get_number(temp1);
            intptr_t val2 = _singleBubble_get_number(temp2);
            VECTOR_SET(temp1->content, 0, (val1-val2));
            VECTOR_PUSH(bm->content, temp1);
            VECTOR_SET(b1->content, VECTOR_SIZE(b1->content)-i, 0);
        }
    } else if (b2->isdouble) {
        // just b2 is double, b1 is single
        bm = make_doubleBubble();
        intptr_t val = _singleBubble_get_number(b1);
        size_t len = VECTOR_SIZE(b2->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }

        for (size_t i = 0; i < len; ++i) {
            Bubble * temp = VECTOR_GET(b2->content, i);
            if (temp->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val2 = _singleBubble_get_number(temp);
            intptr_t valm = (rev) ? (val-val2) : (val2-val);
            VECTOR_SET(temp->content, 0, valm);
            VECTOR_PUSH(bm->content, temp);
            VECTOR_SET(b2->content, i, 0);
        }
    } else {
        // both are single
        bm = make_singleBubble(_singleBubble_get_number(b2) -
                               _singleBubble_get_number(b1));
        
    }
    free_bubble(b1);
    free_bubble(b2);
    VECTOR_PUSH(in->bubbles->content, bm);

    return 1;
}
int _interpreter_multiply(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling mul\n");
        stop_interpreter(in,1);
    }

    //
    Bubble * b1, *b2, *bm;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_SET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1, 0);
    VECTOR_POP(in->bubbles->content);
    VECTOR_POP(in->bubbles->content);
    
    if (b1->isdouble) {
        // swap with bm as a temporary hold
        bm = b1;
        b1 = b2;
        b2 = bm;
        // now b2 is guaranteed to be a double if at least one is a double
    }

    if (b1->isdouble) {
        // both are double
        bm = make_doubleBubble();
        size_t len = VECTOR_SIZE(b2->content);
        if (len > VECTOR_SIZE(b1->content)) len = VECTOR_SIZE(b1->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }

        for (size_t i = 1; i <= len; ++i) {
            Bubble * temp1 = VECTOR_GET(b1->content, VECTOR_SIZE(b1->content)-i);
            Bubble * temp2 = VECTOR_GET(b2->content, VECTOR_SIZE(b2->content)-i);
            if (temp1->isdouble || temp2->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val1 = _singleBubble_get_number(temp1);
            intptr_t val2 = _singleBubble_get_number(temp2);
            VECTOR_SET(temp1->content, 0, (val1*val2));
            VECTOR_PUSH(bm->content, temp1);
            VECTOR_SET(b1->content, VECTOR_SIZE(b1->content)-i, 0);
        }
    } else if (b2->isdouble) {
        // just b2 is double, b1 is single
        bm = make_doubleBubble();
        intptr_t val = _singleBubble_get_number(b1);
        size_t len = VECTOR_SIZE(b2->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }

        for (size_t i = 0; i < len; ++i) {
            Bubble * temp = VECTOR_GET(b2->content, i);
            if (temp->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val2 = _singleBubble_get_number(temp);
            VECTOR_SET(temp->content, 0, (val*val2));
            VECTOR_PUSH(bm->content, temp);
            VECTOR_SET(b2->content, i, 0);
        }
    } else {
        // both are single
        bm = make_singleBubble(_singleBubble_get_number(b1) *
                               _singleBubble_get_number(b2));
        
    }
    free_bubble(b1);
    free_bubble(b2);
    VECTOR_PUSH(in->bubbles->content, bm);

    return 1;
}
int _interpreter_divide(Interpreter * in) {
    
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling 4dd\n");
        stop_interpreter(in,1);
    }

    //
    Bubble * b1, *b2, *bd, *bm, *bh;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    VECTOR_POP(in->bubbles->content);
    VECTOR_POP(in->bubbles->content);
    
    int rev = 0;
    if (b1->isdouble) {
        // swap with bm as a temporary hold
        bm = b1;
        b1 = b2;
        b2 = bm;
        rev = 1;
        // now b2 is guaranteed to be a double if at least one is a double
    }

    if (b1->isdouble) {
        // both are double, rev = 1
        size_t len = VECTOR_SIZE(b2->content);
        if (len > VECTOR_SIZE(b1->content)) len = VECTOR_SIZE(b1->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }
        bh = make_doubleBubble();
        VECTOR_RESIZE(bh->content, len);
        bh->content.size = len;

        for (size_t i = 1; i <= len; ++i) {
            Bubble * temp1 = VECTOR_GET(b1->content, VECTOR_SIZE(b1->content)-i);
            Bubble * temp2 = VECTOR_GET(b2->content, VECTOR_SIZE(b2->content)-i);
            if (temp1->isdouble || temp2->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val1 = _singleBubble_get_number(temp1);
            intptr_t val2 = _singleBubble_get_number(temp2);
            if (val2 == 0) {
                fprintf(stderr, "Divide by 0 encountered\n");
                stop_interpreter(in,1);
            }
            VECTOR_SET(temp1->content, 0, (val1/val2));
            VECTOR_SET(temp2->content, 0, (val1%val2));
            bm = make_doubleBubble();
            VECTOR_PUSH(bm->content, temp2);
            VECTOR_PUSH(bm->content, temp1);
            VECTOR_SET(bh->content, len-i, bm);
            VECTOR_SET(b1->content, VECTOR_SIZE(b1->content)-i, 0);
            VECTOR_SET(b2->content, VECTOR_SIZE(b2->content)-i, 0);
        }
    } else if (b2->isdouble) {
        // just b2 is double, b1 is single
        bh = make_doubleBubble();
        intptr_t val = _singleBubble_get_number(b1);
        size_t len = VECTOR_SIZE(b2->content);
        if (len == 0) {
            fprintf(stderr, "Cannot add using an empty double bubble\n");
            stop_interpreter(in,1);
        }
        VECTOR_RESIZE(bh->content, len);

        for (size_t i = 0; i < len; ++i) {
            Bubble * temp = VECTOR_GET(b2->content, i);
            if (temp->isdouble) {
                fprintf(stderr, "Cannot 4dd using double bubble within double bubble\n");
                stop_interpreter(in,1);
            }
            intptr_t val2 = _singleBubble_get_number(temp);

            if ((rev && val2==0) || (!rev && val==0)) {
                fprintf(stderr, "Divide by 0 encountered\n");
                stop_interpreter(in,1);
            }

            intptr_t vald = (rev) ? (val/val2) : (val2/val);
            intptr_t valm = (rev) ? (val%val2) : (val2%val);
            bd = make_singleBubble(vald);
            bm = make_singleBubble(valm);
            free_bubble(temp);
            VECTOR_SET(b2->content, i, 0);
            temp = make_doubleBubble();
            VECTOR_PUSH(temp->content, bm);
            VECTOR_PUSH(temp->content, bd);
            VECTOR_PUSH(bh->content, temp);
        }
    } else {
        // both are single
        intptr_t denom = _singleBubble_get_number(b1);
        if (denom == 0) {
            fprintf(stderr, "Divide by 0 encountered\n");
            stop_interpreter(in,1);
        }
        bd = make_singleBubble(_singleBubble_get_number(b2) / denom);
        bm = make_singleBubble(_singleBubble_get_number(b2) % denom);
        bh = make_doubleBubble();
        VECTOR_PUSH(bh->content, bm);
        VECTOR_PUSH(bh->content, bd);
    }
    VECTOR_PUSH(in->bubbles->content, bh);
    free_bubble(b1);
    free_bubble(b2);

    return 1;
}
int _interpreter_count(Interpreter * in) {
    Bubble * b;
    if (VECTOR_SIZE(in->bubbles->content) == 0) {
        b = make_singleBubble(0);
        VECTOR_PUSH(in->bubbles->content, b);
    } else {
        b = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
        if (b->isdouble) {
            b = make_singleBubble(VECTOR_SIZE(b->content));
        } else {
            b = make_singleBubble(0);
        }
        VECTOR_PUSH(in->bubbles->content, b);
    }
    return 1;
}
int _interpreter_jump(Interpreter * in, intptr_t num) {
    if (num < 0 || 32 <= num) {
        fprintf(stderr, "invalid label index encountered\n");
        stop_interpreter(in,1);
    }
    int jumpto = in->p->labels[num];
    if (jumpto < 0) {
        fprintf(stderr, "lbl %ld does not exist\n", num);
        stop_interpreter(in,1);
    }
    in->i = jumpto;
    return 1;
}

void _interpreter_skip_command(Interpreter * in) {
    intptr_t com = 0;
    if (in->i >= in->end) return;
    
    com = (intptr_t) VECTOR_GET(in->p->tokens, in->i);
    if (!isCommand(com)) {
        stop_interpreter(in,1);
    }
    
    if (requiresbits(com)) {
        ++in->i;
        if (in->i >= in->end) {
            stop_interpreter(in,1);
        }
    }

    ++in->i;

    return;
}
int _interpreter_equal_to(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling eql\n");
        stop_interpreter(in,1);
    }

    Bubble * b1, *b2;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    if (b1->isdouble || b2->isdouble) {
        fprintf(stderr, "Cannot call eql on one or more double bubbles\n");
        stop_interpreter(in,1);
    }
    if (_singleBubble_get_number(b2) != _singleBubble_get_number(b1)) {
        _interpreter_skip_command(in);
    }
    return 1;
}
int _interpreter_less_than(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling lss\n");
        stop_interpreter(in,1);
    }

    Bubble * b1, *b2;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    if (b1->isdouble || b2->isdouble) {
        fprintf(stderr, "Cannot call lss on one or more double bubbles\n");
        stop_interpreter(in,1);
    }
    if (!(_singleBubble_get_number(b2) < _singleBubble_get_number(b1))) {
        _interpreter_skip_command(in);
    }
    return 1;
}
int _interpreter_greater_than(Interpreter * in) {
    if (VECTOR_SIZE(in->bubbles->content) < 2) {
        fprintf(stderr, "Bubble abyss must have at least 2 elements when calling gr8\n");
        stop_interpreter(in,1);
    }

    Bubble * b1, *b2;
    b1 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-2);
    b2 = VECTOR_GET(in->bubbles->content, VECTOR_SIZE(in->bubbles->content)-1);
    if (b1->isdouble || b2->isdouble) {
        fprintf(stderr, "Cannot call gr8 on one or more double bubbles\n");
        stop_interpreter(in,1);
    }
    if (!(_singleBubble_get_number(b2) > _singleBubble_get_number(b1))) {
        _interpreter_skip_command(in);
    }
    return 1;
}

int run_command(Interpreter * in, intptr_t com, intptr_t num) {
    switch(com) {
        case (Token_no_op)        :
        case (Token_label)        : return 1;   // essentially skip command
        case (Token_print)        : return _interpreter_print(in);
        case (Token_print_num)    : return _interpreter_print_num(in);
        case (Token_read)         : return _interpreter_read(in);
        case (Token_read_num)     : return _interpreter_read_num(in);
        case (Token_blow)         : return _interpreter_blow(in,num);
        case (Token_submerge)     : return _interpreter_submerge(in,num);
        case (Token_pop)          : return _interpreter_pop(in);
        case (Token_duplicate)    : return _interpreter_duplicate(in);
        case (Token_surround)     : return _interpreter_surround(in,num);
        case (Token_merge)        : return _interpreter_merge(in);
        case (Token_add)          : return _interpreter_add(in);
        case (Token_subtract)     : return _interpreter_subtract(in);
        case (Token_multiply)     : return _interpreter_multiply(in);
        case (Token_divide)       : return _interpreter_divide(in);
        case (Token_count)        : return _interpreter_count(in);
        case (Token_jump)         : return _interpreter_jump(in,num);
        case (Token_equal_to)     : return _interpreter_equal_to(in);
        case (Token_less_than)    : return _interpreter_less_than(in);
        case (Token_greater_than) : return _interpreter_greater_than(in);
        case (Token_terminate)    : return 0;
    }
    return -1;
}

/////

int interpret_command(Interpreter * in) {
    intptr_t com = 0, num = 0;
    if (in->i >= in->end) return 0;
    
    com = (intptr_t) VECTOR_GET(in->p->tokens, in->i);
    if (!isCommand(com)) {
        stop_interpreter(in,1);
    }
    
    int req = requiresbits(com);
    if (req) {
        ++in->i;
        if (in->i >= in->end) {
            stop_interpreter(in,1);
        }
        num = (intptr_t) VECTOR_GET(in->p->tokens, in->i);
    }

    // run specifics of command here
    ++in->i;
    int com_success = run_command(in, com, num);
    if (com_success == -1) {
        fprintf(stderr, "Invalid command encountered\n");
        stop_interpreter(in,1);
    }
    if (com != Token_label) ++in->cycles;

    return com_success;
}

void run_interpreter(Interpreter * in, int stats) {
    if (in->bubbles) {
        free_bubble(in->bubbles);
    }
    in->bubbles = make_doubleBubble();
    in->cycles = 0;
    in->i = 0;

    // run commands
    while(interpret_command(in)) {
        if (in->max_inst > 0 && in->cycles > in->max_inst) {
            fprintf(stderr, "Timout: maximum cycles allowed was exceeded");
            stop_interpreter(in,1);
        }
    }

    putchar('\n');
    if (stats) {
        printf("-----------------------\ncommands: %d, cycles %d\n", in->p->commands, in->cycles);
        _print_bubble(in->bubbles);
        putchar('\n');
    }
    stop_interpreter(in, 0);
}

/////

void interpretParser(Parser * p, long long max_inst, int stats) {
    Interpreter in;
    make_interpreter(&in, p, max_inst);
    run_interpreter(&in, stats);
}