# Purun

A native port of **LocoRoco 2** (PlayStation Portable) made by static recompilation: the game's PSP code is translated ahead of time into C++ and compiled for your machine, then run on a reimplementation of the PSP system software. It is not a decompilation.

It is built on [PortableKit](https://github.com/TeamGDB/PortableKit), the shared recompiler and runtime, the same way [Yakumo](https://github.com/TeamGDB/Yakumo) and [Tenkawa](https://github.com/TeamGDB/Tenkawa) are.

> **This project does not include any game assets.** You must provide the files from your own legally obtained copy of LocoRoco 2 to install or build Purun. The supported release is the European one, `UCES-01059`.

> It is not an emulator in the sense of a JIT or an interpreter at the heart of it. There *is* an interpreter, and early in a port's life it does the work: it runs the game before any of its code has been recompiled, which is how you find out what the game needs without waiting hours for a recompile first.

## Status

**Playable from the start of a new game**, recompiled, at full speed.

It installs from a disc image, boots, draws its language screen, shows its logos and title intro, takes START and New, and starts the game: the first scene's dialogue and the tutorial, where L and R tilt the world and the character rolls. Movies do not play yet, and nothing past the first minute of the tutorial has been tried.

| | |
| --- | --- |
| Imports the game makes | 269 |
| Of those, with no implementation | 68 — run with `PURUN_LIST_STUBS=1` to see them |
| Recompiled | 19201 functions, 667264 addresses, 193 C++ units |
| Addresses the recompiler cannot lower | the game's own 154 `break` traps, and nothing else |
| Falls back to the interpreter | never, from boot into the tutorial (`PSPRECOMP_NO_INTERPRETER=1`) |

[`docs/MISSING.md`](docs/MISSING.md) says what is in the way, most blocking first, and the [issues](https://github.com/TeamGDB/Purun/issues) are the work.

## How the port is built

This repository is a **profile**: one file of constants describing the game, and a short `CMakeLists.txt`. Everything else — the recompiler, the kernel, the system modules, the Vulkan renderer, audio, save data, ad hoc networking, the interface and the installer — is [PortableKit](https://github.com/TeamGDB/PortableKit), included here as a submodule.

```bash
git clone --recursive https://github.com/TeamGDB/Purun.git
cd Purun
cmake -S . -B out -G Ninja
cmake --build out -j2
out/bin/PurunNative --install /path/to/your/image.iso
out/bin/PurunNative
```

Keep `-j` low: each recompiled unit needs more than a gigabyte to compile. PortableKit's [`docs/BUILDING.md`](https://github.com/TeamGDB/PortableKit/blob/main/docs/BUILDING.md) explains where the time goes, and [`docs/BRINGING_UP_A_GAME.md`](https://github.com/TeamGDB/PortableKit/blob/main/docs/BRINGING_UP_A_GAME.md) explains how to work on a port that does not run yet.

## Legal disclaimer

**Purun** does not include any game assets or original game files: no disc image, no copy of the game's executable or data, and no textures, models, audio or video from the game. You must provide the files from your own legally obtained copy of LocoRoco 2 to install or build **Purun**; the installer checks that copy and accepts only the original release.

Users are solely responsible for obtaining, dumping, extracting, and using their game copy in accordance with the laws applicable in their jurisdiction.

**Purun** does not support, provide, link to, or encourage the use of unauthorized or pirated copies of the game.

Any references to the original game or its trademarks are made solely for identification, compatibility, and interoperability purposes.

Screenshots and other depictions of the original game may be used solely to document or demonstrate **Purun's** functionality. All depicted third-party game content remains the property of its respective rights holders.

The license covering **Purun** applies only to the project's own original code and materials and does not grant any rights to third-party intellectual property.

**Purun** provides the software, not the game. You must provide your own legally obtained copy.

MIT licensed; see [LICENSE](LICENSE).
