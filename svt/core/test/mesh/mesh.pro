##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Mesh Test Program ---
#
# Maik Boltes 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##  debug o_max 

TEMPLATE		=	app
CONFIG			+=	o_max warn_on opengl console

#

SOURCES 		=	mesh.cpp volio.cpp


TARGET			=   mesh

linux-g++:LIBS        	+=	-lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_basics -lsvt_system 
win32-g++:LIBS        	+=	-lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_basics -lsvt_system
macx-g++:LIBS		+=      -lsvt_core -llive -lsvt_file_io -F/Library/Frameworks -framework Cg -lsvt_shader -lsvt_basics -lsvt_system

win32-msvc:LIBS      	+=	svt_core.libsvt_shader.lib live.lib svt_basics.lib svt_file_io.lib svt_system.lib cg.lib cgGL.lib
