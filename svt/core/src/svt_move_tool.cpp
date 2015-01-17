/***************************************************************************
                          svt_move_tool.cpp  -  description
                             -------------------
    begin                : 07.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_move_tool.h>
#include <svt_init.h>
#include <svt_time.h>
#include <svt_Camera.h>

svt_move_tool::svt_move_tool(const char* name, svt_node* move_node) : svt_tool(name),
    m_bTranslationOnly( false ),
    m_bFlyMode(false),
    m_t0(0),
    m_t1(1),
    m_fSpeed(0),
    m_bAP(false),
    m_fAP_Dist(0.0),
    m_bLocalTrans( false )
{
    setMoveNode(move_node);
    setCenterMode(false);
    setAlwaysMoving(false);
    setNeverMoving(false);

    if (m_pTrans == NULL)
    {
        m_pTrans = (svt_pos*)new svt_matrix4f("move tool");
        m_bLocalTrans = true;
    } else
        m_bLocalTrans = false;

    m_pMoveMat = new svt_matrix4f("move matrix");
    m_pTempMat = new svt_matrix4f("temp mat");
    m_pOldDevMat = new svt_matrix4f("old device mat");
    m_pOldViewMat = new svt_matrix4f("old view mat");
    m_pGlobalMatrix = NULL;

    m_fTranslationScale = 1.0f;
};
svt_move_tool::~svt_move_tool()
{
    if (m_pTrans != NULL && m_bLocalTrans)
        delete m_pTrans;
};

void svt_move_tool::setMoveNode(svt_node* move_node)
{
    m_pMoveNode = move_node;

    if (m_pMoveNode != NULL)
    {
        if (m_pMoveNode->getTransformation() == NULL)
            m_pMoveNode->setTransformation(new svt_matrix4f("move node mat"));
        m_pViewMat = m_pMoveNode->getTransformation();
    }
}

svt_node* svt_move_tool::getMoveNode()
{
    return m_pMoveNode;
};

void svt_move_tool::onButtonPressed()
{
    if (m_pMoveNode && !m_bAP)
    {
        m_pMoveNode->getNodeLock()->P();

        // remember old object mat
        m_pOldViewMat->setFromMatrix(*m_pViewMat);
        if (m_bCenter)
        {
            m_fOldViewX = m_pOldViewMat->getTranslationX();
            m_fOldViewY = m_pOldViewMat->getTranslationY();
            m_fOldViewZ = m_pOldViewMat->getTranslationZ();
            m_pOldViewMat->setTranslationX(0.0f);
            m_pOldViewMat->setTranslationY(0.0f);
            m_pOldViewMat->setTranslationZ(0.0f);
        }

        // remember old device mat
        m_pOldDevMat->setFromMatrix(*getTransformation());
        if (m_pGlobalMatrix && m_pGlobalMatrix != m_pViewMat)
        {
            svt_matrix4f oGlobTemp("Global Temp");
            oGlobTemp.setFromMatrix(*m_pGlobalMatrix);
            if (m_bCenter)
            {
                oGlobTemp.setTranslationX(0.0f);
                oGlobTemp.setTranslationY(0.0f);
                oGlobTemp.setTranslationZ(0.0f);
            }
            applyGlobal(*m_pOldDevMat, oGlobTemp);
        }
        if (m_bCenter)
        {
            m_fOldDevX = m_pOldDevMat->getTranslationX();
            m_fOldDevY = m_pOldDevMat->getTranslationY();
            m_fOldDevZ = m_pOldDevMat->getTranslationZ();
            m_pOldDevMat->setTranslationX(0.0f);
            m_pOldDevMat->setTranslationY(0.0f);
            m_pOldDevMat->setTranslationZ(0.0f);
        }
        m_pOldDevMat->invert();

	//
	// new stuff for fly mode and auto-pilot
	// (fd)
	// NOTE: temporary solution!
	//       new entries are redundant
	//       class needs cleanup and redesign
	m_zMove=0.0;
	if (m_bFlyMode)
	{
	    // save current modelview matrix
	    m_t0=svt_getElapsedTime();
	}

	svt_matrix4<Real32> oMat(*m_pOldViewMat);
        m_oPivotPoint = oMat * m_oOldPivot;


        m_pTempMat->setFromMatrix(*getTransformation());
        m_fTempZ = m_pTempMat->getTranslationZ();

	// just to ensure these are not used unless m_bCenter==true
        m_fTempX = 0.0f;
        m_fTempY = 0.0f;

        m_pMoveNode->getNodeLock()->V();
    }
}

void svt_move_tool::onButtonReleased()
{
  //
  // get new speed value
  //
  if (m_bFlyMode && m_pMoveNode && !m_bAP)
  {
      m_t1 = svt_getElapsedTime();
      if ( m_t1-m_t0 < 100 )
	  m_fSpeed=0.0;
      else
	  m_fSpeed+= m_zMove / (m_t1-m_t0);


  }

}

bool svt_move_tool::recalc()
{
    Real32 m_fMoveZ;

    // Bail if not active
    if (!svt_tool::recalc())
        return false;

    if (m_bNeverMoving)
        return true;

    if (m_pMoveNode && !m_bAP )
    {
        if  (isButtonPressed() || m_bAlways)
        {
            if (m_pMoveMat != NULL && m_pTempMat != NULL && m_pOldViewMat != NULL)
            {
                m_pMoveNode->getNodeLock()->P();

                m_pTempMat->setFromMatrix(*getTransformation());
                m_zMove = m_pTempMat->getTranslationZ();
                m_fMoveZ = m_pTempMat->getTranslationZ() - m_fTempZ;

                if (m_pGlobalMatrix && m_pGlobalMatrix != m_pViewMat)
                {
                    svt_matrix4f oGlobTemp("Global Temp");
                    oGlobTemp.setFromMatrix(*m_pGlobalMatrix);
                    if (m_bCenter)
                    {
                        oGlobTemp.setTranslationX(0.0f);
                        oGlobTemp.setTranslationY(0.0f);
                        oGlobTemp.setTranslationZ(0.0f);
                    }
                    applyGlobal(*m_pTempMat, oGlobTemp);
                }
                if (m_bCenter)
                {
                    m_fTempX = m_pTempMat->getTranslationX();
                    m_fTempY = m_pTempMat->getTranslationY();
                    m_fTempZ = m_pTempMat->getTranslationZ();
                    m_pTempMat->setTranslationX(0.0f);
                    m_pTempMat->setTranslationY(0.0f);
                    m_pTempMat->setTranslationZ(0.0f);
                }
                m_pMoveMat->setFromMatrix(*m_pOldDevMat);
		m_pMoveMat->postMult(*m_pTempMat);

                //
                // now manipulate the node transformation matrix
                //

		// translations only?
                if (getTranslationOnly())
                {
                    Vector3f vTrans;
                    vTrans[0] = m_fTranslationScale * (m_fTempX - m_fOldDevX) + m_fOldViewX;
                    vTrans[1] = m_fTranslationScale * (m_fTempY - m_fOldDevY) + m_fOldViewY;
                    vTrans[2] = m_fTranslationScale * (m_fTempZ - m_fOldDevZ) + m_fOldViewZ;

                    m_pViewMat->setTranslationFromTranslation( vTrans );

                } else {

		    m_pViewMat->setFromMatrix(*m_pOldViewMat);
                    m_pViewMat->postAddTranslation(-m_oPivotPoint.x(), -m_oPivotPoint.y(), -m_oPivotPoint.z());
                    m_pViewMat->postMult(*m_pMoveMat);
                    m_pViewMat->postAddTranslation(m_oPivotPoint.x(), m_oPivotPoint.y(), m_oPivotPoint.z());

                    if (m_bCenter)
                    {
                        m_pViewMat->postAddTranslation(m_fTranslationScale * (m_fTempX - m_fOldDevX),
			                               m_fTranslationScale * (m_fTempY - m_fOldDevY),
						       m_fTranslationScale * (m_fTempZ - m_fOldDevZ));
                        m_pViewMat->postAddTranslation(m_fOldViewX, m_fOldViewY, m_fOldViewZ);
                    }
                }

                m_pMoveNode->getNodeLock()->V();
            }
        }

    }

    //
    // stuff for fly mode
    //
    if (m_pMoveNode && m_bFlyMode && !svt_renderingForRightEye() )
    {
        int t = svt_getElapsedTime();
        Real32 fAdvance=m_fSpeed*(t-m_t1);
        if (m_bAP)
        {
            m_fAP_Dist+=fAdvance;
            setAP_Position(m_fAP_Dist);
        }
        else
            m_pViewMat->postAddTranslation(0, 0, fAdvance);
        m_t1=t;
    }

    return true;
}

/// if center mode is set to true, the transformed object will get rotated around its current position (default = false)
void svt_move_tool::setCenterMode(bool tf)
{
    m_bCenter = tf;
}

/// returns the center mode setting
bool svt_move_tool::getCenterMode()
{
    return m_bCenter;
}

/**
 * always moving, or only if the button is pressed (Default: true)
 * \param bAlways true if the object is always moving around
 */
void svt_move_tool::setAlwaysMoving(bool bAlways)
{
    m_bAlways = bAlways;
}

/**
 * always moving, or only if the button is pressed (Default: true)
 * \return always moving the objects around, or only if the button is pressed?
 */
bool svt_move_tool::getAlwaysMoving()
{
    return m_bAlways;
}

/**
 * set a global transformation matrix
 * \param pMatrix pointer to the global matrix
 */
void svt_move_tool::setGlobalMatrix(svt_matrix4f* pMatrix)
{
    m_pGlobalMatrix = pMatrix;
}
/**
 * get a global transformation matrix
 * \return pointer to the global matrix
 */
svt_matrix4f* svt_move_tool::getGlobalMatrix()
{
    return m_pGlobalMatrix;
}

/**
 * this function applies a global matrix
 */
void svt_move_tool::applyGlobal(svt_matrix4f& oLocal, svt_matrix4f& oGlobal)
{
    svt_matrix4f oTemp("Temp");
    svt_matrix4f oInvGlob("InvGlob");

    oInvGlob.setFromMatrix(oGlobal);
    oInvGlob.invert();

    oTemp.setFromMatrix(oGlobal);
    oTemp.postMult(oLocal);
    oTemp.postMult(oInvGlob);

    oLocal.setFromMatrix(oTemp);
}

/**
 * switch on/off the moving
 * \param bNeverMoving if true the target is never moved around
 */
void svt_move_tool::setNeverMoving(bool bNeverMoving)
{
    m_bNeverMoving = bNeverMoving;
    // JJH FIXME
    if (bNeverMoving)
	printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx\n");
}
/**
 * get the state of the on/off switch
 * \return if true the target is never moved around
 */
bool svt_move_tool::getNeverMoving()
{
    return m_bNeverMoving;
}

void svt_move_tool::setFlyMode(bool b) 
{
  m_bFlyMode=b; 
  if (b) 
	m_t1=svt_getElapsedTime();
} 


void svt_move_tool::turnLeft()
{
  if (m_pViewMat && !m_bAP)
	m_pViewMat->postAddRotation(1,-0.06);

}

void svt_move_tool::turnRight()
{
  if (m_pViewMat && !m_bAP)
	m_pViewMat->postAddRotation(1,0.06);

}

void svt_move_tool::up()
{
  if (m_pViewMat && !m_bAP )
	m_pViewMat->postAddTranslation(0,0.2,0);

}

void svt_move_tool::down()
{
  if (m_pViewMat && !m_bAP)
	m_pViewMat->postAddTranslation(0,-0.2,0);


}


void svt_move_tool::startAutoPilot(const char* pszFilename, 
								   const svt_vector4<Real32>& upDir, 
								   Real32 initialSpeed)
{
  try {
	m_oAP_Route.load(pszFilename);

	cout << "loaded route with " << m_oAP_Route.size() << " data points" 
		 << endl;

	m_vUp=upDir;
	m_vUp.normalize();

	setAP_Position(-1);
	m_fAP_Dist=0.0;
	m_bAP = true;
	setSpeed(initialSpeed);

  }
  
  catch (svt_exception& oError) {
	oError.uiMessage();
  }




}


void svt_move_tool::setAP_Position(float fDist)
{
  static svt_vector4<Real32> old_pos(0,0,0,1);

  svt_vector4<Real32> dir;
  svt_vector4<Real32> pos;


  if (fDist <= 0)
	{
	  // initial position
	  pos = m_oAP_Route[0];
	  old_pos=pos;
	  dir = m_oAP_Route[1] - pos;
	}
  else
	{
	  // next position

	  pos=m_oAP_Route.point(fDist, pos);
	  if (pos[3] < 0)
		{
		  stopAutoPilot();
		  return;
		}

	  dir = pos-old_pos;
	  old_pos=pos;
	}

  if (isPositive(dir.lengthSq()))
	{
	dir.normalize();

	svt_vector4<Real32> s=vectorProduct(dir, m_vUp);
	svt_vector4<Real32> u=vectorProduct(s, dir);
  
	m_pViewMat->setToId();

	for (int i=0; i<3; i++)
	  {
		m_pViewMat->setMat(i,0,s[i]);
		m_pViewMat->setMat(i,1,u[i]);
		m_pViewMat->setMat(i,2,-dir[i]);
	  }


	m_pViewMat->preAddTranslation(-pos[0], -pos[1], -pos[2]);
	}
}

svt_vector4<Real32> svt_move_tool::APpos()
{
  svt_vector4<Real32> vec;
  vec = m_oAP_Route.point(m_fAP_Dist, vec);
  vec[3] = m_oAP_Route.currentLeftPoint();
  return vec;
}

/**
 * set pivot point (default: origin)
 * \param oPivotPoint svt_vector4<Real32> with the pivot point information
 */
void svt_move_tool::setPivotPoint(svt_vector4<Real32> oPivotPoint)
{
    m_oPivotPoint = oPivotPoint;
    m_oOldPivot = oPivotPoint;
}
/**
 * get pivot point (default: origin)
 * \return svt_vector4<Real32> with the pivot point information
 */
svt_vector4<Real32> svt_move_tool::getPivotPoint()
{
    return m_oPivotPoint;
}

/**
 * Only return translations? (Default: false)
 * \param bTranslationOnly true if only translations of tool are used
 */
void svt_move_tool::setTranslationOnly(bool bTranslationOnly)
{
    m_bTranslationOnly = bTranslationOnly;
}
/**
 * Only return translations?
 * \return Only translations of tool are used?
 */
bool svt_move_tool::getTranslationOnly()
{
    return m_bTranslationOnly;
}

/**
 * Set the translation scaling
 */
void svt_move_tool::setTranslationScale(Real32 fTranslationScale)
{
    m_fTranslationScale = fTranslationScale;
}

/**
 * Get the translation scaling
 */
Real32 svt_move_tool::getTranslationScale()
{
    return m_fTranslationScale;
}

