###############################################################################
# SVT_ITK Project Include File
###############################################################################

###############################################################################
# Check ITK Installation
###############################################################################

LOCALITKINC   =	$$(ITK_INC_DIR)
LOCALITKLIB   =	$$(ITK_LIB_DIR)

isEmpty( LOCALITKINC ) {

  LOCALITKINC = /usr/local/include/InsightToolkit

}
isEmpty( LOCALITKLIB ) {

  LOCALITKLIB = /usr/local/lib/InsightToolkit

}

exists( LOCALITKINC ) {
      message( "ITK include-directory not found!" )
}

message( ITK : $$LOCALITKINC (include))
message( ITK : $$LOCALITKLIB (lib))

INCLUDEPATH 	+= $$LOCALITKINC
INCLUDEPATH 	+= $$LOCALITKINC/Code
INCLUDEPATH 	+= $$LOCALITKINC/Code/Algorithms
INCLUDEPATH 	+= $$LOCALITKINC/Code/BasicFilters
INCLUDEPATH 	+= $$LOCALITKINC/Code/Common
INCLUDEPATH 	+= $$LOCALITKINC/Code/IO
INCLUDEPATH 	+= $$LOCALITKINC/Code/Numerics
INCLUDEPATH 	+= $$LOCALITKINC/Code/Numerics/Statistics
INCLUDEPATH 	+= $$LOCALITKINC/Code/Numerics/vxl
INCLUDEPATH 	+= $$LOCALITKINC/Code/Numerics/vxl/vcl
INCLUDEPATH 	+= $$LOCALITKINC/Code/Numerics/vxl/vnl
INCLUDEPATH 	+= $$LOCALITKINC/Code/Patented
INCLUDEPATH 	+= $$LOCALITKINC/Code/Review
INCLUDEPATH 	+= $$LOCALITKINC/Code/SpatialObject
INCLUDEPATH 	+= $$LOCALITKINC/Utilities/
INCLUDEPATH 	+= $$LOCALITKINC/Utilities/MetaIO
INCLUDEPATH 	+= $$LOCALITKINC/Utilities/vxl
INCLUDEPATH 	+= $$LOCALITKINC/Utilities/vxl/vcl
INCLUDEPATH 	+= $$LOCALITKINC/Utilities/vxl/core

INCLUDEPATH 	+= $$LOCALITKLIB
INCLUDEPATH 	+= $$LOCALITKLIB/Utilities/vxl
INCLUDEPATH 	+= $$LOCALITKLIB/Utilities/vxl/vcl
INCLUDEPATH 	+= $$LOCALITKLIB/Utilities/vxl/core

LIBPATH	    	+= $$LOCALITKLIB/bin


OBJECTS_DIR 	= obj

LIBS += -lITKCommon
LIBS += -lITKBasicFilters
LIBS += -lITKAlgorithms
LIBS += -lITKNumerics
LIBS += -lITKStatistics
LIBS += -lITKIO
LIBS += -lITKMetaIO
LIBS += -lITKniftiio
LIBS += -lITKNrrdIO
LIBS += -lITKDICOMParser
LIBS += -lITKFEM
LIBS += -lITKznz
LIBS += -lITKEXPAT
LIBS += -lITKSpatialObject
LIBS += -lITKStatistics

LIBS += -litksys
LIBS += -litkvnl
LIBS += -litkvnl_algo
LIBS += -litkvnl_inst
LIBS += -litkv3p_netlib
LIBS += -litkvcl
LIBS += -litkjpeg8
LIBS += -litkjpeg12
LIBS += -litkjpeg16
LIBS += -litkopenjpeg
LIBS += -litkpng
LIBS += -litktiff
LIBS += -litkgdcm
LIBS += -litkzlib

INCLUDEPATH	+= $(SVT_DIR)/itk/include

