##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- PDB Module ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	qt dll console stl 
QT			+=	opengl

SOURCES 		=	src/svt_atom.cpp                  \
                                src/svt_bond.cpp                  \
                                src/svt_tube.cpp                  \
				src/svt_pdb.cpp             	  \
                                src/svt_atom_colmap.cpp           \
                                src/svt_atom_type_colmap.cpp      \
                                src/svt_atom_resname_colmap.cpp   \
                                src/svt_atom_chainid_colmap.cpp   \
                                src/svt_atom_solid_colmap.cpp     \
                                src/svt_atom_segid_colmap.cpp     \
                                src/svt_atom_structure_colmap.cpp \
                                src/svt_atom_model_colmap.cpp     \
                                src/svt_atom_beta_colmap.cpp      \
                                src/svt_cr_spline.cpp		  \
                                src/svt_pdb_cartoon.cpp		  \
                                src/svt_kb_spline.cpp             \
                                src/svt_container_cartoon.cpp     \
                                src/svt_container_toon.cpp        \
#	                        src/svt_pdb_surface


TARGET			=	svt_pdb
DESTDIR			=	../lib

#
# MinGW
#   This is needed as otherwise linking to standard windows DLLs (like Cg) brings trouble while linking our DLL (cannot export...)
#
win32-g++:QMAKE_LFLAGS_WINDOWS_DLL = -Wl,-subsystem,windows -shared
win32-g++:QMAKE_LFLAGS_CONSOLE_DLL = -Wl,-subsystem,windows -shared
#   This is again because of linking problems with Cg. Only necessary in core, the other modules should only have WIN32 defined.
win32-g++:DEFINES	+=	WIN32_MSVC

macx-g++:LIBS		+=      -lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core -lsvt_stride -F/Library/Frameworks -framework Cg -lsvt_shader -lsvt_surf
linux-g++:LIBS     	+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core -lsvt_stride -lCgGL -lCg -lsvt_shader -lsvt_surf
win32-msvc:LIBS		+=	svt_basics.lib svt_file_io.lib svt_system.lib live.lib svt_stride.lib svt_core.lib svt_shader svt_surf
win32-g++:LIBS         	+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core -lsvt_stride -lCgGL -lCg -lsvt_shader -lsvt_surf

