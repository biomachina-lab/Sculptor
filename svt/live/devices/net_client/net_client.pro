##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live Network Client Device Driver ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
unix:CONFIG			+=	console
win32:CONFIG			+=	dll

#

HEADERS			=	live_net_client.h
SOURCES 		=	live_net_client.cpp


TARGET			=       live_net_client
DESTDIR			=	../../../lib

unix:LIBS		+=	-lsvt_system -llive -lsvt_basics -lsvt_file_io -lsvt_core
win32-msvc:LIBS		+=	live.lib svt_basics.lib user32.lib svt_system.lib svt_core.lib svt_file_io.lib wsock32.lib DELAYIMP.LIB
win32-g++:LIBS		+=	-lsvt_system -llive -lsvt_basics -lsvt_file_io -lsvt_core
