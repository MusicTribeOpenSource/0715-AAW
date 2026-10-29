@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ================================================================
REM Script Name : generate_BCR.bat
REM Description : Copy the application binary into the update folder
REM Version     : 4.0 (plain BIN only: windows\update.exe encrypts it in memory when sending)
REM Usage       : generate_BCR.bat <application.bin>
REM Input       : Keil After Build #1 (fromelf --bincombined --bincombined_padding=1,0x00)
REM               or the GCC / CMake post-build step (objcopy -O binary)
REM Output      : BCR32_FW_V<x.y.z>_<date>_<time>.bin in this folder (older files are kept)
REM ================================================================

REM full path of the input before changing the directory
set "SRC_BIN=%~f1"

REM --- Always run in script directory ---
pushd %~dp0

echo.
echo ===============================================================
echo [INFO] Starting BCR firmware build script
echo ===============================================================
echo.

set "RC=0"

REM ------------------------------------------------
REM STEP 1 : Check the application BIN
REM ------------------------------------------------
echo [STEP 1] Checking "%SRC_BIN%"...

IF "%~1"=="" (
    echo [ERROR] usage: generate_BCR.bat ^<application.bin^>
    goto :error
)
IF NOT EXIST "%SRC_BIN%" (
    echo [ERROR] "%SRC_BIN%" not found: the after-build step ^(Keil fromelf / GCC objcopy^) did not run or failed
    goto :error
)
echo [SUCCESS] Application BIN found.
echo.

REM Version from App\version.h (SOFTWARE_VERSION_MAIN / _SUB / _BUG), date and time of this run
REM (HHmmss without colons: not allowed in file names)
set "SOFTWARE_VERSION_MAIN="
set "SOFTWARE_VERSION_SUB="
set "SOFTWARE_VERSION_BUG="
for /f "tokens=2,3" %%a in ('findstr /r /c:"^#define[ 	]*SOFTWARE_VERSION_[A-Z]*[ 	]" "..\..\App\version.h"') do set "%%a=%%b"
for /f %%d in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd_HHmmss"') do set "BUILD_DATE=%%d"
IF "%SOFTWARE_VERSION_MAIN%"=="" goto :noversion
IF "%SOFTWARE_VERSION_SUB%"=="" goto :noversion
IF "%SOFTWARE_VERSION_BUG%"=="" goto :noversion
IF "%BUILD_DATE%"=="" goto :noversion
set "FW_BIN=BCR32_FW_V%SOFTWARE_VERSION_MAIN%.%SOFTWARE_VERSION_SUB%.%SOFTWARE_VERSION_BUG%_%BUILD_DATE%.bin"

REM ------------------------------------------------
REM STEP 2 : Copy the BIN into the update folder
REM ------------------------------------------------
echo [STEP 2] Copying the firmware...

copy /Y "%SRC_BIN%" "%FW_BIN%" >nul
IF ERRORLEVEL 1 (
    echo [ERROR] Copy failed
    goto :error
)

echo [SUCCESS] "%FW_BIN%" generated. Send it with: windows\update.exe "%FW_BIN%"
echo.

echo ===============================================================
echo [COMPLETE] Firmware generation SUCCESSFUL
echo ===============================================================
echo.

goto :finally

REM ------------------------------------------------
REM ERROR HANDLER
REM ------------------------------------------------
:noversion
echo [ERROR] Version (..\..\App\version.h SOFTWARE_VERSION_*) or date not found
:error
set "RC=1"
echo.
echo ***************************************************************
echo [FAILED] Firmware generation aborted due to error
echo ***************************************************************
echo.

REM ------------------------------------------------
REM FINAL CLEANUP
REM ------------------------------------------------
:finally
popd
exit /b %RC%
