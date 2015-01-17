##
# Sculptor
# ========
#
# Stefan Birmanns 2003
#
#
##

!include( $(SVT_DIR)/qmake/main.pri ){
        error(Please set the SVT_DIR environment variable!)
}

TEMPLATE                =       app
CONFIG                  +=      qt opengl
QT                      +=      opengl

HEADERS                 =       include/clip_document.h                  \
                                include/pdb_document.h                   \
                                include/pdb_document_dialogs.h           \
                                include/pdb_mode.h                       \
                                include/scene_document.h                 \
                                include/sculptor_app.h                   \
                                include/sculptor_codebook_displacement.h \
                                include/sculptor_doc_eraser.h            \
                                include/sculptor_doc_marker.h            \
                                include/sculptor_document.h              \
                                include/sculptor_force_algo.h            \
                                include/sculptor_gacylinder.h            \
                                include/sculptor_gamultifit.h            \
                                include/sculptor_log.h                   \
                                include/sculptor_log_event.h             \
                                include/sculptor_lua.h                   \
                                include/sculptor_mapexplorer.h           \
                                include/sculptor_node.h                  \
                                include/sculptor_powell_cc.h             \
                                include/sculptor_qwtdata.h               \
                                include/sculptor_qwtplotcurve.h          \
                                include/sculptor_scene.h                 \
                                include/sculptor_splitter.h              \
                                include/sculptor_transfer.h              \
                                include/sculptor_transfer_palette.h      \
                                include/sculptor_transfer_picker.h       \
                                include/sculptor_version.h               \
                                include/sculptor_window.h                \
                                include/situs_document.h                 \
                                include/svt_qtMessages.h

SOURCES                 =       src/main.cpp                             \
                                src/clip_document.cpp                    \
                                src/pdb_document.cpp                     \
                                src/pdb_document_dialogs.cpp             \
                                src/pdb_mode.cpp                         \
                                src/scene_document.cpp                   \
                                src/sculptor_app.cpp                     \
                                src/sculptor_codebook_displacement.cpp   \
                                src/sculptor_doc_eraser.cpp              \
                                src/sculptor_doc_marker.cpp              \
                                src/sculptor_document.cpp                \
                                src/sculptor_force_algo.cpp              \
                                src/sculptor_log.cpp                     \
                                src/sculptor_log_event.cpp               \
                                src/sculptor_lua.cpp                     \
                                src/sculptor_mapexplorer.cpp             \
                                src/sculptor_node.cpp                    \
                                src/sculptor_powell_cc.cpp               \
                                src/sculptor_qwtdata.cpp                 \
                                src/sculptor_qwtplotcurve.cpp            \
                                src/sculptor_scene.cpp                   \
                                src/sculptor_splitter.cpp                \
                                src/sculptor_transfer.cpp                \
                                src/sculptor_transfer_palette.cpp        \
                                src/sculptor_transfer_picker.cpp         \
                                src/sculptor_window.cpp                  \
                                src/situs_document.cpp                   \
                                src/svt_qtMessages.cpp


UI_HEADERS_DIR          =       ui/include
UI_SOURCES_DIR          =       ui/src


QMAKE_CXXFLAGS 	+= -fopenmp


MOC_DIR                 =       moc

INCLUDEPATH             +=      xpm

# defines
DEFINES                 +=      QT_DLL

# library
TARGET                  =       sculptor
macx-g++:TARGET         =       Sculptor

# mac icon file
macx-g++:RC_FILE        +=      setup/sculptor.icns
# win32 resource file which contains the information where the icon file is
win32-g++:RC_FILE       +=      sculptor.rc

# libraries
linux-g++:LIBS          +=      -lsvt_core -llive -lsvt_basics -lsvt_file_io -lsvt_system -lsvt_pdb -lsvt_surf -lsvt_lua -l$${QWT_LIB} -lCgGL -lCg -lsvt_shader -lsvt_stride -fopenmp
macx-g++:LIBS           +=      -lsvt_core -llive -lsvt_basics -lsvt_file_io -lsvt_system -lsvt_pdb -lsvt_surf -lsvt_lua -lqwt -framework Cg -lsvt_shader -lsvt_stride -fopenmp
win32-msvc:LIBS         +=      svt_core.lib live.lib svt_stride svt_shader svt_basics.lib svt_file_io.lib svt_system.lib svt_pdb.lib qwt.lib cg.lib cgGL.lib
win32-g++:LIBS          +=      -lsvt_pdb -lsvt_surf -lsvt_lua -lsvt_core -llive -lsvt_file_io -lqwt -lsvt_shader -lsvt_stride -lsvt_basics -lsvt_system -lglu32 -lopengl32 -lCgGL -lCg -fopenmp

contains( DEFINES, PHANTOM_PATCH ) {
        linux-g++:LIBS  +=      -lHD -lHDU
}


# all dialogs. First the UI files, then the headers for all intermediate classes, then their implementations
FORMS           =       ui/dlg_about.ui             \
                        ui/dlg_atomclustering.ui    \
                        ui/dlg_cartoon.ui           \
                        ui/dlg_clustering.ui        \
                        ui/dlg_colmap.ui            \
                        ui/dlg_colmapsecstruct.ui   \
                        ui/dlg_colorselect.ui       \
                        ui/dlg_crop.ui              \
                        ui/dlg_extract.ui           \
                        ui/dlg_featuredocking.ui    \
                        ui/dlg_fetchpdb.ui          \
                        ui/dlg_flexing.ui           \
                        ui/dlg_floodfill.ui         \
                        ui/dlg_mpfloodfill.ui         \
                        ui/dlg_force.ui             \
                        ui/dlg_gacylinder.ui        \
                        ui/dlg_gamultifit.ui        \
                        ui/dlg_lighting.ui          \
                        ui/dlg_luaeditor.ui         \
                        ui/dlg_lualist.ui           \
                        ui/dlg_mapexplorer.ui       \
                        ui/dlg_newtube.ui           \
                        ui/dlg_pdbvdw.ui            \
                        ui/dlg_preferences.ui       \
                        ui/dlg_refine.ui            \
                        ui/dlg_rmsd.ui              \
                        ui/dlg_swapaxis.ui          \
						ui/dlg_DPSV_filter.ui       \
                        ui/dlg_symmetry.ui          \
                        ui/dlg_toon.ui              \
                        ui/dlg_transform.ui         \
                        ui/dlg_wait.ui              \
                        ui/prop_clip.ui             \
                        ui/prop_eraser.ui           \
                        ui/prop_marker.ui           \
                        ui/prop_pdb.ui              \
                        ui/prop_situs.ui            \
                        ui/window_documentlist.ui


HEADERS         +=      include/sculptor_dlg_about.h                \
                        include/sculptor_dlg_atomtable.h            \
                        include/sculptor_dlg_atomclustering.h       \
                        include/sculptor_dlg_cartoon.h              \
                        include/sculptor_dlg_clustering.h           \
                        include/sculptor_dlg_colmap.h               \
                        include/sculptor_dlg_colmapsecstruct.h      \
                        include/sculptor_dlg_colorselect.h          \
                        include/sculptor_dlg_crop.h                 \
                        include/sculptor_dlg_extract.h              \
                        include/sculptor_dlg_featuredocking.h       \
                        include/sculptor_dlg_fetchpdb.h             \
                        include/sculptor_dlg_flexing.h              \
                        include/sculptor_dlg_floodfill.h            \
                        include/sculptor_dlg_mpfloodfill.h          \
                        include/sculptor_dlg_force.h                \
                        include/sculptor_dlg_gacylinder.h           \
                        include/sculptor_dlg_gamultifit.h           \
                        include/sculptor_dlg_lighting.h             \
                        include/sculptor_dlg_luaeditor.h            \
                        include/sculptor_dlg_lualist.h              \
                        include/sculptor_dlg_mapexplorer.h          \
                        include/sculptor_dlg_maptable.h             \
                        include/sculptor_dlg_newtube.h              \
                        include/sculptor_dlg_pdbvdw.h               \
                        include/sculptor_dlg_plot.h                 \
                        include/sculptor_dlg_preferences.h          \
                        include/sculptor_dlg_refine.h               \
                        include/sculptor_dlg_rmsd.h                 \
                        include/sculptor_dlg_swapaxis.h             \
                        include/sculptor_dlg_symmetry.h             \
                        include/sculptor_dlg_toon.h                 \
                        include/sculptor_dlg_transform.h            \
                        include/sculptor_dlg_wait.h                 \
                        include/sculptor_prop_clip.h                \
						include/sculptor_dlg_DPSV_filter.h 			\
                        include/sculptor_prop_eraser.h              \
                        include/sculptor_prop_marker.h              \
                        include/sculptor_prop_pdb.h                 \
                        include/sculptor_prop_situs.h               \
                        include/sculptor_window_documentlist.h      \
                        include/sculptor_window_documentlistitem.h  \
                        include/sculptor_svt_pdb.h


SOURCES         +=      src/sculptor_dlg_about.cpp                \
                        src/sculptor_dlg_atomtable.cpp            \
                        src/sculptor_dlg_atomclustering.cpp       \
                        src/sculptor_dlg_cartoon.cpp              \
                        src/sculptor_dlg_clustering.cpp           \
                        src/sculptor_dlg_colmap.cpp               \
                        src/sculptor_dlg_colmapsecstruct.cpp      \
                        src/sculptor_dlg_colorselect.cpp          \
                        src/sculptor_dlg_crop.cpp                 \
                        src/sculptor_dlg_extract.cpp              \
                        src/sculptor_dlg_featuredocking.cpp       \
                        src/sculptor_dlg_fetchpdb.cpp             \
                        src/sculptor_dlg_flexing.cpp              \
                        src/sculptor_dlg_floodfill.cpp            \
                        src/sculptor_dlg_mpfloodfill.cpp          \
                        src/sculptor_dlg_force.cpp                \
                        src/sculptor_dlg_gacylinder.cpp           \
                        src/sculptor_dlg_gamultifit.cpp           \
                        src/sculptor_dlg_lighting.cpp             \
                        src/sculptor_dlg_luaeditor.cpp            \
                        src/sculptor_dlg_lualist.cpp              \
                        src/sculptor_dlg_mapexplorer.cpp          \
                        src/sculptor_dlg_maptable.cpp             \
                        src/sculptor_dlg_newtube.cpp              \
                        src/sculptor_dlg_pdbvdw.cpp               \
                        src/sculptor_dlg_plot.cpp                 \
                        src/sculptor_dlg_preferences.cpp          \
                        src/sculptor_dlg_refine.cpp               \
                        src/sculptor_dlg_rmsd.cpp                 \
                        src/sculptor_dlg_swapaxis.cpp             \
                        src/sculptor_dlg_symmetry.cpp             \
                        src/sculptor_dlg_transform.cpp            \
                        src/sculptor_dlg_toon.cpp                 \
                        src/sculptor_dlg_wait.cpp                 \
                        src/sculptor_dlg_DPSV_filter.cpp          \
                        src/sculptor_prop_clip.cpp                \
                        src/sculptor_prop_eraser.cpp              \
                        src/sculptor_prop_marker.cpp              \
                        src/sculptor_prop_pdb.cpp                 \
                        src/sculptor_prop_situs.cpp               \
                        src/sculptor_window_documentlist.cpp      \
                        src/sculptor_window_documentlistitem.cpp  \
                        src/sculptor_svt_pdb.cpp
