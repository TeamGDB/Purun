// Everything PortableKit has to be told about LocoRoco 2. The framework reads
// the rest out of the game's own executable.
//
// The numbers here were read off the player's disc, not recalled: the disc id
// and title from PSP_GAME/PARAM.SFO, the hashes from the files themselves, the
// tag from offset 0xD0 of EBOOT.BIN, and the load image's size from the ELF's
// program headers.

#include "profile.hpp"

#include "psprecomp/elf32.hpp"


namespace portablekit {
namespace {

// The executable names this folder itself ("UCES01059"). Whether the game
// keeps anything else on the memory stick has not been seen yet.
constexpr SaveFolder kSaveFolders[] = {
    {"UCES01059", "Game data", true},
};

} // namespace

const GameProfile &game() {
    static const GameProfile profile{
        .app_name = "PurunNative",
        .project_name = "Purun",
        .env_prefix = "PURUN",
        .data_organization = "Purun",
        .data_application = "UCES01059",

        .disc_id = "UCES01059",
        .disc_id_display = "UCES-01059",
        .game_title = "LocoRoco 2",
        .executable_path_on_disc = "PSP_GAME/SYSDIR/EBOOT.BIN",
        .param_sfo_path_on_disc = "PSP_GAME/PARAM.SFO",
        .release_name = "the European release",
        // PSP_GAME/SYSDIR/EBOOT.BIN of the European release, and the
        // executable it decrypts to (identical to BOOT.BIN on the same disc).
        .encrypted_executable_sha256 = "34c0f62f165733e1f1ea6d7c301d3b4c1a0539c76a6e18fcd3494a805bac8d58",
        .executable_sha256 = "e1075b96b183babad034af28d055c96ffbd212ebbccb44e81f4709c9025ca467",
        // This tag selects the older "~PSP" header layout, whose key is a
        // 0x90-byte table rather than 16 bytes. The framework does not
        // decrypt that layout yet, so installing stops here for now.
        .decryption_tag = 0xC0CB167Cu,
        .decryption_key = {},

        // A relocatable executable whose image reaches 0x3B76EC bytes, so it
        // fits the 32 MiB of a PSP-1000 at the usual user base.
        .load_base = psprecomp::kDefaultPspUserLoadBase,
        .guest_ram_bytes = 32u * 1024u * 1024u,
        .boot_path = "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN",
        // The executable's section table has no overlay slots. What it does
        // load at run time is the three system libraries in
        // PSP_GAME/USRDIR/modules, which the framework implements as HLE.
        .overlay_slots = {},

        .save_game_name = "UCES01059",
        .save_folders = kSaveFolders,

        // A guess: the disc id. Nothing about this game's ad hoc play is
        // known yet.
        .adhoc_product_code = "UCES01059",
    };
    return profile;
}

} // namespace portablekit
