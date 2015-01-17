##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- L.I.V.E. - SensAble Phantom Device Driver ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	dll

#

HEADERS			=	liveForceField.h
SOURCES 		=	liveForceField.cpp	\
				live_phantom.cpp

INCLUDEPATH		+=    	$(GHOST_DIR)/include/GHOST
INCLUDEPATH		+=    	$(GHOST_DIR)/include/GHOST/stl

TARGET			=       live_phantom
DESTDIR			=	$(SVT_DIR)/lib

QMAKE_LIBDIR		+=	$(QWT_DIR)/lib

unix:LIBS      		+=	-llive -lghost -lPHANToMIO -lrt
win32-msvc:LIBS		+=	live.lib ghost.lib
win32-g++:LIBS 		+=	-llive -lghost -lPHANToMIO -lrt
