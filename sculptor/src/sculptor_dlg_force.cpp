/***************************************************************************
                          dlg_force.cpp
                          -----------------
    begin                : 11.08.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_window.h>
#include <sculptor_dlg_force.h>
#include <sculptor_scene.h>
#include <sculptor_force_algo.h>

extern sculptor_window* g_pWindow;


/**
 * Constructor
 */
dlg_force::dlg_force(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);

    m_pCheckNegButton->setChecked(g_pWindow->getScene()->getForceTool()->getButtonLogic());
    //m_pCheckCenterMode->setChecked(g_pWindow->getScene()->getForceCenterMode());

    m_pCheckForceArrow->setChecked(g_pWindow->getForceAlgo()->getForceArrowVisible());

    int iVal = (int)(g_pWindow->getScene()->getForceTranslationScale() * 50.0f);
    m_pSliderScaleTranslation->setValue(iVal);

    iVal = (int)(g_pWindow->getForceAlgo()->getCorrForceScale() * 50.0f);
    m_pSliderScaleCorrForce->setValue(iVal);

    iVal = (int)(g_pWindow->getForceAlgo()->getStericForceScale() * 50.0f);
    m_pSliderScaleStericForce->setValue(iVal);

    iVal = (int)(g_pWindow->getForceAlgo()->getCentralSphereScale() * 200.0f);
    m_pSliderScaleCentralSphere->setValue(iVal);

    iVal = (int)(g_pWindow->getForceAlgo()->getForceArrowScale() * 25.0f);
    m_pSliderScaleForceArrow->setValue(iVal);

    float fVal = g_pWindow->getForceAlgo()->getStericClashThreshold();
    QString oStericClashString;
    oStericClashString.sprintf("%6.2f", (double)(fVal));
    m_pEditStericClashThreshold->setText(oStericClashString);

    fVal = g_pWindow->getForceAlgo()->getStericGoodThreshold();
    QString oStericGoodString;
    oStericGoodString.sprintf("%6.2f", (double)(fVal));
    m_pEditStericGoodThreshold->setText(oStericGoodString);

    iVal = (int)(g_pWindow->getForceAlgo()->getSphereColorFunctionRed() * 1000.0f);
    m_pSpinSphereColorFunctionRed->setValue(iVal);
    iVal = (int)(g_pWindow->getForceAlgo()->getSphereColorFunctionBlue() * 1000.0f);
    m_pSpinSphereColorFunctionBlue->setValue(iVal);
    iVal = (int)(g_pWindow->getForceAlgo()->getSphereColorFunctionGreen() * 1000.0f);
    m_pSpinSphereColorFunctionGreen->setValue(iVal);

    m_pCheckTransX->setChecked(g_pWindow->getForceAlgo()->getForceTransX());
    m_pCheckTransY->setChecked(g_pWindow->getForceAlgo()->getForceTransY());
    m_pCheckTransZ->setChecked(g_pWindow->getForceAlgo()->getForceTransZ());
    m_pCheckRotX->setChecked(g_pWindow->getForceAlgo()->getForceRotX());
    m_pCheckRotY->setChecked(g_pWindow->getForceAlgo()->getForceRotY());
    m_pCheckRotZ->setChecked(g_pWindow->getForceAlgo()->getForceRotZ());

    connect(m_pCheckNegButton,                      SIGNAL(toggled(bool)),      this, SLOT(setButtonLogic(bool)));
    connect(m_pCheckForceArrow,                     SIGNAL(toggled(bool)),      this, SLOT(setForceArrow(bool)));
    connect(m_pCheckTransX,                         SIGNAL(toggled(bool)),      this, SLOT(setForceTransX(bool)));
    connect(m_pCheckTransY,                         SIGNAL(toggled(bool)),      this, SLOT(setForceTransY(bool)));
    connect(m_pCheckTransZ,                         SIGNAL(toggled(bool)),      this, SLOT(setForceTransZ(bool)));
    connect(m_pCheckRotX,                           SIGNAL(toggled(bool)),      this, SLOT(setForceRotX(bool)));
    connect(m_pCheckRotY,                           SIGNAL(toggled(bool)),      this, SLOT(setForceRotY(bool)));
    connect(m_pCheckRotZ,                           SIGNAL(toggled(bool)),      this, SLOT(setForceRotZ(bool)));
    //connect(m_pCheckCenterMode,                   SIGNAL(toggled(bool)),      this, SLOT(setForceCenterMode(bool)));
    connect(m_pSliderScaleTranslation,              SIGNAL(valueChanged(int)),  this, SLOT(setForceTransScale()));
    connect(m_pSliderScaleCorrForce,                SIGNAL(valueChanged(int)),  this, SLOT(setCorrForceScale()));
    connect(m_pSliderScaleStericForce,              SIGNAL(valueChanged(int)),  this, SLOT(setStericForceScale()));
    connect(m_pSliderScaleCentralSphere,            SIGNAL(valueChanged(int)),  this, SLOT(setCentralSphereScale()));
    connect(m_pSliderScaleForceArrow,               SIGNAL(valueChanged(int)),  this, SLOT(setForceArrowScale()));
    connect(m_pEditStericClashThreshold,            SIGNAL(editingFinished()),  this, SLOT(setStericClashThreshold()));
    connect(m_pEditStericGoodThreshold,             SIGNAL(editingFinished()),  this, SLOT(setStericGoodThreshold()));
    connect(m_pSpinSphereColorFunctionRed,          SIGNAL(valueChanged(int)),  this, SLOT(setSphereColorFunctionRed()));
    connect(m_pSpinSphereColorFunctionRed,          SIGNAL(editingFinished()),  this, SLOT(setSphereColorFunctionRed()));
    connect(m_pSpinSphereColorFunctionBlue,         SIGNAL(valueChanged(int)),  this, SLOT(setSphereColorFunctionBlue()));
    connect(m_pSpinSphereColorFunctionBlue,         SIGNAL(editingFinished()),  this, SLOT(setSphereColorFunctionBlue()));
    connect(m_pSpinSphereColorFunctionGreen,        SIGNAL(valueChanged(int)),  this, SLOT(setSphereColorFunctionGreen()));
    connect(m_pSpinSphereColorFunctionGreen,        SIGNAL(editingFinished()),  this, SLOT(setSphereColorFunctionGreen()));

}

/**
 * Destructor
 */
dlg_force::~dlg_force()
{
}

/**
 * en/disable the translational forces along the x axis
 * \param bSet if true force is enabled
 */
void dlg_force::setForceTransX(bool bSet)
{
    g_pWindow->getForceAlgo()->setForceTransX(bSet);
}
/**
 * en/disable the translational forces along the y axis
 * \param bSet if true force is enabled
 */
void dlg_force::setForceTransY(bool bSet)
{
    g_pWindow->getForceAlgo()->setForceTransY(bSet);
}
/**
 * en/disable the translational forces along the z axis
 * \param bSet if true force is enabled
 */
void dlg_force::setForceTransZ(bool bSet)
{
    g_pWindow->getForceAlgo()->setForceTransZ(bSet);
}

/**
 * en/disable the rotational forces along the x axis
 * \param bSet if true force is enabled
 */
void dlg_force::setForceRotX(bool bSet)
{
    g_pWindow->getForceAlgo()->setForceRotX(bSet);
}
/**
 * en/disable the rotational forces along the y axis
 * \param bSet if true force is enabled
 */
void dlg_force::setForceRotY(bool bSet)
{
    g_pWindow->getForceAlgo()->setForceRotY(bSet);
}
/**
 * en/disable the rotational forces along the z axis
 * \param bSet if true force is enabled
 */
void dlg_force::setForceRotZ(bool bSet)
{
    g_pWindow->getForceAlgo()->setForceRotZ(bSet);
}

/**
 * set translational force scaling
 */
void dlg_force::setForceTransScale()
{
    int iScale = m_pSliderScaleTranslation->value();
    float fVal = (float)(iScale) * 0.02f;
    g_pWindow->getScene()->setForceTranslationScale(fVal);
}

/**
 * set the correlation force scale
 */
void dlg_force::setCorrForceScale()
{
    int iScale = m_pSliderScaleCorrForce->value();
    float fVal = (float)(iScale) * 0.02f;
    g_pWindow->getForceAlgo()->setCorrForceScale(fVal);
}

/**
 * set the steric force scale
 */
void dlg_force::setStericForceScale()
{
    int iScale = m_pSliderScaleStericForce->value();
    float fVal = (float)(iScale) * 0.02f;
    g_pWindow->getForceAlgo()->setStericForceScale(fVal);
}

/**
 * switch the logic of the button of the force feedback device
 * \param bLogic if true the button logic is switched (if you press the button, no forces are generated).
 */
void dlg_force::setButtonLogic(bool bLogic)
{
    g_pWindow->getScene()->getForceTool()->setButtonLogic(bLogic);
}

/**
 * make force arrow (in)visible
 * \param bVisible show force arrow?
 */
void dlg_force::setForceArrow(bool bVisible)
{
    g_pWindow->getForceAlgo()->setForceArrowVisible(bVisible);
}

/**
 * set the force arrow scale
 */
void dlg_force::setForceArrowScale()
{
    int iScale = m_pSliderScaleForceArrow->value();
    float fVal = (float)(iScale) * 0.04f;
    g_pWindow->getForceAlgo()->setForceArrowScale(fVal);
}

/**
 * set the central sphere scale
 * \param iScale scaling of the central sphere (0..50)
 */
void dlg_force::setCentralSphereScale()
{
    int iScale = m_pSliderScaleCentralSphere->value();
    float fVal = (float)(iScale) * 0.005f;
    g_pWindow->getForceAlgo()->setCentralSphereScale(fVal);
}

/**
 * set the threshold for steric clashes
 */
void dlg_force::setStericClashThreshold()
{
   // if (m_pForceDlg == NULL) return;

    float fVal = m_pEditStericClashThreshold->text().toFloat();
    g_pWindow->getForceAlgo()->setStericClashThreshold(fVal);
    g_pWindow->getForceAlgo()->calcStericClashing();
}

/**
 * set the threshold for steric "good" contacts
 */
void dlg_force::setStericGoodThreshold()
{
  //  if (m_pForceDlg == NULL) return;

    float fVal = m_pEditStericGoodThreshold->text().toFloat();
    g_pWindow->getForceAlgo()->setStericGoodThreshold(fVal);
    g_pWindow->getForceAlgo()->calcStericClashing();
}

/**
 * set the sphere color function Red
 * \param iScale (0..1000)
 */
void dlg_force::setSphereColorFunctionRed()
{
    int iScale = m_pSpinSphereColorFunctionRed->value();
    float fVal = (float)(iScale) * 0.001f;
    g_pWindow->getForceAlgo()->setSphereColorFunctionRed(fVal);
}
/**
 * set the sphere color function Blue
 * \param iScale (0..1000)
 */
void dlg_force::setSphereColorFunctionBlue()
{
    int iScale = m_pSpinSphereColorFunctionBlue->value();
    float fVal = (float)(iScale) * 0.001f;
    g_pWindow->getForceAlgo()->setSphereColorFunctionBlue(fVal);
}
/**
 * set the sphere color function Green
 * \param iScale (0..1000)
 */
void dlg_force::setSphereColorFunctionGreen()
{
    int iScale = m_pSpinSphereColorFunctionGreen->value();
    float fVal = (float)(iScale) * 0.001f;
    g_pWindow->getForceAlgo()->setSphereColorFunctionGreen(fVal);
}


