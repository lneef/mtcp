# mTCP static library. Configuration comes from ../config.mk.
include ../config.mk

MTCP_LIB_DIR = ./lib
MTCP_LIB     = libmtcp.a
INC          = -I./include

### FLAGS ###
ifeq ($(shell uname -m),x86_64)
GCC_OPT = -m64
else
GCC_OPT =
endif
GCC_OPT += -Wall -fPIC -fgnu89-inline -fcommon
GCC_OPT += -DNDEBUG -g -O3 -DNETSTAT -DINFO -DDBGERR -DDBGCERR
GCC_OPT += -D__USRLIB__ -DMAX_CPUS=$(MAX_CPUS)

ifeq ($(LRO),1)
GCC_OPT += -DENABLELRO
endif
ifeq ($(HWCSUM),0)
GCC_OPT += -DDISABLE_HWCSUM
endif
ifeq ($(ENFORCE_RX_IDLE),1)
GCC_OPT += -DENFORCE_RX_IDLE -DRX_IDLE_THRESH=$(RX_IDLE_THRESH)
endif

# DPDK compile flags via pkg-config
CFLAGS := $(shell $(PKG_CONFIG) --cflags libdpdk)

### SOURCE CODE ###
SRCS = core.c tcp_stream.c config.c api.c eventpoll.c socket.c pipe.c \
       tcp_util.c eth_in.c ip_in.c tcp_in.c eth_out.c ip_out.c tcp_out.c \
       arp.c timer.c cpu.c rss.c addr_pool.c fhash.c memory_mgt.c logger.c debug.c \
       tcp_rb_frag_queue.c tcp_ring_buffer.c tcp_send_buffer.c tcp_sb_queue.c tcp_stream_queue.c \
       io_module.c dpdk_module.c icmp.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(MTCP_LIB_DIR)/$(MTCP_LIB)

$(MTCP_LIB_DIR)/$(MTCP_LIB): $(OBJS)
	mkdir -p $(MTCP_LIB_DIR)
	ar rvs $@ $(OBJS)

%.o: %.c ../config.mk
	$(CC) $(CFLAGS) $(GCC_OPT) $(INC) -MMD -MP -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d $(MTCP_LIB_DIR)/$(MTCP_LIB)
