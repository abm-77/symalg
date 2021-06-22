@echo off

set build_options= -DBUILD_WIN32=1 -DENABLE_ASSERT 
set includes= /I \INCLUDE /I source 
set compile_flags= %includes% %build_options% -nologo /Zi /FC /MDd /EHsc 
set platform_link_flags= gdi32.lib user32.lib shell32.lib 
 
cls 

call vcvars64

IF NOT EXIST build mkdir build

pushd build

del *.obj

cl %compile_flags% ..\source\main.cpp %platform_link_flags% /Fe:..\bin\application.exe

del *.obj

popd