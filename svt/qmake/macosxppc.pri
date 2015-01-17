# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# MacOS X
#
# Stefan Birmanns 2006
##

CONFIG += qt opengl

message( SVT configured for: MacOSX (PPC) )

DEFINES	+=	LINUX
DEFINES +=	MACOSX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS3
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
DEFINES	+=	SVT_SYSTEM_QT

QMAKE_LFLAGS += -L/usr/X11R6/lib --disable-prebinding
