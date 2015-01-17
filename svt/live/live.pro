##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live Module ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	qt dll
QT			+=	opengl

HEADERS			=	include/live_channel.h \
				include/live_device.h  \
				include/live.h

SOURCES 		=	src/live.cpp

TARGET			=	live
DESTDIR			=	../lib

unix:LIBS      		+=	-ldl -lsvt_basics -lsvt_system
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib
win32-g++:LIBS        	+=	-lsvt_basics -lsvt_system

