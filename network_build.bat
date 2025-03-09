
@echo off
setlocal

cd /D "%~dp0"

if not exist "bin" (mkdir bin)

pushd bin

set FLAGS=/fsanitize=address /EHsc /MP /Zi /RTC1
rem set FLAGS=/Ox /MP /GL

set SYSTEM_LIBS=User32.lib d2d1.lib windowscodecs.lib ole32.lib dwrite.lib winmm.lib

set SOURCE_FILES=..\source\Network\main.cpp

set OUTPUT_EXE=server.exe

call cl %FLAGS% ^
        %SOURCE_FILES% ^
        %SYSTEM_LIBS% ^
        /link %MIDDLEWARE_LIB_PATH% ^
        /SUBSYSTEM:WINDOWS /OUT:%OUTPUT_EXE% 

popd
endlocal
