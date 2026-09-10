# Squall Reverb for drumlogue

Squall adapts the Mutable Instruments Clouds reverb core to the drumlogue `revfx` runtime.

## Adaptation

- Returns wet-only stereo reverb audio to the drumlogue send bus.
- Stores the Clouds delay memory in a static drumlogue unit buffer.
- Maps four drumlogue parameters to the Clouds reverb bridge.
- Smooths tone, depth, freeze, and scan changes before updating the reverb core.

Squall returns only processed reverb audio. Use the drumlogue's reverb send and
reverb level controls to set how much Squall is heard.

## Controls

- `TONE`: damping and brightness of the tail.
- `DEPTH`: reverb tail length and decay.
- `FREEZE`: holds the circulating reverb tail.
- `SCAN`: shapes input bleed, tail retention, damping, and diffusion while frozen.

## Dependencies

The project requires the logue SDK and Mutable Instruments Eurorack sources. It
uses `./logue-sdk` and `./eurorack` by default. Set `LOGUE_SDK_PATH` or
`EURORACK_PATH` to select other initialized checkouts.

## Build

Clone the project together with its pinned dependencies:

```sh
git clone --recurse-submodules https://github.com/DanielMajid/squall_reverb.git
cd squall_reverb
```

Set up and activate the drumlogue toolchain as described by the logue SDK, then
run:

```sh
make clean
make install
```

The project Makefile contains the complete build method from the official
drumlogue `dummy-revfx` project. A successful install writes
`Squall.drmlgunit` to this directory.
