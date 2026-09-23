# What is missing

What this port does not have yet, most blocking first. Everything here was measured, not guessed: the import lists come from the game's own import table (`PURUN_LIST_STUBS=1`), the recompiler's gaps from the code it generated, and the game's behaviour from bounded runs with scripted input, read off the port's own frame captures (none are kept here: they are the game's art).

Platform for everything below: macOS on Apple Silicon, Vulkan through MoltenVK.

## Where the port is

**The game is playable from the start of a new game, in both the interpreter and the recompiled build.** In order:

1. The language screen, with its labels. ○ picks a language.
2. The publisher and studio logos, then the title intro: the characters, the scenery, the logo.
3. START opens the title menu, **New** and **Continue**. Continue opens the save-data list (there is no save yet). New skips the opening movie, which does not play ([#2](https://github.com/TeamGDB/Purun/issues/2)), and starts the game.
4. The first scene: a character's speech bubbles, advanced with ○, then the tutorial. **L and R tilt the world** and the player's character rolls; the tutorial moves on to "push L and R at the same time".

The recompiled build does all of that at **100% speed**, its frame loop at 20 frames per second, and with `PSPRECOMP_NO_INTERPRETER=1` it never falls back to the interpreter on the way. The whole executable recompiles.

**Not verified:** anything past the first minute of the tutorial; the rest of the first level; finishing a level; saving; sound (every run had audio off); whether 20 frames per second is the PSP's own rate for this game.

### What was in the way, in order

Each was found by running the game and reading where it stopped. All of it is in the framework ([PortableKit#21](https://github.com/TeamGDB/PortableKit/pull/21)); none of it names this game:

| What | Why it was in the way |
| --- | --- |
| The older `~PSP` header layout | The tag (`0xC0CB167C`) selects a 0x90-byte key table, not a 16-byte key; the installer could not prepare the executable. The result is byte for byte the unencrypted `BOOT.BIN` on the same disc |
| `sceKernelLoadModule` by path | The game loads the three stock modules in `PSP_GAME/USRDIR/modules` (`psmf.prx`, `libpsmfplayer.prx`, `libfont.prx`) by path, and waited for ever on the stub's answer |
| Variable-size pools, `sceKernelMaxFreeMemSize` | The game sizes a pool as "all free memory but 1 MiB"; with a stub answering 0 that was negative, and the first allocation crashed |
| `vbfy1` | The interpreter stopped on it building a projection matrix |
| `sceFontGetCharGlyphImage_Clip` | Every piece of text is drawn through it; the language screen had no labels |
| The pad latch (`sceCtrlReadLatch`) | The game reads its buttons only through the latch; no press reached it |
| Streamed ATRAC (`sceAtracSetHalfwayBufferAndGetID`, `AddStreamData`) | The title music is one 77 MB track streamed through a 2 KiB buffer. With stubs it ended at once and the title screen restarted every few seconds ([#1](https://github.com/TeamGDB/Purun/issues/1)) |
| `sceRtcGetCurrentTick`, `sceKernelWaitThreadEnd` | Both called on the way to the title. The stubbed tick wrote nothing, so the game read its stack as the time |
| **Spline surfaces** (GE `SPLINE`) | **The player's character is a spline surface**, and the renderer skipped them: the game ran with its hero invisible |
| Save-data LIST | Asked at the title; answered with a parameter error before |
| 58 NID names | Imports the framework could not name, so could not bind |

## 1. Movies — [#2](https://github.com/TeamGDB/Purun/issues/2)

All fifteen `scePsmfPlayer` imports are stubs. The game creates a player when a new game starts (the opening movie) and when its attract demo ends (the title demo), gets nothing back, releases it and goes on without the movie. The disc has 48 `.pmf` movies: the story scenes and the title demo. The framework has `sceMpeg` over FFmpeg but no PSMF player on top of it.

## 2. Save data — [#3](https://github.com/TeamGDB/Purun/issues/3)

At the title the game asks `sceUtilitySavedata` for **mode 11 (LIST)** of `UCES01059`/`_GameData?`, then GETSIZE for `_GameData0`. LIST is implemented now (the id list's place in the parameter block was read off this request) and answers "no saves" as it should. Continue asks LISTLOAD (mode 4) for `DATA.BIN`, which with no save answers "no data". **Nothing has been saved yet**: the game saves somewhere past the start of the first level, which no run has reached, so whether saving and loading work is not known.

## 3. Rendering — [#5](https://github.com/TeamGDB/Purun/issues/5)

Speech bubbles show a one-pixel cross and a faint outline where the pieces they are drawn from meet, at 2x resolution. Everything else seen so far draws correctly, including the tessellated character.

## 4. The rest of the imports

**68 of the 269 imports** are logging stubs. Called so far:

| Call | Seen |
| --- | --- |
| `SysMemUserForUser 0xEBD5C3E6` | At start-up with `0x03090510`: the SDK version. Its name does not hash to any tried; harmless as a stub |
| `sceKernelGetGPI` | At start-up |
| `sceDisplayWaitVblank` | At start-up |
| `sceFontGetFontList` | At start-up; the game goes on to open fonts by index regardless |
| `sceKernelDcacheWritebackInvalidateRange` | Often; nothing to do on the host |
| `scePower 0xEBD177D6` | At the title, with `333, 333, 166`: setting the clock. Does not hash to any name tried |
| `__sceSasSetNoise` | During the attract demo |
| `scePsmfPlayer*` | See [#2](https://github.com/TeamGDB/Purun/issues/2) |

The rest are imported but have not been called: all of `sceNetAdhocMatching` and five of `sceNetAdhocctl` (the game's ad hoc play), eight more of `sceSasCore` (envelopes, grain), the game-sharing and screenshot dialogs, `sceIoDread`, `sceIoChstat`, thread suspend and resume, and `sceUmdWaitDriveStat`.

## 5. The recompiler

**The whole executable lowers.** 19201 function seeds, 667264 code addresses, 193 C++ units, in three minutes; compiling them took about 40 minutes at `-j2` on an M1, with other builds running beside it. The only sites it cannot lower are the game's own 154 `break` instructions, which are its assertion traps.

## 6. Code overlays

**None.** The executable's section table has no overlay slots, and the only modules the game loads at run time are the three stock libraries above, which the framework serves as HLE. The game's data comes from `PSP_GAME/USRDIR/data/DATA.BIN`, read by sector range. Nothing seen so far loads code from it, and `PSPRECOMP_NO_INTERPRETER=1` found no code outside the executable on the way into the first level; that is not the same as having proved nothing does later.

## 7. Not verified at all

- Linux, Windows, Steam Deck.
- Sound.
- Ad hoc play. The ad hoc product code in the profile is a guess.
