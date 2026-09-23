// Everything PortableKit has to be told about LocoRoco 2. The framework reads
// the rest out of the game's own executable.
//
// The numbers here were read off the player's disc, not recalled: the disc id
// and title from PSP_GAME/PARAM.SFO, the hashes from the files themselves, the
// tag from offset 0xD0 of EBOOT.BIN, and the load image's size from the ELF's
// program headers.

#include "profile.hpp"

#include "psprecomp/elf32.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace portablekit {
namespace {

// The executable names this folder itself ("UCES01059"). Whether the game
// keeps anything else on the memory stick has not been seen yet.
constexpr SaveFolder kSaveFolders[] = {
    {"UCES01059", "Game data", true},
};

// The published 0x90-byte table for tag 0xC0CB167C, as little-endian words.
// This tag selects the older "~PSP" header layout, which has no 16-byte key.
// Checked: EBOOT.BIN decrypted with it is byte for byte the unencrypted
// PSP_GAME/SYSDIR/BOOT.BIN the same disc carries.
constexpr std::array<std::uint32_t, 36> kKeyTableWords = {
    0xDA8E36FAu, 0x5DD97447u, 0x76C19874u, 0x97E57EAFu, 0x1CAB09BDu, 0x9835BAC6u,
    0x03D39281u, 0x03B205CFu, 0x2882E734u, 0xE714F663u, 0xB96E2775u, 0xBD8AAFC7u,
    0x1DD3EC29u, 0xECA4A16Cu, 0x5F69EC87u, 0x85981E92u, 0x7CFCAE21u, 0xBAE9DD16u,
    0xE6A97804u, 0x2EEE02FCu, 0x61DF8A3Du, 0xDD310564u, 0x9697E149u, 0xC2453F3Bu,
    0xF91D8456u, 0x39DA6BC8u, 0xB3E5FEF5u, 0x89C593A3u, 0xFB5C8ABCu, 0x6C0B7212u,
    0xE10DD3CBu, 0x98D0B2A8u, 0x5FD61847u, 0xF0DC2357u, 0x7701166Au, 0x0F5C3B68u,
};

constexpr std::array<std::uint8_t, 0x90> key_table_bytes() {
    std::array<std::uint8_t, 0x90> bytes{};
    for (std::size_t i = 0; i < kKeyTableWords.size(); ++i)
        for (std::size_t b = 0; b < 4; ++b)
            bytes[i * 4 + b] = static_cast<std::uint8_t>(kKeyTableWords[i] >> (8 * b));
    return bytes;
}

constexpr std::array<std::uint8_t, 0x90> kKeyTable = key_table_bytes();

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
        .decryption_tag = 0xC0CB167Cu,
        .decryption_key = {},
        .decryption_key_table = kKeyTable,

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
