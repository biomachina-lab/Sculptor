##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Basics Module Test Programs ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}


TEMPLATE	=	subdirs

SUBDIRS		=	applyMat		\
			array 			\
                        append			\
                        blur			\
                        boxfill			\
                        center			\
                        crop			\
                        clustering              \
                        c_fit                   \
                        c_pdb                   \
                        c_vol                   \
                        extractca		\
                        extractchain		\
                        extractmodel		\
                        field			\
                        flexing                 \
                        floodfill               \
               		gamultifit              \
               		gacylinder              \
                        io_manip 		\
                        mask			\
                        match		       	\
			matrix 			\
                        matrix_streamin 	\
                        matrix_streaminb 	\
                        matrix_streamout	\
                        matrix_streamoutb	\
                        matrix4 		\
                        plane 			\
                        point_cloud_pdb         \
                        powell			\
                        powell_vol              \
                        rmsd			\
                        situs2df3		\
                        streamVector 		\
                        streamList		\
                        vector4   		\
                        volume			\
                        volume_mirror           \
                        volume_rotate
