##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Volume Test Program ---
#
# Stefan Birmanns 2006
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	crop.cpp

TARGET			=	crop

unix:LIBS      		+=	-lsvt_basics -lsvt_system -lsvt_file_io
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib svt_file_io.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_system -lsvt_file_io
