##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Basic Core Test Program ---
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

SOURCES 		=	basic.cpp


TARGET			=       basic

linux-g++:LIBS        	+=	-lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_basics -lsvt_system 
win32-g++:LIBS        	+=	-lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_basics -lsvt_system
macx-g++:LIBS		+=      -lsvt_core -llive -lsvt_file_io -F/Library/Frameworks -framework Cg -lsvt_shader -lsvt_basics -lsvt_system

win32-msvc:LIBS      	+=	svt_core.lib live.lib svt_basics.lib svt_file_io.lib svt_system.lib cg.lib cgGL.lib svt_shader.lib
