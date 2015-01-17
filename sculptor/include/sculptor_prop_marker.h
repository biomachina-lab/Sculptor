/***************************************************************************
                          sculptor_prop_marker.h
                          -----------------
    begin                : 10.05.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_MARKER_H
#define SCULPTOR_DLG_MARKER_H

#include<sculptor_doc_marker.h>

// uic-qt4 includes
#include <ui_prop_marker.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class prop_marker : public QDialog, private Ui::prop_marker
{
    Q_OBJECT

    sculptor_doc_marker * m_pMarkerDoc;

public:

    /**
     * Constructor
     */
    prop_marker(sculptor_doc_marker* pMarkerDoc, QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~prop_marker();

    /**
     * get the X coordinate of the file
     * \return the X coordinate  
     */
    Real64 getX();
    /**
     * get the X coordinate of the file
     * \return the X coordinate  
     */
    void setX(Real64 fCoord);
    /**
     * get the Y coordinate of the file
     * \return the Y coordinate  
     */
    Real64 getY();
    /**
     * get the Y coordinate of the file
     * \return the Y coordinate  
     */
    void setY(Real64 fCoord);
    /**
     * get the Z coordinate of the file
     * \return the Z coordinate  
     */
    Real64 getZ();
    /**
     * get the Z coordinate of the file
     * \return the Z coordinate  
     */
    void setZ(Real64 fCoord);

public slots:
    /**
     * Apply coordinates from the dlg to the marker
     */ 
    void apply();

    /**
     * update the dlg 
     */
    void update();

    /**
     * scale the marker
     */ 
    void scale();
    
    /**
     * set the color
     */ 
    void setColor();

    /**
     * set the Window Current Possition
     */ 
    void setWindowCurrentPos(); 
};

#endif
