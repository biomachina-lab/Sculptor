##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live Network Server ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	app
CONFIG			+=	console

#

HEADERS			=	live_net_server.h
SOURCES 		=	live_net_server.cpp	\
				main.cpp


TARGET			=       live_net_server

unix:LIBS		+=	-llive -lsvt_basics -lsvt_file_io -lsvt_system 
win32-msvc:LIBS		+=	live.lib svt_basics.lib svt_file_io.lib svt_system.lib svt_core.lib wsock32.lib DELAYIMP.LIB
win32-g++:LIBS		+=	-llive -lsvt_basics -lsvt_file_io -lsvt_system 
