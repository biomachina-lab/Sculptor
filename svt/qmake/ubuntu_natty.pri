# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
#
#
#
# MW 2008
##

message( SVT configured for: Ubuntu Natty (11.04) and Linux Mint 11)

DEFINES +=	LINUX
DEFINES +=	GPLUSPLUS2
DEFINES +=	GPLUSPLUS3
DEFINES +=	NOLIMITS
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
DEFINES +=	SVT_SYSTEM_QT
QWT_ADDPATH       = include/qwt-qt4
unix:QMAKE_LIBDIR = /usr/lib

QWT_LIB           = qwt-qt4

