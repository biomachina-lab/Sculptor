/***************************************************************************
                          sculptor_dlg_refine.h
                          -----------------
    begin                : 11.01.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_REFINE_H
#define SCULPTOR_DLG_REFINE_H

// uic-qt4. includes
#include <ui_dlg_refine.h>
#include <svt_types.h>

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_refine : public QDialog, private Ui::dlg_refine
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_refine(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_refine();

    /**
     * get Resolution
     * \return the resolution
     */
    Real64 getResolution();

    /**
     * get maximum number of iterations
     * \return the number of iterations
     */
    Real64 getMaxIter();

    /**
     * get the tolerance 
     * \return the tolerance 
     */
    Real64 getTolerance();

    /**
     * should the translation be optimized 
     * \param iAxis = 0 ~ X (default); = 1~y; = 2~z
     */
    bool getOptTrans(int iAxis);
 
    /**
     * should the rotation be optimized 
     * \param iAxis = 0 ~ X (default); = 1~y; = 2~z
     */
    bool getOptRot(int iAxis);

    /**
     * get cv sampling
     */
    bool getCVSampling();
    
    /**
     * set cv sampling
     */
    void setCVSampling( bool bChecked );

    /**
     * get union sphere 
     */
    bool getUnionSphere();

    /**
     * set union sphere 
     */
    void setUnionSphere( bool bChecked );

    /**
     * get Correlation
     */
    bool getCorrelation();

    /**
     * set Correlation
     */
    void setCorrelation( bool bChecked );

    /**
     * get laplace 
     */
    bool getLaplace();

    /**
     * set laplace 
     */
    void setLaplace( bool bChecked );

};

#endif
