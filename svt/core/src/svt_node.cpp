/***************************************************************************
                          svt_node.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_core.h>
#include <svt_node.h>
#include <svt_window.h>
#include <svt_init.h>
#include <svt_displayList.h>
#include <svt_time.h>
#include <svt_cmath.h>
#include <svt_fstream.h>
#include <svt_opengl.h>
#include <svt_clipplane.h>

svt_properties svt_node::sm_oDefaultProperties;
GLint          svt_node::m_iMaxClipPlanes = -1;
bool*          svt_node::m_aClipPlanes    = NULL;


unsigned int g_iNodeCounter = 0;

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_node::svt_node(svt_pos* pPos , svt_properties* pProp) : 
    m_pNext(NULL),
    m_pSon(NULL),
    m_pProp(pProp),
    m_pRot(NULL),
    m_pTexture(NULL),
    m_pTexture3D(NULL),
    m_pTrans(NULL),
    dlList(NULL),
    dlActive(NULL),
    m_oSema(1),
    m_bVisible(true),
    m_fScaleX(1.0),
    m_fScaleY(1.0),
    m_fScaleZ(1.0),
    m_pDisplayList(NULL),
    m_bAutoDL(true),
    m_bBird(false),
    m_bDrawBox(false),
    m_bAlreadyDrawn(false),
    m_iNodeID( g_iNodeCounter + 1 ),
    m_bDeleteTrans( true ),
    m_bDeleteProp( true ),
    m_bIsFogNode( false ),
    m_fDistanceToCamera( abs(-3.0f) ),
    m_bIgnoreClipPlanes( false )
{
    g_iNodeCounter++;

    setProperties(pProp ? pProp : &sm_oDefaultProperties);
    setPos(pPos);

    m_pDisplayList = new svt_displayList();

    setName( "svt_node" );

}
svt_node::~svt_node()
{
    if (m_pDisplayList)
        m_pDisplayList->destroyCD();

    if (m_bDeleteTrans && m_pTrans)
        delete m_pTrans;
    if (m_bDeleteTrans && m_pRot)
        delete m_pRot;
    if (m_bDeleteProp && m_pProp && m_pProp != &sm_oDefaultProperties)
        delete m_pProp;

    if (m_pTexture3D)
        delete m_pTexture3D;
}

///////////////////////////////////////////////////////////////////////////////
// scenegraph management functions
///////////////////////////////////////////////////////////////////////////////

/**
 * return the node of the next hierarchy level (if there is any, otherwise return NULL).
 * \return pointer of the "son" node.
 */
svt_node* svt_node::getSon()
{
    return m_pSon;
}

/**
 * set the node in the next hierarchy level.
 * \param pSon pointer to the new "son" node.
 */
void svt_node::setSon(svt_node* pSon)
{
    m_pSon = pSon;
}

/**
 * what is the next node in the tree?
 * \return pointer to the next node
 */
svt_node* svt_node::getNext()
{
    return m_pNext;
}
/**
 * set the next pointer
 * \param pNext pointer to the next node
 */
void svt_node::setNext(svt_node* pNext)
{
    m_pNext = pNext;
}

/**
 * \brief adds a node to the objects subtree.
 * This function will add a new node to the subtree. If the subtree was empty, the new node will be the "son", otherwise it will get connected to end the of the list of sons.
 * \param pNode pointer to the node which should be added.
 */
void svt_node::add(svt_node* pNode)
{
    // is there no son? Then the node is the new son....
    if (getSon() == NULL)
    {
        setSon(pNode);
        return;
    }

    // when there is a son, then put it at the end of the list...
    svt_node* current = getSon();
    while(current->getNext() != NULL)
        current = current->getNext();

    current->setNext(pNode);
    pNode->setNext(NULL);

    svt_redraw();
}

/**
 * \brief adds a node to the objects subtree.
 * This function will add a new node to the subtree. If the subtree was empty, the new node will be the "son", otherwise it will get connected to _end_ the of the list of sons.
 * \param pNode pointer to the node which should be added.
 */
void svt_node::addAtTail(svt_node* pNode)
{
    add( pNode );
}

/**
 * \brief adds a node to the objects subtree.
 * This function will add a new node to the subtree. The new node will always be the first "son", i.e. it will get added as the head of the list of nodes in the subtree.
 * \param pNode pointer to the node which should be added.
 */
void svt_node::addAtHead(svt_node* pNode)
{
    pNode->setNext( getSon() );
    setSon(pNode);

    svt_redraw();
}

/**
 * \brief delete a node
 * \param pNode pointer to the node which should be deleted.
 */
void svt_node::del(svt_node* pNode)
{
    // obvious, the user does not want to delete anything...
    if (pNode == NULL)
        return;

    //printf("---------------------------------------------------------\n" );
    //printf("DEBUG: Node %s is deleting node: %s\n", getName(), pNode->getName() );
    //printf("---------------------------------------------------------\n" );

    // OK, how can we delete a node from the subtree, if there is no subtree??
    if (getSon() == NULL)
    {
        printf("DEBUG: Was not able to delete a node: Son was NULL!\n");
        printf("DEBUG: Node: %s\n", pNode->getName() );
        return;
    }

    svt_node* pNewNext = pNode->getNext();
    svt_node* pPrevious = NULL;

    if (pNode != getSon())
    {
        pPrevious = getSon();
        while( pPrevious != NULL && pPrevious->getNext() != pNode )
            pPrevious = pPrevious->getNext();

        if (pPrevious == NULL)
        {
            printf("DEBUG: Was not able to delete a node: Node was not found!\n");
            printf("DEBUG: Node: %s\n", pNode->getName() );
            return;
        }
    }

    //if (svt_getRunning())
    //    printf("DEBUG: SVT is running right now!\n");
    //else
    //    printf("DEBUG: SVT is NOT running right now!\n");

    if ( svt_getScene() )
        svt_getScene()->getSceneSema().P();

    // delete everything underneith the node
    if (pNode->getSon())
        delSubTree( pNode->getSon() );
    delete pNode;

    if (pPrevious)
        pPrevious->setNext( pNewNext );
    else
        setSon( pNewNext );

    if ( svt_getScene() )
        svt_getScene()->getSceneSema().V();

    svt_redraw();
}

/**
 * \brief delete subtree (son node and all following)
 */
void svt_node::delSubTree()
{
    delSubTree( getSon() );
    setSon( NULL );
    svt_redraw();
}

/**
 * delete subtree
 */
void svt_node::delSubTree(svt_node* pTree)
{
    // is there a subtree?
    if (pTree == NULL)
        return;

    if (pTree->getSon() != NULL)
    {
        delSubTree(pTree->getSon());
        pTree->setSon( NULL );
    }

    if (pTree->getNext() != NULL)
    {
        delSubTree(pTree->getNext());
        pTree->setNext( NULL );
    }

    delete pTree;
}

vector< void* > svt_node::g_aMemory;

/**
 * Custom new operator
 */
void* svt_node::operator new(size_t iSize)
{
    void* pPointer = malloc(iSize);
    g_aMemory.push_back( pPointer );

    if ( pPointer == NULL )
    {
        throw "allocation fail : no free memory";
    }

    return pPointer;
};
/**
 * Custom delete operator
 */
void svt_node::operator delete(void* pPointer)
{
    vector< void* >::iterator iResult = find(g_aMemory.begin(), g_aMemory.end(), pPointer);

    if (iResult == g_aMemory.end())
        printf("WARNING: Something got deleted that was not allocated through new!!!! Weird, memory corruption likely!!!\n");
    else
        g_aMemory.erase( iResult );

    free(pPointer);
};

/**
 * Print memory table
 */
void svt_node::dumpMemTable()
{
    for(unsigned int i=0; i<g_aMemory.size(); i++)
    {
        svt_node* pNode = (svt_node*)g_aMemory[i];
        printf("SVT-Node: %i - %s (%s)\n", i, svt_getName( typeid(*pNode).name() ), pNode->getName() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// output functions
///////////////////////////////////////////////////////////////////////////////

/**
 * draws the object, the son and then the next object
 * \param iMode mode for the drawing (e.g. SVT_MODE_GL, SVT_MODE_VRML)
 */
void svt_node::draw(int iMode)
{
    switch (iMode)
    {
    case SVT_MODE_POV:
	outputPOV();
        break;

    case SVT_MODE_IV:
	outputIV();
        break;

    case SVT_MODE_YAFRAY:
	outputYafray();
        break;

    case SVT_MODE_VRML:
        outputVrml();
	break;

    case SVT_MODE_OBJ:
        outputOBJ();
	break;

    default:
    case SVT_MODE_GL:
        if (svt_getScene()->getSelection())
            glLoadName( m_iNodeID );
        outputGL();
	break;
    }
}

/**
 * steer the opengl drawing (first applies the properties and then calls drawGL)
 */
void svt_node::outputGL()
{
    glPushMatrix();

    if (getVisible())
    {
        if (m_bAutoDL && m_pDisplayList->getDisplayList() == -1)
        {
	    m_oSema.P();
            createDL();
            m_oSema.V();
        }

        m_oSema.P();

        // apply all attributes to the object
        applyGL();

#if defined (DEBUG) || defined (EXPERIMENTAL)
	if (svt_checkGLErrors("outputGL: -- after applyGL()"))
	{
	    printName();
	}
#endif
        // scale the object
        glScalef(m_fScaleX, m_fScaleY, m_fScaleZ);

        // apply fog parameters?
	if ( getIsFogNode() )
	    applyFogParams();

        // ignore clip planes?
        if ( getIgnoreClipPlanes() )
        {
            // disable clip planes
            if ( svt_clipplane::getNumUsedClipPlanes() > 0)
            {
                if (m_iMaxClipPlanes == -1)
                {
                    m_iMaxClipPlanes = svt_clipplane::getNumClipPlanes();
                    m_aClipPlanes = svt_clipplane::getClipOcc();
                }

                for (GLint i=0; i<m_iMaxClipPlanes; ++i)
                    if ( m_aClipPlanes[i] )
                        glDisable(GL_CLIP_PLANE0 + i);
            }
        }

        if (m_bBird)
        {
	    do_precomputations();

            glLoadIdentity();
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            const svt_matrix<Real32>& oBox = box();
            //			cout << "Bird mode - box is " << oBox << endl;
            Real32 fOffX=(oBox[0][1]-oBox[0][0])*0.02;
            Real32 fOffY=(oBox[1][1]-oBox[1][0])*0.02;
            Real32 fOffZ=(oBox[2][1]-oBox[2][0])*5;
            glOrtho(oBox[0][0]-fOffX, oBox[0][1]+fOffX,
                    oBox[1][0]-10*fOffY, oBox[1][1]+fOffY,
                    oBox[2][0]-fOffZ, oBox[2][1]+fOffZ);
            glMatrixMode(GL_MODELVIEW);
        }

        // draw the object as wireframe
        if (m_pProp->getWireframe() && svt_getScene()->getPass() == SVT_PASS_OPAQUE)
        {
	    m_pProp->applyGLwireframe();
            if (m_bAutoDL && (m_pDisplayList->getDisplayList() != -1))
                        glCallList(m_pDisplayList->getDisplayList());
            else
                drawGL();
        }

        // draw box
        if (m_bDrawBox)
        {

            bool b = m_pProp->getWireframe();
            m_pProp->setWireframe(true);
            m_pProp->applyGLwireframe();
            m_pProp->setWireframe(b);
            const svt_matrix<Real32>& oBox = box();
            glBegin(GL_QUADS);


            glVertex3f(oBox[0][0], oBox[1][0], oBox[2][0]);
            glVertex3f(oBox[0][1], oBox[1][0], oBox[2][0]);
            glVertex3f(oBox[0][1], oBox[1][1], oBox[2][0]);
            glVertex3f(oBox[0][0], oBox[1][1], oBox[2][0]);

            //
            // rest is commented out because of gl-errors on windows
            // when in bird-mode (glOrtho)

            // 			glVertex3f(oBox[0][0], oBox[1][0], oBox[2][1]);
            // 			glVertex3f(oBox[0][1], oBox[1][0], oBox[2][1]);
            // 			glVertex3f(oBox[0][1], oBox[1][1], oBox[2][1]);
            // 			glVertex3f(oBox[0][0], oBox[1][1], oBox[2][1]);

            // 			glVertex3f(oBox[0][0], oBox[1][0], oBox[2][0]);
            // 			glVertex3f(oBox[0][1], oBox[1][0], oBox[2][0]);
            // 			glVertex3f(oBox[0][1], oBox[1][0], oBox[2][1]);
            // 			glVertex3f(oBox[0][0], oBox[1][0], oBox[2][1]);

            // 			glVertex3f(oBox[0][0], oBox[1][1], oBox[2][0]);
            // 			glVertex3f(oBox[0][1], oBox[1][1], oBox[2][0]);
            // 			glVertex3f(oBox[0][1], oBox[1][1], oBox[2][1]);
            // 			glVertex3f(oBox[0][0], oBox[1][1], oBox[2][1]);

            glEnd();
        }

        // draw polygon normals
        if (m_pProp->getNormals())
        {
            m_pProp->applyGLnormals();
            drawGL_Normals();
        }

        // draw only clipplane rectangles in SVT_PASS_CLIPPLANE_RECTANGLE
        if ( getType() == SVT_NODE_CLIPPLANE && svt_getScene()->getPass() == SVT_PASS_CLIPPLANE_RECTANGLE )
        {
            //m_pProp->applyGLsolid(getTexture()!=NULL); FIXME for now, the opengl settings are done within svt_clipplane
            drawGL();
        }

        // draw the object as solid
        if (m_pProp->getSolid())
        {
            // which pass are we rendering right now? Only render in the correct pass...
            if (
                // 1st case: No color
                m_pProp->getColor() == NULL

                ||

                // 2nd case: Not transparent and opaque pass
                (
                 (m_pProp->getColor()->getTransparency() == 1.0f ||
                  (m_pProp->getColor()->getTransparency() != 1.0f && m_pProp->getColor()->getStipple() == true)
                 ) && svt_getScene()->getPass() == SVT_PASS_OPAQUE
                )

                ||

                // 3rd case: Transparent and transparent pass
                (m_pProp->getColor()->getTransparency() != 1.0f &&
                 m_pProp->getColor()->getStipple() == false &&
                 svt_getScene()->getPass() == SVT_PASS_TRANSPARENT)

               )
            {

                if (m_pProp->getColor() != NULL || (m_pProp->getColor() == NULL && svt_getScene()->getPass() == SVT_PASS_OPAQUE))
                {

                    m_pProp->applyGLsolid(getTexture()!=NULL);

                    if (getTexture() && m_pProp->getTexture())
                    {
                        getTexture()->applyGL();
                    }
                    if (getTexture3D() != NULL)
                        getTexture3D()->applyGL();


                    if (m_pProp->getWireframe())
                        m_bAlreadyDrawn=true;



                    if (m_bAutoDL && (m_pDisplayList->getDisplayList() != -1))
                    {

                        glCallList(m_pDisplayList->getDisplayList());

#if defined (DEBUG) || defined (EXPERIMENTAL)
			if (svt_checkGLErrors("outputGL: -- after glCallList(...)"))
			{
			    printName();
			}
#endif

                    }
		    else
		    {

                        drawGL();

#if defined (DEBUG) || defined (EXPERIMENTAL)
			if (svt_checkGLErrors("outputGL: -- after drawGL()"))
			{
			    printName();
			}
#endif
                    }

                    m_bAlreadyDrawn=false;
                }
            }

        }


        if (m_bBird)
        {

            //
            // draw position and viewing direction as blue arrow
            //

            // get vector from central point to left frustum plane
            // perp. to up-vector on projection plane

	    m_oCam.update();

            Real32 l_x = -(m_oCam.plane(0)[1]);
            Real32 l_y =   m_oCam.plane(0)[0];

            Real32 r_x =   m_oCam.plane(1)[1];
            Real32 r_y = -(m_oCam.plane(1)[0]);


            //svt_vector4<Real32> t = vectorProduct(m_oCam.up(), m_oCam.viewDir());

            //
            // project current position on base plane along up-dir
            //
            const svt_vector4<Real32>& p_o = m_oCam.viewPos();
            svt_vector4<Real32> p(p_o);

            Real32 alpha;
            if (isPositive(fabs(m_oCam.up().z())))
                alpha = (p.z()-0.5) / m_oCam.up().z();
            else
                alpha = 0.0;

            p[0]-=alpha*m_oCam.up().x();
            p[1]-=alpha*m_oCam.up().y();


            //
            // compute direction perp. to projected view-dir
            //
            const svt_vector4<Real32>& v = m_oCam.viewDir();
            svt_vector4<Real32> v_p(v);
            v_p[2]=0.0;
            v_p.normalize();
            v_p*=5;

            Real32 n_x = -v[1];
            Real32 n_y = v[0];


            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glColor3f(0,0,1);
            glDisable(GL_DEPTH_TEST);

            //
            // draw frustum scetch projected to height 0
            //
            glBegin(GL_LINE_STRIP);

            glVertex3f(p[0]+80*l_x, p[1]+80*l_y, 0);
            glVertex3f(p[0],        p[1],        0);
            glVertex3f(p[0]+80*r_x, p[1]+80*r_y, 0);

            glEnd();



            //
            // draw arrow at orig pos

            glBegin(GL_TRIANGLES);

            glVertex3f(p_o[0]-0.1*n_x,      p_o[1]-0.1*n_y,      0);
            glVertex3f(p_o[0]+0.1*n_x,      p_o[1]+0.1*n_y,      0);
            glVertex3f(p_o[0]+0.1*n_x+v[0], p_o[1]+0.1*n_y+v[1], 0);

            glVertex3f(p_o[0]+0.1*n_x+v[0], p_o[1]+0.1*n_y+v[1], 0);
            glVertex3f(p_o[0]-0.1*n_x+v[0], p_o[1]-0.1*n_y+v[1], 0);
            glVertex3f(p_o[0]-0.1*n_x,      p_o[1]-0.1*n_y,      0);

            glVertex3f(p_o[0]+0.22*n_x+0.9*v[0], p_o[1]+0.22*n_y+0.9*v[1], 0);
            glVertex3f(p_o[0]+1.7*v[0],          p_o[1]+1.7*v[1],          0);
            glVertex3f(p_o[0]-0.22*n_x+0.9*v[0], p_o[1]-0.22*n_y+0.9*v[1], 0);

            glEnd();

            glEnable(GL_DEPTH_TEST);
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();                        // FIXME there seems to be no corresponding glPushMatrix for this glPopMatrix
        }


        m_oSema.V();


#if defined (DEBUG) || defined (EXPERIMENTAL)
	if (svt_checkGLErrors("outputGL: -- end"))
	{
	    printName();
// 	    GLint i;
// 	    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &i);
// 	    cout << "GL_MODELVIEW_STACK_DEPTH: " << i << endl;
// 	    glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &i);
// 	    cout << "GL_PROJECTION_STACK_DEPTH: " << i << endl;
// 	    switch (svt_getScene()->getPass())
// 	    {
// 		case SVT_PASSS_OPAQUE:
// 		    cout << "in pass SVT_PASSS_OPAQUE\n"; break;
// 		case SVT_PASSS_TRANSPARENT:
// 		    cout << "in pass SVT_PASSS_TRANSPARENT\n"; break;
// 		case SVT_PASSS_CLIP:
// 		    cout << "in pass SVT_PASSS_CLIP\n"; break;
// 	    }
	}
#endif


        // the rest of the tree
        if (getSon() != NULL)
            getSon()->draw(SVT_MODE_GL);

    }

    glPopMatrix();

    // unignore clip planes?
    // attention: when unignored here, the complete subtree of this node will ignore the clip planes
    if ( getIgnoreClipPlanes() )
    {
        // re-enable clipplanes
        for (GLint i=0; i<m_iMaxClipPlanes; ++i)
        {
            if (m_aClipPlanes[i])
                glEnable(GL_CLIP_PLANE0 + i);
        }
    }

    if (getNext() != NULL)
        getNext()->draw(SVT_MODE_GL);

}

/**
 * draws the object with opengl
 */
void svt_node::drawGL()
{
    // the node should only be used for structural purposes, so there is no output here...
}

/**
 * set all attributes of the object (transformation, rotation, color, material, ....)
 */
void svt_node::applyGL()
{
    if (m_pRot != NULL)
        m_pRot->applyProperty();

    if (m_pTrans != NULL)
        m_pTrans->applyToGL();
}

/**
 * steer the povray output (first applies the properties and then calls drawPOV)
 */
void svt_node::outputPOV()
{
    if (getVisible())
    {
	// small header
	cout << endl;
	cout << "// "; printName();
	cout << "union {" << endl;

	// output the povray code for this object
	drawPOV();

	// the rest of the tree
	if (getSon() != NULL)
	    getSon()->draw(SVT_MODE_POV);

	// transformation
	if (m_fScaleX != 1 || m_fScaleY != 1 || m_fScaleZ != 1)
	{
	    cout << "  scale <" << endl;
	    cout << "    " << m_fScaleX << " , " << m_fScaleY << " , " << m_fScaleZ << endl;
	    cout << "  >" << endl;
	}
	if (m_pTrans)
	{
	    cout << "  matrix <" << endl;
	    cout << "      " << (*m_pTrans)(0,0) << ", " << (*m_pTrans)(0,1) << ", " << (*m_pTrans)(0,2) << ", " << endl;
	    cout << "      " << (*m_pTrans)(1,0) << ", " << (*m_pTrans)(1,1) << ", " << (*m_pTrans)(1,2) << ", " <<  endl;
	    cout << "      " << (*m_pTrans)(2,0) << ", " << (*m_pTrans)(2,1) << ", " << (*m_pTrans)(2,2) << ", " <<  endl;
	    cout << "      " << (*m_pTrans)(3,0) << ", " << (*m_pTrans)(3,1) << ", " << (*m_pTrans)(3,2) << endl;
	    cout << "  >" << endl;
	} else {
	    cout << "  // No Matrix!" << endl;
	}

	// material properties
	if (getProperties())
	{
	    if (getProperties()->getColor())
	    {
		Real32* pAmbient = getProperties()->getColor()->getAmbient();
		Real32* pDiffuse = getProperties()->getColor()->getDiffuse();
		//Real32* pSpecular = getProperties()->getColor()->getSpecular();
		Real32 fShininess = getProperties()->getColor()->getShininess();
		Real32 fTransp = getProperties()->getColor()->getTransparency();

		cout << "  pigment {" << endl;
		if (pDiffuse && pDiffuse[0] > 0)
		    cout << "    color rgb< " << pDiffuse[0] << ", " << pDiffuse[1] << ", " << pDiffuse[2] << " >" << endl;
		if (fTransp != -1)
		    cout << "    transmit " << 1.0f - fTransp << endl;
		cout << "  }" << endl;

		cout << "  finish {" << endl;
		if (pAmbient && pAmbient[0] > 0)
		    cout << "    ambient rgb <" << pAmbient[0] << ", " << pAmbient[1] << ", " << pAmbient[2] << " >" << endl;

                // some basic default values that look good...
		cout << "    brilliance 1.7" << endl;
		cout << "    diffuse 0.8" << endl;
		cout << "    roughness 0.06" << endl;
		cout << "    ambient 0.05" << endl;

		//cout << "    emissiveColor 0.0 0.0 0.0" << endl;
		//if (pSpecular && pSpecular[0] > 0)
		//    cout << "    specularColor " << pSpecular[0] << " " << pSpecular[1] << " " << pSpecular[2] << endl;


		if (fShininess != -1)
		    cout << "    specular " << fShininess/256.0f << endl;
		cout << "  }" << endl;
	    }
	}

	cout << "} // end of svt_node" << endl << endl;
    }

    if (getNext() != NULL)
	getNext()->draw(SVT_MODE_POV);
}

/**
 * draws the object with povray
 */
void svt_node::drawPOV()
{
    cout << "  // blank node" << endl;
}

/**
 * print the iv code (OpenInventor) for the object
 */
void svt_node::outputIV()
{
    // small header
    cout << endl;
    cout << "# "; printName();
    cout << "Separator {" << endl;

    // transformation
    if (m_pTrans)
    {
	cout << "  MatrixTransform {" << endl;
        cout << "    matrix" << endl;
        cout << "      " << (*m_pTrans)(0,0) << " " << (*m_pTrans)(0,1) << " " << (*m_pTrans)(0,2) << " " << (*m_pTrans)(0,3) << endl;
        cout << "      " << (*m_pTrans)(1,0) << " " << (*m_pTrans)(1,1) << " " << (*m_pTrans)(1,2) << " " << (*m_pTrans)(1,3) << endl;
        cout << "      " << (*m_pTrans)(2,0) << " " << (*m_pTrans)(2,1) << " " << (*m_pTrans)(2,2) << " " << (*m_pTrans)(2,3) << endl;
        cout << "      " << (*m_pTrans)(3,0) << " " << (*m_pTrans)(3,1) << " " << (*m_pTrans)(3,2) << " " << (*m_pTrans)(3,3) << endl;
	cout << "  }" << endl;
    } else {
	cout << "  # No Matrix!" << endl;
    }
    if (m_fScaleX != 1 || m_fScaleY != 1 || m_fScaleZ != 1)
    {
	cout << "  Transform {" << endl;
	cout << "    scaleFactor  " << m_fScaleX << " " << m_fScaleY << " " << m_fScaleZ << endl;
	cout << "  }" << endl;
    }

    // material properties
    if (getProperties())
    {
	if (getProperties()->getColor())
	{
	    Real32* pAmbient = getProperties()->getColor()->getAmbient();
	    Real32* pDiffuse = getProperties()->getColor()->getDiffuse();
	    Real32* pSpecular = getProperties()->getColor()->getSpecular();
	    Real32 fShininess = getProperties()->getColor()->getShininess();
	    Real32 fTransp = getProperties()->getColor()->getTransparency();

	    cout << "  Material {" << endl;
	    if (pAmbient && pAmbient[0] > 0)
		cout << "    ambientColor  " << pAmbient[0] << " " << pAmbient[1] << " " << pAmbient[2] << endl;
	    if (pDiffuse && pDiffuse[0] > 0)
		cout << "    diffuseColor  " << pDiffuse[0] << " " << pDiffuse[1] << " " << pDiffuse[2] << endl;
	    cout << "    emissiveColor 0.0 0.0 0.0" << endl;
	    if (pSpecular && pSpecular[0] > 0)
		cout << "    specularColor " << pSpecular[0] << " " << pSpecular[1] << " " << pSpecular[2] << endl;
            if (fShininess != -1)
		cout << "    shininess     " << fShininess/128.0f << endl;
            if (fTransp != -1)
		cout << "    transparency  " << 1.0f - fTransp << endl;
	    else
                cout << "    transparency  0.0" << endl;
	    cout << "  }" << endl;
	}
    }

    drawIV();

    // the rest of the tree
    if (getSon() != NULL)
	getSon()->draw(SVT_MODE_IV);

    cout << "} #end of svt_node" << endl << endl;

    if (getNext() != NULL)
	getNext()->draw(SVT_MODE_IV);
}

/**
 * print the iv code (OpenInventor) for the object
 */
void svt_node::drawIV()
{
    cout << "  # Node drawIV method not implemented" << endl;
}

/**
 * print the vrml code of the object
 */
void svt_node::outputVrml()
{
    if (getPos()!=NULL)
    {
        cout << "Transform {" << endl;
        cout << "translation " << getPos()->getX() << " " << getPos()->getY() << " " << getPos()->getZ() << endl;
    }

    if (getSon() != NULL)
    {
        cout << "children [" << endl;
        getSon()->draw(SVT_MODE_VRML);
        cout << "]" << endl;
    }

    if (getPos()!=NULL)
        cout << "children Shape {" << endl;
    else
        cout << "Shape {" << endl;

    if (getProperties() != NULL)
        getProperties()->applyVrml();

    this->drawVrml();

    cout << "}" << endl;

    if (getPos()!=NULL)
        cout << "}" << endl;

    if (getNext()!=NULL)
        getNext()->draw(SVT_MODE_VRML);
}
/**
 * draw the vrml code of the object
 */
void svt_node::drawVrml()
{
    cout << "# blank node" << endl;
}

/**
 * print the complete yafray code for the object (incl. properties)
 */
void svt_node::outputYafray()
{
    // small header
    cout << endl;
    cout << "<!-- " << endl; printName(); cout << "-->" << endl;
    //cout << "Separator {" << endl;

    // transformation
    if (m_pTrans)
    {
	cout << "  <transform" << endl;
        cout << "    m00=\"" << (*m_pTrans)(0,0) << "\" m01=\"" << (*m_pTrans)(1,0) << "\" m02=\"" << (*m_pTrans)(2,0) << "\" m03=\"" << (*m_pTrans)(3,0) << "\"" << endl;
        cout << "    m10=\"" << (*m_pTrans)(0,1) << "\" m11=\"" << (*m_pTrans)(1,1) << "\" m12=\"" << (*m_pTrans)(2,1) << "\" m13=\"" << (*m_pTrans)(3,1) << "\"" << endl;
        cout << "    m20=\"" << (*m_pTrans)(0,2) << "\" m21=\"" << (*m_pTrans)(1,2) << "\" m22=\"" << (*m_pTrans)(2,2) << "\" m23=\"" << (*m_pTrans)(3,2) << "\"" << endl;
	cout << "    m30=\"" << (*m_pTrans)(0,3) << "\" m31=\"" << (*m_pTrans)(1,3) << "\" m32=\"" << (*m_pTrans)(2,3) << "\" m33=\"" << (*m_pTrans)(3,3) << "\" >" << endl;
        cout << endl;
    } else {
	cout << "  <!-- No Matrix! -->" << endl;
    }

    // material properties
    if (getProperties())
    {
	if (getProperties()->getColor())
	{
	    Real32* pAmbient = getProperties()->getColor()->getAmbient();
	    Real32* pDiffuse = getProperties()->getColor()->getDiffuse();
	    Real32* pSpecular = getProperties()->getColor()->getSpecular();
	    //Real32 fShininess = getProperties()->getColor()->getShininess();
	    //Real32 fTransp = getProperties()->getColor()->getTransparency();

	    cout << "    <shader type = \"generic\" name = \"shader." << (this) << "\">" << endl;
	    cout << "      <attributes>" << endl;
	    if (pAmbient && pAmbient[0] > 0)
		cout << "        <color  r=\"" << pAmbient[0] << "\" g=\"" << pAmbient[1] << "\" b=\"" << pAmbient[2] << "\" />" << endl;
	    if (pDiffuse && pDiffuse[0] > 0)
		cout << "        <reflected  r=\"" << pDiffuse[0] << "\" g=\"" << pDiffuse[1] << "\" b=\"" << pDiffuse[2] << "\" />" << endl;
	    if (pSpecular && pSpecular[0] > 0)
	    cout << "        <specular r=\"" << pSpecular[0] << "\" g=\"" << pSpecular[1] << "\" b=\"" << pSpecular[2] << "\" />" << endl;

            /*if (fShininess != -1)
		cout << "    shininess     " << fShininess/128.0f << endl;
            if (fTransp != -1)
		cout << "    transparency  " << 1.0f - fTransp << endl;
	    else
                cout << "    transparency  0.0" << endl;*/

	    cout << "      </attributes>" << endl;
            cout << "    </shader>" << endl;
	}
    }

    if (getProperties() && getProperties()->getColor())
	cout << "    <object name=\"obj." << (this) << "\" shader_name=\"shader." << (this) << "\" caus_IOR=\"1.500000\" recv_rad=\"on\" emit_rad=\"on\" shadow=\"on\">" << endl;
    else
	cout << "    <object name=\"obj." << (this) << "\" shader_name=\"svt.mat\" caus_IOR=\"1.500000\" recv_rad=\"on\" emit_rad=\"on\" shadow=\"on\">" << endl;

    cout << "      <attributes>" << endl;
    cout << "      </attributes>" << endl;

    drawYafray();

    cout << "    </object>" << endl;

    // the rest of the tree
    if (getSon() != NULL)
	getSon()->draw(SVT_MODE_YAFRAY);

    if (m_pTrans)
	cout << "  </transform>" << endl << endl;

    if (getNext() != NULL)
	getNext()->draw(SVT_MODE_YAFRAY);
}
/**
 * print the yafray code of the object
 */
void svt_node::drawYafray()
{
    cout << "      <!-- blank node -->" << endl;
    cout << "      <mesh>" << endl;
    cout << "        <points><p x=\"0.0\" y=\"0.0\" z=\"0.0\"/></points><faces><f a=\"0\" b=\"0\" c=\"0\"/></faces>" << endl;
    cout << "      </mesh>" << endl;
}

/**
 * print the complete Alias/Wavefront code for the object (incl. properties)
 */
void svt_node::outputOBJ()
{
    if (getVisible())
    {
	// transformation
	if (m_pTrans)
	{
	    glPushMatrix();
	    glLoadIdentity();
            m_pTrans->applyToGL();

	    svt_matrix4<Real32> oMat;
	    oMat.loadModelviewMatrix();

	    svt_getScene()->getAliasOBJ().pushMatrix( oMat );

            glPopMatrix();
	}

	// scaling
	if (m_fScaleX != 1.0 || m_fScaleY != 1.0 || m_fScaleZ != 1.0)
	{
	    glPushMatrix();
	    glLoadIdentity();
            glScalef( m_fScaleX, m_fScaleY, m_fScaleZ );

	    svt_matrix4<Real32> oMat;
            oMat.loadModelviewMatrix();
	    svt_getScene()->getAliasOBJ().pushMatrix( oMat );

            glPopMatrix();
	}

	// material properties
	if (getProperties())
	    if (getProperties()->getColor())
		svt_getScene()->getAliasOBJ().setColor( *getProperties()->getColor() );

	drawOBJ();

	// the rest of the tree
	if (getSon() != NULL)
	    getSon()->draw(SVT_MODE_OBJ);

	// undo scaling
	if (m_fScaleX != 1.0 || m_fScaleY != 1.0 || m_fScaleZ != 1.0)
	    svt_getScene()->getAliasOBJ().popMatrix( );

        // undo transformation
	if (m_pTrans)
	    svt_getScene()->getAliasOBJ().popMatrix();
    }

    if (getNext() != NULL)
	getNext()->draw(SVT_MODE_OBJ);

}

/**
 * print the Alias/Wavefront code of the object
 */
void svt_node::drawOBJ()
{

};


/**
 * print the name of the object
 */
void svt_node::printName()
{
    cout << m_pName << "(" << m_iNodeID << ")" << endl;
}
/**
 * set the name of the object
 * \param pName pointer to array of char with the name of the object
 */
void svt_node::setName( const char* pName )
{
    strncpy( m_pName, pName, 50 );
};
/**
 * get the name of the object
 * \return pointer to array of char with the name of the object
 */
char* svt_node::getName( )
{
    return m_pName;
}

/**
 * get the type of the node - e.g. SVT_NODE_RENDERING or SVT_NODE_CLIPPLANE. Default is SVT_NODE_RENDERING.
 */
int svt_node::getType()
{
    return SVT_NODE_RENDERING;
}

/**
 * print list of all elements of one level
 */
void svt_node::printList()
{
    svt_node* current = this;

    while (current != NULL)
    {
        current->printName();
        current = current->getNext();
    }
}

/**
 * print list of all elements of node and sons
 * \param space number of spacer in front of tree output (default = 0)
 */
void svt_node::printTree(int space)
{
    svtout << " - ";
    for (int i=0; i<space; i++)
	cout << ".";
    if (!getVisible())
        cout << "(inv)";
    printName();
    // the rest of the tree
    if (getSon() != NULL)
	getSon()->printTree(space+4);
    if (getNext() != NULL)
	getNext()->printTree(space);
}

/**
 * Find selected node
 */
void svt_node::findSelected()
{
    if (m_iNodeID == (unsigned int)svt_getScene()->getSelectedNodeID())
        svt_getScene()->setSelectedNode( this );

    // the rest of the tree
    if (getSon() != NULL)
	getSon()->findSelected();
    if (getNext() != NULL)
	getNext()->findSelected();
}

///////////////////////////////////////////////////////////////////////////////
// properties
///////////////////////////////////////////////////////////////////////////////

/**
 * set the properties object
 * \param pProp pointer to svt_properties object
 */
void svt_node::setProperties(svt_properties* pProp)
{
    m_pProp = pProp;

    if (m_pProp == NULL)
        m_pProp = &sm_oDefaultProperties;

    svt_redraw();
};
/**
 * get the properties object
 * \return pointer to svt_properties object
 */
svt_properties* svt_node::getProperties()
{
    return m_pProp;
};

/**
 * set the scaling of the object (all axis the same factor)
 * \param fScale the scaling factor
 */
void svt_node::setScale(float fScale)
{
    m_fScaleX = fScale;
    m_fScaleY = fScale;
    m_fScaleZ = fScale;

    svt_redraw();
};
/**
 * set the scaling of the object
 * \param fScaleX the x axis scaling factor
 * \param fScaleY the y axis scaling factor
 * \param fScaleZ the z axis scaling factor
 */
void svt_node::setScale(float fScaleX, float fScaleY, float fScaleZ)
{
    m_fScaleX = fScaleX;
    m_fScaleY = fScaleY;
    m_fScaleZ = fScaleZ;

    svt_redraw();
};
/**
 * get the x axis scaling of the object
 * \return x axis scaling factor
 */
float svt_node::getScaleX()
{
    return m_fScaleY;
};
/**
 * get the y axis scaling of the object
 * \return y axis scaling factor
 */
float svt_node::getScaleY()
{
    return m_fScaleY;
};
/**
 * get the z axis scaling of the object
 * \return z axis scaling factor
 */
float svt_node::getScaleZ()
{
    return m_fScaleZ;
};

/**
 * set the texture of the object
 * \param pTexture pointer to svt_texture object
 */
void svt_node::setTexture(svt_texture* pTexture)
{
  m_pTexture = pTexture;

  svt_redraw();
}


/**
 * get the texture of the object
 */
svt_texture* svt_node::getTexture()
{
    return m_pTexture;
};

/**
 * set the 3d texture of the object
 */
void svt_node::setTexture3D(svt_texture_3d* pTexture3D)
{
    m_pTexture3D = pTexture3D;

    svt_redraw();
};
/**
 * get the 3d texture of the object
 */
svt_texture_3d* svt_node::getTexture3D()
{
    return m_pTexture3D;
};
/**
 * set the visibility of the object (-subtree)
 * \param bVisible if true the object and its subtree is visible
 */
void svt_node::setVisible(bool bVisible)
{
    if (m_bVisible == bVisible)
	return;

    m_bVisible = bVisible;

    svt_redraw();
};
/**
 * get the visibility of the object (-subtree)
 * \return if true the object and its subtree is visible
 */
bool svt_node::getVisible()
{
    return m_bVisible;
};
/**
 * set if this node should set the fog parameters to OpenGL. It is important to choose a the right
 * node because its translation will have an influence on the fog start and end values. this is to
 * facilitate fog that doesn't change on the objects when the whole scene is moved. Note that only
 * one node in the scene should be made the fog node
 */
void svt_node::setIsFogNode(bool bIsFogNode)
{
    m_bIsFogNode = bIsFogNode;
};
/**
 * get if this node should set the fog parameters to OpenGL
 */
bool svt_node::getIsFogNode()
{
    return m_bIsFogNode;
}
/**
 * Apply the fog settings to opengl
 */
void svt_node::applyFogParams()
{
    svt_scene * pScene = svt_getScene();

    if (pScene == NULL)
	return;


    if (!pScene->getFogEnabled())
    {
	glDisable(GL_FOG);
    }
    else
    {
	// this pointer is used to get and set the OpenGL fog parameters. note that only GL_COLOR
	// uses four floats, all other functions use only f[0]
	GLfloat * f = new GLfloat[4];


	// set the fog coordinate source
	pScene->getFogParam(GL_FOG_COORD_SRC, f);
	glFogfv(GL_FOG_COORD_SRC, f);


	// set the fog color
	pScene->getFogParam(GL_FOG_COLOR, f);
	glFogfv(GL_FOG_COLOR, f);


	// set the fog mode
	pScene->getFogParam(GL_FOG_MODE, f);
	glFogfv(GL_FOG_MODE, f);


	if (f[0] == GL_LINEAR)
	{
	    svt_matrix4< Real32 > oMV;
	    oMV.loadModelviewMatrix();

	    // determine the fog start value. the current z translation of this node should be
	    // added, so that moving the current node does not affect the perceived position of the
	    // scene (relative to the fog)

	    pScene->getFogParam(GL_FOG_START, f);

	    f[1] = (GLfloat) oMV.translationZ() + 3.0f; // FIXME: 3.0 camera translation constant, should be dynamic (for tracker)

	    pScene->setFogParam(GL_LINEAR, &f[1]);      // GL_LINEAR enum used as a helper to set the offset value in svt_scene,
							// where it can be queried by the shader programs
	    f[0] -= f[1];
	    glFogfv(GL_FOG_START, f);


	    // same for fog end value
	    pScene->getFogParam(GL_FOG_END, f);
	    f[0] -= f[1];
	    glFogfv(GL_FOG_END, f);
	}
	else
	{
	    // if an exponential fog is used, only the density needs to be set
	    pScene->getFogParam(GL_FOG_DENSITY, f);
	    glFogfv(GL_FOG_DENSITY, f);
	}

	glEnable(GL_FOG);
	delete[] f;
    }
}

/**
 * Set if this node should ignore clipping planes
 */
void svt_node::setIgnoreClipPlanes(bool bIgnore)
{
    m_bIgnoreClipPlanes = bIgnore;
}

/**
 * Get if this node should ignore clipping planes
 */
bool svt_node::getIgnoreClipPlanes()
{
    return m_bIgnoreClipPlanes;
}

///////////////////////////////////////////////////////////////////////////////
// position and orientation
///////////////////////////////////////////////////////////////////////////////

/**
 * set the position of the object
 * \param pPos pointer to the svt_pos object
 */
void svt_node::setPos(svt_pos* pPos)
{
    m_pTrans = pPos;
};
/**
 * get the position of the object
 * \return pointer to the svt_pos object
 */
svt_pos* svt_node::getPos()
{
    return m_pTrans;
};

/**
 * calc the distance to the viewer of the scene
 * \return the distance to the viewer
 */
float svt_node::getDistance()
{
    GLdouble mvmatrix[16];

    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);

    float px, py, pz, pw;

    px = m_pTrans->getX() * mvmatrix[0] + m_pTrans->getY() * mvmatrix[1] + m_pTrans->getZ() * mvmatrix[2] + mvmatrix[3];
    py = m_pTrans->getX() * mvmatrix[4] + m_pTrans->getY() * mvmatrix[5] + m_pTrans->getZ() * mvmatrix[6] + mvmatrix[7];
    pz = m_pTrans->getX() * mvmatrix[8] + m_pTrans->getY() * mvmatrix[9] + m_pTrans->getZ() * mvmatrix[10] + mvmatrix[11];
    pw = m_pTrans->getX() * mvmatrix[12] + m_pTrans->getY() * mvmatrix[13] + m_pTrans->getZ() * mvmatrix[14] + mvmatrix[15];

    return sqrt(px*px + py*py + pz*pz + pw*pw);
}

/**
 * set the rotation of the object. Deprecated: Please use transformation matrix!
 * \param pRot pointer to svt_rotation object
 */
void svt_node::setRotation(svt_rotation* pRot)
{
    m_pRot = pRot;
};
/**
 * get the rotation of the object. Deprecated: Please use transformation matrix!
 * \return pointer to svt_rotation object
 */
svt_rotation* svt_node::getRotation()
{
    return m_pRot;
};

/**
 * set the transformation matrix.
 * \param pTrans pointer to the matrix which should be used as object transformation matrix
 */
void svt_node::setTransformation(svt_matrix4f *pTrans)
{
    m_pTrans = (svt_pos*)pTrans;
}

/**
 * get the object transformation matrix.
 * \return returns the current object transformation matrix
 */
svt_matrix4f* svt_node::getTransformation()
{
    return m_pTrans;
}

/**
 * Set if the object should delete the transformation matrix by itself or wait for somebody else to do it
 * \param bDeleteTrans if true, the object will delete the transformation matrix in the destructor
 */
void svt_node::setDeleteTrans( bool bDeleteTrans )
{
    m_bDeleteTrans = bDeleteTrans;
};
/**
 * Get if the object should delete the transformation matrix by itself or wait for somebody else to do it
 * \return if true, the object will delete the transformation matrix in the destructor
 */
bool svt_node::getDeleteTrans( )
{
    return m_bDeleteTrans;
};
/**
 * Set if the object should delete the properties object by itself or wait for somebody else to do it
 * \param bDeleteProp if true, the object will delete the properties object in the destructor
 */
void svt_node::setDeleteProp( bool bDeleteProp )
{
    m_bDeleteProp = bDeleteProp;
};
/**
 * Get if the object should delete the properties object by itself or wait for somebody else to do it
 * \return if true, the object will delete the properties object in the destructor
 */
bool svt_node::getDeleteProp( )
{
    return m_bDeleteProp;
};

/**
 * get the x position of the node
 * \return x position
 */
float svt_node::getPosX()
{
    if (m_pTrans)
        return m_pTrans->getTranslationX();
    else
        return 0.0f;
}
/**
 * get the y position of the node
 * \return y position
 */
float svt_node::getPosY()
{
    if (m_pTrans)
        return m_pTrans->getTranslationY();
    else
        return 0.0f;
}
/**
 * get the z position of the node
 * \return z position
 */
float svt_node::getPosZ()
{
    if (m_pTrans)
        return m_pTrans->getTranslationZ();
    else
        return 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// node lock
///////////////////////////////////////////////////////////////////////////////

/**
 * get node lock semaphore
 * \return pointer to a svt_semaphore for the node locking
 */
svt_semaphore* svt_node::getNodeLock()
{
    return &m_oSema;
}

///////////////////////////////////////////////////////////////////////////////
// calculation
///////////////////////////////////////////////////////////////////////////////

/**
 * rewrite this function to implement a special dynamic class (e.g. morphing)
 * \return bool Your function should return false if the following subtree should not be recalculated!
 */
bool svt_node::recalc()
{
    return true;
};

/**
 * this function recalcs the complete subtree - it is called automatically
 * \internal
 */
void svt_node::recalcAll()
{
    // if the node wants it, the rest of the subtree will call recalc, too
    if (recalc())
    {
        // the rest of the subtree
        if (getSon() != NULL)
            getSon()->recalcAll();
    }

    // the next element in the tree
    if (getNext() != NULL)
        getNext()->recalcAll();
}

///////////////////////////////////////////////////////////////////////////////
// display lists
///////////////////////////////////////////////////////////////////////////////

/**
 * rebuild display list
 */
void svt_node::rebuildDL()
{
    //svtout << "DEBUG: rebuildDL called in "; printName();

    svt_redraw();

    if (m_pDisplayList)
        m_pDisplayList->destroyCD();

    m_pDisplayList = new svt_displayList();
};

/**
 * create a display list for the complete object subtree of the scenegraph
 */
void svt_node::createDL()
{
    int iNewList = glGenLists(1);

    if (iNewList != 0)
    {
        glNewList(iNewList, GL_COMPILE);
        drawGL();
        glEndList();

        m_pDisplayList->setDisplayList(iNewList);
    }
}

/**
 * enable/disable the automatical generation of display lists
 * \param bAutoDL if true the display lists are generated automatically (default)
 */
void svt_node::setAutoDL(bool bAutoDL)
{
    m_bAutoDL = bAutoDL;
}
/**
 * get the state of the automatical generation of display lists
 * \return true if the display lists are generated automatically (default)
 */
bool svt_node::getAutoDL()
{
    return m_bAutoDL;
}


const svt_matrix<Real32>& svt_node::box() const
{
  static svt_matrix<Real32> s_defaultBox(3,2);
  static bool s_initialized=false;

  if (!s_initialized)
	{
	  s_defaultBox[0][0]=0.0;
	  s_defaultBox[0][1]=1.0;
	  s_defaultBox[1][0]=0.0;
	  s_defaultBox[1][1]=1.0;
	  s_defaultBox[2][0]=0.0;
	  s_defaultBox[2][1]=1.0;
	  s_initialized=true;
	}
  
  return s_defaultBox;

}

void svt_node::saveTransformation(const char* filename) const
{
  svt_ofstream s(filename);

  for (int i=0; i<4; i++)
	{
	for (int j=0; j<4; j++)
	  s << (*m_pTrans)(i,j) << " ";
	s << endl;
	}
}


void svt_node::readTransformation(const char* filename)
{
  svt_ifstream s(filename);

  for (int i=0; i<4; i++)
	for (int j=0; j<4; j++)
	  {
		Real32 val;
		s >> val;
		m_pTrans->setMat(i,j,val);
	  }
}

/**
 * Get the node-ID, which is used for the selection mechanism
 * \return unsigned int node-ID
 */
unsigned int svt_node::getNodeID()
{
    return m_iNodeID;
};
