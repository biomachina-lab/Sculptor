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

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	volLoadPlugin.cpp


TARGET			=	volLoadPlugin

unix:LIBS      		+=	-lsvt_basics -lsvt_system
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system
