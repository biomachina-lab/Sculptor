##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Basics Module ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

## debug 

TEMPLATE		=	lib
CONFIG			+=	qt dll console
QT			+=	opengl

HEADERS			= 	include/svt_stree.h                  \
				include/svt_binary_tree.h            \
				include/svt_rnd.h                    \
                                include/svt_multivec.h               \
                                include/svt_trn.h                    \
                                include/svt_point_cloud.h            \
                                include/svt_point_cloud_basic.h      \
                                include/svt_point_cloud_match.h      \
                                include/svt_member2.h		     \
                                include/svt_powell_cc.h		     \
                                include/svt_powell_vol.h	     \
				include/svt_ga.h		     \
				include/svt_gamultifit.h 	     \
				include/svt_gacylinder.h


SOURCES 		=	src/svt_scattering_function.cpp      \
                                src/svt_pair_distribution.cpp        \
                                src/svt_pos.cpp                      \
				src/svt_matrix4f.cpp                 \
                                src/svt_matrix4.cpp                  \
                                src/svt_vector4f.cpp                 \
				src/svt_types.cpp                    \
			       	src/svt_streamableObject.cpp         \
				src/svt_exception.cpp                \
				src/svt_iomanip.cpp                  \
				src/svt_messages.cpp                 \
				src/svt_random.cpp                   \
				src/svt_string.cpp                   \
				src/svt_str.cpp                      \
				src/svt_streamableContainers.cpp     \
			    	src/svt_fstream.cpp                  \
				src/svt_bfstream.cpp                 \
                		src/svt_spline.cpp                   \
                                src/svt_volLoadPlugin.cpp            \
                                src/svt_stree.cpp                    \
                                src/svt_binary_tree.cpp              \
				src/svt_rnd.cpp                      \
                                src/svt_powell.cpp                   \
                                src/svt_point_cloud_atom.cpp         \
                                src/svt_point_cloud_bond.cpp         \
                                src/svt_point_cloud_link.cpp         \
                                src/svt_flexing.cpp                  \
                                src/svt_flexing_inverse_dist_weighting.cpp \
                                src/svt_flexing_spline.cpp           \
                                src/svt_flexing_elastic_body_spline.cpp \
                                src/svt_flexing_thin_plate_spline.cpp \
                                src/svt_ga_ind.cpp		     \
				src/svt_gamultifit_ind.cpp	     \
				src/svt_optimizer.cpp		     \
				src/svt_optimizer_beads_cc.cpp       \
				src/svt_gacylinder_ind.cpp	     \
				src/svt_tube.cpp

# library
TARGET			=	svt_basics
DESTDIR			=	../lib

QMAKE_CXXFLAGS 	+= -fopenmp

win32-msvc:LIBS		+=	svt_system.lib svt_stride.lib
win32-g++:LIBS		+=	-lsvt_system -lsvt_stride
macx:LIBS		+=	-lsvt_system -lsvt_stride

DOC_TITLE		=	"SVT Basics"

TEST_TARGETS    	=   	array_test                           \
	       			field_test                           \
				exception_test                       \
				io_manip_test                        \
				matrix_test                          \
                                matrix4_test                         \
				matrix_streamin_test                 \
				matrix_streaminb_test                \
				matrix_streamout_test                \
				matrix_streamoutb_test               \
                                vector4_test                         \
				vector4f_test                        \
                		matrix4f_test                        \
				streamVector_test                    \
				streamList_test                      \
                		point_test                           \
                		plane_test                           \
                        	spline_route                         \
                                point_cloud

irix65-n32:DYNAMIC_TEST_LIBS += m
irix65-64:DYNAMIC_TEST_LIBS += m
