# EDOPro Skins - Setup Guide

## Issue
EDOPro requires skin files to run properly. The system.conf references `Purple - Obsessed` skin but skins were not included in the initial setup.

## Solutions

### Option 1: Download Full Distribution (Recommended)
Download the complete EDOPro distribution which includes all skins:

1. Go to https://github.com/ProjectIgnis/Distribution/releases
2. Download the latest release
3. Extract the `skin` folder
4. Copy to your project:
   ```batch
   xcopy /E /I /Y [downloaded_path]\skin skins\
   ```

### Option 2: Create Minimal Skin
Create a basic skin directory structure to bypass the warning:

```batch
mkdir skins\"Default"
```

Then update `config/system.conf`:
```conf
skin = Default
```

### Option 3: Use Default (No Custom Skin)
Comment out or remove the skin line in `config/system.conf`:
```conf
# skin = Purple - Obsessed
```

## Adding Skins to Build Script

Once you have skins in the `skins/` directory, they will need to be added to the build script.

Update `quick_build.bat` to include:
```batch
REM Copy skins
if exist "skins" (
    echo   - Copying skins...
    xcopy /E /I /Y "skins\*" "bin\release\skins\" >nul 2>&1
)
```

## For Now

The easiest temporary fix is to remove the skin reference from system.conf:

```batch
# Edit config/system.conf
# Comment out or delete line 39:
# skin = Purple - Obsessed

# Then rebuild
quick_build.bat
```

This will allow EDOPro to run with default skin (no custom UI).

## Full Skin Support (TODO)
- [ ] Download Distribution skins
- [ ] Add skins/ directory to project
- [ ] Update build script to deploy skins
- [ ] Test with multiple skin options
