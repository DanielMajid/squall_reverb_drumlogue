# Squall Drumlogue Reverb Unit

`Squall` is a port of Mutable Instruments Clouds reverb for the Korg Drumlogue.

Ported by Daniel Majid Mirzakhani.

Original Clouds DSP copyright Émilie Gillet.


# Controls

- `TONE`: damping and brightness of the tail.
- `DEPTH`: reverb tail length and decay.
- `FREEZE`: holds the circulating reverb tail.
- `SCAN`: shapes input bleed, tail retention, damping, and diffusion while frozen.

# Dependencies

The project requires the logue SDK and Mutable Instruments Eurorack sources. It
uses `./logue-sdk` and `./eurorack` by default.

# Build

From the project folder, initialize the dependencies:

```sh
git submodule update --init --recursive
```

Make sure Docker is running, then download the build image if needed:

```sh
docker pull xiashj/logue-sdk:latest
```

Compile and package the unit:

```sh
./logue-sdk/docker/run_cmd.sh --platform=. build -f --drumlogue .
```

See Korg’s [logue SDK Docker build instructions](https://github.com/korginc/logue-sdk/blob/main/docker/README.md) for the complete build-environment documentation.
