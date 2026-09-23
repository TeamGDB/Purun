# What is missing

What this port does not have yet, most blocking first. Everything here was measured, not guessed: the import lists come from the game's own import table (`PURUN_LIST_STUBS=1`), the recompiler's gaps from the code it generated, and the boot behaviour from running the game and looking at the port's own frame captures.

Platform for everything below: macOS on Apple Silicon, Vulkan through MoltenVK, the game running under the interpreter with no recompiled corpus.

## Where the port is

**It installs, boots and reaches its title screen.** In order, from the captures of a bounded run:

1. The language screen: twelve flags, and the two button labels in each language in turn. Everything on it is drawn correctly.
2. ○ selects a language. The game then loads, shows its publisher and studio logos, and fades in the title screen: the background, the characters moving across it, the copyright line.
3. **Then it loops.** About every 100 frames the title fades to black and comes back, and each time the game re-opens the same file on the disc and starts its music again. That is [#1](https://github.com/TeamGDB/Purun/issues/1): the title music is a streamed ATRAC3 track, and streaming is not implemented, so the track ends before it starts.

The whole executable recompiles; see below.

### What was in the way, in order

Each was found by running the game and reading where it stopped. All of it is in the framework ([PortableKit#21](https://github.com/TeamGDB/PortableKit/pull/21)); none of it names this game:

| What | Why it was in the way |
| --- | --- |
| The older `~PSP` header layout | The tag (`0xC0CB167C`) selects a 0x90-byte key table, not a 16-byte key; the installer could not prepare the executable at all. The result is byte for byte the unencrypted `BOOT.BIN` on the same disc |
| `sceKernelLoadModule` by path | The game loads the three stock modules in `PSP_GAME/USRDIR/modules` (`psmf.prx`, `libpsmfplayer.prx`, `libfont.prx`) by path, and waited for ever on the stub's answer |
| Variable-size pools, `sceKernelMaxFreeMemSize` | The game sizes a pool as "all free memory but 1 MiB"; with a stub answering 0 that was negative, and the first allocation crashed |
| `vbfy1` | The interpreter stopped on it building a projection matrix |
| `sceFontGetCharGlyphImage_Clip` | Every piece of text is drawn through it; the language screen had no labels |
| The pad latch (`sceCtrlReadLatch`) | The game reads its buttons only through the latch; no press reached it |
| 58 NID names | Imports the framework could not name, so could not bind |

## 1. Streamed ATRAC3 music — [#1](https://github.com/TeamGDB/Purun/issues/1)

The game plays its music by streaming: `sceAtracSetHalfwayBufferAndGetID` with a 2 KiB buffer holding a RIFF WAVE header (format `0x0270`, ATRAC3, stereo, 44100 Hz, a 77 MB `data` chunk), and the rest fed in with `sceAtracAddStreamData`. The framework's ATRAC module handles a whole file already in memory, not a stream. `sceAtracSetHalfwayBufferAndGetID`, `sceAtracAddStreamData`, `sceAtracGetNextSample`, `sceAtracSetData`, `sceAtracGetAtracID` and `sceAtracReinit` are stubs. **This is what makes the title screen loop.**

## 2. Movies — [#2](https://github.com/TeamGDB/Purun/issues/2)

All fifteen `scePsmfPlayer` imports are stubs. The game creates a player during its opening, gets nothing back, and releases it again; the disc has 48 `.pmf` movies (the story scenes and the title demo). The framework has `sceMpeg` over FFmpeg but no PSMF player on top of it.

## 3. Save data — [#3](https://github.com/TeamGDB/Purun/issues/3)

At the title the game asks `sceUtilitySavedata` for **mode 11 (LIST)** of `UCES01059`/`_GameData?`, which the framework does not implement (it answers `0x80110308`), then GETSIZE for `_GameData0`. What the game does with the failed LIST has not been established.

## 4. The rest of the imports

**76 of the 269 imports** are logging stubs. Called so far, and so worth doing first:

| Call | Seen |
| --- | --- |
| `SysMemUserForUser 0xEBD5C3E6` | At start-up with `0x03090510`: the SDK version. Its name does not hash to any tried; it is harmless as a stub |
| `sceKernelGetGPI` | At start-up |
| `sceRtcGetCurrentTick` | At start-up |
| `sceDisplayWaitVblank` | At start-up |
| `sceFontGetFontList` | At start-up; the game goes on to open fonts by index regardless |
| `sceKernelDcacheWritebackInvalidateRange` | Every frame or so; nothing to do on the host |
| `scePower 0xEBD177D6` | At the title, with `333, 333, 166`: setting the clock. Does not hash to any name tried |
| `sceKernelWaitThreadEnd` | At the title, once the loop starts. **A stub that returns at once here may matter** |

The rest are imported but have not been called: all of `sceNetAdhocMatching` and five of `sceNetAdhocctl` (the game's ad hoc play), nine of `sceSasCore` (envelopes, grain, noise), the game-sharing and screenshot dialogs, `sceIoDread`, `sceIoChstat`, thread suspend and resume, and `sceUmdWaitDriveStat`.

## 5. The recompiler

**The whole executable lowers.** 19201 function seeds, 667264 code addresses, 193 C++ units, in three minutes. The only sites it cannot lower are the game's own 154 `break` instructions, which are its assertion traps. The two `vbfy1` sites were the only other gap and are lowered now.

## 6. Code overlays

**None.** The executable's section table has no overlay slots, and the only modules the game loads at run time are the three stock libraries above, which the framework serves as HLE. The game's data comes from `PSP_GAME/USRDIR/data/DATA.BIN`, read by sector range. Nothing seen so far loads code from it; that is not the same as having proved nothing does.

## 7. Not verified at all

- Anything past the title screen: START has not been seen to do anything, but the title never stays up long enough to tell.
- Sound: every run so far had audio off.
- The recompiled build.
- Linux and Windows.
- The ad hoc product code in the profile is a guess.
