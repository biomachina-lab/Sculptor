##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Basic Container Test Program ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max opengl console

#

SOURCES 		=	basic_container_arrows_at_origin.cpp


TARGET			=       basic_container_arrows_at_origin

macx-g++:LIBS		+=      -lsvt_core -lsvt_basics -lsvt_file_io -lsvt_system -llive -F/Library/Frameworks -framework Cg
linux-g++:LIBS        	+=	-lsvt_core -lsvt_basics -lsvt_file_io -lsvt_system -llive -lCgGL -lCg
win32-msvc:LIBS      	+=	svt_core.lib live.lib svt_basics.lib svt_file_io.lib svt_system.lib cg.lib cgGL.lib
win32-g++:LIBS        	+=	-lsvt_core -lsvt_basics -lsvt_file_io -lsvt_system -llive -lCgGL -lCg
