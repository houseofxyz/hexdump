@echo off
setlocal

set VSINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\

:: Set up environment for cl.exe if not already set
if not defined DevEnvDir (
    call "%VSINSTALLDIR%\VC\Auxiliary\Build\vcvars64.bat"
)

:: Define variables
set OUTPUT_DIR=bin

:: Create output directory if it doesn't exist
if not exist %OUTPUT_DIR% (
    mkdir %OUTPUT_DIR%
)

:: Check for arguments (build or clean)
if "%1" == "clean" goto clean

:: Compile release version
echo Compiling release version...
cl /nologo /Ox /MT /W0 /GS- /DNDEBUG /Fe%OUTPUT_DIR%\hexdump.exe hexdump.c /link /SUBSYSTEM:CONSOLE /MACHINE:x64

:: Compile debug version
echo Compiling debug version...
cl /nologo /Ox /MT /W0 /GS- /DDEBUG /Fe%OUTPUT_DIR%\hexdump_d.exe hexdump.c /link /SUBSYSTEM:CONSOLE /MACHINE:x64

echo Build complete.
goto end

:clean
:: Clean up the build
if exist %OUTPUT_DIR%\hexdump.exe (
    del /Q %OUTPUT_DIR%\hexdump.*
)
if exist %OUTPUT_DIR%\hexdump_d.exe (
    del /Q %OUTPUT_DIR%\hexdump_d.*
)

if exist *.obj (
    del /Q *.obj
)
if exist %OUTPUT_DIR% (
    rmdir /Q /S %OUTPUT_DIR%
)

echo Clean complete.
goto end

:end
endlocal

