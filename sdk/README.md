# ucSim hardware-plugin SDK

Build ucSim hardware peripherals as **external shared objects (`.so`)** and load
them at runtime with the `loadhw` console command (alias `insmod`) — instead of
compiling every peripheral into `ucsim_51`. Think "kernel modules for ucSim".

This SDK is part of the ucSim tree and is **installed by `make install`**, so
external projects can build plugins against it without a ucSim source checkout
on their include path.

## What's here

```
sdk/
├── README.md              # this file
├── ucsim_hw_plugin.h      # the plugin CONTRACT header (the only new API)
├── ucsim-plugin.mk        # build fragment external plugin Makefiles include
├── export-headers.sh      # re-export ucSim core headers into ./include
├── install.sh             # install headers + fragment + example into a prefix
└── example/               # a reference plugin built against the SDK
    ├── demo_hw.cc         #   minimal unique-id plugin (loader proof)
    └── Makefile
```

The runtime **loader** itself (the `loadhw`/`insmod` command plus the
`-rdynamic`/`-ldl` link flags) is compiled into `ucsim_51` from
`src/core/sim.src/hwload.cc`; see the SDK's `WIRING.md` history for how that was
added.

## Install (part of ucSim)

```bash
cd ucsim
./configure --prefix=/usr/local
make
sudo make install          # installs ucsim_51 AND this SDK
```

`make install` runs `sdk/install.sh`, which places:

- `$(includedir)/ucsim/ucsim_hw_plugin.h` + the re-exported core headers
- `$(datadir)/ucsim/sdk/ucsim-plugin.mk`, `README.md`, and `example/`

You can also install the SDK alone: `./export-headers.sh && ./install.sh`.

## Writing a plugin

Subclass `cl_hw` as usual, then add the factory with one macro:

```cpp
#include "ucsim_hw_plugin.h"
#include "mymodcl.h"

// class cl_mymod : public cl_hw { ... };   // your peripheral

UCSIM_HW_PLUGIN(cl_mymod)                    // exports the two C entry points
```

Build against the **installed** SDK by including the fragment:

```make
UCSIM_PREFIX ?= /usr/local
include $(UCSIM_PREFIX)/share/ucsim/sdk/ucsim-plugin.mk
SRCS = mymod.cc
mymod.so: $(SRCS) mymodcl.h ; $(UCSIM_PLUGIN_LINK)
```

or by hand (the installed header dir is flat, so a single `-I`):

```bash
g++ -fPIC -shared -std=c++11 \
    -I/usr/local/include/ucsim \
    mymod.cc -o mymod.so
```

## Load it at runtime

```
ucsim_51 -t 51
> loadhw "mymod.so"            # alias: insmod
load hw: .../mymod.so loaded (id_string=mymod)
> set hardware mymod ...
```

## Known limitations (be honest about these)

- **C++ ABI boundary — plugin and `ucsim_51` must be built in lockstep.**
  `cl_hw`/`cl_uc` are C++ classes with vtables; a `.so` only works with a
  `ucsim_51` built from the same headers/compiler/`./configure` options. The
  `ucsim_hw_abi()` check catches gross mismatches, not subtle ones. Reinstall
  the SDK and rebuild plugins after a ucSim change.
- **Don't also compile the same module in.** `set hardware <name>` requires a
  unique `id_string` prefix match; a module both compiled in *and* loaded
  resolves to "no hw". Pick one.
- **No `unload` yet.** The loader keeps the `dlopen` handle for the process
  lifetime.
- **Load order.** Runtime-loaded modules register after the core hw (last
  `read()` wins). Load before `run`/`reset` if the module must see the first
  fetch.
