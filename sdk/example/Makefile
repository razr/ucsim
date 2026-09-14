# Build the minimal demo loadable ucSim plugin (demo_hw.so).
#
# Works both in-tree (ucsim/sdk/example, after ../export-headers.sh) and against
# an INSTALLED SDK (share/ucsim/sdk/example). The included fragment auto-detects
# which layout it is in and sets the single -I accordingly.
#
#   make            # builds demo_hw.so
#   make clean

# Point SDK at the directory that holds ucsim-plugin.mk (the parent of this
# example dir in both layouts).
SDK ?= ..
include $(SDK)/ucsim-plugin.mk

SRCS = demo_hw.cc
OUT  = demo_hw.so

$(OUT): $(SRCS)
	$(UCSIM_PLUGIN_LINK)

clean:
	rm -f $(OUT)

.PHONY: clean
