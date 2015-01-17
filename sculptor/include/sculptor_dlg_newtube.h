/*-*-c++-*-******************************************************************
                          sculptor_dlg_newtube.h
                          -----------------
    begin                : 10.11.2010
    author               : Manuel Wahle
                         : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_NEWTUBE_H
#define SCULPTOR_DLG_NEWTUBE_H

// uic-qt4 includes
#include <ui_dlg_newtube.h>
// sculptor includes
class dlg_cartoon;
// svt includes
class svt_pdb;

/**
 * Simple class that implements functions for ui/newtubedlg
 * \author Manuel Wahle
 * \author Francisco Serna
 */
class dlg_newtube : public QDialog, private Ui::dlg_newtube
{
    Q_OBJECT

    // pointer to the cartoon dialog, needed to sync the shader settings
    // (cartoon and newtube share that setting)
    dlg_cartoon * m_pDlgCartoon;
    // pointer to the svt_pdb
    svt_pdb* m_pPdbNode;

public:

    /**
     * Constructor
     * \param pParent pointer to parent widget
     * \param pPdbNode pointer to svt_pdb node
     */
    dlg_newtube(QWidget* pParent, svt_pdb * pPdbNode);

    /**
     * Destructor
     */
    virtual ~dlg_newtube();

    /**
     * Set the profile corners spinbox value from what is stored in the pdb node
     */
    void updateTubeProfileCorners();

    /**
     * Set the segments spinbox value from what is stored in the pdb node
     */
    void updateTubeSegments();

    /**
     * Set the diameter line edit from what is stored in the pdb node
     */
    void updateTubeDiameter();

    /**
     * Set the appropriate checkbox settings for shaders and per pixel lighting using values from pdb node
     */
    void updateUseShader();

    /**
     * Assign cartoon dialog pointer
     * \param pDlgCartoon pointer to dlg_cartoon
     */
    void setCartoonPropDlg(dlg_cartoon * pDlgCartoon);

private slots:

    /**
     * Changes the new tube diameter
     */
    void sSetNewTubeDiameter();

    /**
     * Sets the number of profile corners according to the spinbox
     */
    void sSetNewTubeProfileCorners();

    /**
     * Sets the number of segments according to the spinbox
     */
    void sSetNewTubeSegments();

    /**
     * Enables use of shaders in new tube
     */
    void sToggleNewTubeUseShader();

    /**
     * Enables per pixel lighting in new tube
     */
    void sTogglePerPixelLighting();
};

#endif
