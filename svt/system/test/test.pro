##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- System Module Test Programs ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE	=	subdirs

SUBDIRS		=	teapot	\
			thread
#                        context
