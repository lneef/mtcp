#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mtcp], [3], [mtcp-user@list.ndsl.kaist.edu])
AC_CONFIG_SRCDIR([mtcp/eventpoll.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_LANG(C)
AC_DISABLE_OPTION_CHECKING

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER([linux/sched.h],,AC_MSG_ERROR([Could not find linux/sched.h]))
AC_CHECK_HEADER([pthread.h],,AC_MSG_ERROR([Could not find pthread.h]))
AC_CHECK_HEADER([numa.h],,AC_MSG_ERROR([Could not find numa.h]))
AC_CHECK_HEADER([arpa/inet.h],,AC_MSG_ERROR([Could not find arpa/inet.h]))
AC_CHECK_HEADER([fcntl.h],,AC_MSG_ERROR([Could not find fcntl.h]))
AC_CHECK_HEADER([limits.h],,AC_MSG_ERROR([Could not find limits.h]))
AC_CHECK_HEADER([netdb.h],,AC_MSG_ERROR([netdb.h]))
AC_CHECK_HEADER([netinet/in.h],,AC_MSG_ERROR([Could not find netinet/in.h]))
AC_CHECK_HEADER([stdint.h],,AC_MSG_ERROR([Could not find stdint.h]))
AC_CHECK_HEADER([stdlib.h],,AC_MSG_ERROR([Could not find stdlib.h]))
AC_CHECK_HEADER([string.h],,AC_MSG_ERROR([Could not find string.h]))
AC_CHECK_HEADER([sys/ioctl.h],,AC_MSG_ERROR([Could not find sys/ioctl.h]))
AC_CHECK_HEADER([sys/socket.h],,AC_MSG_ERROR([Could not find sys/socket.h]))
AC_CHECK_HEADER([sys/time.h],,AC_MSG_ERROR([Could not find sys/time.h]))
AC_CHECK_HEADER([unistd.h],,AC_MSG_ERROR([Could not find unistd.h]))
AC_CHECK_HEADER([gmp.h],,AC_MSG_ERROR([Could not find gmp.h]))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNC([clock_gettime],,AC_MSG_ERROR([librt library is missing]))
AC_CHECK_FUNCS([bzero getpagesize gettimeofday memmove memset munmap select socket strchr strerror strstr strtol],,AC_MSG_ERROR([glibc library is missing]))

# DPDK is the only supported packet I/O backend, located via pkg-config.
AC_SUBST(DPDK, 1)
# h/w checksum offloading (enabled by default)
AC_SUBST(HWCSUM, 1)
# dpdk LRO (disabled by default)
AC_SUBST(LRO, 0)
# rx-idle enforcement (disabled by default)
AC_SUBST(ENFORCE_RX_IDLE, 0)
AC_SUBST(RX_IDLE_THRESH, 0)

# Locate DPDK via pkg-config.
PKG_CONFIG=${PKG_CONFIG:-pkg-config}
AC_MSG_CHECKING([for libdpdk via pkg-config])
if $PKG_CONFIG --exists libdpdk; then
	AC_MSG_RESULT([$($PKG_CONFIG --modversion libdpdk)])
else
	AC_MSG_ERROR([libdpdk not found via pkg-config. Install DPDK or set PKG_CONFIG_PATH.])
fi

dnl how many idle cycles before enforcing rx timeout (dpdk-only)
AC_ARG_WITH(rx-idle, [  --with-rx-idle[=ARG]	how many idle cycles before enforcing timeout])
if test "$with_rx_idle" != ""
then
	AC_SUBST(ENFORCE_RX_IDLE, 1)
	AC_SUBST(RX_IDLE_THRESH, $with_rx_idle)
fi

dnl Example of default-disabled feature
AC_ARG_ENABLE([lro],
	AS_HELP_STRING([--enable-lro], [Enable dpdk lro (for relevant NICs)]))

if test "x$enable_lro" = "xyes"
then
	AC_SUBST(LRO, 1)
fi

dnl Example of default-enabled feature
AC_ARG_ENABLE([hwcsum],
	AS_HELP_STRING([--disable-hwcsum], [Disable h/w-based checksum offloading (for relevant NICs)]))

if test "x$enable_hwcsum" = "xno"
then
	AC_SUBST(HWCSUM, 0)
fi

AM_INIT_AUTOMAKE(mtcp, 3)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT(mtcp/Makefile)
AC_OUTPUT(apps/example/Makefile)
AC_OUTPUT(apps/perf/Makefile)
AC_OUTPUT(util/Makefile)
