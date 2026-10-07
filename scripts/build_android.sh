#!/usr/bin/env bash
# Build an arm64 Android APK using PortableKit's shared packaging.
# Requires ANDROID_HOME, an NDK, a JDK, CMake, Ninja, curl and glslangValidator.
# Prepare the European game's executable first; no game files enter the APK.
# Optional: ANDROID_NDK, PURUN_EBOOT, JOBS, KEYSTORE, KEYSTORE_PASS, KEY_ALIAS.
set -euo pipefail

repo="$(cd "$(dirname "$0")/.." && pwd)"
work="${1:-$repo/out/android}"
mkdir -p "$work"
work="$(cd "$work" && pwd)"
sdk="${ANDROID_HOME:?set ANDROID_HOME to the Android SDK}"
ndk="${ANDROID_NDK:-$(ls -d "$sdk"/ndk/* | sort -V | tail -1)}"
toolchain="$ndk/build/cmake/android.toolchain.cmake"
[[ -f "$toolchain" ]] || { echo "error: Android NDK toolchain not found" >&2; exit 1; }
jobs="${JOBS:-2}"
[[ "$jobs" =~ ^[1-9][0-9]*$ ]] || { echo "error: JOBS must be a positive integer" >&2; exit 1; }
source "$repo/portablekit/packaging/sources.sh"
sources="$work/sources"
mkdir -p "$sources" "$work/dist"

sha256() { shasum -a 256 "$1" | cut -d ' ' -f1; }
fetch() {
    local name="$1" url="$2" hash="$3"
    if [[ -f "$sources/$name" && "$(sha256 "$sources/$name")" == "$hash" ]]; then return; fi
    curl -fL --retry 3 -o "$sources/$name.part" "$url"
    [[ "$(sha256 "$sources/$name.part")" == "$hash" ]] ||
        { echo "error: checksum mismatch for $name" >&2; exit 1; }
    mv "$sources/$name.part" "$sources/$name"
}

fetch "SDL3-$SDL3_VERSION.tar.gz" "$SDL3_URL" "$SDL3_SHA256"
fetch NotoSansCJKjp-Regular.otf "$NOTO_CJK_URL" "$NOTO_CJK_SHA256"
fetch NotoSansCJK-LICENSE.txt "$NOTO_CJK_LICENSE_URL" "$NOTO_CJK_LICENSE_SHA256"
sdl_source="$sources/SDL3-$SDL3_VERSION"
[[ -d "$sdl_source" ]] || tar -xzf "$sources/SDL3-$SDL3_VERSION.tar.gz" -C "$sources"
cmake -S "$sdl_source" -B "$work/sdl-build" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-29 -DCMAKE_BUILD_TYPE=Release \
    -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST_LIBRARY=OFF -DSDL_TESTS=OFF \
    -DSDL_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX="$work/sdl"
cmake --build "$work/sdl-build" -j "$jobs"
cmake --install "$work/sdl-build"

if [[ ! -f "$repo/generated/generated_registry.cpp" ]]; then
    elf="${PURUN_EBOOT:-}"
    if [[ -z "$elf" ]]; then
        for candidate in "${PURUN_GAME_DIR:-}/EBOOT.ELF" "${PURUN_DATA_DIR:-}/EBOOT.ELF" \
            "$HOME/Library/Application Support/Purun/UCES01059/EBOOT.ELF" \
            "$HOME/.local/share/Purun/UCES01059/EBOOT.ELF"; do
            if [[ -f "$candidate" ]]; then elf="$candidate"; break; fi
        done
    fi
    [[ -f "$elf" ]] || { echo "error: prepare EBOOT.ELF or set PURUN_EBOOT first" >&2; exit 1; }
    cmake -S "$repo/portablekit" -B "$work/tools" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release -DPSPRECOMP_BUILD_TESTS=OFF
    cmake --build "$work/tools" --target psp_recomp -j "$jobs"
    "$work/tools/psp_recomp" "$elf" --auto "$repo/generated"
fi

cmake -S "$repo" -B "$work/native" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-29 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release \
    -DSDL3_DIR="$work/sdl/lib/cmake/SDL3" -DCMAKE_FIND_ROOT_PATH="$work/sdl" \
    -DPORTABLEKIT_ANDROID_APP=ON -DPORTABLEKIT_RELEASE=ON \
    -DPORTABLEKIT_FFMPEG=bundled -DPSPRECOMP_BUILD_TESTS=OFF \
    -DPSPRECOMP_GENERATED_JOBS="$jobs"
cmake --build "$work/native" --target PurunNative -j "$jobs"

apk="$work/dist/Purun-android-arm64.apk"
APP_ID=io.github.teamgdb.purun GAME_REPO="$repo" \
GAME_RES="$repo/packaging/android/res" NOTICES="$repo/packaging/THIRD_PARTY_NOTICES.md" \
FONT_DIR="$sources" ANDROID_NDK="$ndk" \
    "$repo/portablekit/packaging/android/build_apk.sh" "$work/native" "$sdl_source" \
    "$work/sdl/lib/libSDL3.so" "$apk"
build_tools="$(ls -d "$sdk"/build-tools/* | sort -V | tail -1)"
"$build_tools/apksigner" verify "$apk"
"$build_tools/zipalign" -c -P 16 4 "$apk"
(cd "$work/dist" && shasum -a 256 "$(basename "$apk")" > SHA256SUMS)
echo "APK: $apk"
