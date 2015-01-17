##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Point Test Program ---
#
# Stefan Birmanns 2004
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	demoVLP.cpp


TARGET			=       svt_vlp_demo

DESTDIR			= 	../../../lib

unix:LIBS		+=	-lsvt_system
win32-msvc:LIBS		+=	svt_system.lib
win32-g++:LIBS		+=	-lsvt_system
