/***************************************************************************
                          sculptor_dlg_lighting.h
			  -------------------
    begin                : June 10, 2009
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_LIGHTING_H
#define SCULPTOR_DLG_LIGHTING_H

// uic-qt4 includes
#include <ui_dlg_lighting.h>
// svt includes
#include <svt_opengl.h>
class svt_scene;


class dlg_lighting : public QDialog, private Ui::dlg_lighting
{
    Q_OBJECT

    svt_scene * m_pScene;

    GLfloat f;
    GLfloat m_fNear;

    void getBGColor();
    void getFogMode();
    void getFogStart();
    void getFogEnd();
    void getFogDensity();
    void getFogColor();
    void getAOMethod();
    void getAORadiusScaling();
    void getAOIntensity();

    void createConnections();
    void computeFogDistanceScaling();

    // when disabling the AO groupbox, remember the selected method
    int m_iSelectedAOMethod;

public:

    /*
     * Constructor
     */
    dlg_lighting(QWidget* pParent, svt_scene* pScene);

    /*
     * Destructor
     */
    virtual ~dlg_lighting();
    
    void updateBGColor();

    void setAOGroupBoxEnabled(bool bEnabled);

private slots:

    void setBGColor();
    void setBG2Fog();
    void setFog2BG();
    void setFogMode();
    void setFogStart();
    void setFogEnd();
    void setFogDensity();
    void setFogColor();
    void setAOMethod();
    void setAORadiusScaling();
    void setAOIntensity();
};


#endif // SCULPTOR_DLG_LIGHTING_H
