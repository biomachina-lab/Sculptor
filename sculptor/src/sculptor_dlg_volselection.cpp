/***************************************************************************
                          sculptor_dlg_volselection.cpp
			  -----------------------------
    begin                : 11-03-2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////
#include <sculptor_dlg_volselection.h>
#include <QDesktopWidget>


/**
 * Constructor
 */
dlg_volselection::dlg_volselection(QWidget* pParent, situs_document* pDoc, svt_volume<Real64> oVolume)
    : QDialog( pParent ),
      m_oVolume ( oVolume ),
      m_pDoc ( pDoc )
{
    setupUi(this);
         
    //disable keyBoardTracking on the Max spinboxes
    m_pMaxX->setKeyboardTracking ( 0 );
    m_pMaxY->setKeyboardTracking ( 0 );
    m_pMaxZ->setKeyboardTracking ( 0 );
    
    //spinboxes
    connect (m_pMinX, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMaxX, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
       
    connect (m_pMinY, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMaxY, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    
    connect (m_pMinZ, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    connect (m_pMaxZ, SIGNAL(valueChanged(int)), this, SLOT(setSelection()));
    
    //buttons
    connect( m_pCrop, SIGNAL(clicked()), this, SLOT(crop()) );   
    connect( m_pCancel, SIGNAL(clicked()), this, SLOT(onCancel()) );

    
    //hide the dialog X button
    //FIXME need to test in all QT4.X
    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint);
    
    //the above window hint is pushing the dialog at the top left corener of the window and hence
    //we need to put it back to the center
    
    //put the dialog in screen's center
    //find the center of the desktop
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    
}

/**
 * Destructor
 */
dlg_volselection::~dlg_volselection()
{
}

/**
 * 
 */
void dlg_volselection::setValues()
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
void dlg_volselection::setSelection()
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
void dlg_volselection::crop()
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

void dlg_volselection::onCancel()
{
    //remove the green cuboid
    m_pDoc->turnOffVolSelectCube();
    //hide
    hide();
}

/**
 * 
 */

void dlg_volselection::keyPressEvent ( QKeyEvent * e )
{
    //For handling Esc button
    if (e->key() == Qt::Key_Escape) 
      onCancel(); 
    else
      e->ignore();
}
