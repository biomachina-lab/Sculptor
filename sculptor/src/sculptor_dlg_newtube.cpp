/***************************************************************************
                          sculptor_dlg_newtube.cpp
                          -----------------
    begin                : 10.11.2010
    author               : Manuel Wahle
                         : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_newtube.h>
#include <sculptor_dlg_cartoon.h>
// svt includes
#include <svt_pdb.h>

/**
 *
 */
dlg_newtube::dlg_newtube(QWidget* pParent, svt_pdb * pPdbNode)
    : QDialog(pParent),
      m_pDlgCartoon( NULL ),
      m_pPdbNode(pPdbNode)
{
    setupUi(this);

    updateTubeDiameter();
    updateTubeSegments();
    updateTubeProfileCorners();
    updateUseShader();

    // signals are emitted when widgets are changed.  the default setting is off.  if we have the case where the checkbox remains false (unchecked) after the update, the signal attached to a false setting never gets emitted.  This if statement takes the place of that signal.
    if (!checkBoxShader->isChecked())
        checkBoxPerPixelLighting->setDisabled(true);

    connect( m_pTubeDiameter,            SIGNAL(valueChanged(double)),    this, SLOT(sSetNewTubeDiameter()) );
    connect( m_pTubeDiameter,            SIGNAL(editingFinished()),       this, SLOT(sSetNewTubeDiameter()) );

    connect( spinBoxSegments,            SIGNAL(valueChanged(int)),       this, SLOT(sSetNewTubeSegments()) );
    connect( spinBoxSegments,            SIGNAL(editingFinished()),       this, SLOT(sSetNewTubeSegments()) );

    connect( spinBoxTubeCorners,         SIGNAL(valueChanged(int)),       this, SLOT(sSetNewTubeProfileCorners()) );
    connect( spinBoxTubeCorners,         SIGNAL(editingFinished()),       this, SLOT(sSetNewTubeProfileCorners()) );

    connect( checkBoxPerPixelLighting,   SIGNAL(toggled(bool)),           this, SLOT(sTogglePerPixelLighting()) );
    connect( checkBoxShader,             SIGNAL(toggled(bool)),           this, SLOT(sToggleNewTubeUseShader()) );

    connect( m_pButtonClose,             SIGNAL(clicked()),               this, SLOT(hide()) );
}

/**
 *
 */
dlg_newtube::~dlg_newtube()
{
}

/**
 *
 */
void dlg_newtube::sSetNewTubeDiameter()
{
    m_pPdbNode->setNewTubeDiameter( (float)m_pTubeDiameter->value() );
}

/**
 *
 */
void dlg_newtube::sSetNewTubeProfileCorners()
{
    m_pPdbNode->setNewTubeProfileCorners(spinBoxTubeCorners->value());

    if (m_pDlgCartoon != NULL)
        m_pDlgCartoon->updateTubeProfileCorners();
}

/**
 *
 */
void dlg_newtube::sSetNewTubeSegments()
{
    m_pPdbNode->setNewTubeSegs(spinBoxSegments->value());
}

/**
 *
 */
void dlg_newtube::sToggleNewTubeUseShader()
{
    m_pPdbNode->setCartoonParam(CARTOON_USE_SHADER, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_USE_SHADER)));
    updateUseShader();

    if (m_pDlgCartoon != NULL)
        m_pDlgCartoon->updateUseShader();
}

/**
 *
 */
void dlg_newtube::sTogglePerPixelLighting()
{
    m_pPdbNode->setCartoonParam(CARTOON_PER_PIXEL_LIGHTING, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING)));
    updateUseShader();

    if (m_pDlgCartoon != NULL)
        m_pDlgCartoon->updateUseShader();
}

/**
 *
 */
void dlg_newtube::updateTubeProfileCorners()
{
    spinBoxTubeCorners->blockSignals(true);
    spinBoxTubeCorners->setValue(m_pPdbNode->getNewTubeProfileCorners());
    spinBoxTubeCorners->blockSignals(false);
}

/**
 *
 */
void dlg_newtube::updateTubeSegments()
{
    spinBoxSegments->blockSignals(true);
    spinBoxSegments->setValue( m_pPdbNode->getNewTubeSegs() );
    spinBoxSegments->blockSignals(false);
}

/**
 *
 */
void dlg_newtube::updateTubeDiameter()
{
    m_pTubeDiameter->blockSignals(true);
    m_pTubeDiameter->setValue( (double)m_pPdbNode->getNewTubeDiameter() );
    m_pTubeDiameter->blockSignals(false);
}

/**
 *
 */
void dlg_newtube::updateUseShader()
{
    checkBoxShader->blockSignals(TRUE);
    checkBoxPerPixelLighting->blockSignals(TRUE);

    if ( !m_pPdbNode->getShaderSupported() || !svt_getAllowShaderPrograms() )
    {
        // shader not support/allowed - uncheck and disable all relevant checkboxes
        checkBoxShader->setEnabled(FALSE);
        checkBoxShader->setChecked(FALSE);
        checkBoxPerPixelLighting->setEnabled(FALSE);
        checkBoxPerPixelLighting->setChecked(FALSE);
    }
    else
    {
        // shader supported/allowed
        checkBoxShader->setEnabled(TRUE);

        if (m_pPdbNode->getCartoonParam(CARTOON_USE_SHADER) == 1.0f)
        {
            // shader supported and activated
            checkBoxShader->setChecked(TRUE);
            checkBoxPerPixelLighting->setEnabled(TRUE);

            if (m_pPdbNode->getCartoonParam( CARTOON_PER_PIXEL_LIGHTING ) == 1.0f)
                checkBoxPerPixelLighting->setChecked(TRUE);
            else
                checkBoxPerPixelLighting->setChecked(FALSE);

        }
        else
        {
            // shader supported but not activated
            checkBoxShader->setChecked(FALSE);
            checkBoxPerPixelLighting->setEnabled(FALSE);
            checkBoxPerPixelLighting->setChecked(FALSE);
        }
    }

    checkBoxShader->blockSignals(FALSE);
    checkBoxPerPixelLighting->blockSignals(FALSE);
}

/**
 *
 */
void dlg_newtube::setCartoonPropDlg(dlg_cartoon * pDlgCartoon)
{
    m_pDlgCartoon = pDlgCartoon;
}
