# Purun

A native port of **LocoRoco 2** (PlayStation Portable) made by static recompilation: the game's PSP code is translated ahead of time into C++ and compiled for your machine, then run on a reimplementation of the PSP system software. It is not a decompilation.

> **Work in progress.** Purun is under active development and far from finished. Only the start of the game has been played, only on one platform, and you should expect bugs, crashes, missing sound effects and breaking changes — including to where it keeps its settings and saves. Back up anything you care about.

> **No game data is included.** This repository contains no disc image, no executable, no code generated from the game, and no textures, models, audio or video from it. To build or play Purun you need your own legally obtained copy of LocoRoco 2 — the European release, `UCES-01059` — as a disc image. Purun is not affiliated with, endorsed by or sponsored by Sony Interactive Entertainment or the game's developers or publishers.

It is built on [PortableKit](https://github.com/TeamGDB/PortableKit), the shared recompiler and runtime, the same way [Yakumo](https://github.com/TeamGDB/Yakumo) and [Tenkawa](https://github.com/TeamGDB/Tenkawa) are.

There is an interpreter too, but it is a development tool: it runs code the recompiler has not covered, and runs the game before any of it has been recompiled. A normal build plays entirely from recompiled code.

## Status

**The first level plays through, saves, and continues after a restart**, recompiled, at full speed — on macOS with Apple Silicon, the only platform it has been run on.

It installs from a disc image, boots, shows its logos, title intro and opening movie, and starts a new game. The first level plays to its goal, the game saves, and after a restart Continue picks up at the second level. Nothing past the start of the second level has been tried. Known problems include a hole through the middle of the main character, seams in speech bubbles, a frame rate that looks lower than it should, and sound effects that may clip; [`docs/MISSING.md`](docs/MISSING.md) has the whole list, most blocking first, and the [issues](https://github.com/TeamGDB/Purun/issues) are the work.

| | |
| --- | --- |
| Tested on | macOS on Apple Silicon (M1), Vulkan through MoltenVK. **Linux, Windows and the Steam Deck have not been tried**; PortableKit builds there, but this port has never been run there |
| Imports the game makes | 269 |
| Of those, with no implementation | 53 — run with `PURUN_LIST_STUBS=1` to see them |
| Recompiled | 19201 functions, 667264 addresses, 193 C++ units |
| Addresses the recompiler cannot lower | the game's own 154 `break` traps, and nothing else |
| Falls back to the interpreter | never, from boot into the first level (`PSPRECOMP_NO_INTERPRETER=1`) |

## What you need

- **The game**, as an ISO image of your own UMD disc of the European release (`UCES-01059`). Make it with your own PSP — for example with a homebrew UMD dumping tool, or a custom firmware's USB mode that exposes the disc — and copy it to your computer. Other releases are refused by the installer, which checks the disc id and the SHA-256 of the executable. Nobody here can give you a copy or tell you where to get one.
- **Time and memory for the first build.** The recompiled game is about 190 large C++ files; compiling them takes about 40 minutes on an M1 at `-j2`, and each one needs more than a gigabyte of memory. With `ccache` installed, later builds take seconds.
- **Build tools**, per platform:

| Platform | Packages |
| --- | --- |
| macOS (tested) | Xcode command line tools, then `brew install cmake ninja ccache python sdl3 molten-vk vulkan-loader vulkan-headers glslang` |
| Linux (untested) | A C++20 compiler, CMake 3.20+, Ninja, ccache, Python 3, `make`, pkg-config, SDL3, the Vulkan loader and headers, and `glslangValidator`. On Debian 13, for example: `build-essential cmake ninja-build ccache python3 pkg-config libsdl3-dev libvulkan-dev glslang-tools` |
| Windows (untested) | Visual Studio 2022 with the C++ workload, CMake, Ninja, Python 3, the [Vulkan SDK](https://vulkan.lunarg.com/), and SDL3 where CMake can find it (for example through `CMAKE_PREFIX_PATH`). Build from a developer command prompt |

FFmpeg, for music and movies, is downloaded and built by the build itself (on Windows an official LGPL build is downloaded instead); nothing needs installing for it.

## Building and playing

```bash
git clone --recursive https://github.com/TeamGDB/Purun.git
cd Purun
cmake -S . -B out -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out -j2                                  # the bootstrap: minutes
out/bin/PurunNative --install /path/to/your/image.iso  # check the disc and prepare the executable
scripts/generate.sh out                                # recompile the game's code to C++: minutes
cmake --build out -j2                                  # compile it: the long step
out/bin/PurunNative
```

- **Forgot `--recursive`?** Run `git submodule update --init --recursive`.
- **Keep `-j` low.** The build limits the recompiled units to one compile per 4 GiB of memory whatever `-j` says, but a high `-j` still slows everything else down.
- **`--install`** copies the disc image into Purun's data directory and prepares the executable from it; add `--in-place` to play from the image where it is. Running `PurunNative` with nothing installed opens the same setup on screen, where you can drop the image onto the window.
- **On Windows**, run `scripts/generate.sh` from Git Bash (it needs `bash`), and the program is `out\bin\PurunNative.exe`.
- **Without the generate step** the game still runs, entirely under the interpreter, roughly twenty times slower. That is for development, not for playing.
- **In the game**, Esc (or L3+R3 on a gamepad) opens the menu: controls, video, audio, saves.

PortableKit's [`docs/BUILDING.md`](https://github.com/TeamGDB/PortableKit/blob/main/docs/BUILDING.md) explains where the time goes, and [`docs/BRINGING_UP_A_GAME.md`](https://github.com/TeamGDB/PortableKit/blob/main/docs/BRINGING_UP_A_GAME.md) how to work on a port that does not run yet.

### Where it keeps things

Settings, the prepared executable, the disc image (unless installed in place) and the saves live in one data directory:

| Platform | Data directory |
| --- | --- |
| macOS | `~/Library/Application Support/Purun/UCES01059` |
| Linux | `$XDG_DATA_HOME/Purun/UCES01059`, usually `~/.local/share/Purun/UCES01059` |
| Windows | `%APPDATA%\Purun\UCES01059` |

Saves are in `ms0/PSP/SAVEDATA`, in the PSP's own format; the menu can import and export them.

### Environment variables

Every switch takes the `PURUN_` prefix, so Purun runs beside another port without sharing anything.

| Variable | What it does |
| --- | --- |
| `PURUN_DATA_DIR` | Use another data directory: a second set of settings and saves |
| `PURUN_GAME_DIR` | Play from a directory that already holds `EBOOT.ELF`, `disc.iso` and `ms0/` |
| `PURUN_PERF=log` | Print one line per second: frame rate, the game's own frame rate, speed, and where the time goes |
| `PURUN_NO_AUDIO`, `PURUN_NO_RENDER` | No sound; no window (a boot check) |
| `PURUN_LIST_STUBS=1` | List the system calls the game imports that nothing implements |
| `PURUN_TRACE_*` | Log one subsystem: `GE`, `KERNEL`, `IO`, `SAVEDATA`, `AUDIO`, `ATRAC`, `MPEG`, `PAD`, `FONT` and more |
| `PSPRECOMP_NO_INTERPRETER=1` | Stop instead of falling back to the interpreter, naming the address |

## Reporting bugs

Open an [issue](https://github.com/TeamGDB/Purun/issues/new/choose) with the bug report form. It asks for your platform, the commit you built (`git describe --always --dirty`) and the log (`out/bin/PurunNative > run.log 2>&1`). Check [`docs/MISSING.md`](docs/MISSING.md) first: the problem may already be known. **Never attach game files** — no disc images, executables, saves, generated code, or audio and video taken from the game — and remove your user name from paths in the log.

Contributions are welcome; read [CONTRIBUTING.md](CONTRIBUTING.md) first. Most fixes belong in PortableKit rather than here.

## How the port is built

This repository is a **profile**: one file of constants describing the game (`host/purun_profile.cpp`), and a short `CMakeLists.txt`. Everything else — the recompiler, the kernel, the system modules, the Vulkan renderer, audio, save data, ad hoc networking, the interface and the installer — is [PortableKit](https://github.com/TeamGDB/PortableKit), included here as the `portablekit` submodule.

## Legal disclaimer

**Purun** does not include any game assets or original game files: no disc image, no copy of the game's executable or data, and no textures, models, audio or video from the game. You must provide the files from your own legally obtained copy of LocoRoco 2 to install or build **Purun**; the installer checks that copy and accepts only the original release.

Users are solely responsible for obtaining, dumping, extracting, and using their game copy in accordance with the laws applicable in their jurisdiction.

**Purun** does not support, provide, link to, or encourage the use of unauthorized or pirated copies of the game.

**Purun** is an independent project. It is not affiliated with, endorsed by or sponsored by Sony Interactive Entertainment, or by the developers or publishers of LocoRoco 2. PlayStation and PSP are trademarks of Sony Interactive Entertainment; LocoRoco is a trademark of its owner. Any references to the original game or its trademarks are made solely for identification, compatibility, and interoperability purposes.

Screenshots and other depictions of the original game may be used solely to document or demonstrate **Purun's** functionality. All depicted third-party game content remains the property of its respective rights holders.

The license covering **Purun** applies only to the project's own original code and materials and does not grant any rights to third-party intellectual property.

**Purun** provides the software, not the game. You must provide your own legally obtained copy.

Purun's own code is MIT licensed; see [LICENSE](LICENSE). PortableKit and the third-party components it uses keep their own licences; see PortableKit's [`docs/SOURCE_PROVENANCE.md`](https://github.com/TeamGDB/PortableKit/blob/main/docs/SOURCE_PROVENANCE.md).
