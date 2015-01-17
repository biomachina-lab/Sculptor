##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Array Test Program ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	array.cpp


TARGET			=	array

unix:LIBS		+=	-lsvt_system
win32-msvc:LIBS		+=	svt_system.lib
win32-g++:LIBS		+=	-lsvt_system
