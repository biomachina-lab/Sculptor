/***************************************************************************
                          sculptor_dlg_crop.cpp
                          -----------------------------
    begin                : 11-03-2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_crop.h>


/**
 * Constructor
 */
dlg_crop::dlg_crop(QWidget* pParent, situs_document* pDoc, svt_volume<Real64> oVolume)
    : QDialog( pParent ),
      m_oVolume ( oVolume ),
      m_pDoc ( pDoc )
{
    setupUi(this);

    //spinboxes
    connect (m_pMinX, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMinX, SIGNAL(editingFinished()), this, SLOT(setSelection()));
    connect (m_pMaxX, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMaxX, SIGNAL(editingFinished()), this, SLOT(setSelection()));

    connect (m_pMinY, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMinY, SIGNAL(editingFinished()), this, SLOT(setSelection()));
    connect (m_pMaxY, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMaxY, SIGNAL(editingFinished()), this, SLOT(setSelection()));

    connect (m_pMinZ, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMinZ, SIGNAL(editingFinished()), this, SLOT(setSelection()));
    connect (m_pMaxZ, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMaxZ, SIGNAL(editingFinished()), this, SLOT(setSelection()));

    //buttons
    connect( m_pCrop, SIGNAL(clicked()), this, SLOT(crop()) );
    connect( m_pCancel, SIGNAL(clicked()), this, SLOT(cancel()) );
}

/**
 * Destructor
 */
dlg_crop::~dlg_crop()
{
}

/**
 *
 */
void dlg_crop::setValues()
{
    m_pMinX->setMaximum( m_oVolume.getSizeX()-1 );
    m_pMaxX->setMaximum( m_oVolume.getSizeX()-1 );
    m_pMaxX->setValue( m_oVolume.getSizeX()-1 );

    m_pMinY->setMaximum( m_oVolume.getSizeY()-1 );
    m_pMaxY->setMaximum( m_oVolume.getSizeY()-1 );
    m_pMaxY->setValue( m_oVolume.getSizeY()-1 );

    m_pMinZ->setMaximum( m_oVolume.getSizeZ()-1 );
    m_pMaxZ->setMaximum( m_oVolume.getSizeZ()-1 );
    m_pMaxZ->setValue( m_oVolume.getSizeZ()-1 );

}

/**
 *
 */
void dlg_crop::setSelection()
{
    unsigned int iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ;

    iMinX = m_pMinX->value();
    iMaxX = m_pMaxX->value();
    iMinY = m_pMinY->value();
    iMaxY = m_pMaxY->value();
    iMinZ = m_pMinZ->value();
    iMaxZ = m_pMaxZ->value();

    //Modify Min if Max <= Min
    if (iMaxX <= iMinX)
    {
        iMinX = iMaxX - 1;
        m_pMinX->setValue ( iMinX );
    }

    if (iMaxY <= iMinY)
    {
        iMinY = iMaxY - 1;
        m_pMinY->setValue ( iMinY );
    }

    if (iMaxZ <= iMinZ)
    {
        iMinZ = iMaxZ - 1;
        m_pMinZ->setValue ( iMinZ );
    }

    //Call setSelection at situs_document
    m_pDoc->setSelection( iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ );
}

/**
 *
 */
void dlg_crop::crop()
{
    unsigned int iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ;

    iMinX = m_pMinX->value();
    iMaxX = m_pMaxX->value();
    iMinY = m_pMinY->value();
    iMaxY = m_pMaxY->value();
    iMinZ = m_pMinZ->value();
    iMaxZ = m_pMaxZ->value();

    m_pDoc->crop( iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ );

    //hide dialog after work is done
    accept();
}

/**
 *
 */
void dlg_crop::cancel()
{
    //remove the green cuboid
    m_pDoc->turnOffVolSelectCube();

    //hide
    hide();
}

/**
 *
 */
void dlg_crop::keyPressEvent(QKeyEvent* pEvent )
{
    //For handling Esc button
    if (pEvent->key() == Qt::Key_Escape)
        m_pDoc->turnOffVolSelectCube();
    else if (pEvent->key() == Qt::Key_Return)
    {
        pEvent->accept();
        return;
    }

    QDialog::keyPressEvent(pEvent);
}

/**
 *
 */
void dlg_crop::closeEvent(QCloseEvent* pEvent)
{
    m_pDoc->turnOffVolSelectCube();

    QDialog::closeEvent(pEvent);
}
