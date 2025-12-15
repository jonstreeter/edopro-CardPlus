@echo off
setlocal enabledelayedexpansion

:: Check for Premake5
if not exist premake5.exe (
    echo [ERROR] premake5.exe not found in logic root.
    echo Please download it from https://premake.github.io/download.html and place it here.
    exit /b 1
)

:: Find MSBuild using vswhere
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found. Is Visual Studio installed?
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set "MSBUILD_PATH=%%i"
)

if "%MSBUILD_PATH%"=="" (
    echo [ERROR] MSBuild.exe not found.
    exit /b 1
)

echo Found MSBuild at: "%MSBUILD_PATH%"

echo Generating solution files...
premake5 vs2022
if %errorlevel% neq 0 (
    echo [ERROR] Premake generation failed!
    exit /b 1
)

echo Building Project...
"%MSBUILD_PATH%" build\ygo.sln /p:Configuration=Release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    exit /b 1
)

echo.
echo ========================================
echo  Build Successful!
echo ========================================

REM Deploy assets to bin\release
echo.
echo Deploying assets to bin\release...

REM Create necessary directories
if not exist "bin\release\textures" mkdir "bin\release\textures"
if not exist "bin\release\config" mkdir "bin\release\config"
if not exist "bin\release\pics_modular" mkdir "bin\release\pics_modular"

REM Copy textures (with subdirectories)
echo   - Copying textures...
xcopy /E /I /Y "textures\*" "bin\release\textures\" >nul 2>&1

REM Copy distinct skins path
if exist "skin" (
    echo   - Copying skins...
    if not exist "bin\release\skin" mkdir "bin\release\skin"
    xcopy /E /I /Y "skin\*" "bin\release\skin\" >nul 2>&1
)

REM Copy sounds
if exist "sound" (
    echo   - Copying sounds...
    if not exist "bin\release\sound" mkdir "bin\release\sound"
    xcopy /E /I /Y "sound\*" "bin\release\sound\" >nul 2>&1
)

REM Copy config files (only if they don't exist, to preserve user settings)
echo   - Copying configs (preserving existing)...
for %%f in (config\*) do (
    if not exist "bin\release\config\%%~nxf" copy "%%f" "bin\release\config\" >nul 2>&1
)

REM Copy database
echo   - Copying database...
if exist "cards.cdb" copy /Y "cards.cdb" "bin\release\" >nul 2>&1

REM Copy pics_modular cache directory (empty or existing)
echo   - Setting up art cache...
if not exist "bin\release\pics_modular" mkdir "bin\release\pics_modular"

echo   - Assets deployed successfully!
echo.
echo ========================================
echo  Build Complete!
echo ========================================
echo.
echo Executables: bin\release\ygopro.exe
echo              bin\release\ygoprodll.exe
exit /b 0
