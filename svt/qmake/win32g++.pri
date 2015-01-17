# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# 
#  WINDOWS MINGW32
#
# Stefan Birmanns 2007
##

message( SVT configured for: WINDOWS (MINGW32) )

DEFINES 		+= WINDOWS WIN32 GPLUSPLUS3
DEFINES 		-= UNICODE
DEFINES 		+= SVT_NEW_STL
DEFINES 		+= SVT_NEW_STREAMS

# Qt backend
DEFINES 		+= SVT_SYSTEM_QT

# Win32 backend, which is outdated and has not been tested for years
# DEFINES 		+= SVT_SYSTEM_WIN32

CONFIG 			+= opengl
LIBS   		     	+= -lws2_32
QMAKE_LFLAGS_RELEASE 	+= -static-libgcc
#-static-libstdc+++

# fixes a problem with mingw32. compare http://msdn.microsoft.com/en-us/library/aa383745(v=vs.85).aspx
!contains( DEFINES, _WIN32_WINNT ) {
    DEFINES         += _WIN32_WINNT=0x0501
	}

QWT_ADDPATH=src
