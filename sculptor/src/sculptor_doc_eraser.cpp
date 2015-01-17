/***************************************************************************
                          sculptor_doc_eraser.cpp  -  description
                             -------------------
    begin                : 30.07.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////


// sculptor includes
#include <situs_document.h>
#include <sculptor_doc_eraser.h>
#include <sculptor_prop_eraser.h>
#include <sculptor_window.h>

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);

/**
 *
 */
sculptor_doc_eraser::sculptor_doc_eraser(svt_node* pNode, situs_document* pTargetDoc)
    : sculptor_document( pNode ),
      m_pEraserPropDlg( NULL ),
      m_pEraserTarget( pTargetDoc ),
      m_pTargetVolume( m_pEraserTarget->getVolume() ),
      m_pEraserVolume( new svt_volume<Real64>() ),
      m_fOpacity( 0.0 ),
      m_bInvert( false ),
      m_iEraserShape( SCULPTOR_ERASER_TOOL_SPHERE ),
      m_iEraserSize( 20 )
    //  m_iEraserMethod( SCULPTOR_ERASER_TOOL_METHOD_MODIFY)
{
    //create eraser node and add it to the graphnode
    m_pEraserNode = new svt_node();
    m_pEraserNode->setName("(sculptor_doc_eraser)m_pEraserNode");
    m_pGraphNode->add(m_pEraserNode);

    //set eraser volume width same as target volume width
    setEraserWidth();

    //initialize marching cube
    m_pEraserVolumeMC = new svt_volume_compact<Real64>( *m_pEraserVolume );
    m_pEraserVolumeMC->setName( "(sculptor_doc_eraser)m_pEraserVolumeMC" );
    //setup visualization parameters of MC
    m_pEraserVolumeMC->setVisible(false);
    m_pEraserVolumeMC->getCont()->setProperties(new svt_properties(new svt_color( 255.0f/255.0f, 105.0f/255.0f,  180.0f/255.0f )));
    svt_color* pColor = m_pEraserVolumeMC->getCont()->getProperties()->getColor();
    pColor->setTransparency(0.6);
    m_pEraserNode->add(m_pEraserVolumeMC);
    m_pEraserVolumeMC->setStep(1);
    m_pEraserVolumeMC->setVisible(true);

    //assign size of eraser to the eraser volume
    sSetEraserSize(m_iEraserSize);

}

/**
 *
 */
sculptor_doc_eraser::~sculptor_doc_eraser()
{
    //deletion of property dialog should be done by window_documentlistitem
    delete m_pEraserVolume;
}

/**
 *
 */
svt_node* sculptor_doc_eraser::getNode()
{
    return m_pEraserNode;
}

///////////////////////////////////////////////////////////////////////////////
// properties dlg
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
QWidget* sculptor_doc_eraser::createPropDlg(QWidget* pParent)
{
    if (m_pEraserPropDlg == NULL)
        m_pEraserPropDlg = new prop_eraser( this, pParent );

    return (QWidget*)m_pEraserPropDlg;
}

/**
 *
 */
QWidget* sculptor_doc_eraser::getPropDlg()
{
    return (QWidget*)m_pEraserPropDlg;
}



/**
 *
 */
void sculptor_doc_eraser::setupCube(svt_volume<Real64>* oVol, Real32 fVoxelValue)
{
    for (unsigned int iX = 1; iX < getEraserVolumeSizeX()-1; iX++)
    {
	for (unsigned int iY = 1; iY < getEraserVolumeSizeY()-1; iY++)
	{
	    for (unsigned int iZ = 1; iZ < getEraserVolumeSizeZ()-1; iZ++)
	    {
		oVol->setValue(iX, iY, iZ, fVoxelValue);
	    }
	}
    }
}

/**
 *
 */
void sculptor_doc_eraser::setupSphere(svt_volume<Real64>* oVol, Real32 fVoxelValue)
{
    Real64 fCenter;
    Real64 fDist;
    svt_vector4<Real64> oCenter;
    svt_vector4<Real64> oVoxel;

    fCenter = ((getEraserVolumeSizeX()-1.0)/2.0);
    oCenter.x(fCenter);
    oCenter.y(fCenter);
    oCenter.z(fCenter);

    for (unsigned int iX = 0; iX < getEraserVolumeSizeX(); iX++)
    {
        for (unsigned int iY = 0; iY < getEraserVolumeSizeY(); iY++)
	{
	    for (unsigned int iZ = 0; iZ < getEraserVolumeSizeZ(); iZ++)
	    {
                oVoxel.x( (Real64) iX );
                oVoxel.y( (Real64) iY );
                oVoxel.z( (Real64) iZ );
                fDist = oVoxel.distance(oCenter);
 
		if ( fDist < fCenter)
		{ 
	            oVol->setValue(iX, iY, iZ, fVoxelValue);
		}
            }       
        }     
    }
}

/**
 *
 */
void sculptor_doc_eraser::updateVolumeDisplay()
{
    m_pEraserVolumeMC->setIsolevel( m_pEraserVolume->getMaxDensity() );
    m_pEraserVolumeMC->setScale(getTargetWidth()*(double)(getEraserVolumeSizeX())*1.0E-2,
                                getTargetWidth()*(double)(getEraserVolumeSizeY())*1.0E-2,
                                getTargetWidth()*(double)(getEraserVolumeSizeZ())*1.0E-2);

    m_pEraserVolumeMC->recalcMesh();
    m_pEraserVolumeMC->rebuildDL();
}

/**
 *
 */
int sculptor_doc_eraser::getEraserSize()
{
    return m_iEraserSize;
}


/**
 *
 */
void sculptor_doc_eraser::sSetEraserSize(int iSize)
{
    m_pEraserVolume->allocate(iSize, iSize, iSize);
    updateEraserTool();
}

/**
 *
 */
void sculptor_doc_eraser::updateEraserTool()
{
    m_pEraserVolume->setValue(0.0);

    switch ( m_iEraserShape )
    {
        case SCULPTOR_ERASER_TOOL_CUBE:
            setupCube(m_pEraserVolume);
            break;

        case SCULPTOR_ERASER_TOOL_SPHERE:
            setupSphere(m_pEraserVolume);
            break;
    }

    updateVolumeDisplay();
}

/**
 *
 */
bool sculptor_doc_eraser::getInvertSetting()
{
    return m_bInvert;
}

/**
 *
 */
void sculptor_doc_eraser::sSetInverted(bool bInvert)
{
    m_bInvert = bInvert;
}

/**
 *
 */
int sculptor_doc_eraser::getEraserOpacityGUI()
{
    return (int)(m_fOpacity*100);
}

/**
 *
 */
void sculptor_doc_eraser::sSetOpacity(int iFactor)
{
    m_fOpacity = (Real32)iFactor/100.0;
}

/*
 *
 */

/*void sculptor_doc_eraser::sSetEraserMethod(int iMethod)
{
   m_iEraserMethod = iMethod;
}
*/

/**
 *
 */
void sculptor_doc_eraser::sEraseRegion()
{
/* TO INCLUDE SOON
    if (m_iEraserMethod == SCULPTOR_ERASER_TOOL_METHOD_COPYMODIFY)
    {
        int iTargetIndex = m_pEraserTarget->getDLItem()->getIndex();

        // copy to pCopiedDoc
        //
        svt_volume<Real64>* pCopyVolume = new svt_volume<Real64>(*m_pTargetVolume);
        situs_document* pCopiedTarget = new situs_document(m_pEraserTarget->getNode(), " ", pCopyVolume);

        QString oDisplayName = QString(m_pEraserTarget->getDisplayName());
        oDisplayName.append(" (Copy)");
        pCopiedDoc->setDisplayName(QtoStd(oDisplayName).c_str());


        // do same thing for filename
        // do same thing for filename_SRCB (if != NONE)

        QString oFileName = QString(m_pEraserTarget->getFileName());
        oFileName.append("_Copy");
        pCopiedDoc->setFileName(QtoStd(oFileName).c_str());


        window_documentlistitem* pDLI = new window_documentlistitem(pCopiedDoc, true, false, true);
        g_pWindow->getDocumentList->insertItem(pDLI, iTargetIndex+1);
    }

*/

    svtout << "Erasing....." << endl;
    //get eraser node transformation
    svt_matrix4<Real32> oMatEraserTransformation = svt_matrix4<Real32>(*(m_pEraserNode->getTransformation()));

    //fXOptimizedSearch, fYOptimizedSearch, fZOptimizedSearch used for reducing search space of target volume
    Real32 fXOptimizeSearch, fYOptimizeSearch, fZOptimizeSearch; 
    fXOptimizeSearch = oMatEraserTransformation.translationX()*1.0E2 - getTargetGridX();
    fYOptimizeSearch = oMatEraserTransformation.translationY()*1.0E2 - getTargetGridY();
    fZOptimizeSearch = oMatEraserTransformation.translationZ()*1.0E2 - getTargetGridZ();
    
   // svtout << "Optimized fX,fY,fZ: " << fXOptimizeSearch << ", " << fYOptimizeSearch << ", " << fZOptimizeSearch << endl;

    //fX,fY,fZAdjustNode correspond to the adjustment of the node at the center of the eraser volume, this can be either positive or negative
    Real32 fXAdjustNode = getTargetWidth() * 0.5 * (getEraserVolumeSizeX()-1);
    Real32 fYAdjustNode = getTargetWidth() * 0.5 * (getEraserVolumeSizeY()-1);
    Real32 fZAdjustNode = getTargetWidth() * 0.5 * (getEraserVolumeSizeZ()-1);
    
  //  svtout << "AdjustNode fX,fY,fZ: " << fXAdjustNode << ", " << fYAdjustNode << ", " << fZAdjustNode << endl;  

    //fStartEraserX,Y,Z correspond to the original (smallest) float starting position of an easer (takes into account the eraser volume)
    //fEndEraserX,Y,Z correspond to the opposite - the largest float, end point an eraser volume of its size can possibly have
    //fEndTargetX,Y, Z correspond to the very end of the target volume accounting for targetwidth.
    //iStartX,Y,Z (to be defined later) correspond to the actual starting voxels in the target volume we want to look
    //iEndX,Y,Z correspond to end voxels we want to look within the target volume
    Real32 fStartEraserX, fStartEraserY, fStartEraserZ, fEndEraserX, fEndEraserY, fEndEraserZ, fEndTargetX, fEndTargetY, fEndTargetZ;
    unsigned int iStartX, iStartY, iStartZ, iEndX, iEndY, iEndZ;

    fStartEraserX = fXOptimizeSearch - fXAdjustNode;
    fStartEraserY = fYOptimizeSearch - fYAdjustNode;
    fStartEraserZ = fZOptimizeSearch - fZAdjustNode;

   // svtout << "fStartEraserX, Y, Z: " << fStartEraserX << ", " << fStartEraserY << ", " << fStartEraserZ << endl;

    fEndEraserX = fXOptimizeSearch + fXAdjustNode;
    fEndEraserY = fYOptimizeSearch + fYAdjustNode;
    fEndEraserZ = fZOptimizeSearch + fZAdjustNode;

   // svtout << "fEndEraserX, Y, Z: " << fEndEraserX << ", " << fEndEraserY << ", " << fEndEraserZ << endl;

    fEndTargetX = getTargetWidth() * (m_pTargetVolume->getSizeX() - 1);
    fEndTargetY = getTargetWidth() * (m_pTargetVolume->getSizeY() - 1);
    fEndTargetZ = getTargetWidth() * (m_pTargetVolume->getSizeZ() - 1);

    //check to see if we're outside of the target volume completely.  This can be known given the size of the eraser and starting/ending positions
    //of each x,y,z eraser coordinate and target map coordinate.  If we're outside, simply report outside.  Don't do any calculations.
    if (fEndEraserX < 0 || fStartEraserX > fEndTargetX || fEndEraserY < 0 || fStartEraserY > fEndTargetY || fEndEraserZ < 0 || fStartEraserZ > fEndTargetZ )
    {
   	svtout << "Eraser outside of target volume." << endl;
        return;
    }


    //invert translation
    oMatEraserTransformation.invert();

    //create a copy of the eraser volume
    svt_volume<Real64>* oEraserCopy = new svt_volume<Real64>(*m_pEraserVolume);
  
    //setup proper opacity value if invert is selected
    Real32 fOpacity;
    if (m_bInvert)
    {
	fOpacity = 1.0 - m_fOpacity;
    }
    else
    {
	fOpacity = m_fOpacity;
    }
    //setup the given opacity for values of voxels in eraser.
    switch (m_iEraserShape)
    {
        case SCULPTOR_ERASER_TOOL_CUBE:
            oEraserCopy->setValue(fOpacity);
            break;

        case SCULPTOR_ERASER_TOOL_SPHERE:
	    if (m_bInvert)
            {
		oEraserCopy->setValue(0.0);
            }
	    else
	    {
	        oEraserCopy->setValue(1.0);
	    }
            setupSphere(oEraserCopy, fOpacity);
            break;
    }
    //set the appropriate starting x value.  Since invert sets all other voxels to zero, this method iterates over all voxels
    if (m_bInvert)
    {
	iStartX = 0;
	iEndX = m_pTargetVolume->getSizeX()-1;
    }
    else
    {
   
        if (fStartEraserX < 0)
            iStartX = 0;
        else
            iStartX = (int)(fStartEraserX/getTargetWidth());
    
        if (fEndEraserX > fEndTargetX)
	    iEndX = (int)(fEndTargetX/getTargetWidth());
        else
  	    iEndX = (int)(fEndEraserX/getTargetWidth());

    }

   
    //used to store positions of each voxel in the target volume
    Real32 fX, fY, fZ;     
    //establish a grid location for each voxel of the target volume
    while (iStartX <= iEndX)
    {
        //  svtout << "iStartX: " << iStartX << endl;
	fX = getTargetGridX() + iStartX * getTargetWidth();

        //start appropriate y value
	if (m_bInvert)
	{
	    iStartY = 0;
	    iEndY = m_pTargetVolume->getSizeY()-1;
	}
	else
	{

	    if (fStartEraserY < 0)
                iStartY = 0;
    	    else
                iStartY = (int)(fStartEraserY/getTargetWidth());
    
    	    if (fEndEraserY > fEndTargetY)
	        iEndY = (int)(fEndTargetY/getTargetWidth());
    	    else
  	        iEndY = (int)(fEndEraserY/getTargetWidth());
	}

	while (iStartY <= iEndY)
	{
	    fY = getTargetGridY() + iStartY * getTargetWidth();
 	    //start appropriate z value
	    if (m_bInvert)
	    {
		iStartZ = 0;
		iEndZ = m_pTargetVolume->getSizeZ()-1;
	    }
	    else
	    {
  	        if (fStartEraserZ < 0)
        	    iStartZ = 0;
    	        else
        	    iStartZ = (int)(fStartEraserZ/getTargetWidth());
    
    	        if (fEndEraserZ > fEndTargetZ)
		    iEndZ = (int)(fEndTargetZ/getTargetWidth());
    	        else
  		    iEndZ = (int)(fEndEraserZ/getTargetWidth());
	    }

	    while (iStartZ <= iEndZ)
	    {
	    	fZ = getTargetGridZ() + iStartZ * getTargetWidth();
		//set that grid location as a vector and scale it
		svt_vector4<Real32> oVecTargetVoxel = svt_vector4<Real32>( fX, fY, fZ);
		oVecTargetVoxel*= 1.0E-2;
		//generate a vector corresponding to the eraser voxel position based on the matrix of node * vector of target and scale
		svt_vector4<Real32> oVecEraserPos = oMatEraserTransformation * oVecTargetVoxel;
  		oVecEraserPos*= 1.0E2;
		   
		    
		//create a bounding box that updates values of the target volume only inside the eraser volume.  This bounding box must take into account the node being in the center of the eraser, so to compensate, we adjust the bounding box by negative 1/2 the eraser volume (illustrated with fAdjustNodes)
		if ((oVecEraserPos.x() >= -fXAdjustNode) && (oVecEraserPos.x() <= fXAdjustNode)
		   && (oVecEraserPos.y() >= -fYAdjustNode) && (oVecEraserPos.y() <= fYAdjustNode)
 		   && (oVecEraserPos.z() >= -fZAdjustNode) && (oVecEraserPos.z() <= fZAdjustNode))
		{ 
		    //at that position, multiply the target volume voxel * the realspace value of our eraser copy to give us our new voxel value
		    m_pTargetVolume->setAt( iStartX, iStartY, iStartZ, m_pTargetVolume->at(iStartX, iStartY, iStartZ) * oEraserCopy->getRealSpaceValue(oVecEraserPos.x() + fXAdjustNode , oVecEraserPos.y() + fYAdjustNode, oVecEraserPos.z() + fZAdjustNode));
		}
		   
		else
		{
		    //only used for invert.  If we aren't inside the bounding box, then delete.  Everything outside eraser is set to zero.
		    if(m_bInvert)
		    {
		        m_pTargetVolume->setAt(iStartX, iStartY, iStartZ, 0.0);
		    }
		}
		iStartZ++;
	    }
	    iStartY++;
 	}
        iStartX++;
    }
    //update the change
    m_pEraserTarget->dataChanged();
    delete oEraserCopy;
}

/**
 *
 */
Real64 sculptor_doc_eraser::getTargetWidth()
{
    return m_pTargetVolume->getWidth();
}

/**
 *
 */
void sculptor_doc_eraser::setEraserWidth()
{
    m_pEraserVolume->setWidth( getTargetWidth() );
}

/**
 *
 */
void sculptor_doc_eraser::sSetEraserShape(int iShapeId)
{
    m_iEraserShape = iShapeId;
    updateEraserTool();
}

/**
 *
 */
int sculptor_doc_eraser::getEraserShape()
{
    return m_iEraserShape;
}

/**
 *
 */
Real32 sculptor_doc_eraser::getTargetGridX()
{
    return m_pTargetVolume->getGridX();
}

/**
 *
 */
Real32 sculptor_doc_eraser::getTargetGridY()
{
     return m_pTargetVolume->getGridY();
}

/**
 *
 */
Real32 sculptor_doc_eraser::getTargetGridZ()
{
    return m_pTargetVolume->getGridZ();
}

/**
 *
 */
unsigned int sculptor_doc_eraser::getEraserVolumeSizeX()
{
    return m_pEraserVolume->getSizeX();
}

/**
 *
 */
unsigned int sculptor_doc_eraser::getEraserVolumeSizeY()
{
    return m_pEraserVolume->getSizeY();
}

/**
 *
 */
unsigned int sculptor_doc_eraser::getEraserVolumeSizeZ()
{
    return m_pEraserVolume->getSizeZ();
}

/**
 *
 */
int sculptor_doc_eraser::getType()
{
    return SENSITUS_DOC_ERASER;
};
