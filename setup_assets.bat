@echo off
setlocal
echo ========================================
echo  EDOPro Asset Setup Script
echo ========================================
echo.
echo This script will download and set up all required assets for EDOPro.
echo It will download the full ProjectIgnis Distribution (skins, fonts, etc.).
echo.

set "DIST_URL=https://github.com/ProjectIgnis/Distribution/archive/refs/heads/master.zip"
set "ZIP_FILE=distribution.zip"
set "EXTRACT_DIR=distribution_temp"

echo 1. Downloading assets from %DIST_URL%...
powershell -Command "Invoke-WebRequest -Uri '%DIST_URL%' -OutFile '%ZIP_FILE%'"
if %errorlevel% neq 0 (
    echo [ERROR] Download failed!
    goto :cleanup
)

echo.
echo 2. Extracting assets...
if exist "%EXTRACT_DIR%" rmdir /s /q "%EXTRACT_DIR%"
powershell -Command "Expand-Archive -Path '%ZIP_FILE%' -DestinationPath '%EXTRACT_DIR%'"
if %errorlevel% neq 0 (
    echo [ERROR] Extraction failed!
    goto :cleanup
)

echo.
echo 3. Installing assets...
REM The zip structure is usually Distribution-master/

REM Check if extracted folder exists
if not exist "%EXTRACT_DIR%\Distribution-master" (
    echo [ERROR] Unexpected zip structure.
    goto :cleanup
)

REM Move Skins
echo   - Installing skins...
if not exist "skins" mkdir "skins"
xcopy /E /I /Y "%EXTRACT_DIR%\Distribution-master\skin\*" "skins\" >nul

REM Move Fonts (if necessary, though we have them in textures/fonts/ mainly)
REM echo   - Installing fonts...
REM xcopy /E /I /Y "%EXTRACT_DIR%\Distribution-master\fonts\*" "textures\fonts\" >nul

REM Move Sound (if we want sounds)
if not exist "sound" mkdir "sound"
echo   - Installing sounds...
xcopy /E /I /Y "%EXTRACT_DIR%\Distribution-master\sound\*" "sound\" >nul

echo.
echo 4. Updating config for default skin...
REM Copy the default skin name into system.conf if commented out
powershell -Command "(Get-Content config\system.conf) -replace '# skin = Purple - Obsessed', 'skin = Purple - Obsessed' | Set-Content config\system.conf"

echo.
echo 5. Deploying to bin/release...
call quick_build.bat

echo.
echo ========================================
echo  Asset Setup Complete!
echo ========================================

:cleanup
if exist "%ZIP_FILE%" del "%ZIP_FILE%"
if exist "%EXTRACT_DIR%" rmdir /s /q "%EXTRACT_DIR%"
endlocal
pause
