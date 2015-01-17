# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# MacOS X
#
# Stefan Birmanns 2006
##

CONFIG += qt opengl

message( SVT configured for: MacOSX )

DEFINES	+=	LINUX
DEFINES +=	MACOSX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS3
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
DEFINES	+=	SVT_SYSTEM_QT

# INCLUDEPATH  += /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/include
# QMAKE_LFLAGS += -L/usr/X11R6/lib --disable-prebinding
