/*-*-c++-*-*****************************************************************
			  sculptor_window_documentlist
			  ------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SCULPTOR_WINDOW_DOCUMENTLIST_H
#define SCULPTOR_WINDOW_DOCUMENTLIST_H

// uic-qt4
#include <ui_window_documentlist.h>
// sculptor
class sculptor_window;
#include <sculptor_document.h>
class window_documentlist;
#include <sculptor_window_documentlistitem.h>
// svt
#include <svt_stlVector.h>
// qt4
class QMouseEvent;
class QStackedLayout;


///////////////////////////////////////////////////////////////////////////////
// mousefilter class to orchestrate mouse clicks on the documentlist
///////////////////////////////////////////////////////////////////////////////


/**
 * This class is a QObject that implements an eventFilter method. It is the mouse button filter for
 * the doucmentlist.
 * \author Manuel Wahle
 */
class window_documentlist_mousefilter : public QObject
{
    Q_OBJECT

    // this is the documentlist, whose methods for left, right, and double click this mousefilter calls
    window_documentlist* m_pDL;
    // this is the documentlist's table to get the cell coordinates from the position of a mouse click
    QTableWidget* m_pDLTable;
    // this is the documentlist viewport whose events should be filtered
    QObject *m_pDLViewport;

public:

    /**
     * Constructor
     */
    window_documentlist_mousefilter(window_documentlist* pDocumentlist, QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~window_documentlist_mousefilter()
    {};

    /**
     * this is the virtual method from QObject where the event filter must be implemented
     */
    bool eventFilter(QObject *pObject, QEvent *pEvent);
};


///////////////////////////////////////////////////////////////////////////////
// the documentlist
///////////////////////////////////////////////////////////////////////////////


/**
 * \brief This is the documentlist. It manages a list of sculptor_document of different type.
 *
 * It provides functions to manage the documents, like selecting them or setting the internal
 * document's state. It also displays these states in the list.
 * \author Manuel Wahle
 */
class window_documentlist : public QWidget, public Ui::window_documentlist
{
    Q_OBJECT

    // pointer to the main window
    sculptor_window * m_pWindow;

    // pointer to the widget where the document prop dialogs should be displayed. set from outside
    QWidget* m_pPropDialogArea;
    // a stack layout to put into the prop dialog area to switch between the dialogs
    QStackedLayout* m_pPropWidgetStack;
    // pointer to the widget with the currently displayed prop dialog
    QWidget* m_pCurrentPropDialog;
    /**
     * index in the stackedlayout of the current prop dialog. setting this to -1 means that the
     * collapse was not user requested, so it will be automatically uncollapsed if a document with a
     * prop dlg is left clicked or added
     */
    int m_iCurrentPropDialog;
    // as long as this bool is true, the prop dlg area should not be uncollapsed
    bool m_bHidePropDialog;

    // the mousefilter to be installed in the canvas of the document qtablewidget
    window_documentlist_mousefilter * m_pMousefilter;

    // a vector holding pointers to all the documentlistitems
    vector<window_documentlistitem*> m_aItems;

public:

    /**
     * internal method to check if the document index i is a situs doc. if no index given, check
     * the current document
     */
    inline bool isSitus(const int i=-1)
    {
	if (i == -1)
	    return ( getCurrentItem()->getDoc()->getType() == SENSITUS_DOC_SITUS );
	else
	    return ( m_aItems[i]->getDoc()->getType() == SENSITUS_DOC_SITUS );
    };

    /**
     * internal method to check if the document index i is a situs doc. if no index given, check
     * the current document
     */
    inline bool isPDB(const int i=-1)
    {
	if (i == -1)
	    return ( getCurrentItem()->getDoc()->getType() == SENSITUS_DOC_PDB );
	else
	    return ( m_aItems[i]->getDoc()->getType() == SENSITUS_DOC_PDB );
    };

    /**
     * Constructor
     */
    window_documentlist(sculptor_window * pWindow, QWidget *pParent = NULL);

    /**
     * Destructor
     */
    virtual ~window_documentlist() {};

    /**
     * \name Methods related to document list management
     */
    //@{
    int  count();
    int  addItem(window_documentlistitem* pItem);
    void insertItem(window_documentlistitem* pItem, int i = -1);
    void removeItem(const int iIndex);
    bool moveItemDown(const int iIndex);
    bool moveItemUp(const int iIndex);
    bool moveItemUpmost(const int iIndex);
    window_documentlistitem* operator[](int iIndex);
    //@}

    /**
     * \name Methods related to document selection management
     */
    //@{
    void clearSelection();
    void setSelected(const int iIndex, bool bSelected = true);
    bool getSelected(const int iIndex);
    int  getCurrentIndex();// attention: return -1 if no current item
    window_documentlistitem* getCurrentItem();
    void setCurrentItem(const int iIndex);
    //@}

    /**
     * set document iIndex visible
     * \param int index
     * \param bool show or hide
     */
    void setVisible(int iIndex, bool bVisible = true);

    /**
     * is document iIndex visible?
     * \param iIndex
     */
    inline bool getVisible(const int iIndex)
    {
	if ( iIndex<0 || iIndex >= count() )
	{
	    //DEBUG//printf("DL::getVisible %i out of range\n", iIndex);
	    return false;
	}
	return m_aItems[iIndex]->getVisible();
    };

    /**
     * the documentlist needs a pointer to a widget where it can put the stacked layout that will
     * hold the prop dialogs. this may be changed later to let the dialogs float around in their own
     * windows
     */
    void setPropDialogArea(QWidget* pWidget);

    /*
     * return a vector with pointers to the documents themselves.  this vector is passed by value
     * (copied): 1. ensures programmer friendly memory management, 2. any tampering with the
     * document list is avoided
     * \param bSelected if true, only selected documents are returned. default: all documents
     * \param eType a specific sculptor document type can be requested. default: all documents
     */
    vector< sculptor_document* > getDocuments( bool bSelected = false, const int eType = -1 );


    /**
     * is document iIndex visible?
     * \param iIndex
     */
    inline bool getLocked(const int iIndex)
    {
	if ( iIndex<0 || iIndex >= count() )
	{
	    //DEBUG//printf("DL::isLocked %i out of range\n", iIndex);
	    return false;
	}
	return m_aItems[iIndex]->getLocked();
    };

    /**
     * is document iIndex the moved doc?
     * \param iIndex
     */
    inline bool getMoved(const int iIndex)
    {
	if ( iIndex<0 || iIndex >= count() )
	{
	    //DEBUG//printf("DL::getMoved %i out of range\n", iIndex);
	    return false;
	}
	return m_aItems[iIndex]->getMoved();
    };

    /**
     * is document iIndex a solution?
     * \param iIndex
     */
    inline bool getSolution(const int iIndex)
    {
	if ( iIndex<0 || iIndex >= count() )
	{
	    //DEBUG//printf("DL::isSolution %i out of range\n", iIndex);
	    return false;
	}
	return m_aItems[iIndex]->getSolution();
    };

    /**
     * get the document with the resp. index
     * \param iIndex
     * \return sculptor_document*
     */
    inline sculptor_document* getDoc(const int iIndex)
    {
	if ( iIndex<0 || iIndex >= count() )
	{
	    //DEBUG//printf("DL::getDoc %i out of range\n", iIndex);
	    return NULL;
	}
	return m_aItems[iIndex]->getDoc();
    };

    /**
     * get the type of the document  with the resp. index
     * \param iIndex
     * \return int type enum
     */
    inline int getDocType(const int iIndex)
    {
	if ( iIndex<0 || iIndex >= count() )
	{
	    //DEBUG//printf("DL::getDocType %i out of range\n", iIndex);
	    return -1;
	}
	return m_aItems[iIndex]->getDoc()->getType();
    };

public slots:

    /**
     * slot called to put a left click onto the table into a certain cell. usually called from the
     * mousefilter
     * \param iRow int with the row index
     * \param iColumn int with the row index
     */
    void leftClicked(int iRow, int iColumn);

    /**
     * slot called to put a right click onto the table into a certain cell. usually called from the
     * mousefilter
     * \param iRow int with the row index
     * \param iColumn int with the row index
     */
    void rightClicked(QPoint pPoint, int iRow, int iColumn);
    /**
     * slot called to put a double onto the table into a certain cell. usually called from the
     * mousefilter
     * \param iRow int with the row index
     * \param iColumn int with the row index
     */
    void doubleClicked(int iRow, int iColumn);

    /**
     * this slot calls the property dialog of the document with index i, or, if no index given, of
     * the current document
     */
    void showPropDialog(int i=-1);

    /**
     * this method is NOT the opposite of showPropDlg(). instead, it is used to set an internal
     * state that indicates that the user requested that the prop dlg area is collpased. in this
     * case, the documentlist will NOT uincollapse the prop dlg area until the user requests it.
     */
    void hidePropDlg(bool bHide);


    /**
     * \name slots to be called from the menu and toolbar action for situs documents
     */
    //@{
    void currentVolume_showTable();
    void currentVolume_mapExplorer();
    void currentVolume_showHistogramDlg();
    void currentVolume_calcOccupied();
    void currentVolume_setDockToLaplace();
    void currentVolume_subtractMap();
    void currentVolume_changeVoxelWidth();
    void currentVolume_changeOrigin();
    void currentVolume_swapAxis();
    void currentVolume_normalize();
    void currentVolume_threshold();
    void currentVolume_scale();
    void currentVolume_crop();
    void currentVolume_floodfill();
    void currentVolume_multiPointFloodfill();
    void currentVolume_gaussian();
    void currentVolume_laplacian();
    void currentVolume_localNorm();
    void currentVolume_bilateralFilter();
    void currentVolume_DPSV_filter();
    void currentVolume_loadSegmentation();
    void currentVolume_showSegmentation();
    void currentVolume_saveSegmentation();
    void currentVolume_setSegment();
    //@}

    /**
     * \name slots to be called from the menu and toolbar action for pdb documents
     */
    //@{
    void currentPDB_showTable();
    void currentPDB_showSphericity();
    void currentPDB_blur();
    void currentPDB_showExtractDlg();
    void currentPDB_showSymmetryDlg();
    //@}

private slots:

    /**
     * internal slot called from qwidget when the selection has changed. sets the selected state of
     * the docuemtlistitems and calls onSelection and onDeselection, if necessary
     */
    virtual void selectionChanged ();

};

#endif // SCULPTOR_WINDOW_DOCUMENTLIST_H
