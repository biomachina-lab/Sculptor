# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# Fedora Core 4
#
# Stefan Birmanns 2006
##

message( SVT configured for: Fedora Core 4 )

DEFINES	+=	LINUX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS3
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS

#
# Under Linux one can choose between a native X11 backend and a Qt-based one.
# The Qt backend is newer, less efficient right now, but might also be very well supported in the long run.
#
# DEFINES +=	SVT_SYSTEM_X11
DEFINES +=	SVT_SYSTEM_QT

# Patch for Phantom of the VR theater
# DEFINES +=	PHANTOM_PATCH

# QMAKE_CFLAGS  += -Wall -W -Wno-unused -Wno-deprecated
# QMAKE_CXXFLAGS += -Wall -W -Wno-unused -Wno-deprecated

