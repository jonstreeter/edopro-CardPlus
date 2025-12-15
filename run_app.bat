@echo off
REM EDOPro Run Application Script
REM Runs the most recently built EDOPro executable

echo ========================================
echo  EDOPro Launcher (Always Latest Build)
echo ========================================
echo.

REM Check if executable exists
if not exist "bin\release\ygopro.exe" (
    echo [ERROR] ygopro.exe not found in bin\release\
    echo Please run quick_build.bat first to build the application.
    pause
    exit /b 1
)

REM Show last modified time to verify it's the latest
echo Executable info:
for %%F in (bin\release\ygopro.exe) do (
    echo   File: %%~nxF
    echo   Modified: %%~tF
    echo   Size: %%~zF bytes
)
echo.

REM Make sure we're running from the right directory
pushd bin\release

REM Clear any cached version by using start with /wait
echo Starting EDOPro...
echo.
start /wait "" ygopro.exe

popd

echo.
echo ========================================
echo  EDOPro closed.
echo ========================================
pause
