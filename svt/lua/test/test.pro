##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Lua Module Test Programs ---
#
# Stefan Birmanns 2009
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}


TEMPLATE	=	subdirs

SUBDIRS		=	loadScript		\
			runScript		\
                        luna
