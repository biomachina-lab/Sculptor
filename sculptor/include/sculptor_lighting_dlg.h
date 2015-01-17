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

// Sculptor includes
#include <ui_dlg_lighting.h>
// SVT includes
class svt_scene;

class dlg_lighting : public QDialog, private Ui::dlg_lighting
{
    Q_OBJECT

public:

    /*
     * Constructor
     */
    dlg_lighting(QWidget, pParent, svt_scene * pScene);

    /*
     * Destructor
     */
    ~dlg_lighting();
    
    virtual void hide();

    void updateBGColor();

private:

    virtual void setBGColor();
    void getBGColor();
    virtual void setBG2Fog();
    virtual void setFog2BG();

    virtual void setFogMode();
    virtual void getFogMode();
    virtual void setFogStart();
    virtual void getFogStart();
    virtual void setFogEnd();
    virtual void getFogEnd();
    virtual void setFogDensity();
    virtual void getFogDensity();
    virtual void setFogColor();
    virtual void getFogColor();

    virtual void setAOMethod();
    virtual void getAOMethod();
    virtual void setAORadiusScaling();
    virtual void getAORadiusScaling();
    virtual void setAOIntensity();
    virtual void getAOIntensity();

    void createConnections();
    void computeFogDistanceScaling();

    svt_scene * m_pScene;

    GLfloat f;
    GLfloat m_fNear;
};


#endif // SCULPTOR_DLG_LIGHTING_H
