##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Stride Module ---
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

HEADERS			= 	include/svt_stride.h 		\
				include/sseindexer.h  		\
				include/nsc.h    		


SOURCES 		=	src/sseindexer.cpp		\
				src/stride_progs.cpp      	\
				src/nsc.cpp			


TARGET			=	svt_stride
DESTDIR			=	../lib

win32-msvc:LIBS		+=	svt_system.lib
win32-g++:LIBS		+=	-lsvt_system
macx:LIBS		+=	-lsvt_system


DOC_TITLE		=	"SVT Stride"

