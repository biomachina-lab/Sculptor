##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Live Device Drivers ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE	=	subdirs

SUBDIRS		=      	net_client
#      		        net_server	
                        
#win32:SUBDIRS   +=	falcon

#win32:SUBDIRS   +=	    space_mouse \
#                        phantom_omni
#                       dinput phantom

#unix:SUBDIRS    +=      space_mouse     \
#                        fastrak 
#			openhaptics     # needs OpenHaptics (http://www.sensable.com)
