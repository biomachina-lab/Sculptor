###############################################################################
# SVT_ITK
###############################################################################

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

!include( $(SVT_DIR)/itk/itk.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

###############################################################################
# Actual Project Information
###############################################################################

CONFIG		= console

TEMPLATE 	= lib

SOURCES     	+= src/svt_itkVolume.cpp

TARGET	    	= svt_itk
DESTDIR		= ../lib

LIBS		+= -lsvt_basics -lsvt_file_io
