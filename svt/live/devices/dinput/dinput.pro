##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live Direct Input Device Driver ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	console

#

HEADERS			=	live_dinput.h
SOURCES 		=	live_dinput.cpp


TARGET			=       live_dinput
DESTDIR			=	../../../lib

unix:LIBS      		+=	-llive
win32:LIBS		+=	live.lib
