Purun for macOS (Apple Silicon)
===============================

A native port of LocoRoco 2 for the PSP, built on PortableKit. Purun does
not include any game assets or original game files: no disc image, no copy
of the game's executable or data, and no textures, models, audio or video
from the game. You must provide the files from your own legally obtained
copy of the game: the European release (UCES-01059), as an uncompressed
.iso disc image. The first start checks that copy and accepts only that
release.

Needs: a Mac with Apple Silicon (M1 or newer) and macOS @MINIMUM_SYSTEM_VERSION@ or newer.
Tested on macOS 27 only.


Install
-------

Drag Purun into the Applications folder.


First start
-----------

This build is not notarized by Apple, so the first time you open it macOS
says that it cannot verify the developer and offers only Done or Move to
Trash. Choose Done, then:

  1. Open System Settings > Privacy & Security.
  2. Scroll down to the message about Purun and click "Open Anyway".
  3. Confirm with your password or Touch ID, then "Open Anyway" once more.

Or, from Terminal:

    xattr -dr com.apple.quarantine /Applications/Purun.app

The first start then asks for your disc image in Purun's own window: pick
the .iso file and let Purun check it. Later starts go straight to the game.
Esc, or L3+R3 on a gamepad, opens Purun's menu with the settings.

Controls: L and R tilt the world (Q and Left Shift on the keyboard), both
together jump, circle bursts the LocoRoco apart; the menu lists every key.


Where your data lives
---------------------

    ~/Library/Application Support/Purun/UCES01059/
        settings.ini   where the image is, and the settings from the menu
        ms0/PSP/SAVEDATA/UCES01059_GameData0/   your save

Updating or deleting Purun.app leaves this folder alone.


Known problems
--------------

See https://github.com/TeamGDB/Purun/blob/main/docs/MISSING.md


Report a problem: https://github.com/TeamGDB/Purun/issues
Third-party software and its licenses: Purun.app/Contents/Resources/licenses/
