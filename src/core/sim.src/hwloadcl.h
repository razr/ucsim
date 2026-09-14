/*
 * hwloadcl.h — declaration of the `load hw` command class.
 *
 * Drop into src/core/sim.src/ alongside hwload.cc.
 */
#ifndef SIM_HWLOAD_CL_HEADER
#define SIM_HWLOAD_CL_HEADER

#include "newcmdcl.h"

/* UC-scoped command: do_work(cl_uc *uc, cmdline, con). */
COMMAND_ON(uc, cl_loadhw_cmd);

#endif /* SIM_HWLOAD_CL_HEADER */
