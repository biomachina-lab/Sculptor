/***************************************************************************
			  sculptor_window_documentlist
			  ------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

// sculptor includes
#include <sculptor_window_documentlist.h>
#include <sculptor_window_documentlistitem.h>
#include <sculptor_document.h>
#include <situs_document.h>
#include <sculptor_window.h>
// sculptor xpm includes
#include <raise.xpm>
#include <lower.xpm>
#include <delete.xpm>
#include <moveSmall.xpm>
#include <eyeSmall.xpm>
#include <top.xpm>
#include <save.xpm>
#include <properties.xpm>
//qt4 includes
#include <QStackedLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QIcon>
// svt includes
#include <svt_iostream.h>


///////////////////////////////////////////////////////////////////////////////
// mousefilter class to orchestrate mouse clicks on the documentlist
///////////////////////////////////////////////////////////////////////////////


/**
 * Constructor
 */
window_documentlist_mousefilter::window_documentlist_mousefilter(window_documentlist* pDocumentlist, QWidget* pTableWidget)
    : QObject( pTableWidget ),
      m_pDL        ( pDocumentlist ),
      m_pDLTable   ( (QTableWidget*)pTableWidget ),
      m_pDLViewport( ((QTableWidget*)pTableWidget)->viewport() )
{
}

/**
 *
 */
bool window_documentlist_mousefilter::eventFilter(QObject* pObject, QEvent* pEvent)
{
    // is the event receiver the viewport of the documentlist?
    if ( pObject == m_pDLViewport )
    {
	QTableWidgetItem * pItem = NULL;
	QMouseEvent* pMouseEvent = NULL;


	if ( pEvent->type() == QEvent::MouseButtonPress )
	{
	    pMouseEvent = (QMouseEvent*) pEvent;

	    // get the table item onto which was clicked
	    pItem =  m_pDLTable->itemAt( pMouseEvent->pos() );

	    // don't filter if clicked elsewhere
	    if ( pItem == NULL )
		return false;

	    switch ( pMouseEvent->button() )
	    {
                case Qt::MidButton:
                    // filter middle button away
                    return true;

		case Qt::RightButton:
                    m_pDL->rightClicked( pMouseEvent->pos(), pItem->row(), pItem->column() );
		    // filter right button away since we have no use for it
		    return true;

		case Qt::LeftButton:
		    m_pDL->leftClicked( pItem->row(), pItem->column() );
		    // filter left clicks on the left three columns away
                    if ( pItem->column() < 3 )
                        return true;
		    break;

		default:
		    break;
	    }
	}

	if ( pEvent->type() == QEvent::MouseButtonDblClick )
	{
	    pMouseEvent = (QMouseEvent*) pEvent;

	    // get the table item onto which was clicked
	    pItem =  m_pDLTable->itemAt( pMouseEvent->pos() );

	    // don't filter if clicked elsewhere
	    if ( pItem == NULL )
		return false;

	    switch ( pMouseEvent->button() )
	    {
                case Qt::MidButton:
                    // filter middle button away
                    return true;

		case Qt::RightButton:
		    // right button can be filtered away
		    return true;

		case Qt::LeftButton:
		    m_pDL->doubleClicked( pItem->row(), pItem->column() );
                    // now that we did our own stuff, filter left doubleclick away
		    return true;

		default:
		    break;
	    }
	}
    }

    // do not filter any other events
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// the documentlist
///////////////////////////////////////////////////////////////////////////////


window_documentlist::window_documentlist( sculptor_window * pWindow, QWidget *pParent )
    : QWidget( pParent ),
      m_pWindow( pWindow ),
      m_pPropDialogArea( NULL ),
      m_pPropWidgetStack( NULL ),
      m_pCurrentPropDialog( NULL ),
      m_iCurrentPropDialog( -1 ),
      m_bHidePropDialog( false )
{
    setupUi(this);

    //
    // set the icons for the buttons below the documentlist
    //
    m_pButtonDelete    ->setIcon( QIcon(QPixmap(deleteIcon)) );
    m_pButtonMove      ->setIcon( QIcon(QPixmap(moveSmallIcon)) );
    m_pButtonVisible   ->setIcon( QIcon(QPixmap(eyeSmallIcon)) );
    m_pButtonUp        ->setIcon( QIcon(QPixmap(raiseIcon)) );
    m_pButtonDown      ->setIcon( QIcon(QPixmap(lowerIcon)) );
    m_pButtonTop       ->setIcon( QIcon(QPixmap(top)) );
    m_pButtonSave      ->setIcon( QIcon(QPixmap(saveIcon)) );
//#ifdef SVT_SYSTEM_X11
    m_pButtonProperties->setIcon( QIcon(QPixmap(propIcon)) );
//#else
//    m_pButtonLayout->removeWidget( m_pButtonProperties );
//#endif

    //
    // signal slot connections
    //
    connect( m_pTableWidget, SIGNAL( itemSelectionChanged() ), this, SLOT(selectionChanged()) );


    m_pTableWidget->setIconSize(QSize(30,30));

    //
    // these signals are sent to slots in the main window
    //
    connect( m_pButtonDelete,     SIGNAL(clicked()), m_pWindow, SLOT(sDocsRemove()) );
    connect( m_pButtonUp,         SIGNAL(clicked()), m_pWindow, SLOT(sDocMoveUp()) );
    connect( m_pButtonDown,       SIGNAL(clicked()), m_pWindow, SLOT(sDocMoveDown()) );
    connect( m_pButtonTop,        SIGNAL(clicked()), m_pWindow, SLOT(sDocMakeTop()) );
    connect( m_pButtonMove,       SIGNAL(clicked()), m_pWindow, SLOT(toggleMovedDoc()) );
    connect( m_pButtonVisible,    SIGNAL(clicked()), m_pWindow, SLOT(toggleViewDoc()) );
    connect( m_pButtonSave,       SIGNAL(clicked()), m_pWindow, SLOT(save()) );
    connect( m_pButtonProperties, SIGNAL(clicked()), m_pWindow, SLOT(togglePropDlgArea()) );


    //
    // install the mouse filter tablewidget's viewport to orchestrate mouse button events
    //
    m_pMousefilter = new window_documentlist_mousefilter( this, m_pTableWidget );
    m_pTableWidget->viewport()->installEventFilter(m_pMousefilter);
}

/*
 * slot called whenever the selection changes.
 * - set the selected state of the documentlistitems
 * - call the onSelection and onDeselection methods of the documents
 * - enable/disable structure/volume menu according to type of selected documents
 */
void window_documentlist::selectionChanged()
{
    m_pTableWidget->blockSignals(true);

    //DEBUG//("selectionChanged:\n");

    // for counting selected docs
    int iPDBsSelected = 0;
    int iVolsSelected = 0;
    int i;

    for ( i=0; i<count(); ++i )
    {
	// call the onSelection and onDeselection methods if necessary
	if ( m_aItems[i]->getPendingSelection() )
	{
	    //DEBUG//(" pendingSelection: %i", i);
	    setSelected(i, true);
	}
	else if ( m_aItems[i]->getPendingDeselection() )
	{
	    //DEBUG//(" pendingDeselection: %i", i);
	    setSelected(i, false);
	}

	// count selected pdbs and volumes
	if ( getSelected(i) )
	{
	    switch ( getDocType(i) )
	    {
		case SENSITUS_DOC_PDB:
		    ++iPDBsSelected;
		    break;

		case SENSITUS_DOC_SITUS:
		    ++iVolsSelected;
		    break;
	    }
	}
    }

    m_pWindow->updateMenus( getSelected(0), iPDBsSelected, iVolsSelected );
    m_pWindow->updateMenuItems();

    // if more than one document selected, collapse the prop dlg area and indicate that this was an
    // automatic collapse (and not a enduring, user requested one)
    if ( iPDBsSelected + iVolsSelected > 1 )
    {
        m_pWindow->setPropDlgAreaVisible(false);
        m_iCurrentPropDialog = -1;
    }

    //DEBUG//("\n");

    m_pTableWidget->blockSignals(false);
}

/*
 *
 */
void window_documentlist::leftClicked(int iRow, int iCol)
{
    if ( iRow >= count() )
	return;

    //DEBUG//printf("leftClicked column %i\n", iCol);

    switch (iCol)
    {
	case 0:
            if ( isPDB(iRow) )
                m_pWindow->setDockDoc( iRow);
            else if( isSitus(iRow) )
                m_pWindow->setTargetDoc( iRow);
	    break;

	case 1:
	    setVisible( iRow, !m_aItems[iRow]->getVisible() );
	    break;

	case 2:
	    m_pWindow->setMovedDoc(iRow);
            if ( getDocType(iRow) != SENSITUS_DOC_SITUS )
            {
                setCurrentItem(iRow);
                showPropDialog(iRow);
            }
	    break;

	case 3:
            showPropDialog(iRow);
	    break;

	default:
	    break;
    }
}

/*
 *
 */
void window_documentlist::rightClicked(QPoint oPoint, int iRow, int iColumn)
{
    switch (iColumn)
    {
        case 0:
            break;

        case 1:
            break;

        case 2:
            break;

        case 3:

            if ( QApplication::keyboardModifiers() == Qt::ControlModifier )
            {
                // control key only affects the selection of a single item. if pressed, do not
                // deselect any other items
                m_pTableWidget->setCurrentCell(iRow, iColumn, QItemSelectionModel::Select);
            }
            else if ( QApplication::keyboardModifiers() == Qt::ShiftModifier )
            {
                int iStart, iEnd;

                // shift modifies the selection behavior for a region. find out about start and end
                // index of that region
                if      ( getCurrentIndex() < iRow )
                {
                    iStart = getCurrentIndex();
                    iEnd   = iRow;
                }
                else if ( getCurrentIndex() > iRow )
                {
                    iStart = iRow;
                    iEnd   = getCurrentIndex();
                }
                else
                {
                    iStart = iRow;
                    iEnd   = iRow;
                }
                
                // select the range between iStart and iEnd
                m_pTableWidget->setRangeSelected( QTableWidgetSelectionRange(iStart, 3, iEnd, 3), true );
            }
            else
            {
                // no modifier. select current item and deselect others
                m_pTableWidget->setCurrentCell(iRow, iColumn);
            }

            // process event queue. this emits the selectionChanged signal, which is handled in the
            // document list to let the documents do their onSelection stuff
            svt_forceUpdateGUIWindows();

            if ( getDocType(iRow) == SENSITUS_DOC_PDB)
                m_pWindow->popupStructureMenu( mapToGlobal(oPoint) );
            else if ( getDocType(iRow) == SENSITUS_DOC_SITUS)
                m_pWindow->popupVolumeMenu( mapToGlobal(oPoint) );

            break;

	default:
	    break;
    }
}

/*
 *
 */
void window_documentlist::doubleClicked(int iRow, int iColumn)
{
    //DEBUG//printf("doubleClicked column %i\n", iCol);
    pdb_document* pProbeDoc;

    switch (iColumn)
    {
	case 0:
            pProbeDoc = (pdb_document*)m_pWindow->getProbeDoc();
            if ( pProbeDoc != NULL && pProbeDoc->getDLItem()->getIndex() == iRow )
                m_pWindow->setSolutionDoc(iRow);
            break;

        case 2:
            // on double click onto move column (de)activate IGD
            if ( getDocType(iRow) == SENSITUS_DOC_PDB )
            {
                m_pWindow->disableInput();

                if ( getDoc(iRow)->getIGDActive() )
                {
                    getDoc(iRow)->setIGDActive(false);
                }
                else
                {
                    if ( getDoc(iRow)->getEliDataLoaded() ||
                         (getDoc(iRow)->getOrigProbeDoc() && getDoc(iRow)->getOrigProbeDoc()->getEliDataLoaded()) )
                        getDoc(iRow)->setIGDActive(true);
                }
                m_aItems[iRow]->showMoved();

                m_pWindow->enableInput();
            }
            break;

	case 3:
	    m_pWindow->togglePropDlgArea();
	    break;

	default:
	    break;
    }
}

/*
 *
 */
void window_documentlist::clearSelection()
{
    if ( count() <= 1 )
	return;

    m_pTableWidget->clearSelection();
}

/*
 *
 */
void window_documentlist::setSelected(const int iIndex, bool bSelected)
{
    if ( iIndex<0 || iIndex>= count() )
    {
	//DEBUG//("window_documentlist::setSelected(%i): %i out of range\n", iIndex, iIndex);
	return;
    }

    // if (bSelected)
    // 	(" DL setSelected(%i, true)", iIndex);
    // else
    // 	(" DL setSelected(%i, false)", iIndex);

    m_aItems[iIndex]->setSelected(bSelected);

    //DEBUG//("\n");
}

/*
 *
 */
bool window_documentlist::getSelected(int iIndex)
{
    if ( iIndex<0 || iIndex >= count() )
    {
	//DEBUG//("DL::getSelected %i out of range\n", iIndex);
	return false;
    }

    return m_aItems[iIndex]->getSelected();
}

/*
 *
 */
int window_documentlist::getCurrentIndex()
{
    // attention: returns -1 if no current item
    return m_pTableWidget->currentRow();
}

/*
 *
 */
window_documentlistitem* window_documentlist::getCurrentItem()
{
    // attention: returns NULL if no current item
    if ( getCurrentIndex() < 0 || getCurrentIndex() >= count() )
	return NULL;

    return m_aItems[getCurrentIndex()];
}

/*
 *
 */
void window_documentlist::setCurrentItem(int iIndex)
{
    //DEBUG//("DL setCurrentItem(%i)\n", iIndex);

    m_pTableWidget->setCurrentCell(iIndex, 3);
}

/*
 *
 */
window_documentlistitem* window_documentlist::operator[](int iIndex)
{
    if ( iIndex<0 || iIndex>=(int)m_aItems.size() )
	return NULL;
    else
	return m_aItems[iIndex];
}

/*
 *
 */
int window_documentlist::addItem(window_documentlistitem* pItem)
{
    //DEBUG//("DL addItem: ");

    insertItem(pItem);
    return count() - 1;
}

/*
 *
 */
void window_documentlist::insertItem(window_documentlistitem* pItem, int i)
{
    int r;

    // if no position is given, add after last row
    if ( i<1 || i>count() )
	r  = count();
    else
	r = i;

    m_pTableWidget->blockSignals(true);

    //DEBUG//("DL insertItem at row %i\n", r);

    // save the documentlistitem (last position)
    vector<window_documentlistitem*>::iterator itr = m_aItems.begin();
    itr += r;
    m_aItems.insert(itr, pItem);

    // insert the cells into the table
    m_pTableWidget->insertRow(r);
    m_pTableWidget->setItem(r, 0, pItem->getCell_Dock());
    m_pTableWidget->setItem(r, 1, pItem->getCell_Visible());
    m_pTableWidget->setItem(r, 2, pItem->getCell_Moved());
    m_pTableWidget->setItem(r, 3, pItem->getCell_Name());

    m_pTableWidget->blockSignals(false);
}

/*
 *
 */
int window_documentlist::count()
{
    return m_pTableWidget->rowCount();
}

/*
 *
 */
void window_documentlist::removeItem(const int iIndex)
{
    if ( iIndex<1 || iIndex>(int)m_aItems.size() )
	return;

    if ( m_aItems[iIndex]->getLocked() )
        return;

    m_pTableWidget->blockSignals(true);


    // if the item was selected, deselect it so it may clean up things
    if ( getSelected(iIndex) )
	setSelected(iIndex, false);


    // get a pointer to the documentlistitem to be deleted
    vector<window_documentlistitem*>::iterator itr = m_aItems.begin();
    itr += iIndex;


    // check if the documentlistitem has created a property dialog earlier
    int iDlgIndex = -2;
    if ( (*itr)->getPropWidget() != NULL )
        iDlgIndex = m_pPropWidgetStack->indexOf( (*itr)->getPropWidget() );


    // if it has a property dialog, and it is shown:
    // 1. collapse the prop dlg area
    // 2. indicate the automatic collapse by setting m_iCurrentPropDialog to -1
    // 3. remove the property dialog widget from the stack
    // the property dialog will be deleted in the destructor of the documentlistitem
    if ( iDlgIndex >= 0 && iDlgIndex == m_iCurrentPropDialog )
    {
        m_pWindow->setPropDlgAreaVisible(false);
        m_iCurrentPropDialog = -1;
	m_pPropWidgetStack->removeWidget(m_pCurrentPropDialog);
    }

    // if there is a property dialog, but it was not shown, just remove it from the stack
    else if ( iDlgIndex >= 0 )
        m_pPropWidgetStack->removeWidget(m_pCurrentPropDialog);


    // now, remove the corresponding row from the table
    m_pTableWidget->removeRow(iIndex);

    // delete the documentlistitem
    delete *itr;

    // remove the entry from the vector holding all documentlistitems
    m_aItems.erase(itr);


    m_pTableWidget->blockSignals(false);
}

/*
 *
 */
bool window_documentlist::moveItemUpmost(const int i)
{
    if ( i<2 || i > (int)(m_aItems.size()-1) )
	return false;

    if ( m_aItems[i]->getLocked() )
        return false;

    m_pTableWidget->blockSignals(true);

    // copy i-th entry
    window_documentlistitem* pDLI = m_aItems[i];

    // erase i-th entry
    vector<window_documentlistitem*>::iterator itr = m_aItems.begin();
    itr+=i;
    m_aItems.erase(itr);

    // set itr to point to entry no. 1
    itr = m_aItems.begin();
    ++itr;
    // and insert the previously copied element
    m_aItems.insert(itr, pDLI);


    // now the table widget row
    //
    m_pTableWidget->insertRow(1);

    // move the corresponding cells in the table
    int iColumn;
    for (iColumn=0; iColumn<4; ++iColumn)
    {
	QTableWidgetItem * pCell = m_pTableWidget->takeItem(i+1, iColumn);
	m_pTableWidget->setItem(1, iColumn, pCell);
    }

    m_pTableWidget->removeRow(i+1);

    setCurrentItem(1);

    m_pTableWidget->blockSignals(false);

    return true;
}

/*
 *
 */
bool window_documentlist::moveItemUp(const int i)
{
    if ( i<2 || i > (int)(m_aItems.size()-1) )
	return false;

    if ( m_aItems[i]->getLocked() )
        return false;

    m_pTableWidget->blockSignals(true);

    // swap document no. i-1 with document no. i
    window_documentlistitem* pDLI = m_aItems[i-1];
    m_aItems[i-1] = m_aItems[i];
    m_aItems[i] = pDLI;

    // swap the corresponding cells in the table
    int iColumn;
    for (iColumn=0; iColumn<4; ++iColumn)
    {
	QTableWidgetItem * pTWI_upper = m_pTableWidget->takeItem(i-1, iColumn);
	QTableWidgetItem * pTWI_lower = m_pTableWidget->takeItem( i , iColumn);

	m_pTableWidget->setItem(i-1, iColumn, pTWI_lower);
	m_pTableWidget->setItem( i , iColumn, pTWI_upper);
    }

    setCurrentItem(i-1);

    m_pTableWidget->blockSignals(false);

    return true;
}

/*
 *
 */
bool window_documentlist::moveItemDown(const int i)
{
    if ( i<0 || i > (int)(m_aItems.size()-2) )
	return false;

    if ( m_aItems[i]->getLocked() )
        return false;

    m_pTableWidget->blockSignals(true);

    // swap document no. i-1 with document no. i
    window_documentlistitem* pDLI = m_aItems[i+1];
    m_aItems[i+1] = m_aItems[i];
    m_aItems[i] = pDLI;

    // swap the corresponding cells in the table
    int iColumn;
    for (iColumn=0; iColumn<4; ++iColumn)
    {
	QTableWidgetItem * pTWI_upper = m_pTableWidget->takeItem( i , iColumn);
	QTableWidgetItem * pTWI_lower = m_pTableWidget->takeItem(i+1, iColumn);

	m_pTableWidget->setItem( i , iColumn, pTWI_lower);
	m_pTableWidget->setItem(i+1, iColumn, pTWI_upper);
    }

    setCurrentItem(i+1);

    m_pTableWidget->blockSignals(false);

    return true;
}

/*
 *
 */
void window_documentlist::setPropDialogArea(QWidget* pWidget)
{
    m_pPropDialogArea = pWidget;

    m_pPropWidgetStack = new QStackedLayout(m_pPropDialogArea);
    //m_pPropWidgetStack->setContentsMargins(0,0,0,0);
}

/*
 *
 */
void window_documentlist::showPropDialog(int i)
{
    bool bPendingUncollapse = false;

    // if the user requested the prop dlg area to be collapsed, do nothing
    if ( m_bHidePropDialog )
        return;

    // m_iCurrentPropDialog == -1 means the prop dlg area was collapsed by clicking on a document
    // that does not have a prop dlg, so uncollapse the area now
    if ( m_iCurrentPropDialog == -1 )
        bPendingUncollapse = true;

    // if not, and if the prop dialog area is collapsed, do nothing
    else if ( !m_pWindow->getPropDlgAreaVisible() )
	return;


    // if no specific document's dialog is requested, get the current document
    if (i == -1)
    {
	i = getCurrentIndex();

	if (i == -1)
	    return;
    }

    // if there are already dialogs in the stack, and if the requested prop dlg is already shown,
    // only uncollapse the prop dlg area if necessary
    if ( m_pPropWidgetStack->count() > 0 && m_pPropWidgetStack->currentWidget() == m_aItems[i]->getPropWidget() )
    {
        if (bPendingUncollapse)
            m_pWindow->setPropDlgAreaVisible(true);

        m_pCurrentPropDialog = m_aItems[i]->getPropWidget();
        m_iCurrentPropDialog = m_pPropWidgetStack->indexOf(m_pCurrentPropDialog);

    	return;
    }


    // get pointer to the document's property widget
    m_pCurrentPropDialog = m_aItems[i]->createPropWidget();

    if (m_pCurrentPropDialog != NULL)
    {
        // check if the prop dlg widget already exists in the stack
	int iIndex = m_pPropWidgetStack->indexOf(m_pCurrentPropDialog);

        // if not, create the prop dlg widget
	if ( iIndex == -1 )
	    iIndex = m_pPropWidgetStack->addWidget(m_pCurrentPropDialog);

        // pull the dialog to the front of the stack
        m_pPropWidgetStack->setCurrentIndex(iIndex);

        // remember the current index
	m_iCurrentPropDialog = iIndex;

        // uncollapse the prop dlg area if necessary
        if (bPendingUncollapse)
            m_pWindow->setPropDlgAreaVisible(true);
    }
    else
    {
        // the document does not have a prop dlg widget. collapse the prop dlg area and remember
        // this by setting m_iCurrentPropDialog = -1
	m_pWindow->setPropDlgAreaVisible(false);
        m_iCurrentPropDialog = -1;
    }
}

/*
 *
 */
void window_documentlist::hidePropDlg(bool bHide)
{
    m_bHidePropDialog = bHide;
}


/*
 *
 */
vector< sculptor_document* > window_documentlist::getDocuments( bool bSel, int eType )
{
    int i;
    vector< sculptor_document* > aDocs;
    sculptor_document* pDoc;

    // no specific type is requested: check only if document should be and is selected
    if ( eType == -1 )
    {
	for ( i=1; i<count(); ++i)
	{
	    pDoc = getDoc(i);

	    if ( !bSel || getSelected(i) )
		aDocs.push_back(getDoc(i));
	}
    }
    // check for document type  and  if document should be and is selected
    else
    {
	for ( i=0; i<count(); ++i)
	{
	    pDoc = getDoc(i);

	    if ( pDoc->getType() == eType && (!bSel || getSelected(i)) )
		aDocs.push_back(getDoc(i));
	}
    }

    return aDocs;
}

/*
 *
 */
void window_documentlist::setVisible(int iIndex, bool bVisible)
{
    m_aItems[iIndex]->setVisible(bVisible);
    m_pWindow->updateMenuItems();
}

/*
 *
 */
void window_documentlist::currentVolume_showTable()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->showTable();
}

/*
 *
 */
void window_documentlist::currentVolume_mapExplorer()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->createMapExplorer();
}

/*
 *
 */
void window_documentlist::currentVolume_showHistogramDlg()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->showHistogramDlg();
}

/*
 *
 */
void window_documentlist::currentVolume_calcOccupied()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->calcOccupied();
}

/*
 *
 */
void window_documentlist::currentVolume_setDockToLaplace()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->setDockToLaplace(m_pWindow->getDockToLaplace());
}

/*
 *
 */
void window_documentlist::currentVolume_subtractMap()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->subtractMap();
}

/*
 *
 */
void window_documentlist::currentVolume_changeVoxelWidth()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->changeVoxelWidth();
}

/*
 *
 */
void window_documentlist::currentVolume_changeOrigin()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->changeOrigin();
}

/*
 *
 */
void window_documentlist::currentVolume_swapAxis()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->swapaxis();
}

/*
 *
 */
void window_documentlist::currentVolume_normalize()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->normalize();
}

/*
 *
 */
void window_documentlist::currentVolume_threshold()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->threshold();
}

/*
 *
 */
void window_documentlist::currentVolume_scale()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->scale();
}

/*
 *
 */
void window_documentlist::currentVolume_crop()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->crop();
}

/*
 *
 */
void window_documentlist::currentVolume_floodfill()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->floodfill();
}

/*
 *
 */
void window_documentlist::currentVolume_multiPointFloodfill()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->multiPointFloodfill();
}

/*
 *
 */
void window_documentlist::currentVolume_gaussian()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->gaussian();
}

/*
 *
 */
void window_documentlist::currentVolume_laplacian()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->laplacian();
}

/*
 *
 */
void window_documentlist::currentVolume_localNorm()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->localNorm();
}

/*
 *
 */
void window_documentlist::currentVolume_bilateralFilter()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->bilateralFilter();
}

/*
 *
 */
void window_documentlist::currentVolume_DPSV_filter()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->DPSV_filter();
}

/*
 *
 */
void window_documentlist::currentVolume_loadSegmentation()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->loadSegmentation();
}

/*
 *
 */
void window_documentlist::currentVolume_showSegmentation()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->showSegmentation();
}

/*
 *
 */
void window_documentlist::currentVolume_saveSegmentation()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->saveSegmentation();
}

/*
 *
 */
void window_documentlist::currentVolume_setSegment()
{
    if ( !isSitus() )
	return;
    situs_document* pSitus = (situs_document*) getCurrentItem()->getDoc();
    pSitus->setSegment();
}

/*
 *
 */
void window_documentlist::currentPDB_showTable()
{
    if ( !isPDB() )
	return;
    pdb_document* pPDB = (pdb_document*) getCurrentItem()->getDoc();
    pPDB->showTable();
}

/*
 *
 */
void window_documentlist::currentPDB_showSphericity()
{
    if ( !isPDB() )
	return;
    pdb_document* pPDB = (pdb_document*) getCurrentItem()->getDoc();
    pPDB->showSphericity();
}

/*
 *
 */
void window_documentlist::currentPDB_blur()
{
    if ( !isPDB() )
	return;
    pdb_document* pPDB = (pdb_document*) getCurrentItem()->getDoc();
    pPDB->blur();
}

/*
 *
 */
void window_documentlist::currentPDB_showExtractDlg()
{
    if ( !isPDB() )
	return;
    pdb_document* pPDB = (pdb_document*) getCurrentItem()->getDoc();
    pPDB->showExtractDlg();
}

/*
 *
 */
void window_documentlist::currentPDB_showSymmetryDlg()
{
    if ( !isPDB() )
	return;
    pdb_document* pPDB = (pdb_document*) getCurrentItem()->getDoc();
    pPDB->showSymmetryDlg();
}
