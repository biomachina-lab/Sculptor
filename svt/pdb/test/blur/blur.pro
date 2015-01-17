##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# Atomic structure blurring program
#
# Stefan Birmanns 2005
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on o_max opengl console

#

SOURCES 		=	blur.cpp


TARGET			=       blur

unix:LIBS     		+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core -lsvt_pdb
win32-msvc:LIBS		+=	svt_basics.lib svt_file_io.lib svt_system.lib live.lib svt_core.lib svt_pdb.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lsvt_core -lsvt_pdb
