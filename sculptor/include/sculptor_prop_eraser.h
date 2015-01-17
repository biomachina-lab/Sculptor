/*-*-c++-*-*****************************************************************
                          sculptor_prop_eraser.h  -  description
                             -------------------
    begin                : 17.09.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_PROP_ERASER_H
#define SCULPTOR_PROP_ERASER_H

// uic-qt4 includes
#include <ui_prop_eraser.h>
// scultpor includes
class sculptor_doc_eraser;


/**
 * This class is the intermediary class that ties the eraser document property gui to the functions of the eraser document.
 * \author Francisco Serna
 */
class prop_eraser : public QWidget, private Ui::prop_eraser
{ 
    Q_OBJECT

protected:

    //stores pointer to eraser document for method calls
    sculptor_doc_eraser* m_pEraserDoc;

public:

    /**
     * Constructor
     * \param pParent pointer to QWidget parent
     * \param pEraserDoc pointer to sculptor_doc_eraser
     */
    prop_eraser(sculptor_doc_eraser* pEraserDoc, QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~prop_eraser();

    /**
     * update eraser shape toolbox to what is set in the sculptor_doc_eraser
     */
    void updateEraserShape();

    /**
     * update eraser size from what's set in sculptor_doc_eraser
     */
    void updateEraserSize(); 

    /**
     * update invert checkbox in gui from what's set in sculptor_doc_eraser
     */ 
    void updateInvertCheckBox();

    /**
     * update eraser opacity from value set in sculptor_doc_eraser
     */
    void updateEraserOpacity();
 	
};

#endif
