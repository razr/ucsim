#!/usr/bin/env bash
#
# export-headers.sh — collect the ucSim core headers a cl_hw plugin needs into
# a single FLAT directory (./include), so external plugins compile with just
# one -I. ucSim's headers use basename-only includes (e.g. #include "stypes.h")
# and, across the four dirs a plugin needs, there are no basename collisions —
# so a flat copy resolves every include with a single include path.
#
# This SDK lives INSIDE the ucSim tree (ucsim/sdk/), so the checkout is found
# automatically as ../ — you normally run it with no arguments:
#
#   ./export-headers.sh            # -> ./include populated from ../src
#   ./export-headers.sh <ucsim>    # override the source checkout
#   ./export-headers.sh <ucsim> <out_dir>
#
# The plugin contract header (ucsim_hw_plugin.h) lives beside this script and
# is copied into the flat dir too, so a single -I covers everything.
#
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
UCSIM="${1:-$HERE/..}"
OUT="${2:-$HERE/include}"

src="$UCSIM/src"
[ -d "$src/core/sim.src" ] || { echo "not a ucSim checkout: $UCSIM" >&2; exit 1; }

rm -rf "$OUT"
mkdir -p "$OUT"

# Flat-copy the public headers from the dirs a plugin transitively needs.
# (These four have no colliding basenames — verified.) ddconfig.h lives in
# utils.src and is picked up by the glob.
for d in core/sim.src core/cmd.src core/utils.src sims/s51.src; do
  cp "$src/$d"/*.h "$OUT/"
done

# The plugin contract header.
cp "$HERE/ucsim_hw_plugin.h" "$OUT/"

echo "SDK headers exported (flat) to: $OUT   ($(ls "$OUT"/*.h | wc -l) headers)"
echo "Plugins build with a single:  -I$OUT"
