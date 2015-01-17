# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
#
#
#
# MW 2008
##

message( SVT configured for: Debian Sid )

DEFINES +=	LINUX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS2
DEFINES +=	GPLUSPLUS3
DEFINES +=	GPLUSPLUS4
DEFINES +=	NOLIMITS
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
DEFINES +=	SVT_SYSTEM_QT
QWT_ADDPATH       =
unix:QMAKE_LIBDIR = /usr/lib
QWT_LIB           = qwt-qt4

