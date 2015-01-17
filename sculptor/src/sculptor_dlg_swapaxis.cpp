/***************************************************************************
                          sculptor_dlg_swapaxis.ccp
                          -----------------
    begin                : 11.04.2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <situs_document.h>
#include <sculptor_dlg_swapaxis.h>


/**
 * Constructor
 */
dlg_swapaxis::dlg_swapaxis(QWidget* pParent, situs_document* pDoc)
    : QDialog( pParent )
{
    setupUi(this);

    connect( m_pOKButton, SIGNAL(clicked()), pDoc, SLOT(swapaxis_DOIT()) );

    m_pButtonGroupX->setId(m_pNewX_X, situs_document::SITUS_DOC_SWAPAXIS_OLD_X_X);
    m_pButtonGroupX->setId(m_pNewX_Y, situs_document::SITUS_DOC_SWAPAXIS_OLD_X_Y);
    m_pButtonGroupX->setId(m_pNewX_Z, situs_document::SITUS_DOC_SWAPAXIS_OLD_X_Z);

    m_pButtonGroupY->setId(m_pNewY_X, situs_document::SITUS_DOC_SWAPAXIS_OLD_Y_X);
    m_pButtonGroupY->setId(m_pNewY_Y, situs_document::SITUS_DOC_SWAPAXIS_OLD_Y_Y);
    m_pButtonGroupY->setId(m_pNewY_Z, situs_document::SITUS_DOC_SWAPAXIS_OLD_Y_Z);

    m_pButtonGroupZ->setId(m_pNewZ_X, situs_document::SITUS_DOC_SWAPAXIS_OLD_Z_X);
    m_pButtonGroupZ->setId(m_pNewZ_Y, situs_document::SITUS_DOC_SWAPAXIS_OLD_Z_Y);
    m_pButtonGroupZ->setId(m_pNewZ_Z, situs_document::SITUS_DOC_SWAPAXIS_OLD_Z_Z);

}

/**
 * Destructor
 */
dlg_swapaxis::~dlg_swapaxis()
{
}

/**
 * get which button is pressed for X axis
 */

int dlg_swapaxis::getOldX()
{
	return m_pButtonGroupX->checkedId();
}

/**
 * get which button is pressed for Y axis
 */
int dlg_swapaxis::getOldY()
{
	return m_pButtonGroupY->checkedId();
}

/**
 * get which button is pressed for Z axis
 */

int dlg_swapaxis::getOldZ()
{
	return m_pButtonGroupZ->checkedId();
}

/**
 * check if axis X button is check in dialog window
 */
bool dlg_swapaxis::getAxisXcheck()
{
	return m_pNewX_Flip->isChecked();
}

/**
 * check if axis Y button is check in dialog window
 */
bool dlg_swapaxis::getAxisYcheck()
{
	return m_pNewY_Flip->isChecked();
}

/**
 * check if axis Z button is check in dialog window
 */
bool dlg_swapaxis::getAxisZcheck()
{
	return m_pNewZ_Flip->isChecked();
}

