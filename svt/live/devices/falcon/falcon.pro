##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- L.I.V.E. - Novint Falcon Driver ---
#
# Jochen Heyd 2007
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	dll

#

SOURCES			=	live_falcon.cpp

TARGET			=       live_falcon
DESTDIR			=	../../../lib

INCLUDEPATH		+=	$(3DTOUCH_BASE)/include
INCLUDEPATH		+=	$(3DTOUCH_BASE)/utilities/include
INCLUDEPATH		+=	$(NOVINT_SDK)/include

QMAKE_LIBDIR		+=	$(3DTOUCH_BASE)/lib
QMAKE_LIBDIR		+=	$(3DTOUCH_BASE)/utilities/lib
QMAKE_LIBDIR		+=	$(SVT_DIR)/lib
QMAKE_LIBDIR		+=	$(NOVINT_SDK)/lib

win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib live.lib hdl.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_system -lhdl -llive 

QMAKE_CFLAGS	+=	-MD
QMAKE_CXXFLAGS	+=	-MD
