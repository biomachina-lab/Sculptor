##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- System Module ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

##

TEMPLATE		=	lib
CONFIG			+=	qt dll thread
QT			+=	opengl

contains(DEFINES, SVT_SYSTEM_QT) {
	message( "QT: svt_window_qt runs through moc!" )
	HEADERS		+=	include/svt_window_qt.h 
}

SOURCES 		=	src/svt_window_win32.cpp \
				src/svt_window_x11.cpp   \
                                src/svt_window_qt.cpp    \
	 			src/svt_semaphore.cpp    \
				src/svt_system.cpp       \
				src/svt_teapot.cpp       \
				src/svt_threads.cpp      \
				src/svt_time.cpp         \
				src/svt_window.cpp       \
                                src/svt_opengl.cpp


TARGET			=	svt_system

DESTDIR			=	../lib

MOC_DIR			=	moc

win32-msvc:LIBS		+=	cg.lib imagehlp.lib 
win32-g++:LIBS		+=	-lCg -lpthread 
macx-g++:LIBS		+=	-lGL -lGLU -lpthread -F/Library/Frameworks -framework Cg
linux-g++:LIBS		+=	-lCg -lpthread


##

TEST_TARGETS    	=   	teapot \
				thread_test \
				context_specific_test

DYNAMIC_TEST_LIBS       =       svt_basics
