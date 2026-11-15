# Notice / licensing

This repository combines several bodies of work under different terms. Please
respect all of them.

## reVC / re3 engine code (most of `src/`, `gamefiles/`)

The reVC authors did not place a formal license on the code. Their statement:

> We don't feel like we're in a position to give this code a license.
> The code should only be used for educational, documentation and modding
> purposes. We do not encourage piracy or commercial use. Please keep derivate
> work open source and give proper credit.

This Switch 2 port is published in that same spirit: for **preservation,
education and homebrew on hardware you own**. It is **not** for commercial use
and **must not** be used to enable piracy.

The original reVC/re3 repositories were removed from GitHub following a DMCA
takedown. Anyone mirroring or redistributing this code should understand it may
be subject to the same.

## librw (`vendor/librw/`)

MIT License, Copyright (c) 2014 aap. See
[`vendor/librw/LICENSE`](vendor/librw/LICENSE). The Switch-2 changes in
`vendor/librw/src/gl/gl3device.cpp` are offered under the same MIT terms.

## Switch 2 port changes

The Switch-2-specific additions authored for this port — `switch_log.{h,cpp}`,
the resolution and build changes, `build-switch2.sh` and the documentation —
follow the license of the file they live in: MIT for the librw change, and the
reVC authors' educational-use terms above for the reVC changes. The standalone
new files (`switch_log.*`, scripts, docs) are released under MIT.

## Game assets — NOT included

*Grand Theft Auto: Vice City* and all of its data, trademarks and artwork are
© **Rockstar Games / Take-Two Interactive**. **No game assets are included in
this repository or its release archives.** You must own a legitimate copy of the
game and provide its files yourself.

## Homebrew / warranty

Running homebrew modifies your console and carries risk. This software is
provided "as is", without warranty of any kind. You are responsible for your own
hardware.
