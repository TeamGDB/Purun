Purun for Linux (x86-64)
========================

A native port of LocoRoco 2 for the PSP, built on PortableKit. Purun does
not include any game assets or original game files. You must provide your
own legally obtained copy of the game: the European release (UCES-01059),
as an uncompressed .iso disc image. The first start checks that copy and
accepts only that release.

Start it with

    ./purun

The first start asks for your disc image and prepares the game from it in
Purun's data directory (~/.local/share/Purun/UCES01059). Saves are kept in
the same directory, under ms0/PSP/SAVEDATA.

    ./purun --install /path/to/image.iso   set up from a terminal instead
    ./purun --help                         all options

Needs: an x86-64 Linux with a recent glibc, a Vulkan driver, and Wayland or
X11. This Linux build has not been tested yet; please report how it runs:
https://github.com/TeamGDB/Purun/issues

Known problems: https://github.com/TeamGDB/Purun/blob/main/docs/MISSING.md
Third-party software and its licenses: licenses/THIRD_PARTY_NOTICES.md
