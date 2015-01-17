##-*-makefile-*-
##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Shader Module ---
#
# Manuel Wahle 2010
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE		=	lib
CONFIG			+=	dll opengl console stl 


SOURCES 		=       src/svt_shader_manager.cpp          \
                                src/svt_shader_glsl.cpp             \
                                src/svt_shader_cg.cpp               \
                                src/svt_shader_cg_volume.cpp        \
                                src/svt_shader_cg_volume_nospec.cpp \
                                src/svt_shader_cg_volume_fast.cpp   \
                                src/svt_shader_cg_gpgpu.cpp         \
                                src/svt_shader_cg_toon.cpp          \
                                src/svt_shader_cg_cartoon.cpp       \
                                src/svt_shader_cg_cartoon_fragment.cpp \
                                src/svt_shader_cg_tube.cpp          \
                                src/svt_shader_cg_ao.cpp



TARGET			=	svt_shader
DESTDIR			=	../lib

#
# MinGW
#   This is needed as otherwise linking to standard windows DLLs (like Cg) brings trouble while linking our DLL (cannot export...)
#
win32-g++:QMAKE_LFLAGS_WINDOWS_DLL = -Wl,-subsystem,windows -shared
win32-g++:QMAKE_LFLAGS_CONSOLE_DLL = -Wl,-subsystem,windows -shared
#   This is again because of linking problems with Cg. Only necessary in core, the other modules should only have WIN32 defined.
win32-g++:DEFINES	+=	WIN32_MSVC

#
# Libraries
#
macx-g++:LIBS		+=      -lsvt_basics -lsvt_system  -F/Library/Frameworks -framework Cg 
linux-g++:LIBS     	+=	-lsvt_basics -lsvt_system -lCgGL -lCg
win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib cg.lib cgGL.lib
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_system -lCgGL -lCg
