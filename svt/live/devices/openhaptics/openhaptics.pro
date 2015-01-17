##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- L.I.V.E. - SensAble OpenHaptics Driver ---
#
# Stefan Birmanns 2005
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	dll

#

SOURCES			=	live_openhaptics.cpp

TARGET			=       live_openhaptics
DESTDIR			=	"$(SVT_DIR)/lib"

INCLUDEPATH		+=	$(3DTOUCH_BASE)/include
INCLUDEPATH		+=	$(3DTOUCH_BASE)/utilities/include

QMAKE_LIBDIR		+=	$(3DTOUCH_BASE)/lib
QMAKE_LIBDIR		+=	$(3DTOUCH_BASE)/utilities/lib
QMAKE_LIBDIR		+=	$(SVT_DIR)/lib

unix:LIBS      		+=	-llive -lHD -lHDU -lrt
#win32:LIBS		+=	svt_basics.lib live.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib hd.lib hdu.lib
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib live.lib hd.lib
#svt_system.lib  hdu.lib
win32-g++:LIBS 		+=	-llive -lHD -lHDU -lrt

QMAKE_CFLAGS	+=	-MD
QMAKE_CXXFLAGS	+=	-MD
