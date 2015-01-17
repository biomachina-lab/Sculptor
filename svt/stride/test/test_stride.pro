##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Test_stride Program ---
#
#
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max console

#

SOURCES 		=	main.cpp

TARGET			=	test_stride

unix:LIBS      		+=	-lsvt_basics -lsvt_stride -lsvt_system -lsvt_file_io
win32-msvc:LIBS		+=	svt_basics.lib svt_stride.lib  svt_system.lib svt_file_io.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_stride -lsvt_system -lsvt_file_io
