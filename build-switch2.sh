#!/usr/bin/env bash
# Build reVC (GTA Vice City decompilation) for Nintendo Switch 2 / kaiHEN.
#
# This recompiles the existing Switch 1 reVC port for Switch 2 using the
# ounce-devkit: libounce replaces libnx, and OunceVK provides the EGL/OpenGL(ES)
# driver for the Switch 2 GPU. The CMake toolchain keeps NINTENDO_SWITCH true
# (so reVC's existing __SWITCH__ code applies) and adds NINTENDO_SWITCH2 /
# __SWITCH2__.
#
# Prerequisites (one-time), see PORTING-SWITCH2.md for details:
#   1. devkitPro with the Switch (devkitA64) toolchain + switch-zlib.
#   2. ounce-devkit installed into $DEVKITPRO  (libounce + portlibs/switch2 /
#      OunceVK + switch2vars.sh + libounce/cmake/Switch2.cmake).
#   3. Switch portlibs whose object code we relink against OunceVK:
#        dkp-pacman -S switch-glfw switch-mpg123 switch-openal-soft
#      (switch-mesa, switch-sdl2, switch-zlib come in as dependencies).
#   4. A CMake >= 3.14 on PATH (e.g. `pipx install cmake`).
#   5. The vendored renderer: vendor/librw must be populated
#        git clone https://github.com/aap/librw.git vendor/librw
#      (this tree was built against librw commit 5501c4f, Aug 2021).
#
# Usage:  ./build-switch2.sh [clean]
set -eu

HERE="$(cd "$(dirname "$0")" && pwd)"
export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"

# pipx-installed cmake lives in ~/.local/bin; make sure it is reachable.
export PATH="$HOME/.local/bin:$PATH"

# Switch 2 build environment (sets CFLAGS/LDFLAGS, PATH to the switch2 wrapper,
# -D__SWITCH__ -D__SWITCH2__, OunceVK include/lib paths).
# shellcheck disable=SC1091
. "${DEVKITPRO}/switch2vars.sh"

if [ "${1:-}" = "clean" ]; then
    rm -rf "${HERE}/build"
fi

# aarch64-none-elf-cmake here is the switch2 wrapper (first on PATH): it sets
# CMAKE_TOOLCHAIN_FILE to libounce/cmake/Switch2.cmake.
aarch64-none-elf-cmake -S "${HERE}" -B "${HERE}/build" \
    -DREVC_AUDIO=OAL \
    -DLIBRW_PLATFORM=GL3 \
    -DLIBRW_GL3_GFXLIB=GLFW \
    -DREVC_WITH_OPUS=False \
    -DREVC_VENDORED_LIBRW=True \
    -DREVC_INSTALL=True \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build "${HERE}/build" --parallel

# Package NRO + gamefiles into reVC-oal-switch.tar.xz (same layout as the
# upstream Switch 1 release).
( cd "${HERE}/build" && cpack )

echo
echo "Done. Outputs:"
echo "  NRO     : ${HERE}/build/src/reVC.nro"
echo "  Package : ${HERE}/build/reVC-oal-switch.tar.xz"
