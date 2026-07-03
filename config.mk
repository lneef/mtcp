# mTCP build configuration.
# Edit the values below, then run `make` from the top level.
# (Command-line overrides also work, e.g. `make MAX_CPUS=32`.)

# C compiler (defaults to gcc unless you set CC in the environment or on
# the command line; make's built-in default of "cc" is not used).
ifeq ($(origin CC),default)
CC = gcc
endif

# Feature toggles (1 = on, 0 = off)
HWCSUM          ?= 1    # NIC hardware checksum offload
LRO             ?= 0    # DPDK large-receive-offload (for relevant NICs)
ENFORCE_RX_IDLE ?= 0    # enforce an rx-idle timeout
RX_IDLE_THRESH  ?= 0    # idle cycles before the timeout fires (needs ENFORCE_RX_IDLE=1)

# Number of CPU cores mTCP may use.
# The NIC must expose at least this many RSS queues.
MAX_CPUS ?= 16

# DPDK (>= 20.11) is located via pkg-config.
PKG_CONFIG ?= pkg-config
