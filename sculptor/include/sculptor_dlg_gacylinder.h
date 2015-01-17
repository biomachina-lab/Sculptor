/***************************************************************************
                          sculptor_dlg_gacylinder.h
                          -----------------
    begin                : 10.29.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_GACYLINDER_H
#define SCULPTOR_DLG_GACYLINDER_H

// uic-qt4 includes
#include <ui_dlg_gacylinder.h>
#include <sculptor_gacylinder.h>
#include <svt_node.h>

class situs_document;

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_gacylinder : public QDialog, private Ui::dlg_gacylinder
{
    Q_OBJECT
protected:
    svt_node* m_pDataTopNode;

    int m_iObjectCount;
    Real64 m_fRes;
    Real64 m_fAni;
    Real64 m_fLamdba;
    Real64 m_fAcceptedMoveRatio;
    Real64 m_fExpansionTemplateRadius;
    Real64 m_fSearchTemplateRadius;
    int m_iExpansionTemplateRepeats;
    int m_iSearchTemplateRepeats;
    int m_iPopSize;
    int m_iSyncGen;
    Real64 m_fCrawlingStep;
    Real64 m_fTabuRegionSize;
    Real64 m_fDistBetweenRepeats;
    
public:

    /**
     * Constructor
     */
    dlg_gacylinder(QWidget* pParent, svt_node *pDataTopNode);
    /**
     * Destructor
     */
    virtual ~dlg_gacylinder();

    /**
     * get the number of objects
     */
    int getObjectCount();

    /**
     * set the number of objects
     */
     void setObjectCount(Real64 fObjectCount);

    /**
     * get the resolution
     */
    Real64 getResolution();
    
    /**
     * set the resolution
     */
    void setResolution(Real64 fRes);

    /**
     * get the radius
     */
    Real64 getExpansionTemplateRadius();

    /**
     * set the radius
     */
     void setExpansionTemplateRadius(Real64 fRadius);
    
    /**
     * get the radius
     */
    Real64 getSearchTemplateRadius();

    /**
     * set the radius
     */
    void setSearchTemplateRadius(Real64 fRadius);
     
    /**
     * get the Repeats of the expansion Template
     */
    int getExpansionTemplateRepeats();

    /**
     * set the Repeats of the expansion Template
     */
    void setExpansionTemplateRepeats(int iRepeats);

    /**
     * get the repeats of the search Template
     */
    int getSearchTemplateRepeats();

    /**
     * set the Repeats of the expansion Template
     */
    void setSearchTemplateRepeats(int iRepeats);

    /**
     * get the population size
     */
    int getPopSize();
     /**
     * set the popsize
     */
    void setPopSize( unsigned int iPopSize); 

    /**
     * get the maximum number of generations
     */
    int getMaxGen();
    /**
     * get the number of threads
     */
    int getMaxThread();

    /**
     * get the generation when to syncronize
     */
    int getSyncGen();
    
    /**
     * set the generation when to syncronize
     */
    void setSyncGen(int iSyncGen);


    /**
     * get the maximun number of crawls 
     */
    int getMaxFailedCrawls();

    /**
     * get the accept move ratio
     */
    Real64 getAcceptMoveRatio();
    
    /**
     * set the accept move ratio
     */
    void setAcceptMoveRatio(Real64 fRatio);

     /**
     * get the tabu regions size
     */
    Real64 getTabuRegionSize();

     /**
     * set the tabu regions size
     */
    void setTabuRegionSize(Real64 fTabuRegionSize);

    /**
     * get voxel width 
     */
    Real64 getVoxelWidth();
    
    /**
     * set voxel width 
     */
    void setVoxelWidth(Real64 fVoxelWidth);

    /**
     * get the crawling step
     */
    Real64 getCrawlingStep();
    
    /**
     * set the crawling step
     */
    void setCrawlingStep(Real64 fCrawlingStep);


    /**
     * get the anisotropic correction factor
     */
    Real64 getAni();

    /**
     * set the anisotropic correction factor
     */
    void setAni(Real64 fAni);

    /*
     * get lambda for correction
     */
    Real64 getLambda();

    /*
     * set lambda for correction
     */
    void setLambda( Real64 fLambda );

    /**
     * get distance between repeats 
     */
    Real64 getDistBetweenRepeats();

    /**
     * set distance between repeats 
     */
    void setDistBetweenRepeats(Real64 fDist);

 public slots: 
    /**
     * Create document with the expansion template
     */
    void createExpansionTemplate();
    
    /**
     * Create document with the search template
     */
    void createSearchTemplate();

    /**
     * Switch between tracing helices and filopodia
     */
    void setDefaultParameters();

};


#endif
