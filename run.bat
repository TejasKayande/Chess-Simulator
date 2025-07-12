
@echo off
setlocal

cd /D "%~dp0"

call "bin\chess.exe"

popd
endlocal
