##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live Polhemus Fastrak Device Driver ---
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

HEADERS			=	live_fastrak.h \
				live_fastrak_drv.h

SOURCES 		=	live_fastrak.cpp \
				live_fastrak_drv.cpp

TARGET			=       live_fastrak

DESTDIR			=	../../../lib

unix:LIBS      		+=	-llive
win32:LIBS		+=	live.lib
