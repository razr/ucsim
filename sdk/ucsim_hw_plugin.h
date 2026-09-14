/*
 * ucsim_hw_plugin.h — contract header for LOADABLE ucSim hardware modules.
 *
 * This is the one genuinely-new API in the "external module" scheme. A plugin
 * is an ordinary `cl_hw` subclass (exactly like a compile-in module) compiled
 * into a shared object (.so) instead of linked into ucsim_51. At runtime the
 * `loadhw <file.so>` console command (alias `insmod`) dlopen()s the file,
 * checks the ABI, and calls the factory below to construct + register it.
 *
 * A conforming plugin MUST export these two C symbols (unmangled, so the
 * loader can dlsym() them by name):
 *
 *     extern "C" int    ucsim_hw_abi(void);              // return UCSIM_HW_ABI
 *     extern "C" cl_hw *ucsim_hw_create(class cl_uc *uc); // new your_cl_hw(uc)
 *
 * IMPORTANT — this crosses a C++ ABI boundary. cl_hw / cl_uc / cl_memory_cell
 * are C++ classes with vtables; a plugin .so is only compatible with a
 * ucsim_51 built from the SAME headers, SAME compiler/ABI, SAME ./configure
 * options. The ucsim_hw_abi() gate is a coarse guard, NOT a compatibility
 * guarantee: bump UCSIM_HW_ABI in this header whenever the exposed class
 * surface changes, and rebuild plugins against the matching SDK.
 *
 * Build a plugin (no linking against the ucSim core — symbols resolve at
 * dlopen time from the -rdynamic ucsim_51). The exported header dir is FLAT,
 * so a single -I covers everything; the simplest way is to include the
 * installed fragment ucsim-plugin.mk in your Makefile, or by hand:
 *
 *     g++ -fPIC -shared -std=c++11 -I<sdk-include> mymod.cc -o mymod.so
 *
 * See README.md, example/ and modules/ in this directory.
 */
#ifndef UCSIM_HW_PLUGIN_H
#define UCSIM_HW_PLUGIN_H

/* Bump on ANY change to the exposed cl_hw / cl_uc class surface. */
#define UCSIM_HW_ABI 1

/* Pull in the base class the plugin subclasses. These headers come from the
 * ucSim source tree, re-exported into this SDK (see export-headers.sh). */
#include "hwcl.h"
#include "uccl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return UCSIM_HW_ABI. The loader rejects the plugin if this does not match
 * the value compiled into ucsim_51. */
int ucsim_hw_abi(void);

/* Construct the plugin's cl_hw. The loader then does add_hw()+init() on it;
 * do NOT call add_hw()/init() yourself here. Return NULL on failure. */
class cl_hw *ucsim_hw_create(class cl_uc *uc);

#ifdef __cplusplus
}
#endif

/* Convenience so a plugin .cc can just write UCSIM_HW_PLUGIN(cl_myhw). */
#define UCSIM_HW_PLUGIN(CLASS)                                            \
  extern "C" int        ucsim_hw_abi(void)    { return UCSIM_HW_ABI; }    \
  extern "C" class cl_hw *ucsim_hw_create(class cl_uc *uc)                \
                                              { return new CLASS(uc); }

#endif /* UCSIM_HW_PLUGIN_H */
