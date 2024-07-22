C=gcc
CFLAGS=-g -pedantic -std=c99
CFLAGDEBUG=-Wall -Wextra -Werror -fno-omit-frame-pointer -fsanitize=address,undefined
LDLIBS=

SRCS=awac.c awai.c
DEPS=vec.c common.c awaparser.c awacompiler.c awainterpreter.c
BINS=$(SRCS:%.c=%.exe)
OBJS=$(DEPS:%.c=%.o)

all: clean $(BINS)

%.o: %.c %.h
	$(C) $(CFLAGS) -c -o $@ $<

%.exe: %.c $(OBJS)
	$(C) $(CFLAGS) -o $(patsubst %.exe,%,$@) $^ $(LDLIBS)

.PHONY: clean _clean

clean _clean:
	rm -f *.o *.awa *.awasm
	$(foreach var,$(SRCS:%.c=%), rm -f $(var))