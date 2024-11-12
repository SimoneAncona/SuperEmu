@echo off
if "%1" == "" (
    echo Unspecified environment variable
    exit 1
)
if "%2" == "" (
    echo Unspecified target
    exit 2
)
cmake -B .\build\ -S .
MSBuild .\build\SuperEmu.sln
copy .\libs\sdl\bin\win\%2\SDL2.dll .\build\%1\SDL2.dll