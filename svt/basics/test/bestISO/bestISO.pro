##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# bestISO test driver - Rebecca Cooper 2006 
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	warn_on exceptions stl console

#

SOURCES 		=	driver.cpp


TARGET			=       driver

unix:LIBS     		+=	-lsvt_basics -lsvt_file_io
win32-msvc:LIBS       	+=	svt_basics.lib svt_file_io.lib
win32-g++:LIBS         	+=	-lsvt_basics -lsvt_file_io
