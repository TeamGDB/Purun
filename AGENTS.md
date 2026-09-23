# AGENTS.md

Working notes for anyone changing this repository: people and coding agents alike. Read this first.

Purun is a native port of *LocoRoco 2* (the European release, `UCES-01059`), built on [PortableKit](https://github.com/TeamGDB/PortableKit). **Almost nothing lives here.** The recompiler, the kernel, the system modules, the renderer, audio, save data, ad hoc networking, the interface and the installer are all in the framework, which is the `portablekit` submodule. This repository is the profile: `host/purun_profile.cpp`, a short `CMakeLists.txt`, and the documentation for this port.

Read [PortableKit's AGENTS.md](portablekit/AGENTS.md) too; its rules apply here.

## Where a change belongs

This is the first question for every change, and getting it wrong is how a framework stops being one.

- **It names this game, its addresses, its folders or its behaviour** → here, in the profile.
- **It is true of the PSP, of a file format, or of any game that makes the same call** → PortableKit, on a branch of its own with its own pull request.

An unimplemented system call is almost always the framework's. The older `~PSP` header layout this game's executable uses was found here and implemented in the framework, because there is nothing about this game in it; only the key table for its tag is in the profile.

If this repository ever grows its own kernel code, its own renderer changes or a copy of something in the framework, a seam is missing. Add the seam.

## The state of the port

[`docs/MISSING.md`](docs/MISSING.md) says what is missing, measured rather than guessed, most blocking first; the [issues](https://github.com/TeamGDB/Purun/issues) are the work. Keep both current as the port moves — a finding that lives only in a session's output is lost.

## Rules

- **English only** in everything committed.
- **Branch and pull request.** Never push to `main`.
- **No game data, ever.** No disc image, no executable, no generated code, no overlay corpora, no saves, no captures of the game's own art. `.gitignore` covers `game/`, `generated/`, `overlays/`, `analysis/` and `out/`. The same goes for anything personal: home paths, user names, machine names, addresses.
- **Write it yourself.** Constants, offsets and format facts from public sources are fine; source from a project whose licence is incompatible with MIT is not.
- **Say what you did not verify.**

## Building and running

```bash
git clone --recursive https://github.com/TeamGDB/Purun.git
cmake -S . -B out -G Ninja
cmake --build out -j2                      # keep -j low: each unit needs >1 GiB
out/bin/PurunNative --install /path/to/your/image.iso
scripts/generate.sh out                    # recompile the executable
cmake --build out -j2                      # compile the result: hours
```

**Do not wait for that last step before working on the port.** With no corpus the port runs the whole game under the interpreter, which reaches the same kernel, HLE and renderer, so the first run already names the system calls the game needs. PortableKit's [`docs/BRINGING_UP_A_GAME.md`](portablekit/docs/BRINGING_UP_A_GAME.md) is the method; the loop is: run, read the last `[hle-stub]` line before it stops, implement that call in the framework, run again.

```bash
PURUN_NO_RENDER=1 PURUN_NO_AUDIO=1 timeout 90 out/bin/PurunNative
```

**Bound every run with `timeout`** (`timeout -s KILL` for a run with a window). Never leave the game running, and never drive it with an open-ended input loop: it does not converge, and someone may be watching the screen.

Every switch takes this port's own prefix — `PURUN_TRACE_GE`, `PURUN_DATA_DIR`, `PURUN_INPUT_SCRIPT` — so it can run beside another port without sharing anything.

## Things in the profile that are guesses

They are marked as such in `host/purun_profile.cpp`. Do not quietly rely on them:

- `adhoc_product_code` is set to the disc id, which is a guess.
- `save_folders` names only the folder the executable itself names; whether the game writes anything else has not been seen.

## Trace the game; do not recall the hardware

GE register numbers, PSP struct layouts and HLE semantics taken from memory have been wrong before in this codebase, and each wrong guess cost a debugging session. Turn on the relevant `PURUN_TRACE_*` switch and read what the game actually does. This matters more here than in a finished port, because nothing about this game's behaviour has been established yet.
