# What is missing

What this port does not have yet, most blocking first. Everything here is to be measured, not guessed.

## Where the port is

Nothing has run yet. The executable uses the older `~PSP` header layout, whose tag (`0xC0CB167C`) selects a 0x90-byte key table; the framework only decrypts the newer layout, so the installer cannot prepare the executable.
