/***************************************************************************
                          sculptor_dlg_clustering.h
                          -----------------
    begin                : 29.10.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_CLUSTERING_H
#define SCULPTOR_DLG_CLUSTERING_H

// uic-qt4 includes
#include <ui_dlg_clustering.h>
// svt includes
#include <svt_point_cloud_pdb.h>
//forward declaration
class sculptor_document;

/**
 * Simple class that inherits the clustering dialog and passes those parameters to the proper method.
 * \author Francisco Serna
 */
class dlg_clustering : public QDialog, private Ui::dlg_clustering
{
    Q_OBJECT

public:

    /**
     * Constructor
     * \param pParent  pointer to the dialog's parent
     * \param pSculptorDoc pointer to the sculptor document to which the methods will be applied
     */
    dlg_clustering(QWidget* pParent, sculptor_document* pSculptorDoc);

    /**
     * Destructor
     */ 
    virtual ~dlg_clustering();

    /**
     * If sculptor document is a situs document, hide resolution and voxel options in dialog
     */ 
    void hideResolutionVoxelOptions();

    /**
     * Hide appropriate document elements if the method is laplaceQuantization
     */
    void setupLapLaceOptions();

    /**
     * Sets up parameter values given a codebook vector PDB, hides options that are not needed
     * \param pCV pointer to the pdb containing codebook vectors
     */
    void setCVStartPos(svt_point_cloud_pdb< svt_vector4<Real64> >* pCV);
    
    /**
     * Hides T elements of the dialog
     */ 
    void hideTElements();

    /**
     * Hides Cutoff values of the dialog
     */
    void hideCutoffs();

    /**
     * Since elements are shown or hidden depending on sculptor document type, include an independent adjustSize to resize dialog
     */ 
    void adjustSizeDlg();

    /**
     * Assign a method using enum from sculptor document (CLUSTER_METHOD_LAPLACE or CLUSTER_METHOD_CLUSTERING) to keep track of which method to call
     * \param iMethod int as an enum for the method to call (CLUSTER_METHOD_LAPLACE or CLUSTER_METHOD_CLUSTERING)
     */
    void assignMethod(int iMethod);

    /**
     * If the dialog exists, show all options, set method and pointer to codebook vector pdb to null, and reassign sculptor document to point to
     * \param pSculptorDoc pointer to the new sculptor document we are calling the methods on
     */ 

    void resetDlg(sculptor_document* pSculptorDoc);

public slots:

    /**
     * Slot method that extracts the needed parameters from the dialog and calls the appropriate method in sculptor document, passing those parameters
     */ 

    void extractParameters();

private:

    sculptor_document* m_pSculptorDoc;
    svt_point_cloud_pdb< svt_vector4<Real64> >* m_pCV;
    int m_iMethod;

};

#endif
