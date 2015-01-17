/***************************************************************************
                          sculptor_powell.h
                          -----------------
    begin                : 12.17.2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_POWELL_CC_H
#define SCULPTOR_POWELL_CC_H

//svt includes
#include <sculptor_document.h>
#include <svt_basics.h>
#include <svt_powell_cc.h>
#include <svt_threads.h>

/** This class stores a svt_powell object
 * @author Mirabela Rusu
 */
class sculptor_powell_cc : public svt_powell_cc
{
protected:
    //pointers to the documents used to create the powell object
    vector< sculptor_document *> m_oDocs;
public:
    
    /**
     * constructor
     */
    sculptor_powell_cc(vector< svt_point_cloud_pdb< svt_vector4<Real64> > > oUnits, vector< svt_matrix4<Real64> > oUnitsMat, svt_volume<Real64>& rVolume, svt_matrix4<Real64>& rVolMat);
    
    /**
     * set documents
     * \param a vector with the pointers to the documents used
     */
    void setDocs( vector< sculptor_document*> &oDocs );

    /**
     *  get documents
     */ 
    vector<sculptor_document* >& getDocs();

    /**
     * function to create the thread 
     */
    void initThread();
    
    /**
     * Update documents - usually at the end of powell
     * ATTENTION: needs svt_redraw in GUI
     */
    void setPowellTransf();

    /**
     * refresh document transformation
     */
    virtual void outputResult();

};

#endif

