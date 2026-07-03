# Top-level build. Tunables live in config.mk.
include config.mk

.PHONY: all clean mtcp util apps

all: apps

mtcp:
	$(MAKE) -C mtcp

util:
	$(MAKE) -C util

# The example apps link against libmtcp.a and the util objects.
apps: mtcp util
	$(MAKE) -C apps/example

clean:
	$(MAKE) -C mtcp clean
	$(MAKE) -C util clean
	$(MAKE) -C apps/example clean
