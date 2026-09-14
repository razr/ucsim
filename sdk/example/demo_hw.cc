/*
 * demo_hw.cc — a minimal, self-contained loadable ucSim hw plugin used to
 * prove the loader end-to-end with a UNIQUE id_string (so it does not collide
 * with any compiled-in module during testing).
 *
 * It registers no cells and does nothing to the sim; it only responds to
 *   set hardware demohw <anything>
 * so we can confirm the runtime-loaded module's set_cmd is reachable.
 */
#include <stdio.h>
#include "ucsim_hw_plugin.h"
#include "stypes.h"

class cl_demo_hw: public cl_hw
{
public:
  cl_demo_hw(class cl_uc *auc): cl_hw(auc, HW_DUMMY, 0, "demohw") {}
  virtual int init(void) { cl_hw::init(); return 0; }
  virtual bool set_cmd(class cl_cmdline *cmdline, class cl_console_base *con)
  {
    con->dd_printf("demohw: set_cmd reached (runtime-loaded plugin OK)\n");
    return true;
  }
  virtual void print_info(class cl_console_base *con)
  {
    con->dd_printf("%s[%d]  (loadable demo plugin)\n", id_string, id);
  }
};

UCSIM_HW_PLUGIN(cl_demo_hw)

/* End of demo_hw.cc */
