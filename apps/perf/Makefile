# Performance client. Configuration from ../../config.mk.
# Not built by the top-level make; run `make -C apps/perf` after building mtcp.
include ../../config.mk

MTCP_FLD = ../../mtcp
MTCP_LIB = $(MTCP_FLD)/lib/libmtcp.a

UTIL_FLD = ../../util
UTIL_OBJ = $(UTIL_FLD)/http_parsing.o $(UTIL_FLD)/tdate_parse.o $(UTIL_FLD)/netlib.o

CFLAGS  = -g -O3 -Wall -fgnu89-inline -fcommon -DMAX_CPUS=$(MAX_CPUS)
CFLAGS += $(shell $(PKG_CONFIG) --cflags libdpdk)
INC     = -I./include -I$(UTIL_FLD)/include -I$(MTCP_FLD)/include
LIBS    = -pthread -lrt $(MTCP_LIB) -lnuma -ldl -lgmp -lm \
          $(shell $(PKG_CONFIG) --libs libdpdk)

ifeq ($(shell uname -m),x86_64)
CFLAGS += -m64
LIBS   += -m64
endif

.PHONY: all clean

all: client

client: client.o $(MTCP_LIB)
	$(CC) $< $(LIBS) $(UTIL_OBJ) -o $@

client.o: client.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f *~ *.o client log_*
