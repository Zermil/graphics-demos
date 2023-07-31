@echo off

REM Change this to your visual studio's 'vcvars64.bat' script path
set MSVC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
set CXXFLAGS=/std:c++14 /EHsc /W4 /WX /FC /MT /wd4996 /wd4201 /nologo %*
set INCLUDES=/I"deps\include"
set LIBS="deps\lib\SDL2\SDL2.lib" "deps\lib\SDL2\SDL2main.lib" shell32.lib

call %MSVC_PATH%\vcvars64.bat

pushd %~dp0
if not exist .\build mkdir build
cl %CXXFLAGS% %INCLUDES% "code\barycentric.cpp" /Fo:build\ /Fe:build\barycentric.exe /link %LIBS% /SUBSYSTEM:CONSOLE
cl %CXXFLAGS% %INCLUDES% "code\rectangles.cpp" /Fo:build\ /Fe:build\rectangles.exe /link %LIBS% /SUBSYSTEM:CONSOLE
cl %CXXFLAGS% %INCLUDES% "code\circles.cpp" /Fo:build\ /Fe:build\circles.exe /link %LIBS% /SUBSYSTEM:CONSOLE
cl %CXXFLAGS% %INCLUDES% "code\voronoi.cpp" /Fo:build\ /Fe:build\voronoi.exe /link %LIBS% /SUBSYSTEM:CONSOLE
cl %CXXFLAGS% %INCLUDES% "code\ellipses.cpp" /Fo:build\ /Fe:build\ellipses.exe /link %LIBS% /SUBSYSTEM:CONSOLE

cd build
del *.obj
cd ..
popd