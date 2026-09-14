# ucsim-plugin.mk — shared build fragment for external ucSim hw plugins.
#
# Two ways to use it, depending on whether you build against an INSTALLED SDK
# (the normal case, `make install` in ucSim) or an in-tree ucsim/sdk/.
#
# 1) Installed SDK (flat headers under $(includedir)/ucsim):
#
#      UCSIM_PREFIX ?= /usr/local
#      include $(UCSIM_PREFIX)/share/ucsim/sdk/ucsim-plugin.mk
#      SRCS = mymod.cc
#      mymod.so: $(SRCS) ; $(UCSIM_PLUGIN_LINK)
#
# 2) In-tree SDK (ucsim/sdk/, headers exported into ./include):
#
#      SDK ?= /path/to/ucsim/sdk
#      include $(SDK)/ucsim-plugin.mk
#      ...
#
# Because the exported header dir is FLAT (no colliding basenames), a plugin
# needs exactly ONE include path. The fragment auto-detects which layout it
# lives in by checking for the installed contract header next to itself.

CXX      ?= g++
CXXFLAGS ?= -fPIC -shared -std=c++11 -Wall -Wno-unused

# Where this fragment lives, resolved at include time.
UCSIM_PLUGIN_MK := $(lastword $(MAKEFILE_LIST))
UCSIM_MK_DIR    := $(patsubst %/,%,$(dir $(UCSIM_PLUGIN_MK)))

# Installed layout: $(datadir)/ucsim/sdk/ucsim-plugin.mk with flat headers at
# $(includedir)/ucsim. Derive includedir from the datadir this file sits in:
#   .../share/ucsim/sdk  ->  .../include/ucsim
UCSIM_INSTALL_INC := $(abspath $(UCSIM_MK_DIR)/../../../include/ucsim)

ifneq ($(wildcard $(UCSIM_INSTALL_INC)/ucsim_hw_plugin.h),)
  # ---- installed SDK: single flat include dir ----
  UCSIM_PLUGIN_INCLUDES = -I$(UCSIM_INSTALL_INC)
else
  # ---- in-tree SDK (run ./export-headers.sh first): single flat include dir ----
  SDK ?= $(UCSIM_MK_DIR)
  UCSIM_PLUGIN_INCLUDES = -I$(SDK)/include
endif

# Convenience recipe: compiles $(SRCS) into $@ as a plugin .so.
UCSIM_PLUGIN_LINK = $(CXX) $(CXXFLAGS) $(UCSIM_PLUGIN_INCLUDES) $(SRCS) -o $@
