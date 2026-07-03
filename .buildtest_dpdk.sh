#!/usr/bin/env bash
# Compile smoke-test: builds libmtcp and the example apps against the
# system DPDK located via pkg-config (DPDK >= 20.11).
set -e

pkg-config --exists libdpdk || {
	echo "libdpdk not found via pkg-config; install DPDK or set PKG_CONFIG_PATH" >&2
	exit 1
}

make
