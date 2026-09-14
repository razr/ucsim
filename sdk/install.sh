#!/usr/bin/env bash
#
# install.sh — install the ucSim hardware-plugin SDK into a prefix, so external
# projects can build loadable cl_hw plugins (.so) against it with no ucSim
# source checkout on their include path.
#
# Invoked by the top-level ucSim `make install` (see Makefile.in), but also
# runnable by hand:
#
#   ./install.sh [DESTDIR] [prefix] [includedir] [datadir]
#
# Defaults match the GNU layout: prefix=/usr/local.
# Installs:
#   $(includedir)/ucsim/ucsim_hw_plugin.h        the plugin contract header
#   $(includedir)/ucsim/<core headers…>          re-exported ucSim headers
#   $(datadir)/ucsim/sdk/ucsim-plugin.mk         build fragment for plugins
#   $(datadir)/ucsim/sdk/example/                reference demo plugin
#   $(bindir? no) — none
#
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"

DESTDIR="${1:-}"
prefix="${2:-/usr/local}"
includedir="${3:-$prefix/include}"
datadir="${4:-$prefix/share}"

incdir="$DESTDIR$includedir/ucsim"
shdir="$DESTDIR$datadir/ucsim/sdk"

# Make sure the flat header dir has been exported from this same checkout.
if [ ! -f "$HERE/include/ucsim_hw_plugin.h" ]; then
  echo "sdk/include not found — running export-headers.sh first" >&2
  "$HERE/export-headers.sh"
fi

echo "installing ucSim plugin SDK:"
echo "  headers -> $incdir"
echo "  data    -> $shdir"

# Flat header dir: contract header + all re-exported core headers in one place.
mkdir -p "$incdir"
cp "$HERE/include/"*.h "$incdir/"

mkdir -p "$shdir/example"
cp "$HERE/ucsim-plugin.mk" "$shdir/"
cp "$HERE/README.md"       "$shdir/"
cp "$HERE/example/"*.cc "$HERE/example/Makefile" "$shdir/example/"

echo "done."
echo
echo "Build an external plugin against the installed SDK with a single -I:"
echo "  g++ -fPIC -shared -std=c++11 -I$includedir/ucsim mymod.cc -o mymod.so"
