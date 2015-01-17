##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- PDB Module Test Programs ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE	=	subdirs

SUBDIRS		=	pdb_view
#			blur

macx-g++:LIBS		+=      -lsvt_pdb -lsvt_stride -lsvt_core -lsvt_basics -lsvt_file_io -lsvt_system -llive  -F/Library/Frameworks -framework Cg
linux-g++:LIBS        	+=	-lsvt_pdb -lsvt_stride -lsvt_core -lsvt_basics -lsvt_file_io -lsvt_system -llive  -lCgGL -lCg
win32-msvc:LIBS      	+=	svt_pdb.lib svt_stride.lib svt_core.lib live.lib svt_basics.lib svt_file_io.lib svt_system.lib  cg.lib cgGL.lib
win32-g++:LIBS        	+=	-lsvt_pdb -lsvt_stride -lsvt_core -lsvt_basics -lsvt_file_io -lsvt_system -llive  -lCgGL -lCg
