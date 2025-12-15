@echo off
REM EDOPro Clean Build Script
REM Performs a clean build by removing all build artifacts first

echo ========================================
echo  EDOPro - Clean Build
echo ========================================
echo.

echo Cleaning build artifacts...
if exist obj (
    rmdir /s /q obj
    echo   - Removed obj directory
)
if exist bin (
    rmdir /s /q bin
    echo   - Removed bin directory
)
if exist build (
    rmdir /s /q build
    echo   - Removed build directory
)

echo.
echo Starting clean build...
call quick_build.bat

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Clean build failed!
    exit /b 1
)

echo.
echo ========================================
echo  Clean Build Successful!
echo ========================================
