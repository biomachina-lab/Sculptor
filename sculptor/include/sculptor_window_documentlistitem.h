/*-*-c++-*-*****************************************************************
			  sculptor_window_documentlistitem
			  ------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SCULPTOR_WINDOW_DOCUMENTLISTITEM_H
#define SCULPTOR_WINDOW_DOCUMENTLISTITEM_H


// sculptor includes
class sculptor_document;
// qt4 includes
#include <QObject>
class QTableWidgetItem;
class QIcon;
class QBrush;

class window_documentlistitem : public QObject
{
    Q_OBJECT

    // when the last instance of a window_documentlistitem is deleted, also delete the icons, so
    // this static int counts the number of instances
    static int m_iNumInstances;

    // all the icons for the documents states
    static QIcon * m_pClipMedium;
    static QIcon * m_pEyeMedium;
    static QIcon * m_pEyeMediumDisable;
    static QIcon * m_pMolBwGridMedium;
    static QIcon * m_pMolBwMedium;
    static QIcon * m_pMolGreenGridMedium;
    static QIcon * m_pMolGreenMedium;
    static QIcon * m_pMolOrangeGridMedium;
    static QIcon * m_pMolOrangeMedium;
    static QIcon * m_pMoveClashingMedium;
    static QIcon * m_pMoveIGDMedium;
    static QIcon * m_pMoveMedium;
    static QIcon * m_pMoveMediumLocked;
    static QIcon * m_pVolBwGridMedium;
    static QIcon * m_pVolBwMedium;
    static QIcon * m_pVolGreenGridMedium;
    static QIcon * m_pVolGreenMedium;
    static QIcon * m_pVolOrangeGridMedium;
    static QIcon * m_pVolOrangeMedium;
    static QIcon * m_pWorldMedium;
    static QIcon * m_pLockedMedium;
    static QIcon * m_pEraserMedium;
    static QIcon * m_pMarker;
    // brush for the cell background to show document color
    static QBrush* m_pColorRectBrush;

    // bools to track the document states
    bool m_bVisible;
    bool m_bMoved;
    bool m_bTarget;
    bool m_bDock;
    bool m_bSolution;
    bool m_bSelected;

    // bool to set if a document's color should be shown
    bool m_bShowColor;

    // pointer to sculptor document
    sculptor_document* m_pDoc;

    // pointer to a widget that acts as container for the document's property dialog
    QWidget* m_pPropWidget;

    // the qtablewidgetitems for interaction with the the document
    QTableWidgetItem * m_pCell_Dock;
    QTableWidgetItem * m_pCell_Visible;
    QTableWidgetItem * m_pCell_Moved;
    QTableWidgetItem * m_pCell_Name;

public:

    /**
     * Constructor
     * \param pDoc pointer to the represented sculptor_document
     * \param bVisible visibility (default: true)
     * \param bMoved move status (default: true)
     * \param bTarget target status (default: false)
     * \param bDock dock status (default: false)
     * \param bSolution solution status (default: false)
     */
    window_documentlistitem( sculptor_document* pDoc = NULL,
			     bool bVisible = true,
			     bool bMoved = false,
			     bool bTarget = false,
			     bool bDock = false,
			     bool bSolution = false);

    /**
     * Destructor
     */
    virtual ~window_documentlistitem();

    /**
     * \name Documentlistitem Management
     */
    //@{

    /**
     * return a pointer to the widget containing the property widget. does not create the dialog if
     * it doesn't exist
     */
    QWidget* getPropWidget();

    /**
     * return a pointer to the widget containing the property widget. asks the sculptor_document to
     * create a property dialog, and if so, puts the property dialog into a scrollarea and returns
     * that
     */
    QWidget* createPropWidget();

    /**
     * return the document item's index in the documentlist
     */
    int getIndex();
    //@}

    /**
     * \name Cell Management
     * Get pointers to the cells that show the document states. Needed by the documentlist to put
     * the cells into the list.
     */
    //@{

    /**
     * return cell that shows dock state
     */
    QTableWidgetItem* getCell_Dock();

    /**
     * return cell that shows visibility state
     */
    QTableWidgetItem* getCell_Visible();

    /**
     * return cell that shows moved state
     */
    QTableWidgetItem* getCell_Moved();

    /**
     * return cell that shows document name
     */
    QTableWidgetItem* getCell_Name();
    //@}

    /**
     * \name Document Interaction
     * Methods for interaction with the document
     */
    //@{

    /**
     * return a pointer to the document
     */
    sculptor_document* getDoc();

    /**
     * set the document (normally not needed)
     */
    void setDoc(sculptor_document* pDoc);

    /**
     * return the document type
     */
    int getDocType();

    /**
     * indicate with a prepended * in the name if the document changed. this does not affect the
     * state in the document, it just queries document's state and displays this fact. acutally,
     * this is just a convenience function for convenience and code clarity
     */
    void showChanged();

    /**
     * if a document's color should be shown, call this function
     */
    void showColor(bool bShowColor = true);

    /**
     * show the name of the document
     */
    void showDocName();

    /**
     * set the selection status
     * \return true if the documentlistitem should be selected
     */
    void setSelected(bool bSelected = true);

    /**
     * get the selection status
     * \return true if this documentlistitem is selected
     */
    bool getSelected();

    /**
     * if the document was selected, this method checks if the document's internal state still has
     * to be set to selected. this helps to avoid calling the documents onSelection() method when it
     * is not appropriate
     * \return true if this document's onSelection() method needs to be called
     */
    bool getPendingSelection();

    /**
     * if the document was deselected, this method checks if the document's internal state still has
     * to be set to deselected. this helps to avoid calling the documents onDeselection() method
     * when it is not appropriate
     * \return true if this document's onDeselection() method needs to be called
     */
    bool getPendingDeselection();
    //@}

    /**
     * \name Document States
     * Methods for document state management
     */
    //@{

    /**
     * set the document visibility: check if visibility has actually changed, and if so, calls
     * setVisible_DOIT( )
     * \param bVisible true if the document should be visible
     */
    void setVisible( bool bVisible );

    /**
     * get the document visibility
     * \return true if the document is visible
     */
    bool getVisible( );

    /**
     * set the moved status: check if moved has actually changed, and if so, call setMoved_DOIT( )
     * \param bMoved true if the document can be moved around
     */
    void setMoved( bool bMoved );

    /**
     * get the moved status
     * \return true if the document can be moved around
     */
    bool getMoved( );

    /**
     * show the moved status. used to update the move icon display
     */
    void showMoved();

    /**
     * set the target status
     * \param bTarget true if the document belongs to a set of documents, with which the force for a
     * force-feedback device should be calculated
     */
    void setTarget( bool bTarget );

    /**
     * get the target status
     * \return true if the document belongs to a set of documents, with which the force for a
     * force-feedback device should be calculated
     */
    bool getTarget( );

    /**
     * set the dock status: check if it has actually changed, and if so, call setDock_DOIT( )
     * \param bDock true if the document belongs to a set of documents, with which the force for a
     * forcefeedback device should be calculated
     */
    void setDock( bool bDock );

    /**
     * get the dock status
     * \return true if the document belongs to a set of documents, with which the force for a
     * forcefeedback device should be calculated
     */
    bool getDock( );

    /**
     * set the solution status
     * \param bSolution true if the document solution candidate
     */
    void setSolution( bool bSolution );

    /**
     * get the solution status
     * \return true if the document is a solution candidate
     */
    bool getSolution( );

    /**
     * set the locked status
     * \param true if the document is to be locked
     */
    void setLocked(bool bLocked);

    /**
     * get the lock status
     * \return true if the document is locked
     */
    bool getLocked();

    /**
     * show the lock status
     */
    void showLocked();

protected:

    /**
     * actually set the visibility status according to the internal boolean. only for internal use
     */
    void setVisible_DOIT( );

    /**
     * actually set the moved status according to the internal boolean. only for internal use
     */
    void setMoved_DOIT( );

    /**
     * actually set the dock status according to the internal booleans. only for internal use
     */
    void setDock_DOIT( );
    //@}
};

#endif
