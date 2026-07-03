# Helper objects used by the example apps. Configuration from ../config.mk.
include ../config.mk

ifeq ($(shell uname -m),x86_64)
GCC_OPT = -m64
else
GCC_OPT =
endif
GCC_OPT += -Wall -fPIC -fcommon
GCC_OPT += -DNDEBUG -O3 -DNETSTAT -DINFO -DDBGERR -DDBGCERR

CFLAGS = -I./include/

SRCS = tdate_parse.c http_parsing.c netlib.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(OBJS)

%.o: %.c
	$(CC) $(GCC_OPT) $(CFLAGS) -c $< -o $@

clean:
	rm -f *~ *.o
