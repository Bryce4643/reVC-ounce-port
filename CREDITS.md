# Credits

This Switch 2 port is a thin layer on top of a large amount of other people's
work. All credit for the engine, the renderer and the homebrew platform goes to
the projects below.

## reVC / re3 — the GTA decompilation

The entire game is **reVC**, the reverse-engineered source port of GTA: Vice
City (the `miami` branch of **re3**), by the **GTAmodding / re3 team** —
including **aap, Fire_Head, shfil, erorcun, Nick007J, Serge** and many other
contributors.

The original repository (`github.com/GTAmodding/re3`) and its reVC releases were
removed from GitHub following a DMCA takedown, so that link no longer resolves.

## librw — the RenderWare reimplementation

Rendering is handled by **librw**, by **aap**
(https://github.com/aap/librw). It is vendored in `vendor/librw`, pinned at
commit `5501c4f` (2021-08-19), with a small Switch-2 resolution fix on top.

## reVC Nintendo Switch (Switch 1) port

This Switch 2 build is a recompile of the earlier **reVC Nintendo Switch
(Switch 1) port**, which established the `__SWITCH__` code paths, the GLFW + GL3
+ OpenAL configuration and the CMake Switch target that this work builds on. The
original project page / download links are no longer available online.

<!-- If you know the specific author(s) of the Switch 1 port, add their name and
     a working link here so they get proper credit. -->

## kaiHEN and the ounce-devkit — the Switch 2 platform

Running and building homebrew on Switch 2 is made possible by **kaiHEN** and the
**ounce-devkit** (libounce, OunceVK, libkaihen), by the **ouncebrew / kaiHEN
contributors** (https://github.com/ouncebrew).

## Switch 2 port

- Recompile for Switch 2, OunceVK resolution fix, on-device logging/crash
  handler, build scripting and packaging: **_&lt;add your name/handle here&gt;_**

## Assets

GTA: Vice City and all of its data are © Rockstar Games / Take-Two Interactive.
No game assets are included in this project; players must supply their own from a
copy they own.
