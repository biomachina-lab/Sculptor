##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- loadScript Test Program ---
#
# Stefan Birmanns 2009
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	main.cpp

TARGET			=	loadScript

unix:LIBS      		+=	-lsvt_basics -lsvt_system -lsvt_lua
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib svt_lua.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_system -lsvt_lua
