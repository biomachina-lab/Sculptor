###############################################################################
# WATERSHED Test Project File
# Created: 07/02/2007
# Author: Stefan Birmanns
# Email: sculptor@biomachina.org
###############################################################################

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

!include( $(SVT_DIR)/itk/itk.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

###############################################################################
# Actual Project Information
###############################################################################

TEMPLATE		=	app
CONFIG			+=	console

SOURCES 		=	src/main.cpp

TARGET			=       watershed

linux-g++:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_itk
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib svt_itk.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_itk
macosx-g++:LIBS		+=	-lsvt_basics -lsvt_system -lsvt_itk
