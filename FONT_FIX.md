# Quick Fix Guide - Font Loading Error

## Problem
"Failed to load text font" error when running EDOPro.

## Root Cause
The `system.conf` file had incorrect font paths:
- **Old (wrong):** `fonts/NotoSansJP-Regular.otf`
- **New (correct):** `textures/fonts/NotoSansJP-Regular.otf`

EDOPro runs from `bin/release/` so it needs the full path including `textures/`.

## Solution Applied

Updated `config/system.conf`:
```conf
textfont = textures/fonts/NotoSansJP-Regular.otf 12
numfont = textures/fonts/NotoSansJP-Regular.otf
```

## How to Apply the Fix

### Option 1: Rebuild (Recommended)
```batch
quick_build.bat
```
This will copy the corrected config automatically.

### Option 2: Manual Copy
```batch
Copy-Item config\system.conf bin\release\config\system.conf -Force
```

## Verification

Check that the font paths are correct:
```batch
Get-Content bin\release\config\system.conf | Select-String -Pattern "font"
```

Should show:
```
textfont = textures/fonts/NotoSansJP-Regular.otf 12
numfont = textures/fonts/NotoSansJP-Regular.otf
```

## Run the App
```batch
run_app.bat
```

The font error should now be resolved!
