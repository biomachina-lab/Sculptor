###############################################################################
# FAM Test Project File
# Created: 08/23/2007
# Author: Stefan Birmanns
# Email: sculptor@biomachina.org
###############################################################################

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

#

TEMPLATE		=	app
CONFIG			+=	console

SOURCES 		=	main.cpp

TARGET			=       fam

unix:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_file_io
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib svt_file_io.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_file_io
