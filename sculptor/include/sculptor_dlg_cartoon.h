/*-*-c++-*-*****************************************************************
                          sculptor_dlg_cartoon.h
			  -------------------
    begin                : 10/06/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_CARTOON_H
#define SCULPTOR_DLG_CARTOON_H

// uic-qt4 includes
#include <ui_dlg_cartoon.h>
// svt includes
class pdb_document;
class svt_pdb;
class dlg_colmapsecstruct;
class dlg_newtube;

class dlg_cartoon : public QDialog, private Ui::dlg_cartoon
{
    Q_OBJECT

    // svt_pdb where the parameters are to be set
    svt_pdb * m_pPdbNode;
    // needed to get information about an opwn sec structure color dialog
    pdb_document * m_pPdbDocument;
    // when the minimum number of rtesidues per sheet/helix is changed, the sec struc color dialog
    // needs to know about it
    dlg_colmapsecstruct * m_pMyColmapDlgSecStruct;
    // when the tube diameter or shader use is changed, the newtube dialog needs to know about it
    dlg_newtube * m_pMyNewTube_Options;

public:

    /*
     * Constructor
     */
    dlg_cartoon(QWidget* pParent, pdb_document* pPdbDocument);

    /*
     * Destructor
     */
    virtual ~dlg_cartoon();

    /**
     * update if this value is changed in the sec structure color dialog
     */
    void updateMinRes();

    /**
     * update if this value is changed in the newtube dialog
     */
    void updateTubeProfileCorners();

    /**
     * update if this setting is changed in the newtube dialog
     */
    void updateUseShader();

    /**
     * minimum number of rtesidues per sheet/helix is shared with the sec struc color dialog, so
     * need to know about it
     */
    void setColorDlgSecStruct(dlg_colmapsecstruct* pMyColmapDlgSecStruct);

    /**
     * tube diameter and shader use is shared with the newtube dialog, so need to know about it
     */
    void setNewTubePropDlg(dlg_newtube * pMyNewTube_Options);    

private slots:

    void setMinResPerSheet();
    void setMinResPerHelix();
    void toggleCartoonShader();
    void togglePerPixelLighting();
    void toggleCartoonHelixCylinder();
    void toggleCartoonPeptidePlanes();
    void toggleCartoonHelixArrowheads();
    void setCartoonSheetWidth();
    void setCartoonSheetHeight();
    void setCartoonHelixWidth();
    void setCartoonHelixHeight();
    void setCartoonTubeDiameter(double fValue = 0.0);
    void setCartoonSegments();
    void setCartoonSheetHelixProfileCorners(int = 0);
    void setNewTubeProfileCorners();

};

#endif // SCULPTOR_DLG_CARTOON_H
