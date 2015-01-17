echo off

REM ###################################################################################################
REM # Sculptor build script to generate a full setup.exe windows distribution file
REM ###################################################################################################

REM 
REM Step 1: Directory
REM

cd \Devel
REM del /Q /S release
REM mkdir release
cd release

set SVT_DIR=\Devel\release\svt
set SVT_RELEASE=RELEASE
set SVT_PLATFORM=win32g++
set QWT_DIR=\Devel\qwt5

REM
REM Step 2: Checkout the sources
REM

REM cvs co svt
cd svt
REM mingw32-make distclean
qmake -after "CONFIG+=static" "CONFIG-=dll" "DEFINES-=QT_DLL" "DEFINES+=STATICQT" "QMAKE_CXXFLAGS_WARN_ON=-w" -o Makefile svt.pro
mingw32-make
cd ..

REM cvs co sculptor
cd sculptor
REM mingw32-make distclean
qmake -after "QMAKE_LFLAGS+=-static" "CONFIG+=static" "CONFIG-=console" "CONFIG-=dll" "DEFINES-=QT_DLL" "DEFINES+=STATICQT" "QMAKE_CXXFLAGS_WARN_ON=-w" -o Makefile
mingw32-make
cd ..
cd ..

REM
REM Step 3: Build the distribution file
REM

set PATH=%PATH%;C:\Devel\InnoSetup\
Compil32 /cc "\Devel\release\sculptor\setup\sculptor.iss"
