##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Core Module Test Programs ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE	=	subdirs

SUBDIRS		=	basic  			\
                        basic_container		\
                        vol3d			\
                        extrude			\
                        mesh
#                        heightfield		\
#                        heightfield_precompute	\
#			 sky			\
