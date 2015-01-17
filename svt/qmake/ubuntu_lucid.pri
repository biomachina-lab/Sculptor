# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# 
# Ubuntu Lucid Lynx (10.04)
#
# Stefan Birmanns 2010
##

message( SVT configured for: Ubuntu Lucid Lynx )

DEFINES	+=	LINUX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS3
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
#DEFINES += 	SVT_SYSTEM_X11
DEFINES +=	SVT_SYSTEM_QT

# Patch for Phantom of the VR theater
# DEFINES +=	PHANTOM_PATCH

QWT_ADDPATH	= include/qwt-qt4
QWT_LIB		= qwt-qt4
