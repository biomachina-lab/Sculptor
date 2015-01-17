/***************************************************************************
                          sculptor_lighting_dlg.cpp
			  -------------------
    begin                : June 10, 2009
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// Sculptor includes
#include <sculptor_lighting_dlg.h>
// QT includes
#include <q3groupbox.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcolordialog.h>
#include <qcombobox.h>
// SVT includes
#include <svt_init.h>
#include <svt_color.h>
#include <svt_opengl.h>


#define FOG_DENSITY_SCALING 3000.0f
#define FOG_DISTANCE_SCALING 30.0f
#define AO_RADIUS_SCALING 350.0f
#define AO_INTENSITY_SCALING 500.0f



/*
 * Constructor
 */
sculptor_lighting_dlg::sculptor_lighting_dlg(svt_scene * pScene) :
    m_pScene( pScene )
{
    comboBoxFogMode->insertItem("Disabled", 0);
    comboBoxFogMode->insertItem("Linear", 1);
    comboBoxFogMode->insertItem("Exponential", 2);
    comboBoxFogMode->insertItem("Strong Exponential", 3);

    m_fNear = svt_getNear();
    getFogMode();
    getFogStart();
    getFogEnd();
    getFogDensity();
    getFogColor();

    comboBoxAOMethod->insertItem("Disabled", 0);
    comboBoxAOMethod->insertItem("Point Based", 1);
    comboBoxAOMethod->insertItem("Line Based", 2);

    getAOMethod();
    getAORadiusScaling();
    getAOIntensity();

    getBGColor();
    QColor oCol(127,127,127);
    pushButtonBGColor->setPaletteForegroundColor(oCol);
    pushButtonFogColor->setPaletteForegroundColor(oCol);


    // ambient occlusion not possible?
    if (!m_pScene->getAOSupported())
	groupBoxAO->setDisabled(true);
    
    createConnections();
}

/*
 * Destructor
 */
sculptor_lighting_dlg::~sculptor_lighting_dlg()
{
}

void sculptor_lighting_dlg::createConnections()
{
    connect( m_pOK_Button,           SIGNAL( clicked() ),         this, SLOT( hide() ) );
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

void sculptor_lighting_dlg::hide()
{
    QWidget::hide();
}

void sculptor_lighting_dlg::setFogMode()
{
    switch (comboBoxFogMode->currentItem())
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
	    comboBoxFogMode->setCurrentItem(GL_NONE);
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
	    if (comboBoxFogMode->currentItem() == 2)
		f = (GLfloat)GL_EXP;
	    else //if (comboBoxFogMode->currentItem() == 3)
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

void sculptor_lighting_dlg::getFogMode()
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
	comboBoxFogMode->setCurrentItem(0);
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
		comboBoxFogMode->setCurrentItem(1);
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
		comboBoxFogMode->setCurrentItem(f);
		comboBoxFogMode->blockSignals(false);
		break;
	    }
	    default:
		break;
	}
    }
}

void sculptor_lighting_dlg::setFogStart()
{
    f = (GLfloat)(sliderFogStart->value()) / FOG_DISTANCE_SCALING;
    f += m_fNear;
    m_pScene->setFogParam(GL_FOG_START, &f);

    svt_redraw();
}

void sculptor_lighting_dlg::getFogStart()
{
    m_pScene->getFogParam(GL_FOG_START, &f);
    f -= m_fNear;
    f *= FOG_DISTANCE_SCALING;
    sliderFogStart->blockSignals(true);
    sliderFogStart->setValue((int)f);
    sliderFogStart->blockSignals(false);
}

void sculptor_lighting_dlg::setFogEnd()
{
    f = (GLfloat)(sliderFogEnd->value()) / FOG_DISTANCE_SCALING;
    f += m_fNear;
    m_pScene->setFogParam(GL_FOG_END, &f);

    svt_redraw();
}

void sculptor_lighting_dlg::getFogEnd()
{
    m_pScene->getFogParam(GL_FOG_END, &f);
    f -= m_fNear;
    f *= FOG_DISTANCE_SCALING;
    sliderFogEnd->blockSignals(true);
    sliderFogEnd->setValue((int)f);
    sliderFogEnd->blockSignals(false);
}

void sculptor_lighting_dlg::setFogColor()
{
    GLfloat aColor[4];
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
	pushButtonFogColor->setPaletteBackgroundColor(oCol);

	svt_redraw();
    }
}

void sculptor_lighting_dlg::setFogDensity()
{
    f = (GLfloat)(sliderFogDensity->value())/FOG_DENSITY_SCALING;
    m_pScene->setFogParam(GL_FOG_DENSITY, &f);

    svt_redraw();
}

void sculptor_lighting_dlg::getFogDensity()
{
    m_pScene->getFogParam(GL_FOG_DENSITY, &f);
    f *= FOG_DENSITY_SCALING;
    sliderFogDensity->blockSignals(true);
    sliderFogDensity->setValue((int)f);
    sliderFogDensity->blockSignals(false);
}

void sculptor_lighting_dlg::getFogColor()
{
    GLfloat aColor[4];
    m_pScene->getFogParam(GL_FOG_COLOR, aColor);

    float fRed   = (255.0f * (float)aColor[0]);
    float fGreen = (255.0f * (float)aColor[1]);
    float fBlue  = (255.0f * (float)aColor[2]);

    QColor oColor;
    oColor.setRgb( (int)fRed, (int)fGreen, (int)fBlue );

    pushButtonFogColor->setPaletteBackgroundColor(oColor);

}

void sculptor_lighting_dlg::setAOMethod()
{
    switch (comboBoxAOMethod->currentItem())
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

void sculptor_lighting_dlg::getAOMethod()
{
    int i;

    comboBoxAOMethod->blockSignals(true);

    if (m_pScene->getAOEnabled() == false)
    {
	comboBoxAOMethod->setCurrentItem(0);
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
		comboBoxAOMethod->setCurrentItem(1);
		break;
	    }
	    case SVT_AO_LINE_BASED:
	    {
		comboBoxAOMethod->setCurrentItem(2);
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

void sculptor_lighting_dlg::setAORadiusScaling()
{
    f=((Real32)(sliderAORadius->value())) / AO_RADIUS_SCALING;

    m_pScene->setAOSampleRadiusScaling(f);

    svt_redraw();
}

void sculptor_lighting_dlg::getAORadiusScaling()
{
    f = (Real32)(m_pScene->getAOSampleRadiusScaling());

    sliderAORadius->blockSignals(true);
    sliderAORadius->setValue((int) (f * AO_RADIUS_SCALING));
    sliderAORadius->blockSignals(false);
}

void sculptor_lighting_dlg::setAOIntensity()
{
    f=((Real32)(sliderAOIntensity->value())) / AO_INTENSITY_SCALING;

    m_pScene->setAOIntensity(f);

    svt_redraw();
}

void sculptor_lighting_dlg::getAOIntensity()
{
    f = (Real32)(m_pScene->getAOIntensity());

    sliderAOIntensity->blockSignals(true);
    sliderAOIntensity->setValue((int) (f * AO_INTENSITY_SCALING));
    sliderAOIntensity->blockSignals(false);
}

void sculptor_lighting_dlg::setBGColor()
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

	//svt_setBackgroundColor(pColor);

	pushButtonBGColor->setPaletteBackgroundColor(oCol);

	svt_redraw();
    }
}

void sculptor_lighting_dlg::getBGColor()
{
    svt_color * pColor = svt_getBackgroundColor();

    float fRed   = (255.0f * pColor->getR());
    float fGreen = (255.0f * pColor->getG());
    float fBlue  = (255.0f * pColor->getB());

    QColor oColor;
    oColor.setRgb( (int)fRed, (int)fGreen, (int)fBlue );

    pushButtonBGColor->setPaletteBackgroundColor(oColor);
}

void sculptor_lighting_dlg::updateBGColor()
{
    getBGColor();
}

void sculptor_lighting_dlg::setBG2Fog()
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

void sculptor_lighting_dlg::setFog2BG()
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
