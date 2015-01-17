##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Context Specific Data Test Program ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max opengl thread

#

SOURCES 		=	context_specific.cpp


TARGET			=       context_specific

unix:LIBS      		+=	-lsvt_system -lsvt_basics
win32:LIBS     		+=	svt_system.lib svt_basics.lib
