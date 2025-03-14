
@echo off
setlocal

cd /D "%~dp0"

if not exist "bin" (mkdir bin)

pushd bin

set FLAGS=/fsanitize=address /EHsc /MP /Zi /RTC1 /W4
rem set FLAGS=/Ox /MP /GL

set SYSTEM_LIBS=user32.lib d2d1.lib windowscodecs.lib ole32.lib dwrite.lib winmm.lib

set SOURCE_FILES=..\source\main.cpp ^
                 ..\source\renderer.cpp ^
                 ..\source\Platform\platform.cpp ^
                 ..\source\Chess\board.cpp ^
                 ..\source\Chess\move.cpp

set OUTPUT_EXE=chess.exe

call cl %FLAGS% %SOURCE_FILES% %SYSTEM_LIBS% ^
        /link /SUBSYSTEM:CONSOLE /OUT:%OUTPUT_EXE% 

popd
endlocal
