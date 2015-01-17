# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Platform Configuration File ---
#
#
#
# SB 2009
##

message( SVT configured for: Release-Build )

DEFINES +=	LINUX
DEFINES +=	GPLUSPLUS
DEFINES +=	GPLUSPLUS2
DEFINES +=	GPLUSPLUS3
DEFINES +=	GPLUSPLUS4
DEFINES +=	NOLIMITS
DEFINES +=	SVT_NEW_STL
DEFINES +=	SVT_NEW_STREAMS
DEFINES +=	SVT_SYSTEM_QT

QMAKE_LFLAGS_RELEASE  = -static-libgcc

QMAKE_LIBS_X11       += -lXfixes -lXrender -lexpat -lfreetype -lXau -lXdmcp
QMAKE_LIBS_OPENGL_QT += -lGLU
QMAKE_LIBS_OPENGL    += -lGLU
# QMAKE_LIBS           += --lstlport 
# QMAKE_LINK            = gcc -lsupc++
# QMAKE_LINK            = g++ -pthread -fexceptions -O2 -nostdlib `g++ -print-file-name=crt1.o` `g++ -print-file-name=crti.o` `g++ -print-file-name=crtbegin.o` -lstlport -lgcc_s -lpthread -lc -lm `g++ -print-file-name=crtend.o` `g++ -print-file-name=crtn.o`
