##
# 
# Created :			01/16/09
# Author:			Mirabela Rusu 
# 
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE		=	app
CONFIG			+=	console

TARGET			= 	gaMultiFit

SOURCES			+= 	main.cpp

unix:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_file_io
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib svt_file_io.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_file_io

# DEFINES 		+= 	QMAKE_CGLIBCPP_FORCE_NEW=1 GLIBCXX_FORCE_NEW=1
