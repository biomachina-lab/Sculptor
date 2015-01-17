##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- File I/O Module ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	dll x11 console
QT			+=	opengl

SOURCES 		=	src/svt_config.cpp           \
                                src/svt_file_utils.cpp       \
                                src/svt_pic_reader.cpp       \
                                src/svt_pic_reader_bmp.cpp   \
                                src/svt_pic_reader_tga.cpp   \
                                src/svt_save_framebuffer.cpp \
                                src/svt_swap.cpp \
                                src/svt_tinystr.cpp \
                                src/svt_tinyxml.cpp \
                                src/svt_tinyxmlerror.cpp \
                                src/svt_tinyxmlparser.cpp


TARGET			=	svt_file_io
DESTDIR			=	../lib

LIBS			+=	-lsvt_basics -lsvt_system
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system

