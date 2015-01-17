#include<sculptor_codebook_displacement.h>
#include<svt_container.h>
#include<svt_arrow.h>
/**
 * Constructor
 */
sculptor_codebook_displacement::sculptor_codebook_displacement(
    sculptor_document *pTailDoc, sculptor_document *pHeadDoc, svt_vector4<Real64> oOrigin, svt_node* pTopNode,
    Real32 fSphereRadius, Real32 fCylinderRadius, Real32 fConeRadius, Real32 fRho) : svt_node(),
	m_pTailDoc( NULL ),
	m_pHeadDoc( NULL ),
        m_fSphereRadius( fSphereRadius ),
        m_fCylinderRadius( fCylinderRadius ),
	m_fConeRadius( fConeRadius ),
        m_fRho( fRho ),
	m_pCylinderColor( new svt_color(1,1,1)),
	m_pConeColor( new svt_color(1,1,1)),
	m_pSphereColor( new svt_color(1,1,1)),
        m_iSteps( 25 ),
	m_bAjustDimensions( false )
{
    m_pCont = new svt_container();
    add(m_pCont);
    
    m_pTopNode = pTopNode;
    m_pTopNode->add(this);
   
    setOrigin( oOrigin ); 
    setHead( pHeadDoc );
    setTail( pTailDoc );

    svt_color * pColor = new svt_color(0.75f,0.0f,0.0f);
    setColor(pColor);

    this->drawGL();
}
	
/**
 * Destructor
 */
sculptor_codebook_displacement::~sculptor_codebook_displacement(){};

/**
 *set the Tail Document
 */
 void sculptor_codebook_displacement::setTail(sculptor_document *pTailDoc)
{
    m_pTailDoc = pTailDoc;  

    if (m_pTailDoc != NULL)
        m_pTailDoc->getCodebookRealPos(m_pTailCV, m_oOrigin);
}
        
/**
 *set the Head Document
 */
void sculptor_codebook_displacement::setHead(sculptor_document *pHeadDoc)
{
    m_pHeadDoc = pHeadDoc;
    
    if (m_pHeadDoc != NULL)
        m_pHeadDoc->getCodebookRealPos(m_pHeadCV, m_oOrigin);

}

/**
 * set origin
 */
void sculptor_codebook_displacement::setOrigin(svt_vector4<Real64> oOrigin)
{
    m_oOrigin = oOrigin;
    setPos(new svt_pos(m_oOrigin.x(),m_oOrigin.y(),m_oOrigin.z()));
    
    if (m_pTailDoc != NULL)
        m_pTailDoc->getCodebookRealPos(m_pTailCV, m_oOrigin);
    
    if (m_pHeadDoc != NULL)
        m_pHeadDoc->getCodebookRealPos(m_pHeadCV, m_oOrigin);

}
	
/**
 * Add Geometrical object: Arrows (Sphere, Cylinder and Closed Cone) for each pair of vectors 
 */
void sculptor_codebook_displacement::addArrows()
{
    svt_vector4<Real32> pTail;
    svt_vector4<Real32> pHead;
    svt_vector4<Real32> pDisp;

    m_pCont->delAll();
    
    Real32 fDist;
    
    if (m_oMatch.size()!=m_pTailCV.size())
    {
        char pErrorMsg[1256];
        
        sprintf(pErrorMsg, "Mismatch between the feature point count (%d) and output of the matching algo (%d)!", m_pTailCV.size(),(int)m_oMatch.size());
        svt_exception::ui()->info(pErrorMsg);
        return;
    }
    
    //svt_arrow *pArrow;
    for (unsigned int iIndexCdbk = 0; iIndexCdbk < m_pTailCV.size(); iIndexCdbk++)
    {
	if (m_bAjustDimensions)
	{
	    fDist = m_pTailCV[iIndexCdbk].distance(m_pHeadCV[iIndexCdbk]);
	    m_fSphereRadius     = fDist*(1.0f-m_fRho)/3.0f;
	    m_fCylinderRadius   = fDist*(1.0f-m_fRho)/4.0f;
	    m_fConeRadius       = fDist*(1.0f-m_fRho)/2.0f;
	    
	    // if the distance is to small them the arrows are to small: set them specific parameters to make them bigger  
	    if ( m_fSphereRadius < 1.2f )
	    {
		m_fSphereRadius     = 1.2f;
		m_fCylinderRadius   = 0.75f;
		m_fConeRadius       = 1.7f;
		m_fRho = 0.5f;
	    }
	}
        
        if (m_oMatch.size()>iIndexCdbk && m_oMatch[iIndexCdbk] < (int)m_pHeadCV.size() )
        {
            // *1e-2 to transform that in GL coordinates
            pTail.set(m_pTailCV[iIndexCdbk].x()*1.0e-2f,
                      m_pTailCV[iIndexCdbk].y()*1.0e-2f,
                      m_pTailCV[iIndexCdbk].z()*1.0e-2f);
            pHead.set(m_pHeadCV[ m_oMatch[iIndexCdbk] ].x()*1.0e-2f,
                      m_pHeadCV[ m_oMatch[iIndexCdbk] ].y()*1.0e-2f, 
                      m_pHeadCV[ m_oMatch[iIndexCdbk] ].z()*1.0e-2f);
            
            pDisp = pHead - pTail;
            pDisp.normalize();
            pDisp *=1.0e-2f*1.5f;
	    
	    //pArrow = new svt_arrow( new svt_pos( pTail.x(), pTail.y(), pTail.z() ) );
	    //pArrow->setHeight( pDisp.length() );
	    //pArrow->setTransformation( new svt_matrix4<Real64> );
	    //m_pCont->add(pArrow);
            
            m_pCont->addSphere(pTail.x(), pTail.y(), pTail.z(), m_fSphereRadius*1.0e-2f, *m_pSphereColor);
            m_pCont->addCylinder(pTail, pHead, m_fCylinderRadius*1.0e-2f, *m_pCylinderColor, m_iSteps);
            m_pCont->addCone(pHead, pHead+m_fSphereRadius*pDisp, m_fConeRadius*1.0e-2f, *m_pConeColor, m_iSteps);
        }
    }
}

/**
 * DrawGL
 */
void sculptor_codebook_displacement::drawGL()
{
    if (m_pTailDoc != NULL && m_pHeadDoc != NULL && m_pTailDoc->getVisible() && m_pHeadDoc->getVisible())
    {
        svt_matrix4<Real64> oCurrentTransTail;  
        svt_matrix4<Real64> oCurrentTransHead; 
        
        oCurrentTransTail.set( *(m_pTailDoc->getNode()->getTransformation()) );
        oCurrentTransHead.set( *(m_pHeadDoc->getNode()->getTransformation()) );
    
        //not automatic update
        setAutoDL(false);
        
        long int iMatchTargetID =  1;
        long int iMatchProbeID  =  1;
        long int iThisTargetID  = -1;
        long int iThisProbeID   = -1;
        
        if (m_pTailDoc->getTransformCount()>0)
        {
            iMatchTargetID = m_pTailDoc->getCurrentMatchTargetID();
            iMatchProbeID  = m_pTailDoc->getCurrentMatchProbeID();
            iThisTargetID  = m_pHeadDoc->getCodebookID();
            iThisProbeID  =  m_pTailDoc->getCodebookID();
        }
        
        //rebuild the arrows only when the documents have been moved, the matching document have been reseted or the objects just became visible
        if (m_oOldTransTail!=oCurrentTransTail || m_oOldTransHead!=oCurrentTransHead || iMatchTargetID != iThisTargetID || iMatchProbeID != iThisProbeID || (m_pTailDoc->getVisible() && m_pHeadDoc->getVisible()) )
        {
            m_oOldTransTail = oCurrentTransTail;
            m_oOldTransHead = oCurrentTransHead;
            
            setVisible(false);    
            //Get the new location of the codebook vectors
            m_pTailCV.delAllPoints();
            m_pTailDoc->getCodebookRealPos(m_pTailCV, m_oOrigin);
        
            m_pHeadCV.delAllPoints();
            m_pHeadDoc->getCodebookRealPos(m_pHeadCV, m_oOrigin);
        
            //get the match if any or create it
            if (m_pTailDoc->getTransformCount()>0 && (iMatchTargetID == iThisTargetID && iMatchProbeID == iThisProbeID) )
            {
                m_oMatch = *(m_pTailDoc->getCurrentMatch());
                if (m_oMatch.size()==0)
                    for(unsigned int iIndex=0; iIndex< m_pTailCV.size() ; iIndex++)
                        m_oMatch.push_back(iIndex);
                
            }
            else
            {
                m_oMatch.clear();
                for(unsigned int iIndex=0; iIndex< m_pTailCV.size() ; iIndex++)
                    m_oMatch.push_back(iIndex);
            }
            
            //delete all the previous arrows; add the new arrows and rerender
            m_pCont->delAll();
            addArrows();
            m_pCont->rebuildDL();
            setVisible(true);
        }
        else
            m_pCont->drawGL();
    }
    else
    {
        m_pCont->delAll();
        m_pCont->rebuildDL();
        setVisible(true);
    }
}

