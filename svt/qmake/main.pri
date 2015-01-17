# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Qmake Configuration File ---
#
# Stefan Birmanns 2003
##

###############################################################################
# Global config
###############################################################################

DEBCONF		=	$$(SVT_RELEASE)

# Release configuration
contains(DEBCONF,RELEASE) {
    message( "Release configuration." )

    CONFIG  +=  o_max stl release warn_on exceptions
    
    # Avoid the stack protection, so that Sculptor has a chance to run also on systems with an older glibc
    linux-g++:QMAKE_CFLAGS_RELEASE += -fno-stack-protector 
    linux-g++:QMAKE_CXXFLAGS_RELEASE += -fno-stack-protector 
}


# Experimental configuration (optimized like release, but with experimental features compiled in)
contains(DEBCONF,EXPERIMENTAL) {
    message( "Experimental configuration: Experimental features compiled in. Code optimized, no debug symbols." )

    CONFIG  +=  o_max stl release warn_on exceptions
    DEFINES +=  EXPERIMENTAL
}


# Debug configuration
contains(DEBCONF,DEBUG) {
    message( "Debug configuration: Experimental features compiled in. Debugging symbols included." )

    DEFINES +=  EXPERIMENTAL
    DEFINES +=  DEBUG
    CONFIG  += debug stl warn_on exceptions
}

# What additional directory path needs to be added to the QWT_DIR path, so that the compiler finds the qwt includes? Can be overridden in the platform file...
QWT_ADDPATH	=	include/qwt
QWT_LIB		=	qwt

###############################################################################
# Platform
###############################################################################

PLATFORM 	= 	$(SVT_PLATFORM)

isEmpty( PLATFORM ) {
	error(Please set the SVT_PLATFORM environment variable!)
}

PLATFORM_FILE	=	$(SVT_DIR)/qmake/$${PLATFORM}.pri

!include( $${PLATFORM_FILE} ) {
        error( "Platform configuration file $${PLATFORM_FILE} not found!" )
}

###############################################################################
# Patch for VR theater
###############################################################################

contains( DEFINES, PHANTOM_PATCH ) {
        message( "Applying VR theater Phantom patch" )
}

###############################################################################
# Qt
###############################################################################

contains( DEFINES, SVT_SYSTEM_QT ) {
	CONFIG += thread
        message( "QT backend (with thread support)" )
} 
contains( DEFINES, SVT_SYSTEM_X11 ) {
	message( "X11 backend" )
}
contains( DEFINES, SVT_SYSTEM_WIN32 ) {
	message( "Win32 backend" )
}

###############################################################################
# Qwt
###############################################################################

LOCALQWT              	=     	$$(QWT_DIR)

isEmpty( LOCALQWT ) {
	unix:LOCALQWT   =      	/usr
	win32:LOCALQWT  =      	..
}

INCLUDEPATH           	+=     	$${LOCALQWT}/$${QWT_ADDPATH}

unix:message( QWT: $${LOCALQWT}/$${QWT_ADDPATH} )

unix:QMAKE_LIBDIR       +=      $(QWT_DIR)/lib
win32:QMAKE_LIBDIR      +=      $(QWT_DIR)\\lib

###############################################################################
# STL include files
###############################################################################

LOCALSTL		=	$$(STL_DIR)
!isEmpty( LOCALSTL ) {
    INCLUDEPATH		+=	$(STL_DIR)
    message( STL: $(STL_DIR) )
} 

###############################################################################
# CG include/lib files
###############################################################################

LOCALCG		=	$$(CG_INC_PATH)
LOCALCG2	=	$$(CG_DIR)
!isEmpty( LOCALCG ) {
  # normally windows
  INCLUDEPATH	+=	$(CG_INC_PATH)
  QMAKE_LIBDIR  +=	$(CG_LIB_PATH)
  message( CG : $(CG_INC_PATH) (include))
} else:!isEmpty( LOCALCG2 ) {
  # normally linux
  INCLUDEPATH	+=	$(CG_DIR)/include
  QMAKE_LIBDIR  +=	$(CG_DIR)/lib
  message( CG : $(CG_DIR) ) 
}

#
# Mac Cg Includepath
#
macx:INCLUDEPATH	+=	/Developer/Headers/FlatCarbon
macx:LIBS		+=	-framework CoreFoundation

###############################################################################
# Includepath, obj directory and library directory
###############################################################################

INCLUDEPATH	+=	./include
INCLUDEPATH	+=	$(SVT_DIR)/basics/include
INCLUDEPATH	+=	$(SVT_DIR)/stride/include
INCLUDEPATH	+=	$(SVT_DIR)/file_io/include
INCLUDEPATH	+=	$(SVT_DIR)/system/include
INCLUDEPATH	+=	$(SVT_DIR)/live/include
INCLUDEPATH	+=	$(SVT_DIR)/core/include
INCLUDEPATH	+=	$(SVT_DIR)/pdb/include
INCLUDEPATH	+=	$(SVT_DIR)/lua/include
INCLUDEPATH	+=	$(SVT_DIR)/shader/include
INCLUDEPATH	+=	$(SVT_DIR)/surf/include
INCLUDEPATH	+=	./
INCLUDEPATH     +=      /usr/include/svt

OBJECTS_DIR	=	obj

unix:QMAKE_LIBDIR    +=	$(SVT_DIR)/lib
win32:QMAKE_LIBDIR   +=	$(SVT_DIR)\\lib
# $(SVT_DIR)\3rdParty\lib

macx:INCLUDEPATH	+=	/Library/Frameworks/Cg.framework/Headers

###############################################################################
# GPROF
###############################################################################

# uncomment for GPROF support
# QMAKE_CFLAGS+=-pg
# QMAKE_CXXFLAGS+=-pg
# QMAKE_LFLAGS+=-pg
# uncomment for OpenGL performance analysis (replaces all glVertex commands with glNormals to test for a geometry limited condition).
# DEFINES += OGLPROF

###############################################################################
# NVPerfSDK
###############################################################################

# if you want SVT to be compiled with NVPerfSDK capabilities,
# uncomment the following line (i.e. add SVT_NVPM to the DEFINES variable)
# and adapt the paths below or copy the NVPerfSDK lib and header to these paths

#DEFINES += SVT_NVPM

contains( DEFINES, SVT_NVPM ) {
        message( "Compiling SVT with NVPerfSDK capabilities." )
        linux-g++:INCLUDEPATH        += "/usr/local/include"
        linux-g++:LIBPATH            += "/usr/local/lib"
        linux-g++:LIBS               += -lNVPerfSDK
}
