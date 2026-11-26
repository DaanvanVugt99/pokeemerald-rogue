# Pokemon Emerald Rogue: Divergence

Divergence is a standalone fork of Pokemon Emerald Rogue with custom abilities,
battle mechanics, and an underground hub.

## Build and play

On macOS, install the Xcode Command Line Tools and use Homebrew to install
the build and test dependencies:

```sh
brew install make pkgconf libpng python cmake
brew install --cask gcc-arm-embedded
export PATH="$(brew --prefix make)/libexec/gnubin:$PATH"
```

For interactive play, install mGBA with `brew install --cask mgba-app`.
From the repository root, fetch Poryscript and build the debug ROM:

```sh
./init_deps.sh
./scripts/launch_build_debug.sh
open -a mGBA pokeemerald.gba
```

For a release build, use `./scripts/launch_build_release.sh`. Build launchers
default to at most four jobs. Set `BUILD_JOBS` to override that limit.

## Test

Headless tests use `stdbuf`, available on macOS 15 and later. Build the native
runner once:

```sh
./init_deps.sh --with-tests
```

Then run a focused check or the full split suites:

```sh
./scripts/launch_build_test.sh --check --suite ability --filter "Intimidate"
./scripts/launch_build_test.sh --check-all-suites
```

Run a focused check during development. Use the full split suites for CI and
release validation.

[Release notes](CHANGELOG_ROGUE.md) · [Third-party notices](NOTICE.md)
