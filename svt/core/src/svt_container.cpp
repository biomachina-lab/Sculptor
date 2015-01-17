/***************************************************************************
                          svt_container
                          -------------
    begin                : 12/05/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_container.h>
#include <svt_window.h>
#include <svt_init.h>
#include <svt_opengl.h>
#include <svt_opengl.h>

///////////////////////////////////////////////////////////////////////////////
// Static members
///////////////////////////////////////////////////////////////////////////////

// static vertex array with coordinates of a sphere
vector< svt_c4ub_n3f_v3f > svt_container::s_aSphereCoords;
vector< unsigned int >     svt_container::s_aSphereFaces;

///////////////////////////////////////////////////////////////////////////////
// Member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_container::svt_container( svt_pos* pPos, svt_properties* pProp ) : svt_node( pPos, pProp ),
    m_bVBO_Generated( false )
{
    // do we have to create a sphere first?
    if (s_aSphereCoords.size() == 0)
        createSphere();

    if (svt_checkEXT_VBO())
    {
	m_oIndexBuffer.genBuffer();
	m_oVertexBuffer.genBuffer();
    }
}

/**
 * Destructor
 * Attention: The desctructor deletes all the elements of the container!!
 */
svt_container::~svt_container()
{
}

/**
 * draw the content of the container (OpenGL)
 */
void svt_container::drawGL()
{
    unsigned int i, iSize;

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    //
    // Draw the points
    //
    if (m_aPointVertArray.size() > 0)
    {
        glColorPointer(  3, GL_FLOAT, sizeof( svt_c3f_v3f ), (void*)&(m_aPointVertArray[0]) );
        glVertexPointer( 3, GL_FLOAT, sizeof( svt_c3f_v3f ), (char*)&(m_aPointVertArray[0]) + (sizeof(Real32)*3) );

        glDisable(GL_LIGHTING);
        glDrawArrays(GL_POINTS, 0, m_aPointVertArray.size() );
        glEnable(GL_LIGHTING);
    }

    //
    // Draw the lines
    //
    if (m_aLineVertArray.size() > 0)
    {
        glLineWidth( 0.1f );
        glDisable(GL_LINE_SMOOTH);
        glColorPointer(  3, GL_FLOAT, sizeof( svt_c3f_v3f ), (void*)&(m_aLineVertArray[0]) );
        glVertexPointer( 3, GL_FLOAT, sizeof( svt_c3f_v3f ), (char*)&(m_aLineVertArray[0]) + (sizeof(Real32)*3) );

        glDisable(GL_LIGHTING);
        glDrawArrays(GL_LINES, 0, m_aLineVertArray.size() );
        glEnable(GL_LIGHTING);
    }

    glEnableClientState( GL_NORMAL_ARRAY );

    //
    // All the other primitives use the same vertex array...
    //
    if (m_aVertArray.size() > 0)
    {
        if (svt_checkEXT_VBO())
        {
            if (!m_bVBO_Generated)
            {
                m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
                m_oVertexBuffer.bufferData( GL_ARRAY_BUFFER_ARB, sizeof(svt_c4ub_n3f_v3f)*m_aVertArray.size(), &(m_aVertArray[0]), GL_STATIC_DRAW_ARB);

                m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
                m_oIndexBuffer.bufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(unsigned int)*m_aVertIndices.size(), &(m_aVertIndices[0]), GL_STATIC_DRAW_ARB);

                m_bVBO_Generated = true;

            } else {

                m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
                m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
            }

            glVertexPointer( 3, GL_FLOAT,         sizeof( svt_c4ub_n3f_v3f ), (char*)(NULL) + 20);
            glNormalPointer(    GL_FLOAT,         sizeof( svt_c4ub_n3f_v3f ), (char*)(NULL) + 8);
            glColorPointer(  4, GL_UNSIGNED_BYTE, sizeof( svt_c4ub_n3f_v3f ), (void*)(NULL));

        } else {

            glVertexPointer( 3, GL_FLOAT,         sizeof( svt_c4ub_n3f_v3f ), (char*)&(m_aVertArray[0]) + 20);
            glNormalPointer(    GL_FLOAT,         sizeof( svt_c4ub_n3f_v3f ), (char*)&(m_aVertArray[0]) + 8);
            glColorPointer(  4, GL_UNSIGNED_BYTE, sizeof( svt_c4ub_n3f_v3f ), (void*)&(m_aVertArray[0]));

        }

	glEnable(GL_COLOR_MATERIAL);
        glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

        //
        // Now draw the quads
        //
	iSize = m_aQuadStartEnd.size();
	if (iSize > 0)
	{
	    for(i=0; i<iSize; i+=2)
	    {
                if (m_bVBO_Generated)
                    glDrawElements(GL_QUADS, m_aQuadStartEnd[i+1] - m_aQuadStartEnd[i], GL_UNSIGNED_INT, (char*)(NULL) + (sizeof(unsigned int)*m_aQuadStartEnd[i]) );
                else
                    glDrawElements(GL_QUADS, m_aQuadStartEnd[i+1] - m_aQuadStartEnd[i], GL_UNSIGNED_INT, &(m_aVertIndices[0]) + m_aQuadStartEnd[i] );
	    }
	}

	//
	// Now draw the globally colored triangles: do not use the color array information by
        // switching off the color client state, that means the triangles receive the global color
        // stored in the svt_container->properties object
        //
	iSize = m_aGloballyColoredTriStartEnd.size();
	if (iSize > 0)
	{
	    glDisableClientState( GL_COLOR_ARRAY );

	    if (getProperties() && getProperties()->getColor())
		getProperties()->getColor()->applyGL();

	    for(i=0; i<iSize; i+=2)
	    {
                if (m_bVBO_Generated)
                    glDrawElements(GL_TRIANGLES, m_aGloballyColoredTriStartEnd[i+1] - m_aGloballyColoredTriStartEnd[i], GL_UNSIGNED_INT, (char*)(NULL) + (sizeof(unsigned int)*m_aGloballyColoredTriStartEnd[i]) );
                else
                    glDrawElements(GL_TRIANGLES, m_aGloballyColoredTriStartEnd[i+1] - m_aGloballyColoredTriStartEnd[i], GL_UNSIGNED_INT, &(m_aVertIndices[0]) + m_aGloballyColoredTriStartEnd[i] );
	    }

	    // re-enable the color array again
	    glEnableClientState( GL_COLOR_ARRAY );
	}

	//
	// Now draw the triangles
        //
	iSize = m_aTriStartEnd.size();
	if (iSize > 0)
	{
	    for(i=0; i<iSize; i+=2)
	    {
                if (m_bVBO_Generated)
                    glDrawElements(GL_TRIANGLES, m_aTriStartEnd[i+1] - m_aTriStartEnd[i], GL_UNSIGNED_INT, (char*)(NULL) + (sizeof(unsigned int)*m_aTriStartEnd[i]) );
                else
                    glDrawElements(GL_TRIANGLES, m_aTriStartEnd[i+1] - m_aTriStartEnd[i], GL_UNSIGNED_INT, &(m_aVertIndices[0]) + m_aTriStartEnd[i] );
            }
	}

	//
	// Now draw the trianglestrips
	//
	iSize = m_aTriStripStartEnd.size();
	if (iSize > 0)
	{
	    for(i=0; i<iSize; i+=2)
	    {
                if (m_bVBO_Generated)
                    glDrawElements(GL_TRIANGLE_STRIP, m_aTriStripStartEnd[i+1] - m_aTriStripStartEnd[i], GL_UNSIGNED_INT, (char*)(NULL) + (sizeof(unsigned int)*m_aTriStripStartEnd[i]) );
                else
                    glDrawElements(GL_TRIANGLE_STRIP, m_aTriStripStartEnd[i+1] - m_aTriStripStartEnd[i], GL_UNSIGNED_INT, &(m_aVertIndices[0]) + m_aTriStripStartEnd[i] );
	    }
	}

	//
	// Now draw the trianglefans
	//
	iSize = m_aTriFanStartEnd.size();
	if (iSize > 0)
	{
	    for(i=0; i<iSize; i+=2)
	    {
                if (m_bVBO_Generated)
                    glDrawElements(GL_TRIANGLE_FAN, m_aTriFanStartEnd[i+1] - m_aTriFanStartEnd[i], GL_UNSIGNED_INT, (char*)(NULL) + (sizeof(unsigned int)*m_aTriFanStartEnd[i]) );
                else
                    glDrawElements(GL_TRIANGLE_FAN, m_aTriFanStartEnd[i+1] - m_aTriFanStartEnd[i], GL_UNSIGNED_INT, &(m_aVertIndices[0]) + m_aTriFanStartEnd[i] );
	    }
	}

        glDisable( GL_COLOR_MATERIAL );

        //
        // Cleanup
        //
        if (m_bVBO_Generated)
        {
            m_oVertexBuffer.releaseBuffer( GL_ARRAY_BUFFER_ARB );
            m_oIndexBuffer.releaseBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );

        }
    }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
}

/**
 * draw the content of the container (OpenInventor)
 */
/*void svt_container::drawIV()
{
    int i;
    int iEnd = m_aObjects.numElements();

    // info
    cout << endl;
    cout << "  Separator {" << endl;
    cout << "    Info {" << endl;
    cout << "      string \"svt_container\"" << endl;
    cout << "    }" << endl;
    cout << "  }" << endl;

    for(i=0; i<iEnd; i++)
    {
        cout << endl << "  Separator {" << endl;
	m_aObjects[i]->drawIV();
	cout << "  }" << endl;
    }
}*/

/**
 * draw the content of the container (PovRay)
 */
/*void svt_container::drawPOV()
{
    int i;
    int iEnd = m_aObjects.numElements();

    for(i=0; i<iEnd; i++)
	m_aObjects[i]->drawPOV();
}*/

/**
 * print the complete yafray code for the object (incl. properties)
 */
/*void svt_container::outputYafray()
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

    drawYafray();

    // the rest of the tree
    if (getSon() != NULL)
	getSon()->draw(SVT_MODE_YAFRAY);

    if (m_pTrans)
	cout << "  </transform>" << endl << endl;

    if (getNext() != NULL)
	getNext()->draw(SVT_MODE_YAFRAY);
}*/

/**
 * draw the content of the container (Yafray)
 */
/*void svt_container::drawYafray()
{
    int i;
    int iEnd = m_aObjects.numElements();

    for(i=0; i<iEnd; i++)
	m_aObjects[i]->drawYafray();
}*/

/**
 * draw the content of the container (Alias/Wavefront)
 */
void svt_container::drawOBJ()
{
    vector< svt_face > aFaces;
    vector< svt_vector4<Real32> > aVecs;
    vector< svt_vector4<Real32> > aNorms;

    unsigned int i,j;
    svt_face oFace;
    svt_vector4<Real32> oVec;
    svt_vector4<Real32> oNorm;
    svt_color oCurrCol;

    //
    // First: Pump all the vertices into the array
    //
    for( i=0; i<m_aVertArray.size(); i++)
    {
        // vertices of the face
        oVec.set(
                 m_aVertArray[ i ].m_oV[0],
                 m_aVertArray[ i ].m_oV[1],
                 m_aVertArray[ i ].m_oV[2]
                );
        aVecs.push_back( oVec );

        // normals of the face
        oNorm.set(
                  m_aVertArray[ i ].m_oN[0],
                  m_aVertArray[ i ].m_oN[1],
                  m_aVertArray[ i ].m_oN[2]
                 );
        aNorms.push_back( oNorm );
    }

    if (m_aVertArray.size() > 0)
        oCurrCol.set(
                     (Real32)(m_aVertArray[ 0 ].m_oC[0]) / 255.0f,
                     (Real32)(m_aVertArray[ 0 ].m_oC[1]) / 255.0f,
                     (Real32)(m_aVertArray[ 0 ].m_oC[2]) / 255.0f
                    );
    svt_getScene()->getAliasOBJ().setColor( oCurrCol );

    //
    // draw all triangles
    //
    if (m_aTriStartEnd.size() > 0)
    {
	for(i=0; i<m_aTriStartEnd.size()-1; i+=2)
	{
	    for (j=m_aTriStartEnd[i]; j<m_aTriStartEnd[i+1]; j+=3)
            {
                // check the normals
                if (normalCheck( j, j+1, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j+1, j ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j] +1 );

                } else if (normalCheck( j+1, j, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j, j+2, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j+1, j+2, j ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j] +1 );

                } else {

                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );
                }

                // did the colors change?
                if (
                    fabs( oCurrCol.getR() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getG() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getB() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f ) > 0.01f )
                {
		    oCurrCol.set(
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f
				);
		    svt_getScene()->getAliasOBJ().setColor( oCurrCol, aFaces.size() );
		}

                aFaces.push_back( oFace );
	    }
	}
    }


    //
    // draw all globally colored triangles
    //
    if (m_aGloballyColoredTriStartEnd.size() > 0)
    {
	for(i=0; i<m_aGloballyColoredTriStartEnd.size()-1; i+=2)
	{
	    for (j=m_aGloballyColoredTriStartEnd[i]; j<m_aGloballyColoredTriStartEnd[i+1]; j+=3)
            {
                // check the normals
                if (normalCheck( j, j+1, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j+1, j ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j] +1 );

                } else if (normalCheck( j+1, j, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j, j+2, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j+1, j+2, j ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j] +1 );

                } else {

                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );
                }

                // did the colors change?
                if (
                    fabs( oCurrCol.getR() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getG() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getB() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f ) > 0.01f )
                {
		    oCurrCol.set(
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f
				);
		    svt_getScene()->getAliasOBJ().setColor( oCurrCol, aFaces.size() );
		}

                aFaces.push_back( oFace );
	    }
	}
    }


    //
    // draw all triangle strips
    //
    if (m_aTriStripStartEnd.size() > 0)
    {
	for(i=0; i<m_aTriStripStartEnd.size()-1; i+=2)
	{
            //
	    // write all faces
	    //
            unsigned int iCount = 0;
	    for (j=m_aTriStripStartEnd[i]; j<m_aTriStripStartEnd[i+1]-2; j++)
            {
		// did the colors change?
                if (
                    fabs( oCurrCol.getR() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getG() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getB() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f ) > 0.01f )
		{
		    oCurrCol.set(
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f
				);
		    svt_getScene()->getAliasOBJ().setColor( oCurrCol, aFaces.size() );
		}


                // check the normals
                if (normalCheck( j, j+1, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j+1, j ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j] +1 );

                } else if (normalCheck( j+1, j, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j, j+2, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j+1, j+2, j ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j] +1 );

                } else {

                    oFace.setVI_A( m_aVertIndices[j] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );
                }

                // add face
		if (iCount % 2 == 0)
		{
		    // face
		    oFace.setVI_A( m_aVertIndices[j] +1 );
		    oFace.setVI_C( m_aVertIndices[j+1] +1 );
		    oFace.setVI_B( m_aVertIndices[j+2] +1 );
		    oFace.setNI_A( m_aVertIndices[j] +1 );
		    oFace.setNI_C( m_aVertIndices[j+1] +1 );
		    oFace.setNI_B( m_aVertIndices[j+2] +1 );
		    aFaces.push_back( oFace );

		} else {
		    // face
		    oFace.setVI_A( m_aVertIndices[j] +1 );
		    oFace.setVI_B( m_aVertIndices[j+1] +1 );
		    oFace.setVI_C( m_aVertIndices[j+2] +1 );
		    oFace.setNI_A( m_aVertIndices[j] +1 );
		    oFace.setNI_B( m_aVertIndices[j+1] +1 );
		    oFace.setNI_C( m_aVertIndices[j+2] +1 );
		    aFaces.push_back( oFace );
		}

                iCount++;

	    }
	}
    }

    //
    // draw all triangle fans
    //
    if (m_aTriFanStartEnd.size() > 0)
    {
	for(i=0; i<m_aTriFanStartEnd.size()-1; i+=2)
	{
            //
	    // write all faces
	    //
            unsigned int iCount = 0;
	    for (j=m_aTriFanStartEnd[i]; j<m_aTriFanStartEnd[i+1]-2; j++)
            {
		// did the colors change?
                if (
                    fabs( oCurrCol.getR() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getG() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f ) > 0.01f ||
                    fabs( oCurrCol.getB() - (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f ) > 0.01f )
		{
		    oCurrCol.set(
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[0]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[1]) / 255.0f,
				 (Real32)(m_aVertArray[ m_aVertIndices[j] ].m_oC[2]) / 255.0f
				);
		    svt_getScene()->getAliasOBJ().setColor( oCurrCol, aFaces.size() );
		}


                // check the normals
                if (normalCheck( m_aTriFanStartEnd[i], j+1, j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, j+1, m_aTriFanStartEnd[i] ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );

                } else if (normalCheck( j+1, m_aTriFanStartEnd[i], j+2 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );

                } else if (normalCheck( j+2, m_aTriFanStartEnd[i], j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[j+2] +1 );
                    oFace.setVI_B( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[j+2] +1 );
                    oFace.setNI_B( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( m_aTriFanStartEnd[i], j+2, j+1 ))
                {
                    oFace.setVI_A( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[j+1] +1 );
                    oFace.setNI_A( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[j+1] +1 );

                } else if (normalCheck( j+1, j+2, m_aTriFanStartEnd[i] ))
                {
                    oFace.setVI_A( m_aVertIndices[j+1] +1 );
                    oFace.setVI_B( m_aVertIndices[j+2] +1 );
                    oFace.setVI_C( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_A( m_aVertIndices[j+1] +1 );
                    oFace.setNI_B( m_aVertIndices[j+2] +1 );
                    oFace.setNI_C( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );

                } else {

                    oFace.setVI_A( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setVI_B( m_aVertIndices[j+1] +1 );
                    oFace.setVI_C( m_aVertIndices[j+2] +1 );
                    oFace.setNI_A( m_aVertIndices[m_aTriFanStartEnd[i]] +1 );
                    oFace.setNI_B( m_aVertIndices[j+1] +1 );
                    oFace.setNI_C( m_aVertIndices[j+2] +1 );
                }

		    aFaces.push_back( oFace );

                iCount++;

	    }
	}
    }

    if (aFaces.size() > 0)
	svt_getScene()->getAliasOBJ().addMesh( aVecs, aNorms, aFaces );
}

/**
 * delete all elements
 */
void svt_container::delAll()
{
    m_aLineVertArray.clear();
    m_aPointVertArray.clear();
    m_aVertArray.clear();
    m_aVertIndices.clear();
    m_aTriStartEnd.clear();
    m_aGloballyColoredTriStartEnd.clear();
    m_aTriFanStartEnd.clear();
    m_aTriStripStartEnd.clear();
    m_aQuadStartEnd.clear();

    m_bVBO_Generated = false;
};

/**
 * Start a set of triangles. After that command, please add vertices with addVertex. When done, call endTriangle.
 */
void svt_container::beginTriangles( )
{
    // if the last element was also a triangle, remove the stop index, so that it is just a longer set of triangles (more efficient to draw).
    if (m_aTriStartEnd.size() > 0 && m_aTriStartEnd.back() == m_aVertIndices.size())

        m_aTriStartEnd.pop_back();

    else

	m_aTriStartEnd.push_back( m_aVertIndices.size() );
};
/**
 * End a triangle
 */
void svt_container::endTriangles( )
{
    m_aTriStartEnd.push_back( m_aVertIndices.size() );
};

/**
 * Start a set of triangles. After that command, please add vertices with addVertex. When done,
 * call endGloballyColoredTriangles. Differs from beginTriangles() in that it is meant for triangles
 * that receive global color information: color client state is disabled when these are rendered
 */
void svt_container::beginGloballyColoredTriangles( )
{
    // if the last element was also a triangle, remove the stop index, so that it is just a longer set of triangles (more efficient to draw).
    if (m_aGloballyColoredTriStartEnd.size() > 0 && m_aGloballyColoredTriStartEnd.back() == m_aVertIndices.size())

        m_aGloballyColoredTriStartEnd.pop_back();

    else

	m_aGloballyColoredTriStartEnd.push_back( m_aVertIndices.size() );
};
/**
 * End a triangle
 */
void svt_container::endGloballyColoredTriangles( )
{
    m_aGloballyColoredTriStartEnd.push_back( m_aVertIndices.size() );
};

/**
 * Start a triangle strip. See beginTriangles.
 */
void svt_container::beginTriStrip( )
{
    m_aTriStripStartEnd.push_back( m_aVertIndices.size() );
};
/**
 * End a triangle strip. See endTriangles.
 */
void svt_container::endTriStrip( )
{
    m_aTriStripStartEnd.push_back( m_aVertIndices.size() );
};

/**
 * Start a triangle fan. See beginTriangles.
 */
void svt_container::beginTriFan( )
{
    m_aTriFanStartEnd.push_back( m_aVertIndices.size() );
};
/**
 * End a triangle fan. See endTriangles.
 */
void svt_container::endTriFan( )
{
    m_aTriFanStartEnd.push_back( m_aVertIndices.size() );
};

/**
 * Start a set of quads. See beginTriangles.
 */
void svt_container::beginQuads()
{
    if (m_aQuadStartEnd.size() > 0 && m_aQuadStartEnd.back() == m_aVertIndices.size())
    {
	m_aQuadStartEnd.pop_back();

    } else

	m_aQuadStartEnd.push_back( m_aVertIndices.size() );
};
/**
 * End a set of quads. See endTriangles.
 */
void svt_container::endQuads()
{
    m_aQuadStartEnd.push_back( m_aVertIndices.size() );
};


/**
 * Add a sphere.
 */
void svt_container::addSphere( const Real64& fX, const Real64& fY, const Real64& fZ, const Real64& rRadius, const svt_color& rColor )
{
    unsigned int i;
    svt_c4ub_n3f_v3f oVert;

    unsigned int iStart = m_aVertArray.size();

    beginTriangles();
    for(i=0; i<s_aSphereCoords.size(); i++)
    {
        oVert = s_aSphereCoords[i];

        // scale
        oVert.m_oV[0] = oVert.m_oV[0] * rRadius;
        oVert.m_oV[1] = oVert.m_oV[1] * rRadius;
	oVert.m_oV[2] = oVert.m_oV[2] * rRadius;
        // translate
        oVert.m_oV[0] = oVert.m_oV[0] + fX;
        oVert.m_oV[1] = oVert.m_oV[1] + fY;
	oVert.m_oV[2] = oVert.m_oV[2] + fZ;
	// color
	oVert.m_oC[0] = (unsigned char)(255.0 * rColor.getR());
	oVert.m_oC[1] = (unsigned char)(255.0 * rColor.getG());
	oVert.m_oC[2] = (unsigned char)(255.0 * rColor.getB());

        addVertexMF( oVert );
    };
    for(i=0; i<s_aSphereFaces.size(); i++)
        addFace( s_aSphereFaces[i] + iStart );
    endTriangles();
};

/**
 * Add a dotted sphere.
 */
void svt_container::addDottedSphere( const Real64& fX, const Real64& fY, const Real64& fZ, const Real64& rRadius, const svt_color& rColor )
{
    unsigned int i;
    svt_c4ub_n3f_v3f oVert;
    svt_c3f_v3f oPoint;

    for(i=0; i<s_aSphereCoords.size(); i++)
    {
        oVert = s_aSphereCoords[i];

        // scale
        oVert.m_oV[0] = oVert.m_oV[0] * rRadius;
        oVert.m_oV[1] = oVert.m_oV[1] * rRadius;
	oVert.m_oV[2] = oVert.m_oV[2] * rRadius;
        // translate
        oPoint[3] = oVert.m_oV[0] + fX;
        oPoint[4] = oVert.m_oV[1] + fY;
	oPoint[5] = oVert.m_oV[2] + fZ;
	// color
	oPoint[0] = rColor.getR();
	oPoint[1] = rColor.getG();
	oPoint[2] = rColor.getB();

        m_aPointVertArray.push_back( oPoint );
    };
};


/**
 * Create a Conical Frustum(TruncatedCone) between two vectors (creates trianglestrips/fans).
 */
void svt_container::addConicalFrustum(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadiusA, Real32 fRadiusB, svt_color& rColor, int m, bool bClosedBase, bool bClosedTop)
{
    // steps
    Real32 fStep = 360.0f / (Real32)(m);

    int i;
    Real32 fTheta;
    svt_vector4<Real32> oPerp1, oPerp2;

    // find an axis not aligned with the cylinder
    svt_vector4<Real32> oAxis;
    oAxis = oVecA - oVecB;
    oAxis.normalize();

    if (fabs(oAxis.x()) < fabs(oAxis.y()) &&
	fabs(oAxis.x()) < fabs(oAxis.z()))
    {
	oPerp1.x( 0 );
	oPerp1.y( oAxis.z() );
	oPerp1.z( -oAxis.y() );
    }
    else if (fabs(oAxis.y()) < fabs(oAxis.z()))
    {
	oPerp1.x( -oAxis.z() );
	oPerp1.y( 0 );
	oPerp1.z( oAxis.x() );
    }
    else
    {
	oPerp1.x( oAxis.y() );
	oPerp1.y( -oAxis.x() );
	oPerp1.z( 0 );
    }
    oPerp1.normalize();
    oPerp2 = vectorProduct( oAxis, oPerp1 );

    // result vector
    svt_c4ub_n3f_v3f oVec;
    oVec.m_oC[0] = (unsigned char)(255.0 * rColor.getR());
    oVec.m_oC[1] = (unsigned char)(255.0 * rColor.getG());
    oVec.m_oC[2] = (unsigned char)(255.0 * rColor.getB());


    // the cone is closed, so create triangle fan
    // note: with a triangle fan, the normal at the tip of the cone points upwards, and
    //       not sidewards as at the base (because naturally there can be only one normal at the tip).
    //       replace the TriFan with a triangle strip, if you want 100% correct lighting
    if (fRadiusB == 0.0f)
    {
	beginTriFan();
	oVec.m_oN[0] = ( -oAxis.x() );
	oVec.m_oN[1] = ( -oAxis.y() );
	oVec.m_oN[2] = ( -oAxis.z() );
	oVec.normalizeNorm();

	oVec.m_oV[0] = ( oVecB.x() );
	oVec.m_oV[1] = ( oVecB.y() );
	oVec.m_oV[2] = ( oVecB.z() );
	addVertex( oVec );

	for (i=m;i>=0;i--)
	{
	    fTheta = deg2rad((Real32)(i) * fStep);

	    oVec.m_oN[0] = ( cos(fTheta) * oPerp1.x() + sin(fTheta) * oPerp2.x() );
	    oVec.m_oN[1] = ( cos(fTheta) * oPerp1.y() + sin(fTheta) * oPerp2.y() );
	    oVec.m_oN[2] = ( cos(fTheta) * oPerp1.z() + sin(fTheta) * oPerp2.z() );
	    oVec.normalizeNorm();

	    oVec.m_oV[0] = ( oVecA.x() + fRadiusA * oVec.m_oN[0] );
	    oVec.m_oV[1] = ( oVecA.y() + fRadiusA * oVec.m_oN[1] );
	    oVec.m_oV[2] = ( oVecA.z() + fRadiusA * oVec.m_oN[2] );
	    addVertex( oVec );
	}
	endTriFan();
    }
    else // the cone is opened, so create triangle strip
    {
	beginTriStrip();
	for (i=m;i>=0;i--)
	{
	    fTheta = deg2rad((Real32)(i) * fStep);

	    oVec.m_oN[0] = ( cos(fTheta) * oPerp1.x() + sin(fTheta) * oPerp2.x() );
	    oVec.m_oN[1] = ( cos(fTheta) * oPerp1.y() + sin(fTheta) * oPerp2.y() );
	    oVec.m_oN[2] = ( cos(fTheta) * oPerp1.z() + sin(fTheta) * oPerp2.z() );
	    oVec.normalizeNorm();

	    oVec.m_oV[0] = ( oVecB.x() + fRadiusB * oVec.m_oN[0] );
	    oVec.m_oV[1] = ( oVecB.y() + fRadiusB * oVec.m_oN[1] );
	    oVec.m_oV[2] = ( oVecB.z() + fRadiusB * oVec.m_oN[2] );
	    addVertex( oVec );

	    oVec.m_oV[0] = ( oVecA.x() + fRadiusA * oVec.m_oN[0] );
	    oVec.m_oV[1] = ( oVecA.y() + fRadiusA * oVec.m_oN[1] );
	    oVec.m_oV[2] = ( oVecA.z() + fRadiusA * oVec.m_oN[2] );
	    addVertex( oVec );

	}
	endTriStrip();
    }    
    
    if (bClosedBase)
    {
	beginTriFan();
	
 	// first add the center of the circle
	oVec.m_oN[0] = oAxis.x();
	oVec.m_oN[1] = oAxis.y();
	oVec.m_oN[2] = oAxis.z();
	oVec.normalizeNorm();

	oVec.m_oV[0] = ( oVecA.x() );
	oVec.m_oV[1] = ( oVecA.y() );
	oVec.m_oV[2] = ( oVecA.z() );
	addVertex( oVec );
	
	// then add point by point all the points on the circle	
	for (i=0;i<=m;i++)
	{
	    fTheta = deg2rad((Real32)(i) * fStep);

	    oVec.m_oV[0] = ( oVecA.x() + fRadiusA * ( cos(fTheta) * oPerp1.x() + sin(fTheta) * oPerp2.x() ) );
	    oVec.m_oV[1] = ( oVecA.y() + fRadiusA * ( cos(fTheta) * oPerp1.y() + sin(fTheta) * oPerp2.y() ) );
	    oVec.m_oV[2] = ( oVecA.z() + fRadiusA * ( cos(fTheta) * oPerp1.z() + sin(fTheta) * oPerp2.z() ) );
	    addVertex( oVec );
	}
       	endTriFan();
    }

    if (bClosedTop)
    {
	beginTriFan();
	
 	// first add the center of the circle
	oVec.m_oN[0] = -oAxis.x();
	oVec.m_oN[1] = -oAxis.y();
	oVec.m_oN[2] = -oAxis.z();
	oVec.normalizeNorm();

	oVec.m_oV[0] = ( oVecB.x() );
	oVec.m_oV[1] = ( oVecB.y() );
	oVec.m_oV[2] = ( oVecB.z() );
	addVertex( oVec );
	
	// then add point by point all the points on the circle	
	for (i=m;i>=0;i--)
	{
	    fTheta = deg2rad((Real32)(i) * fStep);

	    oVec.m_oV[0] = ( oVecB.x() + fRadiusA * ( cos(fTheta) * oPerp1.x() + sin(fTheta) * oPerp2.x() ) );
	    oVec.m_oV[1] = ( oVecB.y() + fRadiusA * ( cos(fTheta) * oPerp1.y() + sin(fTheta) * oPerp2.y() ) );
	    oVec.m_oV[2] = ( oVecB.z() + fRadiusA * ( cos(fTheta) * oPerp1.z() + sin(fTheta) * oPerp2.z() ) );
	    addVertex( oVec );
	}
       	endTriFan();
    }
}


/**
 * Create a cylinder between two vectors (creates trianglestrips).
 */
void svt_container::addCylinder(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadius, svt_color& rColor, int m)
{
    
    addConicalFrustum(oVecA, oVecB, fRadius, fRadius, rColor, m);

}


/**
 * Create a cylinder with capped ends between two vectors (creates trianglestrips).
 */
void svt_container::addCappedCylinder(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadius, svt_color& rColor, int m)
{
    
    addConicalFrustum(oVecA, oVecB, fRadius, fRadius, rColor, m, true, true);

}


/**
 * Create a Cone between two vectors (creates trianglestrips).
 */
void svt_container::addCone(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadius, svt_color& rColor, int m)
{

    addConicalFrustum(oVecA, oVecB, fRadius, 0.0f, rColor, m, true);

}

///////////////////////////////////////////////////////////////////////////////
// Private/Protected Member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Create a sphere with a vertain resolution (creates trianglestrips).
 */
void svt_container::createSphere(int n)
{
   int i,j;
   double theta1,theta2,theta3;
   svt_c4ub_n3f_v3f oVec;
   float TWOPI = PI*2.0;
   float PID2 = PI/2.0;

   for (j=0;j<n/2;j++)
   {
      theta1 = j * TWOPI / n - PID2;
      theta2 = (j + 1) * TWOPI / n - PID2;

      for (i=0;i<n;i++)
      {
         theta3 = i * TWOPI / n;

         oVec.m_oN[0] = cos(theta2) * cos(theta3);
         oVec.m_oN[1] = sin(theta2);
         oVec.m_oN[2] = cos(theta2) * sin(theta3);

         oVec.m_oV[0] = cos(theta2) * cos(theta3);
         oVec.m_oV[1] = sin(theta2);
         oVec.m_oV[2] = cos(theta2) * sin(theta3);

         s_aSphereCoords.push_back( oVec );

         //glTexCoord2f(i/(double)n,2*(j+1)/(double)n);

         oVec.m_oN[0] = cos(theta1) * cos(theta3);
         oVec.m_oN[1] = sin(theta1);
         oVec.m_oN[2] = cos(theta1) * sin(theta3);

	 oVec.m_oV[0] = cos(theta1) * cos(theta3);
         oVec.m_oV[1] = sin(theta1);
         oVec.m_oV[2] = cos(theta1) * sin(theta3);

         //glTexCoord2f(i/(double)n,2*j/(double)n);

	 s_aSphereCoords.push_back( oVec );
      }

      s_aSphereFaces.push_back( (j*n) );
      s_aSphereFaces.push_back( (j*n) + 1 );
   }
}

/**
 * create a model sphere consisting out of triangles
 */
void svt_container::createSphere()
{
    s_aSphereCoords.clear();

    svt_c4ub_n3f_v3f oVecA;
    svt_c4ub_n3f_v3f oVecB;
    svt_c4ub_n3f_v3f oVecC;

    // 1st triangle
    oVecA.setV( 1.0f, 0.0f, 0.0f );  oVecA.setN( 1.0f, 0.0f, 0.0f );  s_aSphereCoords.push_back( oVecA );
    oVecB.setV( 0.7f, 0.7f, 0.0f );  oVecB.setN( 0.7f, 0.7f, 0.0f );  s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 2nd triangle
    oVecA.setV( 0.7f, 0.7f, 0.0f );  oVecA.setN( 0.7f, 0.7f, 0.0f );  s_aSphereCoords.push_back( oVecA );
    oVecB.setV( 0.0f, 1.0f, 0.0f );  oVecB.setN( 0.0f, 1.0f, 0.0f );  s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 3rd triangle
    oVecA.setV( 0.0f, 1.0f, 0.0f );  oVecA.setN( 0.0f, 1.0f, 0.0f );  s_aSphereCoords.push_back( oVecA );
    oVecB.setV( -0.7f, 0.7f, 0.0f ); oVecB.setN( -0.7f, 0.7f, 0.0f ); s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 4th triangle
    oVecA.setV( -0.7f, 0.7f, 0.0f ); oVecA.setN( -0.7f, 0.7f, 0.0f ); s_aSphereCoords.push_back( oVecA );
    oVecB.setV( -1.0f, 0.0f, 0.0f ); oVecB.setN( -1.0f, 0.0f, 0.0f ); s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 5th triangle
    oVecA.setV( -1.0f, 0.0f, 0.0f ); oVecA.setN( -1.0f, 0.0f, 0.0f ); s_aSphereCoords.push_back( oVecA );
    oVecB.setV( -0.7f, -0.7f, 0.0f );oVecB.setN( -0.7f, -0.7f, 0.0f );s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 6th triangle
    oVecA.setV( -0.7f, -0.7f, 0.0f );oVecA.setN( -0.7f, -0.7f, 0.0f );s_aSphereCoords.push_back( oVecA );
    oVecB.setV( 0.0f, -1.0f, 0.0f ); oVecB.setN( 0.0f, -1.0f, 0.0f ); s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 7th triangle
    oVecA.setV( 0.0f, -1.0f, 0.0f ); oVecA.setN( 0.0f, -1.0f, 0.0f ); s_aSphereCoords.push_back( oVecA );
    oVecB.setV( 0.7f, -0.7f, 0.0f ); oVecB.setN( 0.7f, -0.7f, 0.0f ); s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );
    // 8th triangle
    oVecA.setV( 0.7f, -0.7f, 0.0f ); oVecA.setN( 0.7f, -0.7f, 0.0f ); s_aSphereCoords.push_back( oVecA );
    oVecB.setV( 1.0f, 0.0f, 0.0f );  oVecB.setN( 1.0f, 0.0f, 0.0f );  s_aSphereCoords.push_back( oVecB );
    oVecC.setV( 0.0f, 0.0f, 1.0f );  oVecC.setN( 0.0f, 0.0f, 1.0f );  s_aSphereCoords.push_back( oVecC );

    // 9th triangle
    oVecA.setV( 1.0f, 0.0f, 0.0f );  oVecA.setN( 1.0f, 0.0f, 0.0f );
    oVecB.setV( 0.7f, 0.7f, 0.0f );  oVecB.setN( 0.7f, 0.7f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 10th triangle
    oVecA.setV( 0.7f, 0.7f, 0.0f );  oVecA.setN( 0.7f, 0.7f, 0.0f );
    oVecB.setV( 0.0f, 1.0f, 0.0f );  oVecB.setN( 0.0f, 1.0f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 11th triangle
    oVecA.setV( 0.0f, 1.0f, 0.0f );  oVecA.setN( 0.0f, 1.0f, 0.0f );
    oVecB.setV( -0.7f, 0.7f, 0.0f ); oVecB.setN( -0.7f, 0.7f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 12th triangle
    oVecA.setV( -0.7f, 0.7f, 0.0f ); oVecA.setN( -0.7f, 0.7f, 0.0f );
    oVecB.setV( -1.0f, 0.0f, 0.0f ); oVecB.setN( -1.0f, 0.0f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 13th triangle
    oVecA.setV( -1.0f, 0.0f, 0.0f ); oVecA.setN( -1.0f, 0.0f, 0.0f );
    oVecB.setV( -0.7f, -0.7f, 0.0f );oVecB.setN( -0.7f, -0.7f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 14th triangle
    oVecA.setV( -0.7f, -0.7f, 0.0f );oVecA.setN( -0.7f, -0.7f, 0.0f );
    oVecB.setV( 0.0f, -1.0f, 0.0f ); oVecB.setN( 0.0f, -1.0f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 15th triangle
    oVecA.setV( 0.0f, -1.0f, 0.0f ); oVecA.setN( 0.0f, -1.0f, 0.0f );
    oVecB.setV( 0.7f, -0.7f, 0.0f ); oVecB.setN( 0.7f, -0.7f, 0.0f );
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );
    // 16th triangle
    oVecA.setV( 0.7f, -0.7f, 0.0f ); oVecA.setN( 0.7f, -0.7f, 0.0f );
    oVecB.setV( 1.0f, 0.0f, 0.0f );  oVecB.setN( 1.0f, 0.0f, 0.0f ); 
    oVecC.setV( 0.0f, 0.0f, -1.0f ); oVecC.setN( 0.0f, 0.0f, -1.0f );
    s_aSphereCoords.push_back( oVecC ); s_aSphereCoords.push_back( oVecB ); s_aSphereCoords.push_back( oVecA );

    refineSphere( );

    vector< unsigned int > oUnique;
    bool bFound = false;
    unsigned int iFound = 0;
    unsigned int i,j;
    s_aSphereFaces.clear();

    for(i = 0; i<s_aSphereCoords.size(); i++)
    {
        bFound = false;
        for( unsigned int j=0; j<oUnique.size(); j++)
            if (
                (s_aSphereCoords[i].m_oV[0] - s_aSphereCoords[oUnique[j]].m_oV[0]) * (s_aSphereCoords[i].m_oV[0] - s_aSphereCoords[oUnique[j]].m_oV[0]) +
                (s_aSphereCoords[i].m_oV[1] - s_aSphereCoords[oUnique[j]].m_oV[1]) * (s_aSphereCoords[i].m_oV[1] - s_aSphereCoords[oUnique[j]].m_oV[1]) +
                (s_aSphereCoords[i].m_oV[2] - s_aSphereCoords[oUnique[j]].m_oV[2]) * (s_aSphereCoords[i].m_oV[2] - s_aSphereCoords[oUnique[j]].m_oV[2]) < 0.001f)
                bFound = true;

        if (!bFound)
            oUnique.push_back( i );
    }
    for(i = 0; i<s_aSphereCoords.size(); i++)
    {
        iFound = 0;
        for( j=0; j<oUnique.size(); j++)
            if (
                (s_aSphereCoords[i].m_oV[0] - s_aSphereCoords[oUnique[j]].m_oV[0]) * (s_aSphereCoords[i].m_oV[0] - s_aSphereCoords[oUnique[j]].m_oV[0]) +
                (s_aSphereCoords[i].m_oV[1] - s_aSphereCoords[oUnique[j]].m_oV[1]) * (s_aSphereCoords[i].m_oV[1] - s_aSphereCoords[oUnique[j]].m_oV[1]) +
                (s_aSphereCoords[i].m_oV[2] - s_aSphereCoords[oUnique[j]].m_oV[2]) * (s_aSphereCoords[i].m_oV[2] - s_aSphereCoords[oUnique[j]].m_oV[2]) < 0.001f)
                iFound = j;

        s_aSphereFaces.push_back( iFound );
    }
    vector< svt_c4ub_n3f_v3f > oNewCoords;
    for( j=0; j<oUnique.size(); j++)
        oNewCoords.push_back( s_aSphereCoords[ oUnique[j] ] );

    s_aSphereCoords.clear();
    for( i = 0; i<oNewCoords.size(); i++)
        s_aSphereCoords.push_back( oNewCoords[i] );

    return;
}
/**
 * refine sphere model by subdividing the triangles
 */
void svt_container::refineSphere( )
{
    svt_c4ub_n3f_v3f oVecA;
    svt_c4ub_n3f_v3f oVecB;
    svt_c4ub_n3f_v3f oVecC;
    unsigned int i;

    vector< svt_c4ub_n3f_v3f > aNewTris;

    for(i=0; i<s_aSphereCoords.size(); i+=3)
    {
	// first new triangle
	oVecA = s_aSphereCoords[i];
	oVecB = ( s_aSphereCoords[i]   + s_aSphereCoords[i+2] ) * 0.5f;
        oVecC = ( s_aSphereCoords[i]   + s_aSphereCoords[i+1] ) * 0.5f;
        oVecA.normalize(); oVecB.normalize(); oVecC.normalize();
        oVecA.m_oN = oVecA.m_oV; oVecB.m_oN = oVecB.m_oV; oVecC.m_oN = oVecC.m_oV;
        aNewTris.push_back( oVecA );
        aNewTris.push_back( oVecB );
        aNewTris.push_back( oVecC );

	// second new triangle
	oVecA = ( s_aSphereCoords[i]   + s_aSphereCoords[i+2] ) * 0.5f;
	oVecB = s_aSphereCoords[i+2];
	oVecC = ( s_aSphereCoords[i+2] + s_aSphereCoords[i+1] ) * 0.5f;
        oVecA.normalize(); oVecB.normalize(); oVecC.normalize();
        oVecA.m_oN = oVecA.m_oV; oVecB.m_oN = oVecB.m_oV; oVecC.m_oN = oVecC.m_oV;
        aNewTris.push_back( oVecA );
        aNewTris.push_back( oVecB );
        aNewTris.push_back( oVecC );

	// third new triangle
	oVecA = ( s_aSphereCoords[i]   + s_aSphereCoords[i+1] ) * 0.5f;
	oVecB = ( s_aSphereCoords[i]   + s_aSphereCoords[i+2] ) * 0.5f;
	oVecC = ( s_aSphereCoords[i+2] + s_aSphereCoords[i+1] ) * 0.5f;
        oVecA.normalize(); oVecB.normalize(); oVecC.normalize();
        oVecA.m_oN = oVecA.m_oV; oVecB.m_oN = oVecB.m_oV; oVecC.m_oN = oVecC.m_oV;
        aNewTris.push_back( oVecA );
        aNewTris.push_back( oVecB );
        aNewTris.push_back( oVecC );

	// fourth new triangle
	oVecA = ( s_aSphereCoords[i]   + s_aSphereCoords[i+1] ) * 0.5f;
	oVecB = ( s_aSphereCoords[i+2] + s_aSphereCoords[i+1] ) * 0.5f;
	oVecC = s_aSphereCoords[i+1];
        oVecA.normalize(); oVecB.normalize(); oVecC.normalize();
        oVecA.m_oN = oVecA.m_oV; oVecB.m_oN = oVecB.m_oV; oVecC.m_oN = oVecC.m_oV;
        aNewTris.push_back( oVecA );
        aNewTris.push_back( oVecB );
        aNewTris.push_back( oVecC );
    }

    s_aSphereCoords = aNewTris;
}

/**
 * Are the normals of a certain face consistent with the winding
 */
bool svt_container::normalCheck( unsigned int iA, unsigned int iB, unsigned int iC )
{
    svt_c4ub_n3f_v3f oL1;
    svt_c4ub_n3f_v3f oL2;
    svt_c4ub_n3f_v3f oTriNorm;
    Real32 fAng1, fAng2, fAng3;

    // check the normals
    oL1 = m_aVertArray[ m_aVertIndices[iB] ] - m_aVertArray[ m_aVertIndices[iA] ];
    oL2 = m_aVertArray[ m_aVertIndices[iC] ] - m_aVertArray[ m_aVertIndices[iA] ];

    oTriNorm.m_oV[0] = (oL1.m_oV[1] * oL2.m_oV[2]) - (oL1.m_oV[2] * oL2.m_oV[1]);
    oTriNorm.m_oV[1] = (oL1.m_oV[2] * oL2.m_oV[0]) - (oL1.m_oV[0] * oL2.m_oV[2]);
    oTriNorm.m_oV[2] = (oL1.m_oV[0] * oL2.m_oV[1]) - (oL1.m_oV[1] * oL2.m_oV[0]);

    fAng1 =

        (oTriNorm.m_oV[0] * m_aVertArray[ m_aVertIndices[iA] ].m_oN[0]) +
        (oTriNorm.m_oV[1] * m_aVertArray[ m_aVertIndices[iA] ].m_oN[1]) +
        (oTriNorm.m_oV[2] * m_aVertArray[ m_aVertIndices[iA] ].m_oN[2]);

    fAng2 =

        (oTriNorm.m_oV[0] * m_aVertArray[ m_aVertIndices[iB] ].m_oN[0]) +
        (oTriNorm.m_oV[1] * m_aVertArray[ m_aVertIndices[iB] ].m_oN[1]) +
        (oTriNorm.m_oV[2] * m_aVertArray[ m_aVertIndices[iB] ].m_oN[2]);

    fAng3 =

        (oTriNorm.m_oV[0] * m_aVertArray[ m_aVertIndices[iC] ].m_oN[0]) +
        (oTriNorm.m_oV[1] * m_aVertArray[ m_aVertIndices[iC] ].m_oN[1]) +
        (oTriNorm.m_oV[2] * m_aVertArray[ m_aVertIndices[iC] ].m_oN[2]);

    if ( ((fAng1 >= 0.0) || (fAng2 >= 0.0) || (fAng3 >= 0.0)) && ((fAng1 <= 0.0) || (fAng2 <= 0.0) || (fAng3 <= 0.0)) )
        return false;
    else
        return true;
};

