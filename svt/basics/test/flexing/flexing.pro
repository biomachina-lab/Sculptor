##
# FLEXING test program
# Created :			09/08/2006
# Last updated: 		09/19/2006
# Author:			Mirabela Rusu 
# 
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE		=	app
CONFIG			+=	console

TARGET			= 	flexing

SOURCES			+= 	main.cpp

unix:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_file_io
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib svt_file_io.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_file_io
