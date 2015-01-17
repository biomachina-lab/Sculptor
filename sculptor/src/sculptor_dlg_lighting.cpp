/***************************************************************************
                          sculptor_dlg_lighting.cpp
			  -------------------
    begin                : June 10, 2009
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_lighting.h>
// qt4 includes
#include <QGroupBox>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QComboBox>
// svt includes
#include <svt_init.h>
#include <svt_color.h>
#include <svt_opengl.h>
#include <svt_scene.h>
#include <svt_shader_cg_ao.h>

#define FOG_DENSITY_SCALING 3000.0f
#define FOG_DISTANCE_SCALING 30.0f
#define AO_RADIUS_SCALING 350.0f
#define AO_INTENSITY_SCALING 500.0f


/*
 * Constructor
 */
dlg_lighting::dlg_lighting(QWidget* pParent, svt_scene * pScene)
    : QDialog( pParent ),
      m_pScene( pScene )
{
    setupUi(this);

    comboBoxFogMode->insertItem(0, "Disabled");
    comboBoxFogMode->insertItem(1, "Linear");
    comboBoxFogMode->insertItem(2, "Exponential");
    comboBoxFogMode->insertItem(3, "Strong Exponential");

    m_fNear = svt_getNear();
    getFogMode();
    getFogStart();
    getFogEnd();
    getFogDensity();
    getFogColor();

    comboBoxAOMethod->insertItem(0, "Disabled");
    comboBoxAOMethod->insertItem(1, "Point Based");
    comboBoxAOMethod->insertItem(2, "Line Based");

    getAOMethod();
    getAORadiusScaling();
    getAOIntensity();

    getBGColor();
    
    // ambient occlusion supported?
    setAOGroupBoxEnabled( svt_getAllowAmbientOcclusion() );
    
    createConnections();
}

/*
 * Destructor
 */
dlg_lighting::~dlg_lighting()
{
}

void dlg_lighting::createConnections()
{
    connect( pushButtonFogColor,     SIGNAL( clicked() ),         this, SLOT( setFogColor() ) );
    connect( sliderFogStart,         SIGNAL( valueChanged(int) ), this, SLOT( setFogStart() ) );
    connect( sliderFogEnd,           SIGNAL( valueChanged(int) ), this, SLOT( setFogEnd() ) );
    connect( sliderFogDensity,       SIGNAL( valueChanged(int) ), this, SLOT( setFogDensity() ) );
    connect( comboBoxFogMode,        SIGNAL( activated(int) ),    this, SLOT( setFogMode() ) );
    connect( comboBoxAOMethod,       SIGNAL( activated(int) ),    this, SLOT( setAOMethod() ) );
    connect( sliderAORadius,         SIGNAL( valueChanged(int) ), this, SLOT( setAORadiusScaling() ) );
    connect( sliderAOIntensity,      SIGNAL( valueChanged(int) ), this, SLOT( setAOIntensity() ) );
    connect( pushButtonBGColor,      SIGNAL( clicked() ),         this, SLOT( setBGColor() ) );
    connect( pushButtonBG2Fog ,      SIGNAL( clicked() ),         this, SLOT( setBG2Fog() ) );
    connect( pushButtonFog2BG,       SIGNAL( clicked() ),         this, SLOT( setFog2BG() ) );
}

void dlg_lighting::setFogMode()
{
    switch (comboBoxFogMode->currentIndex())
    {
	case 0:
	{
	    m_pScene->setFogEnabled(false);
	    textLabelFog2->setDisabled(true);
	    textLabelFog3->setDisabled(true);
	    sliderFogStart->setDisabled(true);
	    sliderFogEnd->setDisabled(true);
	    pushButtonFogColor->setDisabled(true);
	    textLabelFog5->setDisabled(true);
	    sliderFogDensity->setDisabled(true);
	    textLabelFog4->setDisabled(true);
	    comboBoxFogMode->setCurrentIndex(GL_NONE);
	    break;
	}
	case 1:
	{
	    m_pScene->setFogEnabled(true);
	    textLabelFog2->setEnabled(true);
	    textLabelFog3->setEnabled(true);
	    sliderFogStart->setEnabled(true);
	    sliderFogEnd->setEnabled(true);
	    pushButtonFogColor->setEnabled(true);
	    textLabelFog5->setEnabled(true);
	    sliderFogDensity->setDisabled(true);
	    textLabelFog4->setDisabled(true);
	    f = (GLfloat)GL_LINEAR;
	    m_pScene->setFogParam(GL_FOG_MODE, &f);
	    break;
	}
	case 2:
	case 3:
	{
	    if (comboBoxFogMode->currentIndex() == 2)
		f = (GLfloat)GL_EXP;
	    else //if (comboBoxFogMode->currentIndex() == 3)
		f = (GLfloat)GL_EXP2;
	    m_pScene->setFogEnabled(true);
	    textLabelFog2->setDisabled(true);
	    textLabelFog3->setDisabled(true);
	    sliderFogStart->setDisabled(true);
	    sliderFogEnd->setDisabled(true);
	    pushButtonFogColor->setEnabled(true);
	    textLabelFog5->setEnabled(true);
	    sliderFogDensity->setEnabled(true);
	    textLabelFog4->setEnabled(true);
	    m_pScene->setFogParam(GL_FOG_MODE, &f);
	    break;
	}
	default:
	    break;
    }
    svt_redraw();
}

void dlg_lighting::getFogMode()
{
    if (!m_pScene->getFogEnabled())
    {
	textLabelFog2->setDisabled(true);
	textLabelFog3->setDisabled(true);
	sliderFogStart->setDisabled(true);
	sliderFogEnd->setDisabled(true);
	pushButtonFogColor->setDisabled(true);
	textLabelFog5->setDisabled(true);
	sliderFogDensity->setDisabled(true);
	textLabelFog4->setDisabled(true);
	comboBoxFogMode->blockSignals(true);
	comboBoxFogMode->setCurrentIndex(0);
	comboBoxFogMode->blockSignals(false);
    }
    else
    {
	m_pScene->getFogParam(GL_FOG_MODE, &f);

	switch ((int)f)
	{
	    case GL_LINEAR:
	    {
		textLabelFog2->setEnabled(true);
		textLabelFog3->setEnabled(true);
		sliderFogStart->setEnabled(true);
		sliderFogEnd->setEnabled(true);
		pushButtonFogColor->setEnabled(true);
		textLabelFog5->setEnabled(true);
		sliderFogDensity->setDisabled(true);
		textLabelFog4->setDisabled(true);
		comboBoxFogMode->blockSignals(true);
		comboBoxFogMode->setCurrentIndex(1);
		comboBoxFogMode->blockSignals(false);
		break;
	    }
	    case GL_EXP:
	    case GL_EXP2:
	    {
		textLabelFog2->setDisabled(true);
		textLabelFog3->setDisabled(true);
		sliderFogStart->setDisabled(true);
		sliderFogEnd->setDisabled(true);
		pushButtonFogColor->setEnabled(true);
		textLabelFog5->setEnabled(true);
		sliderFogDensity->setEnabled(true);
		textLabelFog4->setEnabled(true);
		comboBoxFogMode->blockSignals(true);
		if (f == GL_EXP)
		    f = 2;
		else // if (f == GL_EXP2)
		    f = 3;
		comboBoxFogMode->setCurrentIndex(f);
		comboBoxFogMode->blockSignals(false);
		break;
	    }
	    default:
		break;
	}
    }
}

void dlg_lighting::setFogStart()
{
    f = (GLfloat)(sliderFogStart->value()) / FOG_DISTANCE_SCALING;
    f += m_fNear;
    m_pScene->setFogParam(GL_FOG_START, &f);

    svt_redraw();
}

void dlg_lighting::getFogStart()
{
    m_pScene->getFogParam(GL_FOG_START, &f);
    f -= m_fNear;
    f *= FOG_DISTANCE_SCALING;
    sliderFogStart->blockSignals(true);
    sliderFogStart->setValue((int)f);
    sliderFogStart->blockSignals(false);
}

void dlg_lighting::setFogEnd()
{
    f = (GLfloat)(sliderFogEnd->value()) / FOG_DISTANCE_SCALING;
    f += m_fNear;
    m_pScene->setFogParam(GL_FOG_END, &f);

    svt_redraw();
}

void dlg_lighting::getFogEnd()
{
    m_pScene->getFogParam(GL_FOG_END, &f);
    f -= m_fNear;
    f *= FOG_DISTANCE_SCALING;
    sliderFogEnd->blockSignals(true);
    sliderFogEnd->setValue((int)f);
    sliderFogEnd->blockSignals(false);
}

void dlg_lighting::setFogColor()
{
    GLfloat aColor[4];
    QPalette oPalette;

    m_pScene->getFogParam(GL_FOG_COLOR, aColor);

    float fRed   = (255.0f * (float)aColor[0]);
    float fGreen = (255.0f * (float)aColor[1]);
    float fBlue  = (255.0f * (float)aColor[2]);

    QColor oColor;

    oColor.setRgb( (int)fRed, (int)fGreen, (int)fBlue );

    QColor oCol = QColorDialog::getColor(oColor, 0);
    if (oCol.isValid())
    {
        aColor[0] = (GLfloat)(oCol.red()   / 255.0f);
	aColor[1] = (GLfloat)(oCol.green() / 255.0f);
	aColor[2] = (GLfloat)(oCol.blue()  / 255.0f);
	aColor[3] = (GLfloat)1.0f;

	m_pScene->setFogParam(GL_FOG_COLOR, aColor);

	//pushButtonFogColor->setAutoFillBackground( TRUE );
	//oPalette.setColor(pushButtonFogColor->backgroundRole(), oColor);
	//pushButtonFogColor->setPalette(oPalette);

	// some style engines override the palette. so force color via stylesheet
	QString oStr;
	oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", oCol.red(), oCol.green(), oCol.blue() );
	pushButtonFogColor->setStyleSheet(oStr);

	svt_redraw();
    }
}

void dlg_lighting::setFogDensity()
{
    f = (GLfloat)(sliderFogDensity->value())/FOG_DENSITY_SCALING;
    m_pScene->setFogParam(GL_FOG_DENSITY, &f);

    svt_redraw();
}

void dlg_lighting::getFogDensity()
{
    m_pScene->getFogParam(GL_FOG_DENSITY, &f);
    f *= FOG_DENSITY_SCALING;
    sliderFogDensity->blockSignals(true);
    sliderFogDensity->setValue((int)f);
    sliderFogDensity->blockSignals(false);
}

void dlg_lighting::getFogColor()
{
    GLfloat aColor[4];
    m_pScene->getFogParam(GL_FOG_COLOR, aColor);

    float fRed   = (255.0f * (float)aColor[0]);
    float fGreen = (255.0f * (float)aColor[1]);
    float fBlue  = (255.0f * (float)aColor[2]);

    QColor oColor;
    oColor.setRgb( (int)fRed, (int)fGreen, (int)fBlue );

    //pushButtonFogColor->setAutoFillBackground( TRUE );
    //oPalette.setColor(pushButtonFogColor->backgroundRole(), oColor);
    //pushButtonFogColor->setPalette(oPalette);

    // some style engines override the palette. so force color via stylesheet
    QString oStr;
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", oColor.red(), oColor.green(), oColor.blue() );
    pushButtonFogColor->setStyleSheet(oStr);
}

/*
 *
 */
void dlg_lighting::setAOGroupBoxEnabled(bool bEnabled)
{
    comboBoxAOMethod->blockSignals(TRUE);

    if (bEnabled)
    {
        getAOMethod();
    }
    else
    {
        m_iSelectedAOMethod = comboBoxAOMethod->currentIndex();
        comboBoxAOMethod->setCurrentIndex(0);
    }

    groupBoxAO->setEnabled(bEnabled);

    comboBoxAOMethod->blockSignals(FALSE);
}

void dlg_lighting::setAOMethod()
{
    switch (comboBoxAOMethod->currentIndex())
    {
	case 0:
	{
	    sliderAORadius->setDisabled(true);
	    sliderAOIntensity->setDisabled(true);
	    textLabelAO2->setDisabled(true);
	    textLabelAO3->setDisabled(true);
	    m_pScene->setAOEnabled(false);
	    break;
	}
	case 1:
	{
	    textLabelAO2->setEnabled(true);
	    textLabelAO3->setEnabled(true);
	    sliderAORadius->setEnabled(true);
	    sliderAOIntensity->setEnabled(true);
	    m_pScene->setAOMethod(SVT_AO_POINT_BASED);
	    m_pScene->setAOEnabled(true);
	    break;
	}
	case 2:
	{
	    textLabelAO2->setEnabled(true);
	    textLabelAO3->setEnabled(true);
	    sliderAORadius->setEnabled(true);
	    sliderAOIntensity->setEnabled(true);
	    m_pScene->setAOMethod(SVT_AO_LINE_BASED);
	    m_pScene->setAOEnabled(true);
	    break;
	}
    }
    svt_redraw();
}

void dlg_lighting::getAOMethod()
{
    int i;

    comboBoxAOMethod->blockSignals(true);

    if (m_pScene->getAOEnabled() == false)
    {
	comboBoxAOMethod->setCurrentIndex(0);
        m_iSelectedAOMethod = 0;
	sliderAORadius->setDisabled(true);
	sliderAOIntensity->setDisabled(true);
	textLabelAO2->setDisabled(true);
	textLabelAO3->setDisabled(true);
    }
    else
    {
	i = m_pScene->getAOMethod();

	switch (i)
	{
	    case SVT_AO_POINT_BASED:
	    {
		comboBoxAOMethod->setCurrentIndex(1);
                m_iSelectedAOMethod = 1;
		break;
	    }
	    case SVT_AO_LINE_BASED:
	    {
		comboBoxAOMethod->setCurrentIndex(2);
                m_iSelectedAOMethod = 2;
		break;
	    }
	    default: {}
	}

	textLabelAO2->setEnabled(true);
	textLabelAO3->setEnabled(true);
	sliderAORadius->setEnabled(true);
	sliderAOIntensity->setEnabled(true);

    }

    comboBoxAOMethod->blockSignals(false);
}

void dlg_lighting::setAORadiusScaling()
{
    f=((Real32)(sliderAORadius->value())) / AO_RADIUS_SCALING;

    m_pScene->setAOSampleRadiusScaling(f);

    svt_redraw();
}

void dlg_lighting::getAORadiusScaling()
{
    f = (Real32)(m_pScene->getAOSampleRadiusScaling());

    sliderAORadius->blockSignals(true);
    sliderAORadius->setValue((int) (f * AO_RADIUS_SCALING));
    sliderAORadius->blockSignals(false);
}

void dlg_lighting::setAOIntensity()
{
    f=((Real32)(sliderAOIntensity->value())) / AO_INTENSITY_SCALING;

    m_pScene->setAOIntensity(f);

    svt_redraw();
}

void dlg_lighting::getAOIntensity()
{
    f = (Real32)(m_pScene->getAOIntensity());

    sliderAOIntensity->blockSignals(true);
    sliderAOIntensity->setValue((int) (f * AO_INTENSITY_SCALING));
    sliderAOIntensity->blockSignals(false);
}

void dlg_lighting::setBGColor()
{
    svt_color * pColor = svt_getBackgroundColor();

    if (pColor == NULL)
    {
        pColor = new svt_color();
        svt_setBackgroundColor(pColor);
    }
    
    float fRed   = (255.0f * pColor->getR());
    float fGreen = (255.0f * pColor->getG());
    float fBlue  = (255.0f * pColor->getB());

    QColor oColor;
    oColor.setRgb( (int)fRed, (int)fGreen, (int)fBlue );

    QColor oCol = QColorDialog::getColor(oColor, 0);

    if (oCol.isValid())
    {
        pColor->setR(oCol.red()   / 255.0f);
	pColor->setG(oCol.green() / 255.0f);
	pColor->setB(oCol.blue()  / 255.0f);

	//pushButtonBGColor->setAutoFillBackground( TRUE );
	//oPalette.setColor(pushButtonBGColor->backgroundRole(), oColor);
	//pushButtonBGColor->setPalette(oPalette);

	// some style engines override the palette. so force color via stylesheet
	QString oStr;
	oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", oCol.red(), oCol.green(), oCol.blue() );
	pushButtonBGColor->setStyleSheet(oStr);

	svt_redraw();
    }
}

void dlg_lighting::getBGColor()
{
    svt_color * pColor = svt_getBackgroundColor();

    float fRed   = (255.0f * pColor->getR());
    float fGreen = (255.0f * pColor->getG());
    float fBlue  = (255.0f * pColor->getB());

    QColor oColor;
    oColor.setRgb( (int)fRed, (int)fGreen, (int)fBlue );

    //pushButtonBGColor->setAutoFillBackground( TRUE );
    //oPalette.setColor(pushButtonBGColor->backgroundRole(), oColor);
    //pushButtonBGColor->setPalette(oPalette);

    // some style engines override the palette. so force color via stylesheet
    QString oStr;
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", oColor.red(), oColor.green(), oColor.blue() );
    pushButtonBGColor->setStyleSheet(oStr);
}

void dlg_lighting::updateBGColor()
{
    getBGColor();
}

void dlg_lighting::setBG2Fog()
{
    GLfloat aColor[4];
    svt_color * pColor = svt_getBackgroundColor();

    aColor[0] = (GLfloat)(pColor->getR());
    aColor[1] = (GLfloat)(pColor->getG());
    aColor[2] = (GLfloat)(pColor->getB());
    aColor[3] = (GLfloat)(1.0f);

    m_pScene->setFogParam(GL_FOG_COLOR, aColor);

    getFogColor();

    svt_redraw();
}

void dlg_lighting::setFog2BG()
{
    GLfloat aColor[4];
    svt_color * pColor = svt_getBackgroundColor();

    m_pScene->getFogParam(GL_FOG_COLOR, aColor);

    pColor->setR((Real32)(aColor[0]));
    pColor->setG((Real32)(aColor[1]));
    pColor->setB((Real32)(aColor[2]));

    getBGColor();

    svt_redraw();
}
