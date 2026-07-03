/* for I/O module def'ns */
#include "io_module.h"
/* for num_devices decl */
#include "config.h"
/* std lib funcs */
#include <stdlib.h>
/* std io funcs */
#include <stdio.h>
/* strcmp func etc. */
#include <string.h>
/* for isdigit */
#include <ctype.h>
/* for dpdk ethernet functions (get mac addresses) */
#include <rte_ethdev.h>
/* for TRACE_* */
#include "debug.h"
/* for inet_addr */
#include <arpa/inet.h>
/* for getopt()/geteuid() */
#include <unistd.h>
/*----------------------------------------------------------------------------*/
io_module_func *current_iomodule_func = &dpdk_module_func;
/*----------------------------------------------------------------------------*/
#define MIN(a, b)			((a)<(b)?(a):(b))
/*----------------------------------------------------------------------------*/
/**
 * DPDK port id encoded in an interface name, e.g. "dpdk0" -> 0.
 * Returns -1 if the name carries no trailing number.
 */
static int
get_port_index(const char *name)
{
	const char *p = name + strlen(name);

	while (p > name && isdigit((unsigned char)p[-1]))
		p--;
	if (!isdigit((unsigned char)*p))
		return -1;
	return atoi(p);
}
/*----------------------------------------------------------------------------*/
int
SetNetEnv(char *dev_name_list, char *port_stat_list)
{
	int i, j, eidx;
	int ret;

	TRACE_CONFIG("Loading interface setting\n");

	CONFIG.eths = (struct eth_table *)
			calloc(MAX_DEVICES, sizeof(struct eth_table));
	if (!CONFIG.eths) {
		TRACE_ERROR("Can't allocate space for CONFIG.eths\n");
		exit(EXIT_FAILURE);
	}

	/* STEP 1: determine the CPU mask for EAL */
	int cpu = CONFIG.num_cores;
	mpz_t _cpumask;
	char cpumaskbuf[32] = "";
	char mem_channels[8] = "";

	mpz_init(_cpumask);
	if (!mpz_cmp(_cpumask, CONFIG._cpumask)) {
		for (ret = 0; ret < cpu; ret++)
			mpz_setbit(_cpumask, ret);
		gmp_sprintf(cpumaskbuf, "%ZX", _cpumask);
	} else
		gmp_sprintf(cpumaskbuf, "%ZX", CONFIG._cpumask);
	mpz_clear(_cpumask);

	/* STEP 2: determine memory channels per socket */
	if (CONFIG.num_mem_ch == 0) {
		TRACE_ERROR("DPDK module requires # of memory channels "
			    "per socket parameter!\n");
		exit(EXIT_FAILURE);
	}
	sprintf(mem_channels, "%d", CONFIG.num_mem_ch);

	/* STEP 3: initialise EAL; every NIC bound to DPDK is auto-probed */
	char *argv[] = {"", "-c", cpumaskbuf, "-n", mem_channels,
			"--proc-type=auto"};
	int argc = sizeof(argv) / sizeof(argv[0]);

	/*
	 * re-set getopt's optind: rte_eal_init() uses getopt() internally,
	 * and mtcp applications that also parse args would otherwise crash.
	 * see man getopt(3) for more details.
	 */
	optind = 0;
	ret = rte_eal_init(argc, argv);
	if (ret < 0) {
		TRACE_ERROR("Invalid EAL args!\n");
		exit(EXIT_FAILURE);
	}

	num_devices = rte_eth_dev_count_avail();
	if (num_devices == 0) {
		TRACE_ERROR("No DPDK-bound Ethernet port found!\n");
		exit(EXIT_FAILURE);
	}
	num_queues = MIN(CONFIG.num_cores, MAX_CPUS);

	/*
	 * STEP 4: map the configured interfaces to RTE ports.
	 * dev_name_list holds one "<name> <ip> <netmask>" entry per line.
	 */
	char *list = strdup(dev_name_list);
	char *save_line;
	char *line = strtok_r(list, "\n", &save_line);

	while (line != NULL) {
		char *save_tok;
		char *name = strtok_r(line, " \t", &save_tok);
		char *ip   = name ? strtok_r(NULL, " \t", &save_tok) : NULL;
		char *mask = ip   ? strtok_r(NULL, " \t", &save_tok) : NULL;
		int portid;
		struct rte_ether_addr mac;

		if (name == NULL) {
			line = strtok_r(NULL, "\n", &save_line);
			continue;
		}
		if (ip == NULL || mask == NULL) {
			TRACE_ERROR("Interface '%s' needs an IP and netmask, e.g. "
				    "`port = %s 10.0.0.1 255.255.255.0`\n", name, name);
			exit(EXIT_FAILURE);
		}

		portid = get_port_index(name);
		if (portid < 0 || portid >= num_devices) {
			TRACE_ERROR("Interface '%s' maps to DPDK port %d, but only "
				    "%d port(s) are available.\n",
				    name, portid, num_devices);
			exit(EXIT_FAILURE);
		}

		eidx = CONFIG.eths_num++;
		strncpy(CONFIG.eths[eidx].dev_name, name,
			sizeof(CONFIG.eths[eidx].dev_name) - 1);
		CONFIG.eths[eidx].ifindex = portid;
		CONFIG.eths[eidx].ip_addr = inet_addr(ip);
		CONFIG.eths[eidx].netmask = inet_addr(mask);

		/* MAC address comes straight from the RTE port */
		rte_eth_macaddr_get(portid, &mac);
		for (j = 0; j < ETH_ALEN; j++)
			CONFIG.eths[eidx].haddr[j] = mac.addr_bytes[j];

		if (strstr(port_stat_list, name) != NULL)
			CONFIG.eths[eidx].stat_print = TRUE;

		devices_attached[num_devices_attached++] = portid;

		line = strtok_r(NULL, "\n", &save_line);
	}
	free(list);

	if (CONFIG.eths_num == 0) {
		TRACE_ERROR("No interface configured. Add `port = <dpdkN> <ip> "
			    "<netmask>` line(s) to the config.\n");
		exit(EXIT_FAILURE);
	}

	/* check if process is primary or secondary */
	CONFIG.multi_process_is_master =
		(rte_eal_process_type() == RTE_PROC_PRIMARY) ? 1 : 0;

	/* STEP 5: build the port-id -> config-index map */
	CONFIG.nif_to_eidx = (int *)calloc(MAX_DEVICES, sizeof(int));
	if (!CONFIG.nif_to_eidx)
		exit(EXIT_FAILURE);
	for (i = 0; i < MAX_DEVICES; ++i)
		CONFIG.nif_to_eidx[i] = -1;
	for (i = 0; i < CONFIG.eths_num; ++i) {
		j = CONFIG.eths[i].ifindex;
		if (j >= MAX_DEVICES) {
			TRACE_ERROR("ifindex of eths_%d exceeds the limit: %d\n", i, j);
			exit(EXIT_FAILURE);
		}
		CONFIG.nif_to_eidx[j] = i;
	}

	return 0;
}
/*----------------------------------------------------------------------------*/
int
FetchEndianType()
{
	char *argv;
	char **argp = &argv;

	if (current_iomodule_func == &dpdk_module_func) {
		(*current_iomodule_func).dev_ioctl(NULL, CONFIG.eths[0].ifindex, DRV_NAME, (void *)argp);
		if (!strcmp(*argp, "net_i40e"))
			return 1;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int
CheckIOModuleAccessPermissions()
{
	/* sudo privileges are needed for DPDK */
	if (geteuid())
		return -1;

	return 0;
}
/*----------------------------------------------------------------------------*/
