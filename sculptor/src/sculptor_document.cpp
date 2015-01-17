/***************************************************************************
                          sculptor_document.cpp
                          -------------------
    begin                : 27.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_document.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlistitem.h>
#include <sculptor_splitter.h>
#include <sculptor_force_algo.h>
// qt includes
#include <QMessageBox>
// svt includes
#include <svt_file_utils.h>
#include <svt_init.h>
#include <svt_node.h>
#include <svt_clustering_trn.h>
#include <svt_clustering_irank.h>
#include <svt_clustering_fam.h>
#include <svt_powell_cc.h>
#include <svt_cylinder.h>
#include <svt_string.h>

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);


/**
 *
 */
sculptor_document::sculptor_document(svt_node* pNode,
				     string oFileName,
				     string oFileName_SRCB)
    : m_pDLItem( NULL ),
      m_pGraphNode( pNode ),
      m_bSolution( false ),
      m_bCopy( false ),
      m_bSelected( false ),
      m_bChanged( false ),
      m_bLocked ( false ),
      m_iCurrentTrans( 0 ),
      m_pClusteringDlg( NULL ),
      m_pCodebookSpheres( NULL ),
      m_fCV_Red( 1.0f ),
      m_fCV_Green( 1.0f ),
      m_fCV_Blue( 1.0f ),
      m_pStericSpheres( NULL ),
      m_iCVDispSrc( SCULPTOR_CVDISP_NONE ),
      m_iSelectA( -1 ),
      m_iSelectB( -1 )
{
    // set file name
    if ( !oFileName.empty() )
	m_oFileInfo.setFile( QString(oFileName.c_str()) );

     // set SRCB file name (e.g. a psf)
    if ( !oFileName_SRCB.empty() )
	m_oFileInfo_SRCB.setFile( QString(oFileName_SRCB.c_str()) );
    else
	m_oFileInfo_SRCB.setFile( QString("NONE") );

    m_bTransformListModified = false;

    // initialize interactive global docking
    m_bIGDOn           = false;
    m_bIGDWarningShown = false;
    m_bEliDataLoaded   = false;
    m_iDocID           = 0;
    m_pOrigProbeDoc    = NULL;

    m_oInitMat.setToId();

    // set unique document ID
    m_iDocID = svt_getToD();
}

sculptor_document::~sculptor_document()
{
    // if (m_pCodebookSpheres != NULL) FIXME
    //     delete m_pCodebookSpheres;
    if (m_pClusteringDlg != NULL)
	delete (m_pClusteringDlg);    
}

/**
 *
 */
void sculptor_document::setFileName(string oFilename)
{
    if ( !oFilename.empty() )
    {
	m_oFileInfo.setFile( QString(oFilename.c_str()) );
        m_oDisplayName = m_oFileInfo.fileName();
    }
    else
	m_oFileInfo.setFile( QString() );
};

/**
 *
 */
string sculptor_document::getFileName(bool bWithPath)
{
    if (bWithPath)
        return QtoStd(m_oFileInfo.absoluteFilePath());
    else
        return QtoStd(m_oFileInfo.fileName());
};


/**
 *
 */
string sculptor_document::getNameShort()
{
    if ( !m_oFileInfo.baseName().isEmpty() )
        return QtoStd(m_oFileInfo.baseName());
    else
        return QtoStd(m_oDisplayName);
}

/**
 *
 */
string sculptor_document::getFileName_SRCB(bool bWithPath)
{
    if (bWithPath)
        return QtoStd(m_oFileInfo_SRCB.absoluteFilePath());
    else
        return QtoStd(m_oFileInfo_SRCB.fileName());
};

/**
 *
 */
void sculptor_document::setDisplayName(string oName)
{
    if ( !oName.empty() )
	m_oDisplayName = QString(oName.c_str());
    else
	m_oDisplayName = QString();
};

/**
 *
 */
string sculptor_document::getDisplayName()
{
    // if there's no explicit displayname, return the filename with extension (may be empty)
    if ( m_oDisplayName.isEmpty() )
	return QtoStd(m_oFileInfo.fileName());

    // otherwise, return the displayname
    else
	return QtoStd(m_oDisplayName);
};

/**
 *
 */
void sculptor_document::setVisible(bool bVisible)
{
    getNode()->setVisible( bVisible );

    if ( bVisible && (m_iCVDispSrc==SCULPTOR_CVDISP_HEAD || m_iCVDispSrc==SCULPTOR_CVDISP_TAIL) ) // if it has just been made visible
        g_pWindow->updateCVDisplacementObj();
};
/**
 *
 */
bool sculptor_document::getVisible()
{
    return getNode()->getVisible();
};

/**
 * Set visibility status of the codebook
 * \param bVisible if true the feature-vectors are rendered
 */
void sculptor_document::setCV_Visible( bool bVisible )
{
    if (m_pCodebookSpheres)
	m_pCodebookSpheres->setVisible( bVisible );
};
/**
 * Get visibility status of the codebook
 * \return if true the feature-vectors are rendered
 */
bool sculptor_document::getCV_Visible( )
{
    if (m_pCodebookSpheres)
	return m_pCodebookSpheres->getVisible();
    else
        return false;
};

/**
 * Set the visibility status of the steric spheres container
 * \param bVisible if true the spheres are visible
 */
void sculptor_document::setStericSpheresVisible( bool bVisible )
{
    if ( m_pStericSpheres != NULL)
        m_pStericSpheres->setVisible( bVisible );
};

/**
 * Set the visibility status of the steric spheres
 * \param iIndex index of sphere to turn on or off
 * \param bVisible if true the spheres are visible
 */
void sculptor_document::setStericSpheresVisible( unsigned int iIndex, bool bVisible )
{
    if (m_pStericSpheres != NULL)
    {
        unsigned int i = 0;
        svt_node* pIndex = m_pStericSpheres->getSon();

        for (i=0; i<iIndex; i++)
        {
            if (pIndex == NULL)
                return;
            pIndex = pIndex->getNext();
        }
        pIndex->setVisible( bVisible );
    }
};

/**
 * Set the color of individual steric spheres
 * \param iIndex index of sphere to turn on or off
 * \param color 
 */
void sculptor_document::setStericSpheresColor( unsigned int iIndex, svt_color* color)
{
    if (m_pStericSpheres != NULL)
    {
        unsigned int i = 0;
        svt_node* pIndex = m_pStericSpheres->getSon();

        for (i=0; i<iIndex; i++)
        {
            if (pIndex == NULL)
                return;
            pIndex = pIndex->getNext();
        }
        pIndex->getProperties()->setWireframeColor( color );
    }
};

svt_node* sculptor_document::getNode()
{
    return NULL;
}

/**
 * get the type of the document
 * \return SENSITUS_DOC_UNKNOWN, SENSITUS_DOC_PDB or SENSITUS_DOC_SITUS
 */
int sculptor_document::getType()
{
    return SENSITUS_DOC_UNKNOWN;
}

/**
* get rather document is SRC (and which) in a CVDisp Object 
* \return SCULPTOR_CVDISP_NONE, SCULPTOR_CVDISP_HEAD, SCULPTOR_CVDISP_TAIL
*/
int sculptor_document::getCVDispSrc()
{
    return m_iCVDispSrc;
}

/**
* set rather document is SRC (and which) in a CVDisp Object 
* \param SCULPTOR_CVDISP_NONE, SCULPTOR_CVDISP_HEAD, SCULPTOR_CVDISP_TAIL
*/
void sculptor_document::setCVDispSrc(int iCVDispSrc)
{
    m_iCVDispSrc = iCVDispSrc;
}

/**
 * get the position of the sculptor document in real space (e.g. for pdb or situs files)
 * \return svt_vector4f with the position
 */
svt_vector4f sculptor_document::getRealPos()
{
    return svt_vector4f(0,0,0);
}

/**
 * Get the coordinates of the codebook in the real space based on the transfomations that the doc has endured 
 * \param oOrigin the scene's origin
 * \param oCodebook coodebook in the real space coordinates 
 */
void sculptor_document::getCodebookRealPos(svt_point_cloud_pdb<svt_vector4<Real64> >& oCodebook, svt_vector4<Real64> oOrigin)
{
    oCodebook.deleteAllAtoms();
    oCodebook = m_aCodebook;

    // getRealPos returns the center of the molecule in Amstron(pdb value *1.0E-10);  //the transformation matrix is in GL coordinates: 1 GL = 1.0E-2 A
    svt_vector4<Real64> oDocOrigin = getRealPos();
    svt_vector4<Real64> oVec;
    svt_matrix4<Real64> oMat( *(getNode()->getTransformation()) );

    oMat.translate( oOrigin.x(), oOrigin.y(), oOrigin.z() );

    for( unsigned int i=0; i< m_aCodebook.size(); i++)
    {
	// move the codebook to have the same origin as the system they correspond to...
	oVec = m_aCodebook[i];	
	oVec *= 1.0E-2;
	oVec.w( 1.0f );

	oVec = oMat * oVec;

	oVec *= 1.0E2;

	oCodebook[i] = oVec;
    }

};

/**
 * Export the codebook into a pdb file with world coordinates
 */
void sculptor_document::exportCodebook( QString oFilename, svt_vector4f oOrigin )
{
    svt_point_cloud_pdb<svt_vector4<Real64> > oWorldCodebook;

    if (m_aCodebook.size()==0)
    {
        svt_exception::ui()->info( "No codebook vectors available to export!" );
        return;
    }
    
    getCodebookRealPos( oWorldCodebook, oOrigin );

    QFileInfo oFileInf( oFilename );
    QString oFileNameOnly = oFileInf.baseName();
    QString oFilePath     = oFileInf.path();
    QString oFilePDB      = oFilePath + QDir::separator() + oFileNameOnly + QString(".pdb");
    QString oFilePSF      = oFilePath + QDir::separator() + oFileNameOnly + QString(".psf");

    oWorldCodebook.writePDB( QtoStd(oFilePDB).c_str() );
    m_aCodebook.writePSF( QtoStd(oFilePSF).c_str() );

};

/**
 * Export the codebook into a pdb file with world coordinates
 */
void sculptor_document::importCodebook(QString oFilename, svt_vector4f)
{
    svt_point_cloud<svt_vector4<Real64> > oWorldCodebook;

    oWorldCodebook.loadPDB( QtoStd(oFilename).c_str() );

    // getRealPos returns the center of the molecule in Amstron(pdb value *1.0E-10);  //the transformation matrix is in GL coordinates: 1 GL = 1.0E-2 A
    svt_vector4<Real64> oDocOrigin = getRealPos();
    svt_vector4<Real64> oVec;
    svt_matrix4<Real64> oMat( *(getNode()->getTransformation()) );
    oMat.invert();

    m_aCodebook.deleteAllAtoms();
    svt_point_cloud_atom oAtom;

    for( unsigned int i=0; i< oWorldCodebook.size(); i++)
    {
	// move the codebook to have the same origin as the system they correspond to...
        oVec = oWorldCodebook[i];
	oVec *= 1.0E-2;
	oVec.w( 1.0f );

	oVec = oMat * oVec;

	oVec *= 1.0E2;

	m_aCodebook.addAtom( oAtom, oVec);
    }

    // try to load the connectivity information
    char pWOExt[2048];
    svt_stripExtension( (char*)(QtoStd(oFilename).c_str()), pWOExt );
    char pWExt[2048];
    sprintf( pWExt, "%s.psf", pWOExt );
    m_aCodebook.loadPSF( pWExt );

    updateCodebookRendering();

};

/**
 * set solution flag
 */
void sculptor_document::setSolution(bool bSolution)
{
    m_bSolution = bSolution;
    return;
}
/**
 * set solution flag
 */
bool sculptor_document::getSolution()
{
    return m_bSolution;
}
    
/**
 * set copy flag
 */
void sculptor_document::setCopy(bool bCopy)
{
    m_bCopy = bCopy;
    return;
}
/**
 * set copy flag
 */
bool sculptor_document::getCopy()
{
    return m_bCopy;
}
    
/**
 * reset the initial transformation matrix of the document (that means: the matrix after loading the doc)
 */
void sculptor_document::resetInitMat()
{
    if (getNode())
	if (getNode()->getTransformation())
	{
	    getNode()->getTransformation()->setFromMatrix(m_oInitMat);
            getNode()->rebuildDL();
	}
}
/**
 * store the current node transformation matrix as initial matrix
 */
void sculptor_document::storeInitMat()
{
    if (getNode())
        if (getNode()->getTransformation())
            m_oInitMat.setFromMatrix(*getNode()->getTransformation());
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void sculptor_document::setState(TiXmlElement* pElem)
{
    const char *str;
    char pName[256];
    TiXmlElement* pChild;
    const char* pTransName;
    int iParam;
    int i;
    svt_matrix4<Real64> oMat;
    
    int iMatchCount;

    unsigned long iDocID;
    if ((str = pElem->Attribute("DOCID")) != NULL )
    {
        sscanf(str,"%lu",&iDocID);
        setDocID(iDocID);
    }

    if (pElem->Attribute("ISSOLUTION", &iParam))
        setSolution((bool) iParam);

    if (pElem->Attribute("ISCOPY", &iParam))
        setCopy((bool) iParam);

    if (pElem->Attribute("ISIGDON", &iParam))
        setIGDActive((bool) iParam);


    if ((str = pElem->Attribute("TRANSFORM")) != NULL)
    {
        if (getType() != SENSITUS_DOC_SITUS)
            getNode()->getTransformation()->setFromString(str);
// FIXME: here set the current match        
//         sprintf(pName, "TRANSMATCHSIZE");
//         if (pElem->Attribute(pName, &iMatchCount ))
//         {
//             sprintf(pName, "TRANSMATCH", i);
//             if ((str = pElem->Attribute(pName)) != NULL)
//             {
        //here FIXME
//                 setMatchFromString(str, iMatchCount);
//             }
//         }
    }

    int iCount;
    if (pElem->Attribute("TRANSFORMCOUNT", &iCount ))
    {
	for(i=0; i<iCount; i++)
	{
	    sprintf(pName, "TRANS%iNAME", i);
	    if ((pTransName = pElem->Attribute(pName)) != NULL)
	    {
		sprintf(pName, "TRANS%i", i);
		if ((str = pElem->Attribute(pName)) != NULL)
		{
		    oMat.setFromString( str );
                    addTransform( pTransName, oMat );
                    
                    sprintf(pName, "TRANS%iMATCHSIZE", i);
                    if (pElem->Attribute(pName, &iMatchCount ))
                    {
                        sprintf(pName, "TRANS%iMATCH", i);
                        if ((str = pElem->Attribute(pName)) != NULL)
                        {
                            m_aTransforms[m_iCurrentTrans].setMatchFromString(str, iMatchCount);
                        }
                        
                    }
                    sprintf(pName, "TRANS%iTARGETID", i);
                    if ((str = pElem->Attribute(pName)) != NULL )
                    {
                        unsigned long iTargetID;
                        sscanf(str,"%li",&iTargetID);
                        m_aTransforms[m_iCurrentTrans].setTargetID(iTargetID);
                    }
                    sprintf(pName, "TRANS%iPROBEID", i);
                    if ((str = pElem->Attribute(pName)) != NULL )
                    {
                        unsigned long iProbeID;
                        sscanf(str,"%li",&iProbeID);
                        m_aTransforms[m_iCurrentTrans].setProbeID(iProbeID);
                    }
                }
	    }
            
            
	}

    }

    // codebook vectors
    if (pElem->Attribute("CODEBOOK", &iCount))
    {
        char pStr[256];
        m_aCodebook.deleteAllAtoms();
        double fX, fY, fZ;
        svt_vector4<Real64> oVec;
        svt_point_cloud_atom oAtom;

        for(unsigned int j=0; j<(unsigned int)(iCount); j++)
        {
            sprintf( pStr, "CV%i", j );

	    if ((pChild = (TiXmlElement*) pElem->FirstChild( pStr )) != NULL)
	    {
		if (pChild->Attribute("X", &fX))
		    oVec.x( fX );
		if (pChild->Attribute("Y", &fY))
		    oVec.y( fY );
		if (pChild->Attribute("Z", &fZ))
                    oVec.z( fZ );

                m_aCodebook.addAtom( oAtom, oVec );
            }

            // bonds
            sprintf( pStr, "BOND%i", j );
	    if ((pChild = (TiXmlElement*) pElem->FirstChild( pStr )) != NULL)
            {
                int iBondCount = 0;
                if (pChild->Attribute("COUNT", &iBondCount ))
                {
                    for(int k=0; k<iBondCount; k++)
                    {
                        int iBondTo = 0;
                        sprintf( pStr, "TO_%i", k );
                        if (pChild->Attribute(pStr, &iBondTo ))
                            m_aCodebook.addBond( j, iBondTo );
                    }
                }
            }
        }

        if ((str = pElem->Attribute("CODEBOOKID")))
        {
            unsigned long iID;
            sscanf(str,"%li",&iID);
            m_iCodebookID = iID;
        }

        svtout << "Loaded " << iCount << " codebook vectors." << endl;

        updateCodebookRendering();

        setCV_Visible(false);

	g_pWindow->updateForceIcons();
    }

    if (pElem->Attribute("CVVISIBLE", &iParam))
        setCV_Visible((bool) iParam);

    // exhaustive search data
    if ((pChild = (TiXmlElement*) pElem->FirstChild("EXHAUSTIVESEARCH")) != NULL)
    {
        if (pChild->Attribute("CROSSCORRFILE") != NULL && 
	    strlen(pChild->Attribute("CROSSCORRFILE")) > 0){
            setEliCrossCorrFile(pChild->Attribute("CROSSCORRFILE"));
	    loadEliCrossCorr();
	}
        if (pChild->Attribute("ANGLEINDEXFILE") != NULL && 
	    strlen(pChild->Attribute("ANGLEINDEXFILE")) > 0){
            setEliAngleIndexFile(pChild->Attribute("ANGLEINDEXFILE"));
	    loadEliAngleIndex();
	}
        if (pChild->Attribute("ANGLELISTFILE") != NULL && 
	    strlen(pChild->Attribute("ANGLELISTFILE")) > 0){
            setEliAngleListFile(pChild->Attribute("ANGLELISTFILE"));
	    loadEliAngleList();
	}

	setOrigProbeDoc(this);

	setEliDataLoaded(true);
    }
}

/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void sculptor_document::getState(TiXmlElement* pElem)
{
    char pStr[256];
    char pStrLong[256];
    char pName[256];
    vector<int> oMatch;

    sprintf(pStr, "%lu", getDocID());
    pElem->SetAttribute("DOCID", pStr);

    sprintf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
            getNode()->getTransformation()->getAt(0,0), getNode()->getTransformation()->getAt(0,1), getNode()->getTransformation()->getAt(0,2), getNode()->getTransformation()->getAt(0,3), 
            getNode()->getTransformation()->getAt(1,0), getNode()->getTransformation()->getAt(1,1), getNode()->getTransformation()->getAt(1,2), getNode()->getTransformation()->getAt(1,3), 
            getNode()->getTransformation()->getAt(2,0), getNode()->getTransformation()->getAt(2,1), getNode()->getTransformation()->getAt(2,2), getNode()->getTransformation()->getAt(2,3), 
            getNode()->getTransformation()->getAt(3,0), getNode()->getTransformation()->getAt(3,1), getNode()->getTransformation()->getAt(3,2), getNode()->getTransformation()->getAt(3,3));
    pElem->SetAttribute("TRANSFORM", pStr);
    
    // FIXME: here store the current match
//     oMatch = *(getNode()->getCurrentMatch());
//     sprintf(pName, "TRANSMATCHSIZE");
//     pElem->SetAttribute( pName, oMatch.size() );
//     
//     strcpy(pStrLong,"");
//     for (unsigned int iIndexMatch=0; iIndexMatch<oMatch.size(); iIndexMatch++)
//     {
//         if (iIndexMatch==0)
//             sprintf(pStr, "%d", oMatch[iIndexMatch] );
//         else
//             sprintf(pStr, " %d", oMatch[iIndexMatch] );
//         strcat(pStrLong,pStr);
//     }
//     sprintf(pName, "TRANSMATCH");
//     pElem->SetAttribute( pName, pStrLong );


    pElem->SetAttribute("TRANSFORMCOUNT", m_aTransforms.size() );

    for(unsigned int i=0; i<m_aTransforms.size(); i++)
    {
        sprintf(pName, "TRANS%i", i);
	sprintf(pStr, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
		m_aTransforms[i].getMatrix()[0][0], m_aTransforms[i].getMatrix()[0][1], m_aTransforms[i].getMatrix()[0][2], m_aTransforms[i].getMatrix()[0][3],
		m_aTransforms[i].getMatrix()[1][0], m_aTransforms[i].getMatrix()[1][1], m_aTransforms[i].getMatrix()[1][2], m_aTransforms[i].getMatrix()[1][3],
		m_aTransforms[i].getMatrix()[2][0], m_aTransforms[i].getMatrix()[2][1], m_aTransforms[i].getMatrix()[2][2], m_aTransforms[i].getMatrix()[2][3],
		m_aTransforms[i].getMatrix()[3][0], m_aTransforms[i].getMatrix()[3][1], m_aTransforms[i].getMatrix()[3][2], m_aTransforms[i].getMatrix()[3][3]);
	pElem->SetAttribute( pName, pStr);

	sprintf(pName, "TRANS%iNAME", i);
        pElem->SetAttribute( pName, m_aTransforms[i].getName() );
        
        oMatch = *(m_aTransforms[i].getMatch());
        sprintf(pName, "TRANS%iMATCHSIZE", i);
        pElem->SetAttribute( pName, oMatch.size() );
        
        strcpy(pStrLong,"");
        for (unsigned int iIndexMatch=0; iIndexMatch<oMatch.size(); iIndexMatch++)
        {
            if (iIndexMatch==0)
                sprintf(pStr, "%d", oMatch[iIndexMatch] );
            else
                sprintf(pStr, " %d", oMatch[iIndexMatch] );
            strcat(pStrLong,pStr);
        }
        
        sprintf(pName, "TRANS%iMATCH", i);
        pElem->SetAttribute( pName, pStrLong );

        sprintf( pName, "TRANS%iTARGETID", i);
        sprintf( pStrLong, "%li", m_aTransforms[i].getTargetID() );
        pElem->SetAttribute( pName, pStrLong );

        sprintf(pName, "TRANS%iPROBEID", i);
        sprintf( pStrLong, "%li", m_aTransforms[i].getProbeID() );
        pElem->SetAttribute( pName, pStrLong );
    }

    pElem->SetAttribute("ISSOLUTION", getSolution());
    pElem->SetAttribute("ISCOPY", getCopy());

    pElem->SetAttribute("ISIGDON", getIGDActive());

    pElem->SetAttribute("CVVISIBLE", getCV_Visible());

    // codebook vectors
    pElem->SetAttribute("CODEBOOK", m_aCodebook.size());
    vector< svt_point_cloud_bond >& rBonds = m_aCodebook.getBonds();
    for(unsigned int j=0; j<m_aCodebook.size(); j++)
    {
	sprintf( pStr, "CV%i", j );

	TiXmlElement* pCV = new TiXmlElement( pStr );
	pElem->LinkEndChild(pCV);

	pCV->SetDoubleAttribute("X", m_aCodebook[j].x() );
	pCV->SetDoubleAttribute("Y", m_aCodebook[j].y() );
	pCV->SetDoubleAttribute("Z", m_aCodebook[j].z() );

        // bonds
        vector< unsigned int > aBondList = m_aCodebook.atom(j).getBondList();
        if (aBondList.size() > 0)
        {
            sprintf( pStr, "BOND%i", j );
            TiXmlElement* pBond = new TiXmlElement( pStr );
            pElem->LinkEndChild(pBond);
            pBond->SetAttribute("COUNT", aBondList.size());
            for(unsigned k=0; k<aBondList.size(); k++)
            {
                sprintf( pStr, "TO_%i", k );
                if (rBonds[aBondList[k]].getIndexA() == (int)(j))
                    pBond->SetAttribute(pStr, rBonds[aBondList[k]].getIndexB() );
                else
                    pBond->SetAttribute(pStr, rBonds[aBondList[k]].getIndexA() );
            }
        }
    }
    sprintf( pStrLong, "%li", m_iCodebookID );
    pElem->SetAttribute( "CODEBOOKID", pStrLong );

    // exhaustive search data
    if (getEliDataLoaded()) {
	TiXmlElement* pEli = new TiXmlElement("EXHAUSTIVESEARCH");
	pElem->LinkEndChild(pEli);
	pEli->SetAttribute("CROSSCORRFILE",  QtoStd(m_sEliCrossCorrFile));
	pEli->SetAttribute("ANGLEINDEXFILE", QtoStd(m_sEliAngleIndexFile));
	pEli->SetAttribute("ANGLELISTFILE",  QtoStd(m_sEliAngleListFile));
    }
}

/**
 *
 */
void sculptor_document::save(string oFilename)
{
    setFileName( oFilename );
  
    m_bChanged = false;

    // if this document has an entry in the documentlist, let the documentlist know that this
    // document was saved and is thus not changed anymore
    if ( getDLItem() != NULL )
	getDLItem()->showChanged();
}

/**
 * is this document saveable?
 * \return true if the document can be saved to disk
 */
bool sculptor_document::getSaveable()
{
    return false;
}

// Selection

/**
 * if the document is selected it may have to perform some tasks. Obviously, setting the
 * internal boolean variable for being selected makes sense
 */
void sculptor_document::onSelection()
{
    setSelected( true );
}

/**
 * if the document is deselected it has perhaps to do some cleanup
 * \param pParent pointer to parent widget
 */
void sculptor_document::onDeselection()
{
    setSelected( false );
}

/**
 * is the document selected?
 * \return true if document is selected
 */
bool sculptor_document::getSelected()
{
    return m_bSelected;
}
/**
 * set the select-status of the document
 * \param bSelected true if the document is selected
 */
void sculptor_document::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

/**
 * if the document was changed, this function should be called to give the document the opportunity to update the visualization
 */
void sculptor_document::dataChanged()
{

}

// Transformations

/**
 * Add the current transformation.
 */
bool sculptor_document::addTransform(const char* pName)
{
    char pInternal[256]; sprintf( pInternal, "Transform %li", (long int)(m_aTransforms.size()) );

    if (pName == NULL)
        pName = pInternal;

    if (getNode() != NULL && getNode()->getTransformation() != NULL)
    {
	svt_matrix4< Real64 > oMat( *(getNode()->getTransformation()) );
        m_aTransforms.push_back( sculptor_transform(oMat, pName) );
        m_iCurrentTrans = m_aTransforms.size() - 1;
	setTransformListModified( true );
        return true;
    } else
        return false;
};

/**
 * Add a transformation.
 * \return true if successful (if there is no svt node attached yet, the document might fail to add the transform).
 */
bool sculptor_document::addTransform(const char* pName, svt_matrix4<Real64> oMatrix, vector<int>* pMatch, long int iProbeID, long int iTargetID)
{
    m_aTransforms.push_back( sculptor_transform(oMatrix, pName, pMatch, iProbeID, iTargetID) );
    m_iCurrentTrans = m_aTransforms.size() - 1;
    setTransformListModified( true );
    return true;
};

/**
 * Remove a transformation from the list
 * \param iTrans the transformation that has to be removed
 */
void sculptor_document::removeTransform( unsigned int iTrans )
{
    vector< sculptor_transform >::iterator iIter;

    iIter = m_aTransforms.begin();
    iIter += iTrans;

    m_aTransforms.erase( iIter );

    if ( m_iCurrentTrans >= iTrans && m_iCurrentTrans > 0 )
        m_iCurrentTrans--;

    setTransformListModified( true );
    if (g_pWindow->getForceAlgo() != NULL)
	g_pWindow->getForceAlgo()->calcStericClashing();
};

/**
 * Remove all transforms
 */
void sculptor_document::removeAllTransforms()
{
    m_aTransforms.clear();
    m_iCurrentTrans = 0;

    setTransformListModified( true );
    if (g_pWindow->getForceAlgo() != NULL)
	g_pWindow->getForceAlgo()->calcStericClashing();
};

/**
 * Make current
 */
void sculptor_document::applyTransform( )
{
    if (getNode() != NULL && getNode()->getTransformation() != NULL && m_iCurrentTrans < m_aTransforms.size())
    {
        m_aTransforms[m_iCurrentTrans].getMatrix().copyToMatrix4f( *(getNode()->getTransformation()) );

        if ( getDLItem()->getIndex() == 1 )
            g_pWindow->adjustOrigin();

	if (g_pWindow->getForceAlgo() != NULL)
	    g_pWindow->getForceAlgo()->updateDisplay();
    }

    svt_redraw();
};
/**
 * Make previous transform in the list the current one
 */
void sculptor_document::prevTransform( )
{
    if (m_iCurrentTrans > 0)
    {
        m_iCurrentTrans--;
        applyTransform();
    }
};
/**
 * Make next transform in the list the current one
 */
void sculptor_document::nextTransform( )
{
    if ( m_iCurrentTrans < m_aTransforms.size()-2 )
    {
        m_iCurrentTrans++;
        applyTransform();
    }
};

/**
 * Make current
 * \param iTrans the transformation that has to be removed
 */
void sculptor_document::applyTransform( unsigned int iTrans )
{
    if (getNode() != NULL && getNode()->getTransformation() != NULL && iTrans < m_aTransforms.size())
    {
	m_aTransforms[iTrans].getMatrix().copyToMatrix4f( *(getNode()->getTransformation()) );

	if ( getDLItem()->getIndex() == 1 )
            g_pWindow->adjustOrigin();

	if (g_pWindow->getForceAlgo() != NULL)
	    g_pWindow->getForceAlgo()->updateDisplay();
    }

    m_iCurrentTrans = iTrans;

    svt_redraw();
};

/**
 * Transform list modified?
 */
bool sculptor_document::transformListModified( )
{
    return m_bTransformListModified;
}

/**
 * Set Transform list modified
 */
void sculptor_document::setTransformListModified( bool bTransformListModified )
{
    m_bTransformListModified = bTransformListModified;
    return;
}

/**
 * Get name of a transformation
 * \return pointer to array of char
 */
const char* sculptor_document::getTransformName( unsigned int iTrans )
{
    return m_aTransforms[iTrans].getName();
};
/**
 * Set name of a transformation
 * \param iTrans index of transformation
 * \param pName pointer to array of char
 */
void sculptor_document::setTransformName( unsigned int iTrans, const char* pName )
{
    m_aTransforms[iTrans].setName( pName );
};

/**
 * Get number of transforms stored.
 */
unsigned int sculptor_document::getTransformCount()
{
    return m_aTransforms.size();
};

/**
 * Load all the transforms from a file
 * \param pointer to array of char with the filename
 */
bool sculptor_document::loadTransform(const char* pFname)
{
    FILE* pFile = fopen(pFname, "r" );

    if (pFile == NULL)
	return false;

    m_aTransforms.clear();

    char pName[2048];
    char pNewName[2048];
    char pLine[10000];
    Real32 f00, f01, f02, f03, f10, f11, f12, f13, f20, f21, f22, f23, f30, f31, f32, f33;
    while(!feof( pFile ) )
    {
	svt_matrix4<Real64> oMat;

        fgets( pLine, 10000, pFile );

	if ( !feof( pFile ) )
	{
	    if (sscanf( pLine, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %s\n", &f00, &f01, &f02, &f03, &f10, &f11, &f12, &f13, &f20, &f21, &f22, &f23, &f30, &f31, &f32, &f33, pName ) >= 16)
	    {
		oMat[0][0] = f00;
		oMat[0][1] = f01;
		oMat[0][2] = f02;
		oMat[0][3] = f03;

		oMat[1][0] = f10;
		oMat[1][1] = f11;
		oMat[1][2] = f12;
		oMat[1][3] = f13;

		oMat[2][0] = f20;
		oMat[2][1] = f21;
		oMat[2][2] = f22;
		oMat[2][3] = f23;

		oMat[3][0] = f30;
		oMat[3][1] = f31;
		oMat[3][2] = f32;
		oMat[3][3] = f33;

		// exchange all tildes with spaces
		unsigned int iNameLen = strlen( pName );
		unsigned int j;
		for(j=0; j<iNameLen; j++)
		{
		    if (pName[j] == '\n')
			break;

		    if (pName[j] != '~')
			pNewName[j] = pName[j];
		    else
			pNewName[j] = ' ';
		}
		pNewName[j] = 0;

                sculptor_transform oTrans( oMat, pNewName );

		m_aTransforms.push_back( oTrans );
	    }
	}
    }

    m_iCurrentTrans = 0;

    fclose( pFile );

    return true;
};
/**
 * Save all the transforms from to a file
 * \param pointer to array of char with the filename
 */
void sculptor_document::saveTransform(const char* pFname)
{
    FILE* pFile = fopen(pFname, "w" );

    if (pFile == NULL)
	return;

    char pNewName[2048];
    for(unsigned int i=0; i<m_aTransforms.size(); i++)
    {
        for(unsigned int iX=0; iX<4; iX++)
	    for(unsigned int iY=0; iY<4; iY++)
		fprintf( pFile, "%f ", (m_aTransforms[i].getMatrix())[iX][iY] );

        const char* pName = m_aTransforms[i].getName();
	unsigned int iNameLen = strlen( pName );
        unsigned int j;
	for(j=0; j<iNameLen; j++)
	{
	    if (pName[j] == '\n')
                break;

	    if (pName[j] != ' ')
		pNewName[j] = pName[j];
	    else
		pNewName[j] = '~';
	}
        pNewName[j] = 0;

        fprintf( pFile, "%s\n", pNewName );
    }

    fclose( pFile );
};

/**
 * Move an item in the transform list up
 * \param iItem index of the item (must be greater than zero)
 */
void sculptor_document::transformUp( unsigned int iItem )
{
    if (iItem < 1)
        return;

    vector< sculptor_transform >::iterator iIter;
    sculptor_transform oTmp = m_aTransforms[iItem];

    iIter = m_aTransforms.begin();
    iIter += iItem;
    m_aTransforms.erase( iIter );
    iIter--;
    m_aTransforms.insert( iIter, sculptor_transform( oTmp.getMatrix(), oTmp.getName() ) );

    if (iItem == m_iCurrentTrans)
        m_iCurrentTrans--;
}
/**
 * Move an item in the transform list down
 * \param iItem index of the item (cannot be the last element of the list)
 */
void sculptor_document::transformDown( unsigned int iItem )
{
    if (iItem >= m_aTransforms.size())
        return;

    vector< sculptor_transform >::iterator iIter;
    sculptor_transform oTmp = m_aTransforms[iItem];

    iIter = m_aTransforms.begin();
    iIter += iItem;
    m_aTransforms.erase( iIter );
    iIter++;
    m_aTransforms.insert( iIter, oTmp );

    if (iItem == m_iCurrentTrans)
        m_iCurrentTrans++;
}

/**
 * get the matrix of the pdb node as double svt_matrix4
 */
svt_matrix4<double>& sculptor_document::getTransformation()
{
    return m_oDoubleTrans;
}
/**
 * update the svt_node transformation matrix with the content of the sculptor internal double matrix
 */
void sculptor_document::updateSVTTransformation()
{
    m_oDoubleTrans.copyToMatrix4f( *getNode()->getTransformation() );
}
/**
 * update the internal double transformation matrix with the content of the svt matrix
 */
void sculptor_document::updateSculptorTransformation()
{
    m_oDoubleTrans.set( *getNode()->getTransformation() );
}
/**
 * Get the match of the current transform
 */
vector<int>* sculptor_document::getCurrentMatch()
{
    return m_aTransforms[m_iCurrentTrans].getMatch();
}
/**
 * Get the target-map ID of the current transform
 */
long int sculptor_document::getCurrentMatchTargetID()
{
    return m_aTransforms[m_iCurrentTrans].getTargetID();
};
/**
 * Get the probe-molecule ID of the current transform
 */
long int sculptor_document::getCurrentMatchProbeID()
{
    return m_aTransforms[m_iCurrentTrans].getProbeID();
};

/**
 * Delete all the match information, but leave the transformations intact
 */
void sculptor_document::delAllMatch()
{
    vector<int>* pMatch;
    for(unsigned int i=0; i<m_aTransforms.size(); i++)
    {
        pMatch = m_aTransforms[i].getMatch();
        if (pMatch)
            pMatch->clear();
    }
}

/**
 * Get a single sample for the clustering
 */
svt_sampled< svt_vector4<Real64> >& sculptor_document::getSampled()
{
    svt_volume<Real64>* pVolume = new svt_volume<Real64>(16,16,16,0.0);

    return *pVolume;
};

/**
 * 
 */
void sculptor_document::showLaplaceQuantizationDlgOptions()
{
    if (m_pClusteringDlg == NULL)
        m_pClusteringDlg = new dlg_clustering( g_pWindow, this );
    else
	m_pClusteringDlg->resetDlg(this);
    
    //if document is a situs document, hide resolution and voxel options
    if ((*this).getType() != SENSITUS_DOC_PDB)
    {
    	m_pClusteringDlg->hideResolutionVoxelOptions();
    }
    
    m_pClusteringDlg->setupLapLaceOptions();
    m_pClusteringDlg->assignMethod(CLUSTER_METHOD_LAPLACE);
    m_pClusteringDlg->show();
};

/**
 * 
 */
void sculptor_document::laplaceQuantization(Real64 fLi, Real64 fLf, Real64 fEi, Real64 fEf, int iMaxStep, unsigned int iCV, bool bTopologyDetermination, Real64 fResolution, Real64 fWidth)
{
    //setup volume to use - if situs document, use volume provided.  If pdb document, use dialog parameters to create volume from pdb
    svt_volume<Real64>* pVol = NULL;

    // in case of a pdb document we have to blur first
    if ( (*this).getType() == SENSITUS_DOC_PDB )
    {
        svt_pdb* pPdbNode = (svt_pdb*)getNode();
	pVol = pPdbNode->blur( fWidth, fResolution );
	pVol->setWidth( fWidth );

	(*pVol).applyLaplacian();
	(*pVol)*(-1.0);
	(*pVol).threshold(0.0);
	(*pVol).normalize();
	(*pVol).setWidth( fWidth );
    }

    // in case of a situs document we have to
    if ( (*this).getType() == SENSITUS_DOC_SITUS )
    {
        pVol = ((situs_document*)this)->getLaplaceVol( );
    }

    // set parameters for the clustering
    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
    oClusterAlgo.setConnOnline( false );
    oClusterAlgo.setLambda( fLi, fLf );
    oClusterAlgo.setEpsilon( fEi, fEf );
    oClusterAlgo.setMaxstep( iMaxStep );
    svt_sgenrand( svt_getToD() );

    // do the clustering
    m_aCodebook = oClusterAlgo.cluster(iCV, 8, *pVol );
    if ( m_aCodebook.size() != 0 && bTopologyDetermination)
        oClusterAlgo.calcConnections( getSampled() );
    m_aCodebook = oClusterAlgo.getCodebook();

    if ((*this).getType() == SENSITUS_DOC_SITUS)
    {
        unsigned int iCV = m_aCodebook.size();
        svt_vector4<Real64> oUncenter( getRealPos() );
        oUncenter *= 1.0E10;

        for(unsigned int i=0; i<iCV; i++)
            m_aCodebook[i] = m_aCodebook[i] - oUncenter;
    } else
        delete pVol;

    updateCodebookRendering();
    resetCodebookID();
    
};

/**
 * 
 */
void sculptor_document::showClusteringDlgOptions( svt_point_cloud_pdb< svt_vector4<Real64> >* rCV )
{
    //make sure we have a pdb or situs document
    if ((*this).getType() != SENSITUS_DOC_SITUS && (*this).getType() != SENSITUS_DOC_PDB )
    {
	svt_exception::ui()->info( "Please select a structure or volume document..." );
	return;
    }

    if (m_pClusteringDlg == NULL)
        m_pClusteringDlg = new dlg_clustering( g_pWindow, this );
    else
	m_pClusteringDlg->resetDlg(this);

    // prevent that a widget has the focus, that is disabled now... FIXME: huh?
    // QWidget* pFocus = instance()->focusWidget();
    // if (pFocus)
    //     pFocus->clearFocus();

    // the following elements are not needed now
    m_pClusteringDlg->hideResolutionVoxelOptions();
   
    // if there is a starting position
    if (rCV != NULL)
    {
        m_pClusteringDlg->setCVStartPos(rCV);

    } 

    // if pdb document cutoff does not make sense
    if ((*this).getType() != SENSITUS_DOC_SITUS)
    {
	m_pClusteringDlg->hideCutoffs();
    }

    m_pClusteringDlg->adjustSizeDlg();
    m_pClusteringDlg->assignMethod(CLUSTER_METHOD_CLUSTERING);
    m_pClusteringDlg->show();
};

/**
 *
 */
void sculptor_document::clustering(Real64 fLi, Real64 fLf, Real64 fEi, Real64 fEf, int iMaxStep, unsigned int iCV, Real64 fTi, Real64 fTf, bool bTopologyDetermination, svt_point_cloud_pdb< svt_vector4<Real64> >* pCV)
{
    //If pCV is NULL, use FAM clustering, otherwise use TRN clustering
    // set parameters for the clustering
    if (pCV != NULL)
    {

        if (pCV->getBondsNum() == 0)
        {
            QMessageBox::warning(0, QString("Feature Extraction"), QString("Connectivity information missing, please re-extract feature-points with topology calculation turned on!"));
            return;
        }

        svt_clustering_fam< svt_vector4<Real64> > oClusterAlgo;
            
        // set parameters for the clustering
        oClusterAlgo.setLambda( fLi, fLf );
        oClusterAlgo.setEpsilon( fEi, fEf );
        oClusterAlgo.setMaxstep( iMaxStep );
        svt_sgenrand( svt_getToD() );
            
        oClusterAlgo.setSystem( *pCV );
        oClusterAlgo.train( getSampled() ); 
        m_aCodebook = oClusterAlgo.getCodebook();
        resetCodebookID();
    }
    else   
    {
        svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
	    
        oClusterAlgo.setConnOnline( false );
        oClusterAlgo.setLambda( fLi, fLf );
        oClusterAlgo.setEpsilon( fEi, fEf );
        oClusterAlgo.setT( fTi, fTf );
        oClusterAlgo.setMaxstep( iMaxStep );
        svt_sgenrand( svt_getToD() );
    
        // do the clustering
        m_aCodebook = oClusterAlgo.cluster( iCV, 8, getSampled() );
        if ( m_aCodebook.size() != 0 && bTopologyDetermination)
            oClusterAlgo.calcConnections( getSampled() );
        m_aCodebook = oClusterAlgo.getCodebook();
        resetCodebookID();
    }

    if ((*this).getType() == SENSITUS_DOC_SITUS)
    {
        unsigned int iCV = m_aCodebook.size();
        svt_vector4<Real64> oUncenter( getRealPos() );
        oUncenter *= 1.0E10;

        for(unsigned int i=0; i<iCV; i++)
            m_aCodebook[i] = m_aCodebook[i] - oUncenter;
    }

    if (m_aCodebook.size() > 0)
        updateCodebookRendering();
    
};

/**
 * update codebook rendering
 */
void sculptor_document::updateCodebookRendering()
{
    unsigned int iCV = m_aCodebook.size();

    // Since everything is deleted,
    // we need to save the CV state before deleting them
    bool bSavedVisibleState = true;

    if (getNode() != NULL)
    {
        if (m_pCodebookSpheres != NULL)
	{
	    bSavedVisibleState = m_pCodebookSpheres->getVisible();
            getNode()->del( m_pCodebookSpheres );
	}

        m_pCodebookSpheres = new svt_node();
        m_pCodebookSpheres->setName( "Codebook-Spheres" );
        m_pCodebookSpheres->setVisible( bSavedVisibleState );
        getNode()->add( m_pCodebookSpheres );
    }

    svt_vector4< Real64 > oVec;
    svt_color oCol( m_fCV_Red, m_fCV_Green, m_fCV_Blue );
    svt_sphere* pSphere;
    vector< svt_point_cloud_bond >& rBonds = m_aCodebook.getBonds();

    for(unsigned int i=0; i<iCV; i++)
    {
	oVec = m_aCodebook[i];
	oVec *= 1.0E-2;

        // codebook spheres
        pSphere = new svt_sphere( new svt_pos( oVec.x(), oVec.y(), oVec.z() ) );
        char aTest[256];
        sprintf(aTest,"Codebook Vector[%i]", i);
        pSphere->setName( aTest );
        pSphere->setRadius( 0.02f );
        pSphere->setProperties( new svt_properties() );
        if ((int)(i) != m_iSelectA && (int)(i)!= m_iSelectB)
            pSphere->getProperties()->setColor( new svt_color( m_fCV_Red, m_fCV_Green, m_fCV_Blue ) );
        else
            pSphere->getProperties()->setColor( new svt_color( 1.0, 1.0, 0.0 ) );
        m_pCodebookSpheres->add( pSphere );

    }

    // bonds
    for(unsigned int i=0; i<iCV; i++)
    {
        vector< unsigned int > aBondList = m_aCodebook.atom(i).getBondList();
        for(unsigned j=0; j<aBondList.size(); j++)
        {
            oVec = m_aCodebook[rBonds[aBondList[j]].getIndexA()];
            oVec *= 1.0E-2;
            svt_pos* oPosA = new svt_pos( oVec.x(), oVec.y(), oVec.z() );
            oVec = m_aCodebook[rBonds[aBondList[j]].getIndexB()];
            oVec *= 1.0E-2;
            svt_pos* oPosB = new svt_pos( oVec.x(), oVec.y(), oVec.z() );

            svt_cylinder* pCyl = new svt_cylinder( oPosA, oPosB, new svt_properties(), 0.01 );
            pCyl->getProperties()->setColor( new svt_color( m_fCV_Red, m_fCV_Green, m_fCV_Blue ) );
            m_pCodebookSpheres->add( pCyl );
        }
    }

    m_pCodebookSpheres->rebuildDL();
};

/**
 * Set the codebook
 * \param reference to svt_point_cloud object with the codebook vectors
 */
void sculptor_document::setCodebook(svt_point_cloud< svt_vector4<Real64> >& rOther)
{
    m_aCodebook = rOther;
    updateCodebookRendering();
};

/**
 * set the color of the codebook vectors
 */
void sculptor_document::setCodebookColor( Real32 fRed, Real32 fGreen, Real32 fBlue )
{
    m_fCV_Red = fRed;
    m_fCV_Green = fGreen;
    m_fCV_Blue = fBlue;

    updateCodebookRendering();
};

/**
 * Set the steric clashing codebook
 * \param reference to svt_point_cloud object with the codebook vectors
 */
void sculptor_document::setCodebookSC(svt_point_cloud< svt_vector4<Real64> >& rOther)
{
    m_aCodebookSC = rOther;
    updateCodebookSCRendering();
};

/**
 * update steric clashing codebook rendering
 */
void sculptor_document::updateCodebookSCRendering()
{
    unsigned int iCV = m_aCodebookSC.size();

    if (getNode() != NULL)
    {
        if (m_pStericSpheres != NULL)
            getNode()->del( m_pStericSpheres );

        m_pStericSpheres = new svt_node();
        m_pStericSpheres->setName( "Steric-Spheres" );
        getNode()->add( m_pStericSpheres );

    }

    svt_vector4< Real64 > oVec;
    svt_color oCol( m_fCV_Red, m_fCV_Green, m_fCV_Blue );
    svt_sphere* pSphere;
    Real64 fRadius = m_aCodebookSC.averageNNDistance( 0.1 );

    for(unsigned int i=0; i<iCV; i++)
    {
	oVec = m_aCodebookSC[i];
	oVec *= 1.0E-2;

        // steric spheres
        pSphere = new svt_sphere( new svt_pos( oVec.x(), oVec.y(), oVec.z() ) );
        pSphere->setRadius( fRadius * 0.01 );
        pSphere->setProperties( new svt_properties() );
        pSphere->getProperties()->setWireframeColor( new svt_color( 1.0, 0.0, 0.0 ) );
        pSphere->getProperties()->setWireframe( true );
        pSphere->getProperties()->setSolid( false );
        m_pStericSpheres->add( pSphere );
	setStericSpheresVisible( i, false );
    }

    m_pStericSpheres->rebuildDL();
    setStericSpheresVisible( false );
};

/**
 * 
 */
QWidget* sculptor_document::createPropDlg(QWidget*)
{
    return NULL;
}

/**
 * 
 */
QWidget* sculptor_document::getPropDlg()
{
    return NULL;
}

/**
 * Get a representative color for the document
 * \return pointer to svt_color object
 */
svt_color sculptor_document::getColor()
{
    return svt_color( 1.0, 1.0, 1.0 );
}

/**
 * Get the size of the document
 * \return svt_vector4 with the three dimensions of the size of the document
 */
svt_vector4<Real32> sculptor_document::getBoundingBox()
{
    return svt_vector4<Real32>(0,0,0);
};

/**
 * A codebook vector was selected
 */
void sculptor_document::selectCodebookVector( svt_node* pNode )
{
    if (m_pCodebookSpheres != NULL)
    {
        svt_node* pCurrent = m_pCodebookSpheres->getSon();
        int iCount = 0;
        int iSelected = -1;

        while(pCurrent)
        {
            if (pNode == pCurrent)
                iSelected = iCount;

            pCurrent = pCurrent->getNext();
            iCount++;
        }
        if (iSelected == -1 || iSelected > (int)(m_aCodebook.size()))
            return;

        if (iSelected != m_iSelectB && iSelected != m_iSelectA)
        {
            m_iSelectA = m_iSelectB;
            m_iSelectB = iSelected;

            // OK, do we have two selections?
            if ( (m_iSelectA != -1 && m_iSelectB != -1 ) && (m_iSelectA != m_iSelectB) && (m_iSelectA < m_iSelectB) )
            {
                if ( m_aCodebook.isBond( (unsigned int)(m_iSelectA), (unsigned int)(m_iSelectB) ) )
                    m_aCodebook.delBond( (unsigned int)(m_iSelectA), (unsigned int)(m_iSelectB) );
                else
                    m_aCodebook.addBond( (unsigned int)(m_iSelectA), (unsigned int)(m_iSelectB) );

                m_iSelectA = -1;
                m_iSelectB = -1;
            }
            if ( (m_iSelectA != -1 && m_iSelectB != -1 ) && (m_iSelectA != m_iSelectB) && (m_iSelectB < m_iSelectA) )
            {
                if ( m_aCodebook.isBond( (unsigned int)(m_iSelectB), (unsigned int)(m_iSelectA) ) )
                    m_aCodebook.delBond( (unsigned int)(m_iSelectB), (unsigned int)(m_iSelectA) );
                else
                    m_aCodebook.addBond( (unsigned int)(m_iSelectB), (unsigned int)(m_iSelectA) );

                m_iSelectA = -1;
                m_iSelectB = -1;
            }

            updateCodebookRendering();
        }
    }
}
/**
 * De-Select all codebook vectors
 */
void sculptor_document::clearCVSelection()
{
    m_iSelectA = -1;
    m_iSelectB = -1;
    updateCodebookRendering();
}

/**
 * Return an array with all svt_node-IDs of the codebook vector spheres
 */
vector< int > sculptor_document::getCodebookIDs()
{
    vector< int > aIDs;

    if ( m_pCodebookSpheres )
    {
	svt_node* pCurrent = m_pCodebookSpheres->getSon();
	int iCount = 0;

        while(pCurrent && iCount < (int)(m_aCodebook.size()))
	{
	    aIDs.push_back( pCurrent->getNodeID() );
    
	    pCurrent = pCurrent->getNext();
	    iCount++;
	}
    }
    return aIDs;
};

///////////////////////////////////////////////////////////////////////////////
// Interactive global docking
///////////////////////////////////////////////////////////////////////////////

/**
 * set document ID
 */
void sculptor_document::setDocID(unsigned long iDocID)
{
    m_iDocID = iDocID;
}
 
/**
 * get document ID
 */
unsigned long sculptor_document::getDocID()
{
    return m_iDocID;
}
     
/**
 * set pointer to original probe
 */
void sculptor_document::setOrigProbeDoc(sculptor_document* pOrigProbe)
{
    m_pOrigProbeDoc = pOrigProbe;
}
 
/**
 * get pointer to original probe
 */
sculptor_document* sculptor_document::getOrigProbeDoc()
{
    return m_pOrigProbeDoc;
}
     
/**
 * set Eliquos cross-correlation map file name
 */
void sculptor_document::setEliCrossCorrFile(QString sFilename)
{
#ifndef WIN32
    // if not under Windows, the backslashes need to be exchanged
    sFilename.replace("\\", "/");

    // if not under Windows, the drive letters need to be removed
    if (sFilename[1] == ':')
        sFilename = sFilename.right( sFilename.length() - 2 );
#else
    // if under Windows, the slashes need to be exchanged with backslashes
    sFilename.replace("/", "\\");
#endif

    m_sEliCrossCorrFile = sFilename;
}
/**
 * get Eliquos cross-correlation map file name
 */
QString sculptor_document::getEliCrossCorrFile()
{
    return m_sEliCrossCorrFile;
};

/**
 * set Eliquos angle index map file name
 */
void sculptor_document::setEliAngleIndexFile(QString sFilename)
{
#ifndef WIN32
    // if not under Windows, the backslashes need to be exchanged
    sFilename.replace("\\", "/");

    // if not under Windows, the drive letters need to be removed
    if (sFilename[1] == ':')
        sFilename = sFilename.right( sFilename.length() - 2 );
#else
    // if under Windows, the slashes need to be exchanged with backslashes
    sFilename.replace("/", "\\");
#endif

    m_sEliAngleIndexFile = sFilename;
}

/**
 * get Eliquos angle index map file name
 */
QString sculptor_document::getEliAngleIndexFile()
{
    return m_sEliAngleIndexFile;
};

/**
 * set Eliquos angle list file name
 */
void sculptor_document::setEliAngleListFile(QString sFilename)
{
#ifndef WIN32
    // if not under Windows, the backslashes need to be exchanged
    sFilename.replace("\\", "/");

    // if not under Windows, the drive letters need to be removed
    if (sFilename[1] == ':')
        sFilename = sFilename.right( sFilename.length() - 2 );
#else
    // if under Windows, the slashes need to be exchanged with backslashes
    sFilename.replace("/", "\\");
#endif

    m_sEliAngleListFile = sFilename;
}

/**
 * get Eliquos angle list file name
 */
QString sculptor_document::getEliAngleListFile()
{
    return m_sEliAngleListFile;
};
 
/**
 * load Eliquos cross-correlation map
 */
bool sculptor_document::loadEliCrossCorr()
{
    QFileInfo oFileInf( m_sEliCrossCorrFile );
    if (!oFileInf.exists())
    {
        QString oFilename = oFileInf.fileName();
        m_sEliCrossCorrFile = QString(g_pWindow->getCurrentDir().c_str()) + QDir::separator() + oFilename;
    }
    m_cEliCrossCorr.load(QtoStd(m_sEliCrossCorrFile).c_str());
    if (m_cEliCrossCorr.size() > 0) 
      return true;
    else
      return false;
}

/**
 * load Eliquos angle index map
 */
bool sculptor_document::loadEliAngleIndex()
{
    QFileInfo oFileInf( m_sEliAngleIndexFile );
    if (!oFileInf.exists())
    {
        QString oFilename = oFileInf.fileName();
        m_sEliAngleIndexFile = QString(g_pWindow->getCurrentDir().c_str()) + QDir::separator() + oFilename;

        svtout << "DEBUG: m_sEliAngleIndexFile: " << QtoStd(m_sEliAngleIndexFile) << endl;
    }
    m_cEliAngleIndex.load(QtoStd(m_sEliAngleIndexFile).c_str());
    if (m_cEliAngleIndex.size() > 0) 
      return true;
    else
      return false;
}

/**
 * load Eliquos Euler angle list
 */
bool sculptor_document::loadEliAngleList()
{
    QFileInfo oFileInf( m_sEliAngleListFile );
    if (!oFileInf.exists())
    {
        QString oFilename = oFileInf.fileName();
        m_sEliAngleListFile = QString(g_pWindow->getCurrentDir().c_str()) + QDir::separator() + oFilename;

        svtout << "DEBUG: m_sEliAngleListFile: " << QtoStd(m_sEliAngleListFile) << endl;
    }

    svtout << "Loading Euler angle list" << endl;
    FILE  *pEliAngleListFile = fopen(QtoStd(m_sEliAngleListFile).c_str(), "r");
    if (pEliAngleListFile == NULL) {
      svtout << "Could not open Euler angle file" << endl;
      QMessageBox::warning(0, QString("Could not load angle index file"), QString(m_sEliAngleListFile + " is missing or empty"));
      return false;
    }
    while (!feof(pEliAngleListFile)) {
      EULER_ANGLE angle;
      if (fscanf(pEliAngleListFile,"%lf %lf %lf\n",&angle.psi,
	                                           &angle.theta,
						   &angle.phi) != 3) break;
      m_cEliAngleList.push_back(angle);
    }
    fclose(pEliAngleListFile);
    svtout << "  " << m_cEliAngleList[0].psi << " "
                   << m_cEliAngleList[0].theta << " "
                   << m_cEliAngleList[0].phi << endl;
    svtout << "  " << m_cEliAngleList[1].psi << " "
                   << m_cEliAngleList[1].theta << " "
                   << m_cEliAngleList[1].phi << endl;
    svtout << "  " << m_cEliAngleList[2].psi << " "
                   << m_cEliAngleList[2].theta << " "
                   << m_cEliAngleList[2].phi << endl;
    svtout << "  ..." << endl;
    svtout << endl;
    return true;
}

/**
 * is IGD turned on?
 */
bool sculptor_document::getIGDActive()
{
    return m_bIGDOn;
}

/**
 * set IGD on/off
 */
void sculptor_document::setIGDActive(bool bIGDOn)
{
    m_bIGDOn = bIGDOn;
    return;
}

/**
 *  set the status of the m_bIDGWarningShown
 *  \param bool to indicate whether the IDGWarning was already shown or not
 */
void sculptor_document::setIGDWarningShown( bool bIGDWarningShown )
{
    m_bIGDWarningShown = bIGDWarningShown;
};
    
/**
 *  get the status of the m_bIDGWarningShown
 *  \return bool to indicate whether the IDGWarning was already shown or not
 */
bool sculptor_document::getIGDWarningShown()
{
    return m_bIGDWarningShown;
};



/**
 * set Eliquos data loaded flag
 */
void sculptor_document::setEliDataLoaded(bool flag)
{
    m_bEliDataLoaded = flag;
}

/**
 * is Eliquos data loaded?
 */
bool sculptor_document::getEliDataLoaded()
{
    return m_bEliDataLoaded;
}
    
/**
 * Get pointer to cross-correlation volume
 */
svt_volume<Real64>* sculptor_document::getEliCrossCorrVolume()
{
    return &m_cEliCrossCorr;
}

/**
 * Get pointer to angle index volume
 */
svt_volume<Real64>* sculptor_document::getEliAngleIndexVolume()
{
    return &m_cEliAngleIndex;
}

/**
 * Get pointer to angle list
 */
vector<EULER_ANGLE>* sculptor_document::getEliAngleList()
{
    return &m_cEliAngleList;
}

/**
 * Get the ID of the current codebook. Codebook-vectors are often utilized to relate objects with each other, e.g. for a multi-scale docking.
 * If the codebook gets regenerated, all the relationships that are stored elsewhere are invalid and need to be recalculated aswell. Therefore the codebook has a unique ID
 * which one can use to keep track if it remains unchanged. If the codebook gets regenerated, even if the number of vectors stay the same, it receives a new ID.
 */
long int sculptor_document::getCodebookID()
{
    return m_iCodebookID;
};
/**
 * Regenerate the codebook ID. It should not be necessary to call this function.
 */
void sculptor_document::resetCodebookID()
{
    m_iCodebookID = svt_getToD();
};

/**
 * document was changed. This function sets the internal state to "changed". It will automatically get un-set if the document is saved.
 */
void sculptor_document::setChanged()
{
    m_bChanged = true;

    // if this document has an entry in the documentlist, let the documentlist know that this
    // document changed
    if ( getDLItem() != NULL )
	getDLItem()->showChanged();
};
/**
 * was the document changed?
 * \return true if the document was changed
 */
bool sculptor_document::getChanged()
{
    return m_bChanged;
}

/**
 * set locked
 */
void sculptor_document::setLocked(bool bLocked)
{
    m_bLocked = bLocked;

    if ( m_pDLItem != NULL)
        m_pDLItem->showLocked();
};

/**
 * get locked
 * \return whether the document is locked
*/
bool sculptor_document::getLocked()
{
    return m_bLocked;
};

/**
 *
 */
window_documentlistitem* sculptor_document::getDLItem()
{
    return m_pDLItem;
}

/**
 *
 */
void sculptor_document::setDLItem(window_documentlistitem* pDLItem)
{
    m_pDLItem = pDLItem;
}
