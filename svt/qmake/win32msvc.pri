# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
# 
#  WINDOWS VC++
#
# Stefan Birmanns 2003
##

message( SVT configured for: WINDOWS (VC++) )

DEFINES +=      WINDOWS WIN32 WIN32_MSVC TRUE_WIN32
DEFINES -=      UNICODE
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
DEFINES +=	SVT_SYSTEM_WIN32

# The Qt backend is not recommended right now under Windows.
# DEFINES += 	SVT_SYSTEM_QT

CONFIG +=	console opengl
