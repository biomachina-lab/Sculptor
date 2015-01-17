/***************************************************************************
                          sculptor_file_lbi.h  -  description
                             -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SENSITUS_FILE_LBI_H
#define SENSITUS_FILE_LBI_H

#include <qlistbox.h>
#include <qpainter.h>
#include <qpixmap.h>

class sculptor_document;

/** This class provides a Qt listboxitem (derived from QListBoxItem) for the sculptor file list.
 * @author Stefan Birmanns
 */
class sculptor_file_lbi : public QListBoxItem
{
private:

    QPixmap m_cClipMediumPM;
    QPixmap m_cEraserMediumPM;
    QPixmap m_cMarkerPM;
    QPixmap m_cEyeMediumPM;
    QPixmap m_cEyeMediumDisablePM;
    QPixmap m_cMolBwGridMediumPM;
    QPixmap m_cMolBwMediumPM;
    QPixmap m_cMolGreenGridMediumPM;
    QPixmap m_cMolGreenMediumPM;
    QPixmap m_cMolOrangeGridMediumPM;
    QPixmap m_cMolOrangeMediumPM;
    QPixmap m_cMoveClashingMediumPM;
    QPixmap m_cMoveIGDMediumPM;
    QPixmap m_cMoveMediumPM;
    QPixmap m_cMoveMediumLockedPM;
    QPixmap m_cVolBwGridMediumPM;
    QPixmap m_cVolBwMediumPM;
    QPixmap m_cVolGreenGridMediumPM;
    QPixmap m_cVolGreenMediumPM;
    QPixmap m_cVolOrangeGridMediumPM;
    QPixmap m_cVolOrangeMediumPM;
    QPixmap m_cWorldMediumPM;
    QPixmap m_cLockedMediumPM;

    bool m_bVisible;
    bool m_bMove;
    bool m_bTarget;
    bool m_bDock;
    bool m_bSolution;

    // maximum height
    int m_iMaxHeight;
    // width of the complete listboxitem
    int m_iWidth;
    // starting position of the text
    int m_iTextXPos;
    // position of the visibility icon
    int m_iVisibleXPos;
    // position of the move icon
    int m_iMoveXPos;

    // pointer to sculptor document
    sculptor_document* m_pDoc;


public:
    /**
     * Constructor
     * \param pText the text (usually the filename of the document)
     * \param bVisible visiblility (default: true)
     * \param bMove move status (default: true)
     * \param bTarget target status (default: false)
     * \param bDock dock status (default: false)
     * \param bSolution solution status (default: false)
     */
    sculptor_file_lbi( const char *pText, bool bVisible =true, bool bMove =true, bool bTarget =false, bool bDock =false, bool bSolution =false, sculptor_document* pDoc =NULL);

    /**
     * set the document visibility
     * \param bVisible true if the document should be visible
     */
    void setVisible( bool bVisible );
    /**
     * get the document visibility
     * \return true if the document is visible
     */
    bool getVisible( );

    /**
     * set the move status
     * \param bMove true if the document can be moved around
     */
    void setMove( bool bMove );
    /**
     * get the move status
     * \return true if the document can be moved around
     */
    bool getMove( );

    /**
     * set the target status
     * \param bTarget true if the document belongs to a set of documents, with which the force for a force-feedback device should be calculated
     */
    void setTarget( bool bTarget );
    /**
     * get the target status
     * \return true if the document belongs to a set of documents, with which the force for a force-feedback device should be calculated
     */
    bool getTarget( );
    /**
     * set the dock status
     * \param bDock true if the document belongs to a set of documents, with which the force for a forcefeedback device should be calculated
     */
    void setDock( bool bDock );
    /**
     * get the dock status
     * \return true if the document belongs to a set of documents, with which the force for a forcefeedback device should be calculated
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
     * get the position inside the listboxitem, where the text begins
     * \return x pos of text
     */
    int getTextXPos();
    /**
     * get the position inside the listboxitem, where the icon pixmap begins
     * \return x pos of doc icon
     */
    int getIconXPos();
    /**
     * get the position inside the listboxitem, where the visibility pixmap begins
     * \return x pos of visibility pixmap
     */
    int getVisibleXPos();
    /**
     * get the position inside the listboxitem, where the move pixmap begins
     * \return x pos of the move pixmap
     */
    int getMoveXPos();
    /**
     * set the text in the listboxitem
     * \param pText new text for the listbox item
     */
    virtual void setText(const QString& oText);

    /**
     * Set a link to the document the listboxitem represents
     * \param pDoc pointer to sculptor_document
     */
    void setDocument( sculptor_document* pDoc );

protected:
    /**
     * paint the listboxitem
     * \param pPainter pointer to QPainter object
     */
    virtual void paint( QPainter* pPainter);

    /**
     * get the height of the listboxitem
     * \return the height of the listboxitem
     */
    virtual int height( const QListBox * ) const;
    /**
     * get the width of the listboxitem
     * \return the width of the listboxitem
     */
    virtual int width( const QListBox * ) const;

    /**
     * calculate the positions for all the icons and the text
     */
    void recalcSize();
};

#endif
