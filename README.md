[![Build Status](https://travis-ci.org/eunyoung14/mtcp.svg?branch=master)](https://travis-ci.org/eunyoung14/mtcp)
[![Build Status](https://scan.coverity.com/projects/11896/badge.svg)](https://scan.coverity.com/projects/eunyoung14-mtcp)

# README

mTCP is a highly scalable user-level TCP stack for multicore systems. 
mTCP source code is distributed under the Modified BSD License. For 
more detail, please refer to the LICENSE. The license term of the
ported applications may differ from the mTCP’s.

DPDK is the only supported packet I/O backend.

## Prerequisites

We require the following libraries to run mTCP.
- `libdpdk` (>= 20.11, discovered via `pkg-config`)
- `libnuma`
- `libpthread`
- `librt`
- `libgmp`

Building the `dpdk-iface-kmod` helper requires kernel headers.
- For Debian/Ubuntu, try ``apt-get install linux-headers-$(uname -r)``

## Included directories

mtcp: mtcp source code directory
- mtcp/src: source code
- mtcp/src/include: mTCP’s internal header files
- mtcp/lib: library file
- mtcp/include: header files that applications will use

dpdk-iface-kmod: helper LKM exporting DPDK net_device stats to the OS

apps: mTCP applications
- apps/example - example applications (see README)
- apps/perf - mTCP performance client

util: useful source code for applications

config: sample mTCP configuration files (may not be necessary)


## Install guide

mTCP builds against a system DPDK installation discovered via `pkg-config`.

1. Install DPDK (>= 20.11) so that ``pkg-config --exists libdpdk`` succeeds,
   set up hugepages, and bind your NIC to a DPDK-compatible driver
   (e.g. `vfio-pci`) using DPDK's own tooling (`dpdk-hugepages.py`,
   `dpdk-devbind.py`). See https://doc.dpdk.org for details.

2. Build and load the `dpdk-iface-kmod` helper, then register the ports.
   Intel-based interfaces will show up with a `dpdk` prefix.

    ```bash
    cd dpdk-iface-kmod
    make
    sudo insmod ./dpdk_iface.ko
    sudo make run
    cd ..
    ```

3. Bring the dpdk-registered interface up:

    ```bash
    sudo ifconfig dpdk0 x.x.x.x netmask 255.255.255.0 up
    ```

4. Build the mtcp library and example applications:

    ```bash
    autoreconf -ivf
    ./configure
    make
    ```

    - By default, mTCP assumes that there are 16 CPUs in your system.
      You can set the CPU limit, e.g. on a 32-core system, with:

        ```bash
        ./configure CFLAGS="-DMAX_CPUS=32"
        ```
    Your NIC should support RSS queues equal to the MAX_CPUS value
    (mTCP expects a one-to-one RSS queue to CPU binding).

    - checksum offloading in the NIC is ENABLED by default; pass
      ``--disable-hwcsum`` to `./configure` to turn it off.
    - check `libmtcp.a` in `mtcp/lib`
    - check header files in `mtcp/include`
    - check example binary files in `apps/example`

5. Check the configurations in `apps/example`
   - `epserver.conf` for server-side configuration
   - `epwget.conf` for client-side configuration
   - you may write your own configuration file for your application

6. Run the applications!


## Tested environments

mTCP runs on Linux-based operating systems with generic x86_64 CPUs,
but to help evaluation, we provide our tested environments as follows.

    Intel Xeon E5-2690 octacore CPU @ 2.90 GHz 32 GB of RAM (4 memory channels)
    10 GbE NIC with Intel 82599 chipset (specifically Intel X520-DA2)
    Debian 6.0.7 (Linux 2.6.32-5-amd64)

    Intel Core i7-3770 quadcore CPU @ 3.40 GHz 16 GB of RAM (2 memory channels)
    10 GbE NIC with Intel 82599 chipset (specifically Intel X520-DA2)
    Ubuntu 10.04 (Linux 2.6.32-47)

We tested the DPDK version (polling driver) with Linux-3.13.0 kernel.

## Notes

1. mTCP currently runs with fixed memory pools. That means, the size of
   TCP receive and send buffers are fixed at the startup and does not 
   increase dynamically. This could be performance limit to the large 
   long-lived connections. Be sure to configure the buffer size 
   appropriately to your size of workload.

2. The client side of mTCP supports mtcp_init_rss() to create an 
   address pool that can be used to fetch available address space in 
   O(1). To easily congest the server side, this function should be 
   called at the application startup.

3. The supported socket options are limited for right now. Please refer 
   to the mtcp/src/api.c for more detail.

4. The counterpart of mTCP should enable TCP timestamp.

5. mTCP has been tested with the following Ethernet adapters:

    1. Intel-82598       ixgbe          (Max-queue-limit: 16)
    2. Intel-82599       ixgbe          (Max-queue-limit: 16)
    3. Intel-I350        igb            (Max-queue-limit: 08)
    4. Intel-X710        i40e           (Max-queue-limit: ~)
    5. Intel-X722        i40e           (Max-queue-limit: ~)
 
## Frequently asked questions

1. How can I quit the application?
    - Use ^C to gracefully shutdown the application. Two consecutive 
    ^C (separated by 1 sec) will force quit.

2. My application doesn't use the address specified from ifconfig.
    - For some Linux distros(e.g. Ubuntu), NetworkManager may re-assign
    a different IP address, or delete the assigned IP address.

    - Disable NetworkManager temporarily if that's the case.
    NetworkManager will be re-enabled upon reboot.

     ```bash
    sudo service network-manager stop
     ```

3. Can I statically set the routing or arp table?
    - Yes, mTCP allows static route and arp configuration. Go to the 
    config directory and see sample_route.conf or sample_arp.conf. 
    Copy and adapt it to your condition and link (ln -s) the config 
    directory to the application directory. mTCP will find 
    config/route.conf and config/arp.conf for static configuration.

## Caution

1. Do not unbind the DPDK driver while running mTCP applications.
   The application will panic!

## Contacts

GitHub issue board is the preferred way to report bugs and ask questions about mTCP.

***CONTACTS FOR THE AUTHORS***

    User mailing list <mtcp-user at list.ndsl.kaist.edu>
    EunYoung Jeong <notav at ndsl.kaist.edu>
    M. Asim Jamshed <ajamshed at ndsl.kaist.edu>
