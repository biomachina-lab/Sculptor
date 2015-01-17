/***************************************************************************
                          sculptor_powell.h
                          -----------------
    begin                : 12.17.2009
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

// sculptor includes
#include <sculptor_powell_cc.h>
#include <sculptor_window.h>
// svt includes
#include <svt_init.h>


extern sculptor_window* g_pWindow;

/**
 * Constructor
 */
sculptor_powell_cc::sculptor_powell_cc(vector< svt_point_cloud_pdb< svt_vector4<Real64> > > oUnits, vector< svt_matrix4<Real64> > oUnitsMat, svt_volume<Real64>& rVolume, svt_matrix4<Real64>& rVolMat) : svt_powell_cc(oUnits, oUnitsMat, rVolume, rVolMat)
{
    for (unsigned int iUnit=0; iUnit<oUnits.size(); iUnit++)
    {
	m_oDocs.push_back( NULL );
    }
}; 

/**
 * set documents
 * \param a vector with the pointers to the documents used
 */
void sculptor_powell_cc::setDocs( vector< sculptor_document*> &oDocs ) 
{ 
    m_oDocs = oDocs;
};

/**
 *  get documents
 */ 
vector<sculptor_document* >& sculptor_powell_cc::getDocs() 
{ 
    return m_oDocs; 
};


/**
 * Thread for powell
 */
void* maximizeInThread(void* pData)
{
    if (!pData)
        return NULL;
    
    sculptor_powell_cc* pPowell = (sculptor_powell_cc*)pData;
    pPowell->maximize();
    return NULL;
};

/**
 * function to create the thread 
 */
void sculptor_powell_cc::initThread()
{
   svt_createThread(&maximizeInThread, (void*)this, SVT_THREAD_PRIORITY_NORMAL); 
}

/**
 * Update documents - usually at the end of powell
 * ATTENTION: needs svt_redraw in GUI
 */
void sculptor_powell_cc::setPowellTransf()
{
    vector<svt_matrix4<Real64> > oPDBMats = getTrans();    
 
   //apply the transformations
    for (unsigned int iMat=0; iMat<oPDBMats.size(); iMat++)
    {
        oPDBMats[iMat].setTranslationX( oPDBMats[iMat].translationX() * 1.0E-2 );
        oPDBMats[iMat].setTranslationY( oPDBMats[iMat].translationY() * 1.0E-2 );
        oPDBMats[iMat].setTranslationZ( oPDBMats[iMat].translationZ() * 1.0E-2 );

	if (m_oDocs[iMat]!=NULL && m_oDocs[iMat]->getType() == SENSITUS_DOC_PDB)
	{
	    oPDBMats[iMat].copyToMatrix4f( *m_oDocs[iMat]->getNode()->getTransformation() );
	    m_oDocs[iMat]->setChanged();
	}
	else
	    svt_exception::ui()->info("Something when wrong with the assignment of the document or the document does not exist anymore !");
    }
    svt_redraw();
}

/**
 * refresh document transformation
 */
void sculptor_powell_cc::outputResult()
{
    setPowellTransf();
    //svt_redraw();
};


