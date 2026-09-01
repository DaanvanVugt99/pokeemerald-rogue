# Windows Development Setup

Divergence supports WSL1, WSL2, and the MSYS2 environment bundled with
devkitPro. WSL1 works well when the checkout must remain on the Windows `C:`
drive; WSL2 provides the best build performance when the checkout lives in its
Linux filesystem. MSYS2 remains a supported fallback. ARM object files are kept
separate automatically under `build_wsl/` so switching toolchains cannot reuse
incompatible objects.

## WSL1 workflow for a Windows checkout

Install Ubuntu as WSL1 so Windows tools such as Zed can continue to access the
checkout directly:

```powershell
wsl --install Ubuntu-24.04 --version 1 --enable-wsl1
```

Inside Ubuntu, install the native build dependencies:

```sh
sudo apt update
sudo apt install build-essential git libpng-dev zlib1g-dev python3 \
  binutils-arm-none-eabi gcc-arm-none-eabi libnewlib-arm-none-eabi
```

The pinned Poryscript Linux binary belongs at
`tools/poryscript/poryscript-linux/poryscript`; that directory is ignored just
like the Windows binary directory. Install the pinned release from the
repository root inside Ubuntu:

```sh
curl -fL https://github.com/huderlem/poryscript/releases/download/3.5.2/poryscript-linux.zip -o /tmp/poryscript-linux.zip
mkdir -p /tmp/poryscript-linux tools/poryscript/poryscript-linux
python3 -m zipfile -e /tmp/poryscript-linux.zip /tmp/poryscript-linux
install -m 0755 /tmp/poryscript-linux/poryscript-linux/poryscript tools/poryscript/poryscript-linux/poryscript
```

From PowerShell in the repository root, run the normal Linux launchers through
the default WSL distribution:

```powershell
wsl ./scripts/launch_build_debug.sh
wsl ./scripts/launch_build_release.sh
wsl ./scripts/launch_build_test.sh --check --suite ability --filter "Intimidate"
```

The Linux launcher builds the ROM, while `launch_emu_debug.bat` opens it in the
native Windows mGBA application. The launchers use at most four build jobs by
default; set `BUILD_JOBS` to override that limit when more memory is available.

## Fast WSL2 workflow

For the best build throughput, run the checkout from the WSL2 Linux filesystem
instead of `/mnt/c`. This avoids Windows filesystem metadata overhead during
Makefile dependency scans and parallel compilation. Convert the distribution if
needed, then create a Linux-side checkout:

```powershell
wsl --set-version Ubuntu-24.04 2
wsl
```

```sh
mkdir -p ~/src
cd ~/src
git clone <repository-url> pokeemerald-rogue
cd pokeemerald-rogue
./scripts/launch_build_debug.sh
```

If the Windows checkout has uncommitted work, save or commit it before copying
or cloning so the two checkouts cannot diverge accidentally. Windows editors can
still open the Linux-side checkout through `\\wsl$\Ubuntu-24.04\home\...`.

## MSYS2 workflow

Run the repository's `.bat` launchers from PowerShell, Command Prompt, or
Explorer; they locate MSYS2 and then execute the same Bash scripts used on
macOS, Linux, and WSL.

## One-time setup

1. Install the current devkitPro Windows package and select **GBA
   Development**. The default location is `C:\devkitPro`.
2. From the repository root, install the project's pinned Poryscript release
   and audit the remaining dependencies:

   ```powershell
   .\scripts\check_windows_setup.ps1 -InstallPoryscript
   ```

3. Install the mGBA desktop application if you want interactive runs. The
   launcher searches `PATH`, the standard Program Files locations, and
   `DIVERGENCE_MGBA`. Headless tests use the bundled
   `tools\mgba\mgba-rom-test.exe`.

   ```powershell
   winget install --exact --id JeffreyPfau.mGBA
   ```

4. Install Python 3 if you need species report generation. A normal Windows
   Python install supports PowerShell and editor tooling, while Make runs in
   devkitPro's isolated MSYS2 environment and needs its own Python package:

   ```powershell
   winget install --exact --id Python.Python.3.13
   C:\devkitPro\msys2\msys2_shell.cmd -mingw64 -defterm -no-start -c "pacman -S --needed python"
   ```

   The Pokemon profile pipeline additionally needs Mono and MSBuild/XBuild as
   documented in `docs/pokemon_profile_pipeline.md`.

If devkitPro is installed outside its default location, set
`DIVERGENCE_MSYS2_SHELL` to the full Windows path of `msys2_shell.cmd` or
`msys2_shell.bat`. If mGBA is not in `PATH`, set `DIVERGENCE_MGBA` to the full
path of `mGBA.exe`.

Re-run the audit at any time:

```powershell
.\scripts\check_windows_setup.ps1
```

## Build and test

```powershell
.\scripts\launch_build_debug.bat
.\scripts\launch_build_release.bat
.\scripts\launch_build_test.bat --check --suite ability --filter "Intimidate"
```

Run the full split suite explicitly for CI or release validation, not as a
routine local check:

```powershell
.\scripts\launch_build_test.bat --check-all-suites
```

The Windows wrappers forward all arguments and return the underlying build's
real exit code. The Makefile itself remains shared across Windows, macOS, and
Linux.

Use `make tidycheck` for stale test artifacts. Do not use the clean launcher as
a routine reset: the inherited clean target removes generated graphics files
that are not always rebuilt automatically.
