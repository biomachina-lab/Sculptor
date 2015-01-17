##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Volume Test Program ---
#
# Stefan Birmanns 2006
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	volume_rotate.cpp

TARGET			=	volume_rotate

unix:LIBS      		+=	-lsvt_basics -lsvt_system
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_system
