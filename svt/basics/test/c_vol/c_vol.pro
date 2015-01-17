###############################################################################
# C_VOL Test Project File
# Created: 02/12/2006
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

TARGET			=       c_vol

unix:LIBS		+=	-lsvt_basics -lsvt_system
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system
