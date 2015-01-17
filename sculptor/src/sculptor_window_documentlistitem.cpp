/***************************************************************************
			  sculptor_window_documentlistitem
			  ------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
***************************************************************************/


// sculptor includes
#include <sculptor_document.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlistitem.h>
// sculptor xpm includes
#include <clip-medium.xpm>
#include <eye-medium.xpm>
#include <eye-medium-disable.xpm>
#include <mol-bw-grid-medium.xpm>
#include <mol-bw-medium.xpm>
#include <mol-green-grid-medium.xpm>
#include <mol-green-medium.xpm>
#include <mol-orange-grid-medium.xpm>
#include <mol-orange-medium.xpm>
#include <move-clashing-medium.xpm>
#include <move-igd-medium.xpm>
#include <move-medium.xpm>
#include <move-medium-locked.xpm>
#include <vol-bw-grid-medium.xpm>
#include <vol-bw-medium.xpm>
#include <vol-green-grid-medium.xpm>
#include <vol-green-medium.xpm>
#include <vol-orange-grid-medium.xpm>
#include <vol-orange-medium.xpm>
#include <world-medium.xpm>
#include <locked-medium.xpm>
#include <eraser-medium.xpm>
#include <documentColorBrush.xpm>
#include <marker.xpm>
// qt4 includes
#include <QTableWidgetItem>
#include <QScrollArea>
#include <QFileInfo>
#include <QPixmap>
#include <QBitmap>
#include <QString>
#include <QIcon>

extern sculptor_window* g_pWindow;

// the icons are static, so they are not constructed again and again for each document. but they
// have to be pointers, because the qicons (gui elements) need an already existing qmainwindow
int window_documentlistitem::m_iNumInstances = 0;
QIcon * window_documentlistitem::m_pClipMedium          = NULL;
QIcon * window_documentlistitem::m_pEyeMedium           = NULL;
QIcon * window_documentlistitem::m_pEyeMediumDisable    = NULL;
QIcon * window_documentlistitem::m_pMolBwGridMedium     = NULL;
QIcon * window_documentlistitem::m_pMolBwMedium         = NULL;
QIcon * window_documentlistitem::m_pMolGreenGridMedium  = NULL;
QIcon * window_documentlistitem::m_pMolGreenMedium      = NULL;
QIcon * window_documentlistitem::m_pMolOrangeGridMedium = NULL;
QIcon * window_documentlistitem::m_pMolOrangeMedium     = NULL;
QIcon * window_documentlistitem::m_pMoveClashingMedium  = NULL;
QIcon * window_documentlistitem::m_pMoveIGDMedium       = NULL;
QIcon * window_documentlistitem::m_pMoveMedium          = NULL;
QIcon * window_documentlistitem::m_pMoveMediumLocked    = NULL;
QIcon * window_documentlistitem::m_pVolBwGridMedium     = NULL;
QIcon * window_documentlistitem::m_pVolBwMedium         = NULL;
QIcon * window_documentlistitem::m_pVolGreenGridMedium  = NULL;
QIcon * window_documentlistitem::m_pVolGreenMedium      = NULL;
QIcon * window_documentlistitem::m_pVolOrangeGridMedium = NULL;
QIcon * window_documentlistitem::m_pVolOrangeMedium     = NULL;
QIcon * window_documentlistitem::m_pWorldMedium         = NULL;
QIcon * window_documentlistitem::m_pLockedMedium        = NULL;
QIcon * window_documentlistitem::m_pEraserMedium        = NULL;
QIcon * window_documentlistitem::m_pMarker              = NULL;
QBrush* window_documentlistitem::m_pColorRectBrush      = NULL;


/**
 * Constructor
 */
window_documentlistitem::window_documentlistitem( sculptor_document* pDoc,
						  bool bVisible,
						  bool bMoved,
						  bool bTarget,
						  bool bDock,
						  bool bSolution )
    : QObject(),
      m_bVisible( bVisible ),
      m_bMoved( bMoved ),
      m_bTarget( bTarget),
      m_bDock( bDock ),
      m_bSolution( bSolution ),
      m_bSelected( false ),
      m_bShowColor( false ),
      m_pDoc( pDoc ),
      m_pPropWidget( NULL )
{
    ++m_iNumInstances;

    // now construct the icons if this hasn't done before
    if (m_pClipMedium == NULL)
    {
        QBitmap oBitmap = QBitmap(QPixmap(documentColorBrush_xpm));
        m_pColorRectBrush      = new QBrush( oBitmap.scaled(29,29) );

	m_pClipMedium          = new QIcon( QPixmap(clip_medium) );
	m_pEyeMedium           = new QIcon( QPixmap(eye_medium) );
	m_pEyeMediumDisable    = new QIcon( QPixmap(eye_medium_disable) );
	m_pMolBwGridMedium     = new QIcon( QPixmap(mol_bw_grid_medium) );
	m_pMolBwMedium         = new QIcon( QPixmap(mol_bw_medium) );
	m_pMolGreenGridMedium  = new QIcon( QPixmap(mol_green_grid_medium) );
	m_pMolGreenMedium      = new QIcon( QPixmap(mol_green_medium) );
	m_pMolOrangeGridMedium = new QIcon( QPixmap(mol_orange_grid_medium) );
	m_pMolOrangeMedium     = new QIcon( QPixmap(mol_orange_medium) );
	m_pMoveClashingMedium  = new QIcon( QPixmap(move_clashing_medium) );
	m_pMoveIGDMedium       = new QIcon( QPixmap(move_igd_medium) );
	m_pMoveMedium          = new QIcon( QPixmap(move_medium) );
	m_pMoveMediumLocked    = new QIcon( QPixmap(move_medium_locked) );
	m_pVolBwGridMedium     = new QIcon( QPixmap(vol_bw_grid_medium) );
	m_pVolBwMedium         = new QIcon( QPixmap(vol_bw_medium) );
	m_pVolGreenGridMedium  = new QIcon( QPixmap(vol_green_grid_medium) );
	m_pVolGreenMedium      = new QIcon( QPixmap(vol_green_medium) );
	m_pVolOrangeGridMedium = new QIcon( QPixmap(vol_orange_grid_medium) );
	m_pVolOrangeMedium     = new QIcon( QPixmap(vol_orange_medium) );
	m_pWorldMedium         = new QIcon( QPixmap(world_medium) );
	m_pLockedMedium        = new QIcon( QPixmap(locked_medium) );
	m_pEraserMedium        = new QIcon( QPixmap(eraser_medium) );
	m_pMarker              = new QIcon( QPixmap(marker) );
    }

    // the cell to indicate the dock status
    m_pCell_Dock = new QTableWidgetItem();
    m_pCell_Dock->setFlags(Qt::ItemIsEnabled);

    // the cell to indicate the visibility status
    m_pCell_Visible = new QTableWidgetItem();
    m_pCell_Visible->setFlags(Qt::ItemIsEnabled);

    // the cell to indicate the moved status
    m_pCell_Moved = new QTableWidgetItem();
    m_pCell_Moved->setFlags(Qt::ItemIsEnabled);

    // the cell to show the document name
    m_pCell_Name = new QTableWidgetItem();

    if (getDoc() != NULL)
    {
	// init the cells
	setDock_DOIT();
	setMoved_DOIT();
	setVisible_DOIT();
	showDocName();

        // init some cell contents
        switch ( getDoc()->getType() )
        {
            case SENSITUS_DOC_SITUS:
                getCell_Moved()->setToolTip( QString("Volumetric Maps Cannot be Moved") );
                break;

            case SENSITUS_DOC_MARKER:
                getCell_Visible()->setToolTip( QString("This is a Marker Tool") );
                getCell_Dock()->setIcon( *m_pMarker );
                break;

            case SENSITUS_DOC_SCENE:
                getCell_Visible()->setToolTip( QString("") );
                getCell_Dock()->setIcon( *m_pWorldMedium );
                break;

            case SENSITUS_DOC_CLIP:
                getCell_Dock()->setIcon( *m_pClipMedium );
                getCell_Dock()->setToolTip( QString("This is a Clipping Plane") );
                break;

            case SENSITUS_DOC_ERASER:
                getCell_Dock()->setIcon( *m_pEraserMedium );
                getCell_Dock()->setToolTip( QString("This is an Eraser Tool") );
                break;
        }
    }

    // tell the document about its documentlistitem
    getDoc()->setDLItem( this );

    if ( getDocType() == SENSITUS_DOC_SITUS || getDocType() == SENSITUS_DOC_MARKER )
        showColor();
}

/*
 * Destructor
 */
window_documentlistitem::~window_documentlistitem()
{
    --m_iNumInstances;

    // if this is the last instance of this class, then delete all the QIcons
    if (m_pClipMedium != NULL && m_iNumInstances == 0)
    {
	delete m_pClipMedium;          m_pClipMedium = NULL;
	delete m_pEyeMedium;           m_pEyeMedium = NULL;
	delete m_pEyeMediumDisable;    m_pEyeMediumDisable = NULL;
	delete m_pMolBwGridMedium;     m_pMolBwGridMedium = NULL;
	delete m_pMolBwMedium;         m_pMolBwMedium = NULL;
	delete m_pMolGreenGridMedium;  m_pMolGreenGridMedium = NULL;
	delete m_pMolGreenMedium;      m_pMolGreenMedium = NULL;
	delete m_pMolOrangeGridMedium; m_pMolOrangeGridMedium = NULL;
	delete m_pMolOrangeMedium;     m_pMolOrangeMedium = NULL;
	delete m_pMoveClashingMedium;  m_pMoveClashingMedium = NULL;
	delete m_pMoveIGDMedium;       m_pMoveIGDMedium = NULL;
	delete m_pMoveMedium;          m_pMoveMedium = NULL;
	delete m_pMoveMediumLocked;    m_pMoveMediumLocked = NULL;
	delete m_pVolBwGridMedium;     m_pVolBwGridMedium = NULL;
	delete m_pVolBwMedium;         m_pVolBwMedium = NULL;
	delete m_pVolGreenGridMedium;  m_pVolGreenGridMedium = NULL;
	delete m_pVolGreenMedium;      m_pVolGreenMedium = NULL;
	delete m_pVolOrangeGridMedium; m_pVolOrangeGridMedium = NULL;
	delete m_pVolOrangeMedium;     m_pVolOrangeMedium = NULL;
	delete m_pWorldMedium;         m_pWorldMedium = NULL;
	delete m_pLockedMedium;        m_pLockedMedium = NULL;
	delete m_pEraserMedium;        m_pEraserMedium = NULL;
	delete m_pMarker;              m_pMarker = NULL;
        delete m_pColorRectBrush;      m_pColorRectBrush = NULL;
    }

    if ( m_pPropWidget != NULL )
        delete m_pPropWidget;
}

/*
 *
 */
QTableWidgetItem* window_documentlistitem::getCell_Dock()
{
    return m_pCell_Dock;
}

/*
 *
 */
QTableWidgetItem* window_documentlistitem::getCell_Visible()
{
    return m_pCell_Visible;
}

/*
 *
 */
QTableWidgetItem* window_documentlistitem::getCell_Moved()
{
    return m_pCell_Moved;
}

/*
 *
 */
QTableWidgetItem* window_documentlistitem::getCell_Name()
{
    return m_pCell_Name;
}

/*
 *
 */
void window_documentlistitem::showDocName()
{
    //DEBUG//("DLI %i: setDocName\n", getIndex());

    QString oNameDisplay = QString("No Attached Document");
    QString oNameFull    = QString("");

    if ( getDoc() != NULL )
    {
	oNameDisplay = QString( getDoc()->getDisplayName().c_str() );
	oNameFull    = QString( getDoc()->getFileName().c_str() );

	if ( getDoc()->getChanged() )
	    oNameDisplay.prepend("* ");
    }

    getCell_Name()->setText( oNameDisplay );
    getCell_Name()->setToolTip( oNameFull );
}

/*
 *
 */
void window_documentlistitem::setVisible( bool bVisible )
{
    // visibility can always be changed regardless of lock status

    if (m_bVisible == bVisible || m_pDoc->getType() == SENSITUS_DOC_SCENE )
    {
	// if (bVisible)
	//     printf("DLI %i: setVisible(true) no change\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setVisible(false) no change\n", getCell_Name()->row());

	return;
    }
    else
    {
	m_bVisible = bVisible;
	setVisible_DOIT();
    }
}

/*
 *
 */
void window_documentlistitem::setVisible_DOIT()
{
    // if (m_bVisible)
    //     printf("DLI %i: setVisible(true)\n", getCell_Name()->row());
    // else
    //     printf("DLI %i: setVisible(false)\n", getCell_Name()->row());
    QColor oColor;


    // if it exists, enable/disable the property widget
    if ( getPropWidget() != NULL)
        getPropWidget()->setEnabled( m_bVisible );


    // set visibility in the document itself
    getDoc()->setVisible(m_bVisible);


    // show color in a rectangular frame around eye icon?
    if (m_bShowColor)
    {
        svt_color oCol = getDoc()->getColor();
        oColor = QColor( oCol.getR()*255.0f, oCol.getG()*255.0f, oCol.getB()*255.0f );

        m_pColorRectBrush->setColor( oColor );
        getCell_Visible()->setBackground( *m_pColorRectBrush );
    }
    else
    {
        getCell_Visible()->setBackground( QBrush(Qt::NoBrush) );
    }


    if (m_bVisible)
    {
	getCell_Visible()->setIcon   ( *m_pEyeMedium );
	getCell_Visible()->setToolTip( QString("Click Here to Make This Document Invisible") );
    }
    else
    {
	getCell_Visible()->setIcon   ( QIcon("") );
	getCell_Visible()->setToolTip( QString("Click Here to Make This Document Visible") );
    }
}

/*
 *
 */
bool window_documentlistitem::getVisible()
{
    return m_bVisible;
}

/*
 *
 */
void window_documentlistitem::setMoved( bool bMoved )
{
    // if (bMoved)
    //     printf("DLI %i: setMoved(true) ", getIndex());
    // else
    //     printf("DLI %i: setMoved(false) ", getIndex());


    if ( m_bMoved == bMoved  || getDoc()->getType() == SENSITUS_DOC_SITUS )
    {
	//DEBUG//("DLI %i: setMoved: map cannot be moved\n", getCell_Name()->row());
	//DEBUG//printf("DLI %i: setMoved(no change)\n", getCell_Name()->row());
	return;
    }
    else
    {
        m_bMoved = bMoved;
        setMoved_DOIT();
    }
}

/*
 *
 */
void window_documentlistitem::setMoved_DOIT()
{
    // if (m_bMoved)
    //     printf("DLI %i: setMoved_DOIT(true)\n", getCell_Name()->row());
    // else
    //     printf("DLI %i: setMoved_DOIT(false)\n", getCell_Name()->row());


    if (getIndex() == -1)
        return;

    if ( m_bMoved )
    {
        // display icon
        if ( getDoc()->getLocked() )
            getCell_Moved()->setIcon( *m_pMoveMediumLocked );
        else if ( getDoc()->getIGDActive() )
            getCell_Moved()->setIcon( *m_pMoveIGDMedium );
        else
            getCell_Moved()->setIcon( *m_pMoveMedium );

        // set tooltip
	if ( m_pDoc->getType() != SENSITUS_DOC_SCENE )
        {
            if ( getDoc()->getIGDActive() )
                getCell_Moved()->setToolTip( QString("Translations are Done With the Mouse,\nRotations are Determined From IGD Data") );
            else
                getCell_Moved()->setToolTip( QString("Only This Document is Moved") );
        }
	else
	    getCell_Moved()->setToolTip( QString("Whole Scene is Moved") );
    }
    else
    {
        if ( getDoc()->getLocked() )
            getCell_Moved()->setIcon( *m_pLockedMedium );
        else
            getCell_Moved()->setIcon( QIcon() );

	if ( m_pDoc->getType() != SENSITUS_DOC_SCENE )
        {
            
	    getCell_Moved()->setToolTip( QString("Click Here to Move Only This Document") );
        }
	else
	    getCell_Moved()->setToolTip( QString("Click Here to Move the Whole Scene") );
    }
}

/*
 *
 */
bool window_documentlistitem::getMoved()
{
    return m_bMoved;
}

/*
 *
 */
void window_documentlistitem::setDock( bool bDock )
{
    // do nothign if there is no document or the document is locked
    if ( getDoc() == NULL || getLocked() )
	return;

    // set dock only valid for pdb documents
    if ( getDoc()->getType() != SENSITUS_DOC_PDB )
    {
	// if (bDock)
	//     printf("DLI %i: setDock(true) not a structure\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setDock(false) not a structure\n", getCell_Name()->row());

	return;
    }

    // no change -- do nothing
    if (m_bDock == bDock)
    {
	// if (bDock)
	//     printf("DLI %i: setDock(true) no change\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setDock(false) no change\n", getCell_Name()->row());

	return;
    }
    else // ok, so actually set the dock status
    {
	m_bDock = bDock;
	setDock_DOIT();
    }
}

/*
 *
 */
void window_documentlistitem::setDock_DOIT()
{
    switch( getDoc()->getType() )
    {
        case SENSITUS_DOC_PDB:
        {
            if ( getDoc()->getEliDataLoaded() || (m_pDoc->getOrigProbeDoc() && m_pDoc->getOrigProbeDoc()->getEliDataLoaded()) )
            {
                /////////////////////////////////////// IGD on ////////////////////////////////////////

                if ( getDock() )
                {
                    getCell_Dock()->setIcon( *m_pMolOrangeGridMedium );
                    getCell_Dock()->setToolTip( QString("This is the <b>Probe</b> (Atomic Model)\nto be Docked Into the Target Map\nwith <b>IGD Data Attached<b>") );
                }
                else if ( getSolution() )
                {
                    getCell_Dock()->setIcon( *m_pMolGreenGridMedium );
                    getCell_Dock()->setToolTip( QString("This is a <b>Solution Candidate</b> (Atomic Model)\nwith <b>IGD Data Attached<b>") );
                }
                else
                {
                    getCell_Dock()->setIcon( *m_pMolBwGridMedium );
                    getCell_Dock()->setToolTip( QString("This is an Atomic Model\nwith <b>IGD Data Attached<b") );
                }
            }
            else
            {
                /////////////////////////////////////// IGD off ///////////////////////////////////////

                if ( getDock() )
                {
                    getCell_Dock()->setIcon( *m_pMolOrangeMedium );
                    getCell_Dock()->setToolTip( QString("This is the <b>Probe</b> (Atomic Model)\nto be Docked Into the Target Map") );
                }
                else if ( getSolution() )
                {
                    getCell_Dock()->setIcon( *m_pMolGreenMedium );
                    getCell_Dock()->setToolTip( QString("This is a <b>Solution Candidate</b> (Atomic Model)") );
                }
                else
                {
                    getCell_Dock()->setIcon( *m_pMolBwMedium );
                    getCell_Dock()->setToolTip( QString("This is an Atomic Model") );
                }
            }
            break;
        }
        case SENSITUS_DOC_SITUS:
        {
            if ( getTarget() )
            {
                getCell_Dock()->setIcon( *m_pVolOrangeMedium );
                getCell_Dock()->setToolTip( QString("This is the <b>Target Map</b>") );
            }
            else
            {
                getCell_Dock()->setIcon( *m_pVolBwMedium );
                getCell_Dock()->setToolTip( QString("This is a Volumetric Map") );
            }
            break;
        }
    }
}

/*
 *
 */
void window_documentlistitem::showMoved()
{
    setMoved_DOIT();
}

/*
 *
 */
bool window_documentlistitem::getDock()
{
    return m_bDock;
}

/*
 *
 */
void window_documentlistitem::setSolution( bool bSolution )
{
    if ( getLocked() )
	return;


    if ( m_pDoc->getType() != SENSITUS_DOC_PDB )
    {
	// if (bSolution)
	//     printf("DLI %i: setSolution(true) not a structure\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setSolution(false) not a structure\n", getCell_Name()->row());

	return;
    }


    if (m_bSolution == bSolution)
    {
	// if (bSolution)
	//     printf("DLI %i: setSolution(true) no change\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setSolution(false) no change\n", getCell_Name()->row());

	return;
    }
    else
    {
	m_bSolution = bSolution;
	setDock_DOIT();
    }
}

/*
 *
 */
bool window_documentlistitem::getSolution()
{
    return m_bSolution;
}

/*
 *
 */
bool window_documentlistitem::getSelected()
{
    return m_bSelected;
}

/*
 *
 */
void window_documentlistitem::setSelected(bool bSelected)
{
    //DEBUG//(" [DLI %i:", getCell_Name()->row());

    if ( bSelected == true )
    {

	//if ( getPendingSelection() || !getSelected() )
	if ( !getSelected() && getDoc() != NULL )
	{
	    getDoc()->onSelection();
	    //DEBUG//(" onSelection");
	}

	m_bSelected = true;
	//DEBUG//(" setSelected(true)");

    }
    else if ( bSelected == false )
    {

	//if ( getPendingDeselection() || getSelected() )
	if ( getSelected() && getDoc() != NULL )
	{
	    getDoc()->onDeselection();
	    //DEBUG//(" onDeselection");
	}

	m_bSelected = false;
	//DEBUG//(" setSelected(false)");
    }
    //DEBUG//("]");

    return;
}

/*
 *
 */
bool window_documentlistitem::getPendingSelection()
{
    if ( getCell_Name()->isSelected() == true && m_bSelected == false )
	return true;

    return false;
}

/*
 *
 */
bool window_documentlistitem::getPendingDeselection()
{
    if ( getCell_Name()->isSelected() == false && m_bSelected == true )
	return true;

    return false;
}

/*
 *
 */
sculptor_document* window_documentlistitem::getDoc()
{
    return m_pDoc;
}

/*
 *
 */
void window_documentlistitem::setDoc(sculptor_document* pDoc)
{
    m_pDoc = pDoc;

    if (m_pDoc != NULL)
	m_pDoc->setDLItem(this);
}

/**
 *
 */
int window_documentlistitem::getDocType()
{
    return m_pDoc->getType();
}


/*
 *
 */
void window_documentlistitem::setTarget( bool bTarget )
{

    // if document locked, don't allow changes
    if ( getLocked() )
	return;


    if ( m_pDoc->getType() != SENSITUS_DOC_SITUS )
    {
	// if (bTarget)
	//     printf("DLI %i: setTarget(true) not a map\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setTarget(false) not a map\n", getCell_Name()->row());

	return;
    }


    if (m_bTarget == bTarget)
    {
	// if (bTarget)
	//     printf("DLI %i: setTarget(true) no change\n", getCell_Name()->row());
	// else
	//     printf("DLI %i: setTarget(false) no change\n", getCell_Name()->row());

	return;
    }
    else
    {
	m_bTarget = bTarget;
	setDock_DOIT();
    }
}

/*
 *
 */
bool window_documentlistitem::getTarget()
{
    return m_bTarget;
}

/*
 *
 */
void window_documentlistitem::setLocked( bool bLocked )
{
    if ( getDoc()->getLocked() == bLocked )
    {
	//DEBUG//("DLI %i: setLocked(no change)\n", getCell_Name()->row());
	return;
    }
    else
    {
	getDoc()->setLocked(bLocked);
        // because the lock state is displayed in the moved column, update the move column
	setMoved_DOIT();
    }
}

/*
 *
 */
bool window_documentlistitem::getLocked()
{
    return getDoc()->getLocked();
}

/*
 *
 */
QWidget* window_documentlistitem::getPropWidget()
{
    return m_pPropWidget;
}

/*
 *
 */
QWidget* window_documentlistitem::createPropWidget()
{
    if (m_pPropWidget == NULL)
    {
	// if this item has no document return 0
	if (m_pDoc == NULL)
	    return NULL;


	// create a QScrollArea as containing widget for the prop dialog
	QScrollArea* pScrollArea = new QScrollArea();
	pScrollArea->setFrameShape( QFrame::NoFrame );


	// request the prop dialog from the document.
	//
	// note that a scrollarea insists on destroying its widget. so let it have the ownership of
	// the dialog, although it would be more appropriate to let the document own its dialog (and
	// thus also let the document destroy the dialog). FIXME?
	//
	QWidget* pDialog = m_pDoc->createPropDlg( pScrollArea );


	// if the document returned 0, return 0
	if ( pDialog == NULL )
	    return NULL;


	// this grid layout forces the prop dialog to expand
	QGridLayout* pScrollLayout = new QGridLayout();
	pScrollLayout->addWidget(pDialog);
	pScrollArea->viewport()->setLayout(pScrollLayout);
	pScrollArea->setWidget(pDialog);

	// good, now save the pointer to this widget
	m_pPropWidget = (QWidget*) pScrollArea;
    }
    return m_pPropWidget;
}

/*
 *
 */
int window_documentlistitem::getIndex()
{
    return m_pCell_Name->row();
}

/*
 *
 */
void window_documentlistitem::showChanged()
{
    showDocName();
}

/**
 *
 */
void window_documentlistitem::showColor(bool bShowColor)
{
    m_bShowColor = bShowColor;
    setVisible_DOIT();
}

/**
 *
 */
void window_documentlistitem::showLocked()
{
    setMoved_DOIT();
}
