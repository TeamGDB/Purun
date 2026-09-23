# What is missing

What this port does not have yet, most blocking first. Everything here was measured, not guessed: the import lists come from the game's own import table (`PURUN_LIST_STUBS=1`), the recompiler's gaps from the code it generated, and the game's behaviour from bounded runs with scripted input, read off the port's own frame captures (none are kept here: they are the game's art).

Platform for everything below: macOS on Apple Silicon, Vulkan through MoltenVK.

## Where the port is

**The first level can be played through, saved, and continued after a restart**, in the recompiled build at full speed. In order:

1. The language screen, with its labels. ○ picks a language.
2. The publisher and studio logos, then the title intro: the characters, the scenery, the logo.
3. START opens the title menu, **New** and **Continue**. New plays the opening movie and starts the game.
4. The first level, which teaches the controls as it goes: **L and R tilt the world**, L and R together jump, ○ splits the character into small ones and holding ○ brings them back together. It was played to its goal with scripted input.
5. After the goal: a story scene, "Save Game?", then the second level.

The recompiled build does all of that at **100% speed**, its frame loop at 20 frames per second, and with `PSPRECOMP_NO_INTERPRETER=1` it never falls back to the interpreter on the way. The whole executable recompiles.

**Not verified:** anything past the start of the second level; whether 20 frames per second is the PSP's own rate for this game.

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
| The movie player (`scePsmfPlayer`) | Every movie was skipped |
| 58 NID names | Imports the framework could not name, so could not bind |

## 1. Movies — [#2](https://github.com/TeamGDB/Purun/issues/2)

**They play.** The framework now has the stock movie player (`scePsmfPlayer`), and the opening movie (`story_01.pmf`, 480x224, H.264 and ATRAC3plus) plays to its end with its sound. The game sees it finish, and stops and releases the player on its own. With audio running, pictures and sound end together. Only this one movie has been watched; the other 47 on the disc, the title demo among them, have not. Starting a movie part-way through, fast forward and rewind are not implemented; this game has not asked for them.

## 2. Save data — [#3](https://github.com/TeamGDB/Purun/issues/3)

**Saving and loading work, checked by a round trip.** After the first level the game asks "Save Game?". Yes writes `PSP/SAVEDATA/UCES01059_GameData0` (LISTSAVE, 9400 bytes, encrypted as a PSP would). After a restart, Continue finds it (LIST), loads it (LISTLOAD) and the game resumes after the first level, going on into the second. **Not verified:** that a save made here loads on a PSP, or the other way round; more than one save slot.

## 3. Sound

Measured from the port's own mix (`PURUN_AUDIO_DUMP`), not by listening: the title music, the movie's soundtrack and the first level's sounds are all there, tonal rather than noise (spectral flatness 0.03 to 0.28 over five-second windows), at plausible levels, and the movie's sound lasts as long as its pictures. **Nobody has listened to it**, so whether it sounds right is not known. Two things to look at:

- The SAS mixer's own output reaches full scale during the first level (its per-second trace peaks at 32768), and 0.1% of samples in one five-second window of the mix are clipped. Loud effects may be distorted.
- `__sceSasSetNoise` is a stub, and the game calls it: any effect it builds on the noise generator is silent.

## 4. Rendering — [#5](https://github.com/TeamGDB/Purun/issues/5)

Speech bubbles show a one-pixel cross and a faint outline where the pieces they are drawn from meet, at 2x resolution. Everything else seen so far draws correctly, including the tessellated character.

## 5. The rest of the imports

**53 of the 269 imports** are logging stubs (68 less the fifteen of `scePsmfPlayer`; counted from the previous run's list, not by running `PURUN_LIST_STUBS=1` again). Called so far:

| Call | Seen |
| --- | --- |
| `SysMemUserForUser 0xEBD5C3E6` | At start-up with `0x03090510`: the SDK version. Its name does not hash to any tried; harmless as a stub |
| `sceKernelGetGPI` | At start-up |
| `sceDisplayWaitVblank` | At start-up |
| `sceFontGetFontList` | At start-up; the game goes on to open fonts by index regardless |
| `sceKernelDcacheWritebackInvalidateRange` | Often; nothing to do on the host |
| `scePower 0xEBD177D6` | At the title, with `333, 333, 166`: setting the clock. Does not hash to any name tried |
| `__sceSasSetNoise` | During the attract demo |

The rest are imported but have not been called: all of `sceNetAdhocMatching` and five of `sceNetAdhocctl` (the game's ad hoc play), eight more of `sceSasCore` (envelopes, grain), the game-sharing and screenshot dialogs, `sceIoDread`, `sceIoChstat`, thread suspend and resume, and `sceUmdWaitDriveStat`.

## 6. The recompiler

**The whole executable lowers.** 19201 function seeds, 667264 code addresses, 193 C++ units, in three minutes; compiling them took about 40 minutes at `-j2` on an M1, with other builds running beside it. The only sites it cannot lower are the game's own 154 `break` instructions, which are its assertion traps.

## 7. Code overlays

**None.** The executable's section table has no overlay slots, and the only modules the game loads at run time are the three stock libraries above, which the framework serves as HLE. The game's data comes from `PSP_GAME/USRDIR/data/DATA.BIN`, read by sector range. Nothing seen so far loads code from it, and `PSPRECOMP_NO_INTERPRETER=1` found no code outside the executable on the way into the first level; that is not the same as having proved nothing does later.

## 8. Not verified at all

- Linux, Windows, Steam Deck.
- Listening to the sound.
- Ad hoc play. The ad hoc product code in the profile is a guess.
