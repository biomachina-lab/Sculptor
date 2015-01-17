##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Teapot Test Program ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on thread
QT			+=	opengl

#

SOURCES 		=	teapot.cpp


TARGET			=       teapot

macx-g++:LIBS		+=      -lsvt_system
linux-g++:LIBS        	+=	-lsvt_system
win32-msvc:LIBS		+=	svt_system.lib
win32-g++:LIBS		+=	-lsvt_system
