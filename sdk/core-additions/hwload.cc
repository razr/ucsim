/*
 * hwload.cc — runtime loader for external ucSim hardware plugins (.so).
 *
 * Adds a `loadhw <file.so>` console command (alias `insmod`) that dlopen()s a
 * plugin, verifies its ABI, constructs its cl_hw via the ucsim_hw_create()
 * factory, and does add_hw()+init() so it participates like a compile-in
 * module.
 *
 * This file is a DROP-IN addition to src/core/sim.src/ (added to sim.src's
 * object list). The `loadhw` command is registered from cl_uc::build_cmdset
 * (see the hwload.patch that accompanies this file in the ROB3 SDK dir).
 *
 * Linking: ucsim_51 must be linked with -rdynamic (so the plugin's references
 * to cl_hw/cl_uc/register_cell resolve back into the running binary) and -ldl.
 */

#include <dlfcn.h>

#include "hwloadcl.h"
#include "uccl.h"
#include "hwcl.h"
#include "globals.h"
#include "cmdutil.h"

/* Must match ucsim_hw_plugin.h in the SDK. Kept as a literal here so the core
 * does not need the SDK header on its include path. */
#ifndef UCSIM_HW_ABI
#define UCSIM_HW_ABI 1
#endif

typedef int      (*ucsim_hw_abi_fn)(void);
typedef cl_hw   *(*ucsim_hw_create_fn)(class cl_uc *);

/*
 * loadhw "FILE.so"   (alias: insmod)
 *
 * NOTE: we deliberately do NOT reuse `load`, because in stock ucSim `load` is
 * already an alias of `file` (cl_uc::build_cmdset does cmd->add_name("load")
 * on the cl_file_cmd, which loads a HEX image into ROM). A separate top-level
 * command avoids changing that behaviour.
 */
COMMAND_DO_WORK_UC(cl_loadhw_cmd)
{
  const char *fname = 0;

  if ((cmdline->param(0) == 0) ||
      ((fname = cmdline->param(0)->get_svalue()) == NULL))
    {
      con->dd_printf("Usage: loadhw \"FILE.so\"\n");
      return 0;
    }

  void *handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL);
  if (handle == NULL)
    {
      con->dd_printf("load hw: cannot open %s: %s\n", fname, dlerror());
      return 0;
    }

  dlerror(); /* clear */
  ucsim_hw_abi_fn    abi = (ucsim_hw_abi_fn)    dlsym(handle, "ucsim_hw_abi");
  ucsim_hw_create_fn mk  = (ucsim_hw_create_fn) dlsym(handle, "ucsim_hw_create");

  if (abi == NULL || mk == NULL)
    {
      con->dd_printf("load hw: %s is missing ucsim_hw_abi/ucsim_hw_create "
                     "(not a ucSim hw plugin?)\n", fname);
      dlclose(handle);
      return 0;
    }

  int pabi = abi();
  if (pabi != UCSIM_HW_ABI)
    {
      con->dd_printf("load hw: ABI mismatch for %s (plugin=%d, ucsim=%d) — "
                     "rebuild the plugin against this ucSim's SDK\n",
                     fname, pabi, UCSIM_HW_ABI);
      dlclose(handle);
      return 0;
    }

  class cl_hw *hw = mk(uc);
  if (hw == NULL)
    {
      con->dd_printf("load hw: %s factory returned NULL\n", fname);
      dlclose(handle);
      return 0;
    }

  /* Register AFTER the core hw so its read() wins on cells it registers
   * (cl_memory_cell::read() returns the last operator's value). add_hw()
   * appends, so this ordering holds for runtime-loaded modules too. */
  uc->add_hw(hw);
  hw->init();

  con->dd_printf("load hw: %s loaded (id_string=%s)\n",
                 fname, hw->id_string ? hw->id_string : "?");
  /* handle intentionally leaked for the process lifetime: the hw object
   * outlives this call. An `unload hw` command would dlclose() here. */
  return 0;
}

CMDHELP(cl_loadhw_cmd,
        "loadhw \"FILE.so\"",
        "Load an external hardware plugin (shared object)",
        "Load a ucSim hardware plugin built against the ucsim-plugin-sdk.\n"
        "The plugin must export ucsim_hw_abi() and ucsim_hw_create().\n"
        "Alias: insmod.")

/* End of hwload.cc */
