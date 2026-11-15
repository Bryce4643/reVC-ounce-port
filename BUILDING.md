# Building reVC for Nintendo Switch 2

The Switch 2 target recompiles reVC against the **ounce-devkit** (libounce +
OunceVK) instead of libnx + mesa. The CMake toolchain keeps `NINTENDO_SWITCH`
true (so reVC's existing `__SWITCH__` code applies) and adds `NINTENDO_SWITCH2`
/ `__SWITCH2__`. librw is vendored in `vendor/librw` and pinned, so there are no
submodules to fetch.

## Prerequisites (one-time)

1. **devkitPro** with the Switch toolchain. Install
   [devkitPro pacman](https://devkitpro.org/wiki/devkitPro_pacman), then:
   ```sh
   sudo dkp-pacman -S --needed switch-dev switch-zlib dkp-toolchain-vars
   ```

2. **ounce-devkit** (libounce + OunceVK, the Switch 2 CMake toolchain and
   `switch2vars.sh`), installed into `$DEVKITPRO` (default `/opt/devkitpro`):
   ```sh
   # from a checkout of https://github.com/ouncebrew/ounce-devkit
   sh install.sh
   ```
   This adds `$DEVKITPRO/libounce`, `$DEVKITPRO/portlibs/switch2` (OunceVK:
   EGL/GLES/GL/Vulkan), `$DEVKITPRO/switch2vars.sh` and the Switch 2 CMake
   toolchain.

3. **Switch portlibs** reVC links against (their object code is relinked against
   libounce + OunceVK):
   ```sh
   sudo dkp-pacman -S --needed switch-glfw switch-mpg123 switch-openal-soft
   ```
   (`switch-mesa`, `switch-sdl2`, `switch-libdrm_nouveau`, `switch-zlib` come in
   as dependencies.)

4. **CMake ≥ 3.14** on `PATH`. devkitPro ships none; a clean user-space install:
   ```sh
   pipx install cmake       # exposes cmake in ~/.local/bin
   ```

> `vendor/librw` is already populated in this repo at the commit this port was
> built against (aap/librw `5501c4f`). If you ever need a pristine copy:
> `git clone https://github.com/aap/librw.git`.

## Build

```sh
./build-switch2.sh            # configure + build + package
./build-switch2.sh clean      # wipe build/ first
```

`build-switch2.sh` sources `$DEVKITPRO/switch2vars.sh` and runs CMake through the
Switch 2 wrapper with:

```
-DREVC_AUDIO=OAL -DLIBRW_PLATFORM=GL3 -DLIBRW_GL3_GFXLIB=GLFW
-DREVC_WITH_OPUS=False -DREVC_VENDORED_LIBRW=True -DREVC_INSTALL=True
```

### Outputs

- `build/src/reVC.nro` — the Switch 2 homebrew NRO
- `build/src/reVC.elf` — the ELF with symbols (keep for crash analysis)
- `build/reVC-oal-switch.tar.xz` — NRO + reVC's gamefiles, the release archive

## What is different from the Switch 1 build

All Switch 2 changes are guarded so other platforms are unaffected:

- **`src/CMakeLists.txt`** — links SDL2 (+ EGL/glapi/drm_nouveau, which resolve
  to OunceVK) in the Switch + OpenAL path, because devkitPro's
  `switch-openal-soft` implements its audio backend on top of SDL2.
- **`vendor/librw/src/gl/gl3device.cpp`** — on Switch, queries the real EGL
  surface with `eglQuerySurface` for the camera viewport and adopts it as the
  video mode, so the game fills the screen at native resolution (GLFW reported a
  smaller framebuffer than OunceVK's actual surface, which put the image in a
  corner). Re-queried per frame, so it follows dock/undock.
- **`src/skel/glfw/glfw.cpp`** — adopts that native resolution after RenderWare
  init, and wires in the logger below.
- **`src/skel/glfw/switch_log.{h,cpp}`** — new: the on-device log + crash
  handler.

## Diagnostics

The Switch build redirects stdout/stderr (unbuffered) to **`revc_log.txt`** next
to the NRO, emits `[boot]` milestones around the risky init steps, and installs
a CPU exception handler (`__libnx_exception_handler`) that dumps the fault type
and `PC/LR/SP/FAR/X0..X28`.

To map a crash `PC` back to a source line, keep the matching `reVC.elf` and run:

```sh
aarch64-none-elf-addr2line -e build/src/reVC.elf -f -C <PC - NRO_load_base>
```

`reVC.elf` is a PIE, so subtract the NRO load base (from the loader / crash
dump) from the raw `PC` first.
