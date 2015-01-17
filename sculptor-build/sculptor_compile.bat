echo off

REM ###################################################################################################
REM # Simple Sculptor build script for Windows
REM ###################################################################################################

cd \Devel

set SVT_DIR=\Devel\svt
set SVT_RELEASE=DEBUG

cd svt
qmake -after "CONFIG+=staticlib" "CONFIG-=dll" "DEFINES-=QT_DLL" "DEFINES+=STATICQT" "QMAKE_CXXFLAGS_WARN_ON=-w" -o Makefile svt.pro
make
cd ..

cd sculptor
qmake -after "CONFIG-=console" "CONFIG-=dll" "DEFINES-=QT_DLL" "DEFINES+=STATICQT" "QMAKE_CXXFLAGS_WARN_ON=-w" -o Makefile
make
