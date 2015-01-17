/***************************************************************************
                          sculptor_prop_marker.cpp
                          -----------------
    begin                : 09.10.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_window.h>
#include <sculptor_prop_marker.h>
// qt4 includes
#include <QColorDialog>

extern sculptor_window* g_pWindow;
extern QString StdtoQ(string);

/**
 * Constructor
 */
prop_marker::prop_marker(sculptor_doc_marker* pMarkerDoc, QWidget* pParent)
    : QDialog( pParent ),
      m_pMarkerDoc(pMarkerDoc)
{
    setupUi(this);

    // set the scale
    m_pScale->setValue( m_pMarkerDoc->getNode()->getScaleX() );

    // set the color correctly
    svt_color oDocCol = m_pMarkerDoc->getColor();
    QString oStr;
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", (int) ( 255.0f * oDocCol.getR()), (int) ( 255.0f * oDocCol.getG()), (int) ( 255.0f * oDocCol.getB()) );
    m_pColor->setStyleSheet(oStr);

    m_pLineEditName->setText( StdtoQ(m_pMarkerDoc->getDisplayName()) );
    // set connections
    connect( m_pPosX,            SIGNAL(editingFinished()),      this, SLOT(apply()) );
    connect( m_pPosY,            SIGNAL(editingFinished()),      this, SLOT(apply()) );
    connect( m_pPosZ,            SIGNAL(editingFinished()),      this, SLOT(apply()) );
    connect( m_pSetAsCurrent,    SIGNAL(clicked()),              this, SLOT(setWindowCurrentPos()) );
    connect( m_pApply,           SIGNAL(clicked()),              this, SLOT(apply()) );
    connect( m_pScale,           SIGNAL(valueChanged(double)),   this, SLOT(scale()) );  
    connect( m_pScale,           SIGNAL(editingFinished()),      this, SLOT(scale()) );  
    connect( m_pColor,           SIGNAL(clicked()),              this, SLOT(setColor()) );

    connect( m_pLineEditName,    SIGNAL(textEdited(const QString&)), m_pMarkerDoc, SLOT(setName(const QString&)), Qt::DirectConnection );
    connect( m_pCheckBoxName3D,    SIGNAL(toggled(bool)),              m_pMarkerDoc, SLOT(setShowName3D(bool)) );
};

/**
 * Destructor
 */
prop_marker::~prop_marker()
{
};

/**
 * get the X coordinate of the file
 * \return the X coordinate  
 */
Real64 prop_marker::getX()
{
    return m_pPosX->text().toDouble();
};

/**
 * get the X coordinate of the file
 * \return the X coordinate  
 */
void prop_marker::setX(Real64 fCoord)
{
    char pCoord[128]; 
    sprintf(pCoord,"%8.3f",fCoord);
    
    m_pPosX->setText(QString(pCoord));
};

/**
 * get the Y coordinate of the file
 * \return the Y coordinate  
 */
Real64 prop_marker::getY()
{
    return m_pPosY->text().toDouble();
};

/**
 * get the Y coordinate of the file
 * \return the Y coordinate  
 */
void prop_marker::setY(Real64 fCoord)
{    
    char pCoord[128]; 
    sprintf(pCoord,"%8.3f",fCoord);
    
    m_pPosY->setText(QString(pCoord));
};

/**
 * get the Z coordinate of the file
 * \return the Z coordinate  
 */
Real64 prop_marker::getZ()
{
    return m_pPosZ->text().toDouble();
};

/**
 * get the Z coordinate of the file
 * \return the Z coordinate  
 */
void prop_marker::setZ(Real64 fCoord)
{
    char pCoord[128]; 
    sprintf(pCoord,"%8.3f",fCoord);
    
    m_pPosZ->setText(QString(pCoord));
};

/**
 * Apply coordinates from the dlg to the marker
 */ 
void prop_marker::apply()
{
    //reformat the entries in the dlg
    setX(getX());
    setY(getY());
    setZ(getZ());

    m_pMarkerDoc->setMarkerNodePos( getX()*1e-2, getY()*1e-2, getZ()*1e-2 );
    m_pMarkerDoc->getNode()->rebuildDL();

    g_pWindow->enableInput();
};

/**
 * update the dlg 
 */
void prop_marker::update()
{
    blockSignals( TRUE );

    // position
    setX(m_pMarkerDoc->getMarkerNodePosX() * 1e2);
    setY(m_pMarkerDoc->getMarkerNodePosY() * 1e2);
    setZ(m_pMarkerDoc->getMarkerNodePosZ() * 1e2);
    // color
    svt_color oDocCol = m_pMarkerDoc->getColor();
    QString oStr;
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", (int) ( 255.0f * oDocCol.getR()), (int) ( 255.0f * oDocCol.getG()), (int) ( 255.0f * oDocCol.getB()) );
    m_pColor->setStyleSheet(oStr);
    // scale
    m_pScale->setValue(m_pMarkerDoc->getNode()->getScaleX() );
    // display name in 3d window?
    m_pCheckBoxName3D->setChecked( m_pMarkerDoc->getShowName3D() );

    blockSignals( FALSE );
};

/**
 * scale the marker
 */ 
void prop_marker::scale()
{
    m_pMarkerDoc->setScaling(m_pScale->value());
};

/**
 * set the color
 */ 
void prop_marker::setColor()
{
    // get the current color
    svt_color oDocCol = m_pMarkerDoc->getColor();

    int iR = (int) ( 255.0f * oDocCol.getR() );
    int iG = (int) ( 255.0f * oDocCol.getG() );
    int iB = (int) ( 255.0f * oDocCol.getB() );
    QColor oCol = QColorDialog::getColor(QColor(iR, iG, iB));

    if (!oCol.isValid())
	return;

    // ok, what did the user want...
    oDocCol.setR( (float)oCol.red()   / 255.0f );
    oDocCol.setG( (float)oCol.green() / 255.0f );
    oDocCol.setB( (float)oCol.blue()  / 255.0f );

    m_pMarkerDoc->setColor(oDocCol);
 
    QString oStr;
    oStr.sprintf("background-color: rgb(%i, %i, %i); color: rgb(127, 127, 127)", (int) ( 255.0f * oDocCol.getR()), (int) ( 255.0f * oDocCol.getG()), (int) ( 255.0f * oDocCol.getB()) );
    m_pColor->setStyleSheet(oStr);
}

/**
 * set the Window Current Possition
 */ 
void prop_marker::setWindowCurrentPos()
{
    g_pWindow->setCurrentPosition( svt_vector4<Real64>( getX()*1e-2, getY()*1e-2, getZ()*1e-2 ) );
}; 
