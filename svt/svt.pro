##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# Stefan Birmanns 2003
#
#
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
        error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE        =       subdirs

SUBDIRS         =       system          \
                        stride          \
                        basics          \
                        shader          \
                        file_io         \
                        live            \
                        core            \
                        live/devices    \
                        surf		\
                        pdb             \
                        lua             

#
# In release mode the test directories are not build (windows is sooo slow...)
#
contains(DEBCONF,DEBUG) {

SUBDIRS		+=      system/test     \
                        basics/test     \
                        core/test       \
                        live/test       \
                        pdb/test        \
                        lua/test

}
