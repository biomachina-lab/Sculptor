##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live SpaceMouse Device Driver ---
#
# Stefan Birmanns 2003
# Maik Boltes 2005
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE	=	lib
unix: CONFIG	+=	console
win32:CONFIG	+=	dll

unix:HEADERS	=	xdrvlib.h
win32:HEADERS	=	SBTEST32.H \
			si.h \
                   	siapp.h \
                    	spwdata.h \
                    	spwerror.h \
                   	spwmacro.h \
                    	version.h


SOURCES 	=	live_space_mouse.cpp
win32:SOURCES   += 	siapp.c
unix:SOURCES 	+= 	xdrvlib.c

win32:DEFINES	+= 	OS_WIN32

TARGET		= 	live_space_mouse
DESTDIR		=  	../../../lib
QMAKE_LIBDIR	+= 	../../../lib

unix:LIBS	+= 	-llive -lsvt_basics
win32-msvc:LIBS	+= 	live.lib svt_basics.lib user32.lib svt_system.lib svt_core.lib svt_file_io.lib
win32-g++:LIBS	+= 	-llive -lsvt_basics
