##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Surf Module ---
#
# 
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

## debug 

TEMPLATE		=	lib
CONFIG			+=	qt dll console
QT			+=	opengl

HEADERS			= 	include/svt_container_surf.h	\
				include/chull.h			\
                                include/dual.h			\
                                include/linalg.h		\
	                        include/surf.h

SOURCES 		=	src/chull.cpp  			\
				src/compute.cpp  		\
                                src/dual.cpp  			\
                                src/io.cpp  			\
                                src/lp.cpp  			\
                                src/tessel_cases.cpp  		\
                                src/tessel_concave.cpp 		\
                                src/tessel_convex.cpp  		\
                                src/tessel_patches.cpp 		\
                                src/tessel_torus.cpp  		\
                                src/utils.cpp			\
                                src/svt_container_surf.cpp

TARGET			=	svt_surf
DESTDIR			=	../lib

macx-g++:LIBS		+=      -lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core 
linux-g++:LIBS     	+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core
win32-msvc:LIBS		+=	svt_basics.lib svt_file_io.lib svt_system.lib live.lib svt_core.lib
win32-g++:LIBS         	+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core

DOC_TITLE		=	"SVT Surf"
