##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Simple PDB Viewer Program ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	opengl

#

SOURCES 		=	pdb_view.cpp


TARGET			=       pdb_view
                                
linux-g++:LIBS        	+=	-lsvt_pdb -lsvt_core -llive -lsvt_file_io -lCgGL -lCg -lsvt_shader -lsvt_stride -lsvt_basics -lsvt_system
win32-g++:LIBS        	+=	-lsvt_core -llive -lsvt_basics -lsvt_file_io -lsvt_system -lsvt_pdb -lCgGL -lCg -lsvt_shader -lsvt_stride
macx-g++:LIBS		+=      -lsvt_core -llive -lsvt_basics -lsvt_file_io -lsvt_system -lsvt_pdb -F/Library/Frameworks -framework Cg -lsvt_shader -lsvt_stride

win32-msvc:LIBS      	+=	svt_pdb.lib svt_stride.lib svt_core.lib live.lib svt_basics.lib svt_file_io.lib svt_system.lib  cg.lib cgGL.lib svt_shader.lib
