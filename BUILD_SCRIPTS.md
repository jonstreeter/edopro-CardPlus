# EDOPro Build Scripts

This directory contains convenient batch scripts for building and running EDOPro with the modular card renderer.

## Quick Reference

| Script | Purpose |
|--------|---------|
| `quick_build.bat` | Standard build (incremental) + asset deployment |
| `clean_build.bat` | Clean build (removes all artifacts first) |
| `run_app.bat` | Run the application |
| `build_and_run.bat` | Build and run in one command |

## Usage

### Standard Build
```batch
quick_build.bat
```
Performs an incremental build and automatically deploys assets to `bin\release\`.

**What it does:**
1. Generates Visual Studio solution files with premake5
2. Compiles the project with MSBuild
3. **Copies all assets to bin\release:**
   - `textures/` → `bin\release\textures/`
   - `config/` → `bin\release\config/`
   - `cards.cdb` → `bin\release\cards.cdb`
   - Creates `bin\release\pics_modular/` for art cache

Fastest option for development.

### Clean Build
```batch
clean_build.bat
```
Removes `obj/`, `bin/`, and `build/` directories, then performs a fresh build with asset deployment.

Use when:
- Dependencies changed
- Build acting strange
- Want to ensure everything recompiles

### Run Application
```batch
run_app.bat
```
Runs `bin\release\ygopro.exe` from the correct directory. Checks if executable exists first.

### Build and Run
```batch
build_and_run.bat
```
Convenience script that builds (with asset deployment) then immediately runs the application.

## Asset Deployment

The build scripts automatically deploy all necessary assets:

| Asset | Source | Destination |
|-------|--------|-------------|
| **Textures** | `textures/` | `bin/release/textures/` |
| **Modular Assets** | `textures/modular/` | `bin/release/textures/modular/` |
| **Config Files** | `config/` | `bin/release/config/` |
| **Database** | `cards.cdb` | `bin/release/cards.cdb` |
| **Art Cache** | - | `bin/release/pics_modular/` (created) |

**Total Assets Deployed:** ~155 files
- 17 card frame textures
- 70 icon textures  
- 5 modular fonts
- 52 base textures
- 2 NotoSans fonts
- 9 config files
- 1 database file

## Build Requirements

- **Visual Studio 2022** (Community or higher)
- **premake5.exe** (included)
- **vcpkg** at `C:\vcpkg\` with x86-windows-static libraries
- **Git** (for submodules)

## First Time Setup

1. Initialize submodules:
   ```batch
   git submodule update --init --recursive
   ```

2. Build (includes asset deployment):
   ```batch
   quick_build.bat
   ```

3. Run:
   ```batch
   run_app.bat
   ```

## Troubleshooting

**"MSBuild not found"**
- Install Visual Studio 2022 with C++ development tools

**"premake5 not found"**
- Ensure `premake5.exe` is in project root

**"Build failed" with linker errors**
- Try `clean_build.bat`
- Check vcpkg libraries are installed

**"ygopro.exe not found"**
- Build the project first with `quick_build.bat`

**"Failed to load strings!" at runtime**
- Assets weren't deployed - run `quick_build.bat` again
- Check `bin/release/config/` exists

**"Texture not found" errors**
- Run `quick_build.bat` to redeploy assets
- Check `bin/release/textures/` directory

## Files Generated

After a successful build:

**Executables:**
- `bin/release/ygopro.exe` - Main executable (~7.8 MB)
- `bin/release/ygoprodll.exe` - DLL version (~7.8 MB)

**Libraries:**
- `bin/release/*.lib` - Static libraries
- `bin/release/*.dll` - Dynamic libraries

**Assets:**
- `bin/release/textures/` - All texture files
- `bin/release/config/` - Configuration files
- `bin/release/cards.cdb` - Card database
- `bin/release/pics_modular/` - Art cache directory

**Build Artifacts:**
- `obj/` - Intermediate object files
- `build/` - Visual Studio solution files

## Modular Renderer

The modular card renderer is integrated and ready for development:
- **Source:** `gframe/modular_card_renderer.{h,cpp}`
- **Art Manager:** `gframe/modular_art_manager.{h,cpp}`
- **Assets:** `textures/modular/` (auto-deployed to `bin/release/textures/modular/`)

## Development Workflow

```batch
# Make code changes
# ...

# Quick build and test
quick_build.bat
run_app.bat

# Or combined
build_and_run.bat

# If something's wrong
clean_build.bat
```

All assets are automatically kept in sync with each build!
