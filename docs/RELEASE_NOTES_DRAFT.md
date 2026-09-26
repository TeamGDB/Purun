# Purun 0.1.0 — draft release notes

*A draft for the first release. Nothing has been published; the maintainer decides the version, the date and whether to release.*

The first release of Purun, a native port of **LocoRoco 2** (PSP, European release `UCES-01059`) made by static recompilation on [PortableKit](https://github.com/TeamGDB/PortableKit). **You need your own copy of the game as a disc image; nothing of the game is included.**

## What works

- Setting up from a disc image, on screen or with `--install`; only the European release is accepted.
- The language screen, logos, title intro, the opening movie, New and Continue.
- The first level, played to its goal, a story scene, saving, and continuing after a restart into the second level.
- **60 frames a second**, the game's own rate, at full speed; with Video > Frame rate, frames blended up to 90, 120 or the display's rate on faster displays.
- Gamepads, keyboard, the in-game menu (Esc or L3+R3), saves in the PSP's format with import and export.

## Known problems

- Only the first level and the start of the second have been played. Anything later is untried.
- Nobody has listened to the sound: some loud moments may clip; reverb is not modelled.
- A large black silhouette in the first level may or may not be the game's own art.
- Ad hoc play is untried.
- The whole list: [`docs/MISSING.md`](MISSING.md).

## Downloads

| File | For |
| --- | --- |
| `purun-<version>-macos-arm64.dmg` | macOS 13 or newer on Apple Silicon. Signed ad hoc, not notarized: allow it once in System Settings > Privacy & Security |
| `ffmpeg-7.1.5.tar.xz` | The source of the LGPL FFmpeg the release bundles |
| `SHA256SUMS` | Checksums |

Linux (tarball, Flatpak) and Android builds are not part of this release: PortableKit's scripts can pack them, but none has been built or tried.

## Tested

macOS 27 on an M1 (8 GB): the disk image's `Purun.app` installs from the disc image, plays the title and the first level at 60 frames a second, muted, with scripted input. Not tested: other Macs, macOS 13 to 26, Linux, Windows, the Steam Deck, Android.
