# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# 
# RedHat 9 (Shrike)
#
# Stefan Birmanns 2003
##

message( SVT configured for: Fedora Core 2 (Tettnang) )

DEFINES	+=	LINUX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS3
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
#DEFINES += 	SVT_SYSTEM_X11
DEFINES +=	SVT_SYSTEM_QT

# Patch for Phantom of the VR theater
# DEFINES +=	PHANTOM_PATCH

# QMAKE_CFLAGS  += -Wall -W -Wno-unused -Wno-deprecated
# QMAKE_CXXFLAGS += -Wall -W -Wno-unused -Wno-deprecated
