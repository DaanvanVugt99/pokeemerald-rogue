# Pokemon Emerald Rogue: Divergence

Divergence is a standalone fork of Pokemon Emerald Rogue with custom abilities,
battle mechanics, and an underground hub.

## Build and play

On macOS, Linux, or WSL, install GNU Make, Python 3, a host C/C++ toolchain,
libpng, and an ARM toolchain with `arm-none-eabi-gcc` and newlib. Make sure GNU Make is
available as `make` when using the launchers. Install `mgba` to play locally.

```sh
./init_deps.sh
./scripts/launch_build_debug.sh
mgba pokeemerald.gba
```

For a release build, use `./scripts/launch_build_release.sh`. Build launchers
default to at most four jobs. Set `BUILD_JOBS` to override that limit.

On Windows, install devkitPro with **GBA Development** selected, then run:

```powershell
.\scripts\check_windows_setup.ps1 -InstallPoryscript
.\scripts\launch_build_debug.bat
.\scripts\launch_emu_debug.bat
```

With WSL, use the Linux commands inside WSL. Keep the checkout in the Linux
filesystem for best WSL2 build performance.

## Test

Headless tests use `mgba-rom-test` on `PATH` or the repository's bundled
Windows runner. Use the matching `.bat` launcher on native Windows.

```sh
./scripts/launch_build_test.sh --check --suite ability --filter "Intimidate"
./scripts/launch_build_test.sh --check-all-suites
```

Run a focused check during development. Use the full split suites for CI and
release validation.

[Release notes](CHANGELOG_ROGUE.md) · [Third-party notices](NOTICE.md)
