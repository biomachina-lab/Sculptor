/***************************************************************************
                          sculptor_dlg_gamultifit.h
                          -----------------
    begin                : 10.05.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_GAMULTIFIT_H
#define SCULPTOR_DLG_GAMULTIFIT_H

//sculptor includes
#include <sculptor_gamultifit.h>
class pdb_document;
class situs_document;
// uic-qt4 includes
#include <ui_dlg_gamultifit.h>


/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_gamultifit : public QDialog, private Ui::dlg_gamultifit
{
    Q_OBJECT

    // the list of pdb used in MOSAEC
    vector<pdb_document *> m_oPDBs; 
  
    // the list of situs doc in MOSAEC // use first
    vector<situs_document *> m_oSITUSs; 

public:

    /**
     * Constructor
     */
    dlg_gamultifit(QWidget* pParent);
    /**
     * Destructor
     */
    virtual ~dlg_gamultifit();
    /**
    * set pdb used in mosaec
    * \param a list of documents
    */ 
    void setPdb( vector<pdb_document*> &oDocs );
    /**
    * get pdb used in mosaec
    * \return a list of documents
    */ 
    vector<pdb_document *>  getPdb( );
    /**
    * set situs used in mosaec
    * \param a list of documents
    */ 
    void setSitus( vector<situs_document *> &oDocs );
    /**
    * get situs used in mosaec
    * \return a list of documents
    */ 
    vector<situs_document *>  getSitus( );
     /**
     * get the distance penalty
     */
    Real64 getResolution();
    /**
     * get the population size
     */
    int getPopSize();
    /**
     * set the popsize
     */
    void setPopSize( unsigned int iPopSize); 
     /**
     * get weather one should sample around CV
     */ 
    bool getSample();
   /**
     * get the popsize
     */
    unsigned int getPopSizeAroundCenters();
    /**
     * set the popsize
     */
    void setPopSizeAroundCenters( unsigned int iPopSize);

    /**
     * set the m_pShowFitnessCurveDlg (the one from the main window's menu) action in the dialog's
     * checkbox
     * \param QAction pointer pShowFitnessCurveDlg
     */
    void setShowFitnessCurveAction( QAction* pShowFitnessCurveDlg );

    /**
     * get the UseComputedCV
     * \return the m_pUseComputedCV checked
     */ 
    bool getUseComputedCV();
    /**
     * get the CVScore
     *\return if CVScore checked
     */ 
    bool getCVScore();
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
     * get the size of the tabu window
     */
    Real64 getTabuWindowSize();
     /**
     * get the threshold of the tabu regions
     */
    Real64 getTabuThreshold();
     /**
     * get the tabu regions size
     */
    Real64 getTabuRegionSize();
     /**
     * get the distance Threshold
     */
    Real64 getDistanceThreshold();
     /**
     * get the distance penalty
     */
    Real64 getDistancePenalty();
    /**
     * get wether to output to disk
     */ 
    bool getOutput();
    /**
     * get the output path
     */ 
    string getOutputPath();   
    /**
     * get the WriteModelInterval 
     */
    int getWriteModelInterval();

private slots:

    /**
     *Select the output directory for the GA results
     */
    void selectGaOutputDir();
};

#endif
