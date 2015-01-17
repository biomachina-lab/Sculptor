/***************************************************************************
                          sculptor_file_lbi.cpp  -  description
                             -------------------
    begin                : 17.08.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_file_lbi.h>
#include <sculptor_document.h>
#include <sculptor_window.h>

// svt includes
#include <svt_file_utils.h>

// Medium-size icon includes
#include <clip-medium.xpm>
#include <eraser-medium.xpm>
#include <marker.xpm>
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

// qt includes
#include <qpainter.h>
#include <qcolor.h>
#include <qlistbox.h>
#include <qstyle.h>
#include <qapplication.h>
#include <qpalette.h>

extern sculptor_window* g_pWindow;

/**
 * Constructor
 * \param pText the text (usually the filename of the document)
 * \param bVisible visiblility (default: true)
 * \param bMove move status (default: true)
 * \param bTarget target status (default: false)
 * \param bDock dock status (default: false)
 * \param bSolution solution status (default: false)
 */
sculptor_file_lbi::sculptor_file_lbi( const char *pText, bool bVisible, bool bMove, bool bTarget, bool bDock, bool bSolution, sculptor_document* pDoc ) :
    QListBoxItem(),
    m_cClipMediumPM(clip_medium),
    m_cEraserMediumPM(eraser_medium),
    m_cMarkerPM(marker),
    m_cEyeMediumPM(eye_medium),
    m_cEyeMediumDisablePM(eye_medium_disable),
    m_cMolBwGridMediumPM(mol_bw_grid_medium),
    m_cMolBwMediumPM(mol_bw_medium),
    m_cMolGreenGridMediumPM(mol_green_grid_medium),
    m_cMolGreenMediumPM(mol_green_medium),
    m_cMolOrangeGridMediumPM(mol_orange_grid_medium),
    m_cMolOrangeMediumPM(mol_orange_medium),
    m_cMoveClashingMediumPM(move_clashing_medium),
    m_cMoveIGDMediumPM(move_igd_medium),
    m_cMoveMediumPM(move_medium),
    m_cMoveMediumLockedPM(move_medium_locked),
    m_cVolBwGridMediumPM(vol_bw_grid_medium),
    m_cVolBwMediumPM(vol_bw_medium),
    m_cVolGreenGridMediumPM(vol_green_grid_medium),
    m_cVolGreenMediumPM(vol_green_medium),
    m_cVolOrangeGridMediumPM(vol_orange_grid_medium),
    m_cVolOrangeMediumPM(vol_orange_medium),
    m_cWorldMediumPM(world_medium),
    m_cLockedMediumPM(locked_medium),
    m_pDoc(pDoc)

{
    char pTemp[1256];
    char pFname[1256];
    strcpy(pTemp, pText);
    svt_stripPath(pTemp, pFname);

    setText( QString(pFname) );

    recalcSize();

    m_bVisible = bVisible;
    m_bMove = bMove;
    m_bTarget = bTarget;
    m_bDock = bDock;
    m_bSolution = bSolution;

    setCustomHighlighting( TRUE );
}

/**
 * set the document visibility
 * \param bVisible true if the document should be visible
 */
void sculptor_file_lbi::setVisible( bool bVisible )
{
    m_bVisible = bVisible;

    // repaint the listbox
    if (listBox() != NULL) listBox()->triggerUpdate(true);
}
/**
 * get the document visibility
 * \return true if the document is visible
 */
bool sculptor_file_lbi::getVisible( )
{
    return m_bVisible;
}

/**
 * set the move status
 * \param bMove true if the document can be moved around
 */
void sculptor_file_lbi::setMove( bool bMove )
{
    m_bMove = bMove;

    // repaint the listbox
    if (listBox() != NULL) listBox()->triggerUpdate(true);
}
/**
 * get the move status
 * \return true if the document can be moved around
 */
bool sculptor_file_lbi::getMove( )
{
    return m_bMove;
}

/**
 * set the target status
 * \param bTarget true if the document belongs to a set of documents, with which the force for a forcefeedback device should be calculated
 */
void sculptor_file_lbi::setTarget( bool bTarget )
{
    m_bTarget = bTarget;

    // repaint the listbox
    if (listBox() != NULL) listBox()->triggerUpdate(true);
}
/**
 * get the target status
 * \return true if the document belongs to a set of documents, with which the force for a forcefeedback device should be calculated
 */
bool sculptor_file_lbi::getTarget( )
{
    return m_bTarget;
}
/**
 * set the dock status
 * \param bDock true if the document belongs to a set of documents, with which the force for a forcefeedback device should be calculated
 */
void sculptor_file_lbi::setDock( bool bDock )
{
    m_bDock = bDock;

    // repaint the listbox
    if (listBox() != NULL) listBox()->triggerUpdate(true);
}
/**
 * get the dock status
 * \return true if the document belongs to a set of documents, with which the force for a forcefeedback device should be calculated
 */
bool sculptor_file_lbi::getDock( )
{
    return m_bDock;
}

/**
 * set the solution status
 * \param bSolution true if the document solution candidate
 */
void sculptor_file_lbi::setSolution( bool bSolution )
{
    m_bSolution = bSolution;

    // repaint the listbox
    if (listBox() != NULL) listBox()->triggerUpdate(true);
}
/**
 * get the solution status
 * \return true if the document is a solution candidate
 */
bool sculptor_file_lbi::getSolution( )
{
    return m_bSolution;
}

/**
 * get the position inside the listboxitem, where the text begins
 * \return x pos of text
 */
int sculptor_file_lbi::getTextXPos()
{
    return m_iTextXPos;
}

/**
 * get the position inside the listboxitem, where the icon pixmap begins
 * \return x pos of doc icon
 */
int sculptor_file_lbi::getIconXPos()
{
    return 0;
}
/**
 * get the position inside the listboxitem, where the visibility pixmap begins
 * \return x pos of visibility pixmap
 */
int sculptor_file_lbi::getVisibleXPos()
{
    return m_iVisibleXPos;
}
/**
 * get the position inside the listboxitem, where the move pixmap begins
 * \return x pos of the move pixmap
 */
int sculptor_file_lbi::getMoveXPos()
{
    return m_iMoveXPos;
}

/**
 * paint the listboxitem
 * \param pPainter pointer to QPainter object
 */
void sculptor_file_lbi::paint( QPainter* pPainter)
{
    QRect oItemRect = this->listBox()->itemRect( this );
    bool bScene = this->listBox()->item(0) == this;

    QString oText = text();
    if (m_pDoc->isChanged() && m_pDoc->getType() != SENSITUS_DOC_SCENE)
        oText = QString("* ") + oText;

    // compute width
    m_iWidth = 0;
    m_iWidth += m_iTextXPos;
    m_iWidth += listBox()->fontMetrics().width( oText );
    m_iWidth += 6;

    // is the item selected?
    if ( isCurrent() || isSelected() )
        pPainter->fillRect( m_iTextXPos, 0, width( listBox() ) - m_iTextXPos + 1 , height( listBox() ) , listBox()->colorGroup().highlight() );

    // has the item focus?
    if ( isCurrent() )
    {
        QRect r( m_iTextXPos+1, 1, width( listBox() ) - m_iTextXPos-1, height( listBox() )-2 );
        listBox()->style().drawPrimitive( QStyle::PE_FocusRect, pPainter, r, listBox()->colorGroup(), QStyle::Style_Default, QStyleOption ( listBox()->colorGroup().highlight() ) );
    }

    //
    // Draw a filled rectangle where the icons will go
    //
    pPainter->fillRect( 0, 0, 3*24+3, 24, listBox()->colorGroup().button() );

    //
    // Draw document type pixmaps
    //
    QRect oRect_DocType( 0, oItemRect.top(), 24,24 );
    if (m_pDoc->getType() == SENSITUS_DOC_SCENE)
    {
        pPainter->drawPixmap( 0, 0, m_cWorldMediumPM );
        QToolTip::add( this->listBox(), oRect_DocType, "This is the Sculptor scene document." );
    }
    if (m_pDoc->getType() == SENSITUS_DOC_PDB)
    {
        QToolTip::add( this->listBox(), oRect_DocType, "This is an atomic model." );

        if (m_bDock)
            if (m_pDoc->isEliDataLoaded() || (m_pDoc->getOrigProbeDoc() && m_pDoc->getOrigProbeDoc()->isEliDataLoaded()))
            {
                pPainter->drawPixmap( 0, 0, m_cMolOrangeGridMediumPM );
                QToolTip::add( this->listBox(), oRect_DocType, "This is the atomic model to be docked into the map (IGD data attached)." );

            } else {

                pPainter->drawPixmap( 0, 0, m_cMolOrangeMediumPM );
                QToolTip::add( this->listBox(), oRect_DocType, "This is the atomic model to be docked into the map." );

            }
        else
	{
            if (m_bSolution)
            {
		if (    m_pDoc->isEliDataLoaded()
                    || (m_pDoc->getOrigProbeDoc() && m_pDoc->getOrigProbeDoc()->isEliDataLoaded()))
                {
		    pPainter->drawPixmap( 0, 0, m_cMolGreenGridMediumPM );
                    QToolTip::add( this->listBox(), oRect_DocType, "This is an atomic model (solution candidate, IGD data attached)." );
                } else {
		    pPainter->drawPixmap( 0, 0, m_cMolGreenMediumPM );
                    QToolTip::add( this->listBox(), oRect_DocType, "This is an atomic model (solution candidate)." );
                }
            } else
		if (    m_pDoc->isEliDataLoaded()
                    || (m_pDoc->getOrigProbeDoc() && m_pDoc->getOrigProbeDoc()->isEliDataLoaded()))
                {
		    pPainter->drawPixmap( 0, 0, m_cMolBwGridMediumPM );
                    QToolTip::add( this->listBox(), oRect_DocType, "This is an atomic model (IGD data attached)." );
                } else {
                    pPainter->drawPixmap( 0, 0, m_cMolBwMediumPM );
                }
        }
    }
    if (m_pDoc->getType() == SENSITUS_DOC_SITUS)
    {
        QToolTip::add( this->listBox(), oRect_DocType, "This is a volumetric map." );

        if (m_bTarget)
        {
            pPainter->drawPixmap( 0, 0, m_cVolOrangeMediumPM );
            QToolTip::add( this->listBox(), oRect_DocType, "This is the target map." );

        } else
            if (m_bSolution)
		pPainter->drawPixmap( 0, 0, m_cVolGreenMediumPM );
            else
                pPainter->drawPixmap( 0, 0, m_cVolBwMediumPM );
    }
    if (m_pDoc->getType() == SENSITUS_DOC_CLIP)
    {
	pPainter->drawPixmap( 0, 0, m_cClipMediumPM );
        QToolTip::add( this->listBox(), oRect_DocType, "This is a clipping plane." );
    }
    if (m_pDoc->getType() == SENSITUS_DOC_ERASER)
    {
	pPainter->drawPixmap( 0, 0, m_cEraserMediumPM );
	QToolTip::add( this->listBox(), oRect_DocType, "This is an eraser tool." );
    }
    if (m_pDoc->getType() == SENSITUS_DOC_MARKER)
    {
	pPainter->drawPixmap( 0, 0, m_cMarkerPM );
	QToolTip::add( this->listBox(), oRect_DocType, "This is a marker." );
    }
    //
    // Draw the eye icon
    //
    QRect oRect_Visible( 24, oItemRect.top(), 24,24 );
    if (m_bVisible)
    {
        if (m_pDoc != NULL && (m_pDoc->getType() == SENSITUS_DOC_SITUS || m_pDoc->getType() == SENSITUS_DOC_PDB || m_pDoc->getType() == SENSITUS_DOC_MARKER))
        {
            svt_color oColor = m_pDoc->getColor();
            pPainter->fillRect( m_iVisibleXPos+1, 1, 22, 22, QColor( (int)(255.0f*oColor.getR()), (int)(255.0f*oColor.getG()),(int)(255.0f*oColor.getB()) ));
            pPainter->fillRect( m_iVisibleXPos+4, 4, 16, 16, listBox()->colorGroup().background() );
        }

        if (!bScene)
        {
            pPainter->drawPixmap( m_iVisibleXPos, 0, m_cEyeMediumPM );
            QToolTip::add( this->listBox(), oRect_Visible, "Click here to make document invisible!" );
        } else
            pPainter->drawPixmap( m_iVisibleXPos, 0, m_cEyeMediumDisablePM );

    } else
        if (!bScene)
            QToolTip::add( this->listBox(), oRect_Visible, "Click here to make document visible again!" );

    //
    // Draw the move tool icon
    //
    QRect oRect_Move( 48, oItemRect.top(), 24,24 );
    if (m_bMove)
    {
        QToolTip::add( this->listBox(), oRect_Move, "Document is attached to mouse!" );

        if (m_pDoc->isIGDOn())
        {
	    pPainter->drawPixmap( m_iMoveXPos, 0, m_cMoveIGDMediumPM );
            QToolTip::add( this->listBox(), oRect_Move, "Document is attached to mouse, rotations come from IGD!" );

        } else if (m_pDoc->isLocked())
            pPainter->drawPixmap( m_iMoveXPos, 0, m_cMoveMediumLockedPM );
        else
            pPainter->drawPixmap( m_iMoveXPos, 0, m_cMoveMediumPM );

    } else {

        if (m_pDoc->getType() != SENSITUS_DOC_SITUS )
            QToolTip::add( this->listBox(), oRect_Move, "Click here to attach document to mouse!" );

    }
    if (!m_bMove && m_pDoc->isLocked())
        pPainter->drawPixmap( m_iMoveXPos, 0, m_cLockedMediumPM );
    if (m_pDoc->isLocked())
        QToolTip::add( this->listBox(), oRect_Move, "Document is used in an algorithm right now!" );

    //
    // Draw the text
    //
    QRect oRect_Text( m_iTextXPos, oItemRect.top(), width( listBox() ) - m_iTextXPos + 1 , height( listBox() ) );
    if ( !current() && !selected() )
        pPainter->setPen( listBox()->colorGroup().buttonText() );
    else
        pPainter->setPen( listBox()->colorGroup().highlightedText() );
    int yPos;
    QFontMetrics fm = pPainter->fontMetrics();
    if ( m_iMaxHeight < fm.height() )
        yPos = fm.ascent() + fm.leading()/2;
    else
        yPos = m_iMaxHeight/2 - fm.height()/2 + fm.ascent();
    if ( m_pDoc->getType() == SENSITUS_DOC_SITUS || m_pDoc->getType() == SENSITUS_DOC_PDB )
    {
        QString oTip = m_pDoc->getFileName();
        QToolTip::add( this->listBox(), oRect_Text, oTip );
    }
    pPainter->drawText( m_iTextXPos + 3, yPos, oText );
}

/**
 * get the height of the listboxitem
 * \return the height of the listboxitem
 */
int sculptor_file_lbi::height( const QListBox* pListBox) const
{
    return QMAX( m_iMaxHeight, pListBox->fontMetrics().lineSpacing() + 1 );
}
/**
 * get the width of the listboxitem
 * \return the width of the listboxitem
 */
int sculptor_file_lbi::width( const QListBox* ) const
{
    return m_iWidth;
}

/**
 * calculate the positions for all the icons and the text
 */
void sculptor_file_lbi::recalcSize()
{
    m_iTextXPos = 0;
    m_iMaxHeight = 0;
    m_iWidth = 0;

    // doc icon
    m_iTextXPos +=  m_cMolBwMediumPM.width() + 1;
    if (m_cMolBwMediumPM.height() > m_iMaxHeight)
        m_iMaxHeight = m_cMolBwMediumPM.height();
    // visible icon
    m_iVisibleXPos = m_iTextXPos;
    m_iTextXPos +=  m_cEyeMediumPM.width() + 1;
    if (m_cEyeMediumPM.height() > m_iMaxHeight)
        m_iMaxHeight = m_cEyeMediumPM.height();
    // move icon
    m_iMoveXPos = m_iTextXPos;
    m_iTextXPos +=  m_cMoveMediumPM.width() + 1;
    if (m_cMoveMediumPM.height() > m_iMaxHeight)
        m_iMaxHeight = m_cMoveMediumPM.height();
}

/**
 * set the text in the listboxitem
 * \param pText new text for the listbox item
 */
void sculptor_file_lbi::setText(const QString& oText)
{
    QListBoxItem::setText(oText);
    recalcSize();
}

/**
 * Set a link to the document the listboxitem represents
 * \param pDoc pointer to sculptor_document
 */
void sculptor_file_lbi::setDocument( sculptor_document* pDoc )
{
    m_pDoc = pDoc;
};
