@echo off
if not defined DevEnvDir (call vcvarsall x86)
pushd build\
cl /Zi -o main ..\src\*.cpp /I ..\dependencies /D SDL_MAIN_HANDLED /link ..\dependencies\SDL\SDL2.lib
popd