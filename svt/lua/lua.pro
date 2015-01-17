##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- SVT/Lua Module ---
#
# Stefan Birmanns 2009
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

## debug 

TEMPLATE		=	lib
CONFIG			+=	dll console
QT			+=	opengl

HEADERS			= 	include/lapi.h     	\
				include/lcode.h   	\
				include/ldo.h    	\
				include/lgc.h   	\
				include/llimits.h  	\
				include/lobject.h   	\
				include/lparser.h  	\
				include/lstring.h  	\
				include/ltm.h  	       	\
				include/luaconf.h  	\
				include/lundump.h  	\
				include/lzio.h		\
				include/lauxlib.h  	\
				include/ldebug.h  	\
				include/lfunc.h  	\
				include/llex.h  	\
				include/lmem.h     	\
				include/lopcodes.h  	\
				include/lstate.h   	\
				include/ltable.h   	\
				include/lua.h  		\
				include/lualib.h   	\
				include/lvm.h           \
                                include/svt_lua.h

SOURCES 		=	src/lapi.cpp		\
				src/lcode.cpp		\
				src/ldebug.cpp		\
				src/ldo.cpp		\
				src/ldump.cpp		\
				src/lfunc.cpp		\
				src/lgc.cpp		\
				src/llex.cpp		\
				src/lmem.cpp		\
				src/lobject.cpp		\
				src/lopcodes.cpp       	\
				src/lparser.cpp		\
				src/lstate.cpp		\
				src/lstring.cpp		\
				src/ltable.cpp		\
				src/ltm.cpp		\
				src/lundump.cpp		\
				src/lvm.cpp		\
				src/lzio.cpp		\
				src/lauxlib.cpp		\
				src/lbaselib.cpp       	\
				src/ldblib.cpp		\
				src/liolib.cpp		\
				src/lmathlib.cpp       	\
				src/loslib.cpp		\
				src/ltablib.cpp		\
				src/lstrlib.cpp		\
				src/loadlib.cpp		\
				src/linit.cpp		\
				src/svt_lua.cpp

# library
TARGET			=	svt_lua
DESTDIR			=	../lib

win32-msvc:LIBS		+=	svt_basics.lib svt_system.lib
win32-g++:LIBS		+=	-lsvt_basics -lsvt_system
macx:LIBS		+=	-lsvt_basics -lsvt_system

DOC_TITLE		=	"SVT Lua"

TEST_TARGETS    	=   	basic
