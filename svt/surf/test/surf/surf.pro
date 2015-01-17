##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Surf Test Program ---
#
# Stefan Birmanns 2010
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	surf.cpp

TARGET			=	surf

linux-g++:LIBS        	+=	-lsvt_pdb -lsvt_surf -lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_basics -lsvt_system 
win32-g++:LIBS        	+=	-lsvt_surf -lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_basics -lsvt_system
macx-g++:LIBS		+=      -lsvt_surf -lsvt_core -llive -lsvt_file_io -F/Library/Frameworks -framework Cg -lsvt_shader -lsvt_basics -lsvt_system

win32-msvc:LIBS      	+=	svt_surf.lib svt_core.lib live.lib svt_basics.lib svt_file_io.lib svt_system.lib cg.lib cgGL.lib svt_shader.lib
