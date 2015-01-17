##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# Correlation test program
#
# Stefan Birmanns 2010
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on exceptions stl console

#

SOURCES 		=	main.cpp


TARGET			=       correlation

unix:LIBS     		+=	-lsvt_basics -lsvt_file_io -lsvt_system
win32-msvc:LIBS      	+=	svt_basics.lib svt_file_io.lib svt_system.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_file_io -lsvt_system
