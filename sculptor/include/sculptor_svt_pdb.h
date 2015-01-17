/***************************************************************************
                          sculptor_svt_pdb
                          ----------------
    begin                : 11/08/2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_SVT_PDB_H
#define SCULPTOR_SVT_PDB_H

// svt includes
#include <svt_pdb.h>
// qt4 includes
#include <QWidget>

/**
 * Simple class that inherits from the svt_pdb class and overloads the prepareContainer function.
 * \author Stefan Birmanns
 */
class sculptor_svt_pdb : public svt_pdb
{
    QWidget* m_pWidget;

public:

    /**
     * Constructor
     * \param pFileA pointer to a char array with the filename of the first file
     * \param iTypeA file-type of the first file (e.g. SVT_NONE, SVT_PDB, SVT_PSF, SVT_DCD, ...)
     * \param pFileB pointer to a char array with the filename of the second file
     * \param iTypeB file-type of the second file (e.g. SVT_PDB, SVT_PSF, SVT_DCD, ...)
     * \param pPos pointer to svt_pos object for the position
     * \param pProp pointer to svt_properties object
     */
    sculptor_svt_pdb(char* pFileA,        int iTypeA = SVT_PDB,
                     char* pFileB = NULL, int iTypeB = SVT_NONE,
                     svt_pos* pPos = NULL, svt_properties* pProp = NULL);

    /**
     * Constructor
     * Copy an existing svt_point_cloud_pdb object
     * \param rPDB reference to svt_point_cloud_pdb object
     */
    sculptor_svt_pdb( svt_point_cloud_pdb< svt_vector4<Real64 > >& rPDB );

    /**
     * Default Constructor
     */
    sculptor_svt_pdb();

    /**
     * Copy constructor
     */
    sculptor_svt_pdb( svt_pdb& rPDB );

    /**
     * prepare container
     * the new svt_pdb code fills a container of graphics primitives (spheres, cylinders, etc)
     * instead of directly outputting opengl commands.
     */
    virtual void prepareContainer();

    /**
     * Disable a specified QWidget during the next prepareContainer. This will only work exactly
     * once, the next time one changes the rendering mode, one has to call the function again.
     */
    void blockWidget( QWidget* pWidget );
};

#endif
