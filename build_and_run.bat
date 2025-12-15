@echo off
REM EDOPro Build and Run Script
REM Convenience script to build and immediately run the application

echo ========================================
echo  EDOPro - Build and Run
echo ========================================
echo.

REM Build the application
call quick_build.bat
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed! Not running the application.
    exit /b 1
)

echo.
echo ========================================
echo  Build Successful! Starting EDOPro...
echo ========================================
echo.

REM Run the application
call run_app.bat

echo.
echo ========================================
echo  Done
echo ========================================
