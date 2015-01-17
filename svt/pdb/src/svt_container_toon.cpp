/***************************************************************************
                          svt_container_toon
                          -------------
    begin                : 8/12/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_container_toon.h>
#include <svt_shader_cg_toon.h>
#include <svt_init.h>
#include <svt_scene.h>

SVT_PFNGLDRAWELEMENTSEXTPROC svt_container_toon::glDrawElementsEXT = NULL;

bool svt_container_toon::m_bVBO_Supported = false;
bool svt_container_toon::m_bToonSupported = true;

/**
 * Constructor
 */
svt_container_toon::svt_container_toon() :
    svt_node(),
    m_bVBO_Generated( false ),
    m_iNumIndices( 0 ),
    m_pScene( NULL ),
    m_pShaderFrag( NULL ),
    m_pShaderVertex( NULL ),
    m_pShaderFragment( NULL ),
    m_pShaderFragment_FogLinear( NULL ),
    m_pShaderFragment_FogExp( NULL ),
    m_pShaderFragment_FogExp2( NULL ),
    m_bShaderInitialized( false )
{
    setAutoDL(false);

    // check if shader toons are supported
    checkShaderSupport();
    if ( !m_bToonSupported ) 
    {
	setVisible(false);
        return;
    }

    // check if shaders are allowed
    if ( !svt_getAllowShaderPrograms() ) 
    {
	setVisible(false);
        return;
    }

    // all systems on go!
    setVisible(true);


    // toon lighting parameters. should be queried from the GL (FIXME)
    m_oLighting.set(0.2f, 0.8f, 128);
    m_oSpecularColor.set(0.7f, 0.7f, 0.7f);

    m_oV.t[2] = 0.0f; m_oV.t[3] = 0.0f; m_oV.n[0] = 0.0f;
    m_oV.n[1] = 0.0f; m_oV.n[2] = 0.0f;
    m_oV.pad  = 0.0f;

    // if glDrawElementsEXT is not set, this is the first instantiation of this class
    if (glDrawElementsEXT == NULL)
    {
        // do we have vertex buffer objects?
        if (svt_checkEXT_VBO())
            m_bVBO_Supported = true;
    
        // DrawElements is usually not an extension anymore
        glDrawElementsEXT = (SVT_PFNGLDRAWELEMENTSEXTPROC) svt_lookupFunction("glDrawElements");

        // but could be...
        if (glDrawElementsEXT == NULL)
            glDrawElementsEXT = (SVT_PFNGLDRAWELEMENTSEXTPROC) svt_lookupFunction("glDrawElementsARB");
        if (glDrawElementsEXT == NULL)
            glDrawElementsEXT = (SVT_PFNGLDRAWELEMENTSEXTPROC) svt_lookupFunction("glDrawElementsEXT");

        // ok, not available
        if (glDrawElementsEXT == NULL)
            m_bVBO_Supported = false;
    }

    if (m_bVBO_Supported)
    {
	m_oVertexBuffer.genBuffer();
	m_oIndexBuffer.genBuffer();
    }
}


/**
 * Destructor
 */
svt_container_toon::~svt_container_toon()
{
}

/**
 *
 */
bool svt_container_toon::checkShaderSupport()
{
    // check if the CG profiles are supported
    if ( cgGLIsProfileSupported(CG_PROFILE_ARBVP1) == CG_TRUE || cgGLIsProfileSupported(CG_PROFILE_ARBFP1) == CG_TRUE )
    {
        m_bToonSupported = true;
    }
    else
    {
        m_bToonSupported = false;
        //svtout << "High-Quality VDW mode not supported!\n";
    }

    return m_bToonSupported;
}

/**
 * draw the content of the container
 */
void svt_container_toon::drawGL()
{
    // if there are no atoms, do nothing
    if (m_aQuads.size() == 0)
	return;

    if (m_pScene == NULL)
	m_pScene = svt_getScene();

    // in the first run, init the shaders
    if (!m_bShaderInitialized)
        initShader();

    if (m_bVBO_Supported)
    {
	if (!m_bVBO_Generated) // if m_aQuadVertices wasn't generated previously, its data still needs to be copied into the VBO
	{
	    m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
	    m_oVertexBuffer.bufferData( GL_ARRAY_BUFFER_ARB, m_iNumIndices * 64, &(m_aQuads[0]), GL_STATIC_DRAW_ARB );

	    m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
	    m_oIndexBuffer.bufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, m_iNumIndices * sizeof(unsigned int), &(m_aIndices[0]), GL_STATIC_DRAW_ARB);
	}
	else // otherwise, just bind the buffer
	{
	    m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
	    m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
	}

	glColorPointer   ( 4, GL_FLOAT, 64, (char*)(NULL) + 32 );
	glTexCoordPointer( 2, GL_FLOAT, 64, (char*)(NULL) + 16 );
	glVertexPointer  ( 4, GL_FLOAT, 64, (char*)(NULL) );

	m_bVBO_Generated = true;
    }
    else // m_bVBO_Supported == false
    {
	glColorPointer   ( 4, GL_FLOAT, 64, (char*) &m_aQuads[0] + 32 );
	glTexCoordPointer( 2, GL_FLOAT, 64, (char*) &m_aQuads[0] + 16 );
	glVertexPointer  ( 4, GL_FLOAT, 64, (void*) &m_aQuads[0] );
    }

    glEnableClientState( GL_COLOR_ARRAY  );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );



//     // read out of the VBO (for debugging purposes).
//     //printf("sizeof(toon_v4f_t4f_c4f_n3f_64byte): %u\n",  sizeof(toon_v4f_t4f_c4f_n3f_64byte));
//     //printf("sizeof(toon_v4f_t2f_c4ub_32byte): %u\n",     sizeof(toon_v4f_t2f_c4ub_32byte));
//     //
//     unsigned char * pVertVBO = (unsigned char *) m_oVertexBuffer.mapBuffer(GL_ARRAY_BUFFER_ARB, GL_READ_ONLY);
//     if (pVertVBO == NULL) { svtout << "no pointer to vertex mem to read it\n"; return; }
//     //
//     unsigned int *pIndxVBO = (unsigned int *) m_oIndexBuffer.mapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_READ_ONLY);
//     if (pIndxVBO == NULL) { svtout << "no pionter to index mem to read it\n"; return; }
//     //
//     toon_v4f_t4f_c4f_n3f_64byte oTmp;
//     //
//     for (i=0; i<4; i++)
//     {
// 	memcpy(&oTmp, pVertVBO, sizeof(toon_v4f_t4f_c4f_n3f_64byte));
// 	pVertVBO += sizeof(toon_v4f_t4f_c4f_n3f_64byte);
// 	printf("%5i ", *pIndxVBO++);
// 	printf("v: % 2.3f % 2.3f % 2.3f % 2.3f   c: %2.3f %2.3f %2.3f %2.3f   t: % 2.3f % 2.3f % 2.3f % 2.3f   n: % 2.3f % 2.3f % 2.3f pad: %2.3f\n",
// 	       oTmp.v[0], oTmp.v[1], oTmp.v[2], oTmp.v[3],
// 	       oTmp.c[0], oTmp.c[1], oTmp.c[2], oTmp.c[3],
// 	       oTmp.t[0], oTmp.t[1], oTmp.t[2], oTmp.t[3],
// 	       oTmp.n[0], oTmp.n[1], oTmp.n[2],
// 	       oTmp.pad);
//     }
//     m_oVertexBuffer.unmapBuffer(GL_ARRAY_BUFFER_ARB);
//     m_oIndexBuffer.unmapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB);



    // fZ1 and fZ2 are passed to the shader programs, so that they can quickly compute 
    // the projected depth value
    Real32 n = svt_getNear();
    Real32 f = svt_getFar();
    Real32 fZ1 = (n*f) / (f-n);
    Real32 fZ2 = (f+n) / (2.0f*(f-n)) + 0.5f;

    svt_vector4<Real32> oViewport;
    glGetFloatv(GL_VIEWPORT, oViewport.c_data());


    // update the camera so that we can use the viewPos as parameter for the shader programs
    m_oCam.update();


    // check if there is fog, and point to the correct fog shader
    if (m_pScene != NULL)
    {
	if (m_pScene->getFogEnabled())
	{
	    GLfloat f[4];
	    m_pScene->getFogParam(GL_FOG_MODE, f);

	    if (f[0] == GL_LINEAR)
	    {
		m_pShaderFrag = m_pShaderFragment_FogLinear;

		m_pScene->getFogParam(GL_FOG_START, f);
		m_pShaderFrag->setFogStart(f[0]);

		m_pScene->getFogParam(GL_FOG_END, f);
		m_pShaderFrag->setFogEnd(f[0]);

		m_pScene->getFogParam(GL_LINEAR, f);
		m_pShaderFrag->setFogZoomOffset(f[0]);
	    }
	    else if (f[0] == GL_EXP)
	    {
		m_pShaderFrag = m_pShaderFragment_FogExp;

		m_pScene->getFogParam(GL_FOG_DENSITY, f);
		m_pShaderFrag->setFogDensity(f[0]);
	    }
	    else if (f[0] == GL_EXP2)
	    {
		m_pShaderFrag = m_pShaderFragment_FogExp2;

		m_pScene->getFogParam(GL_FOG_DENSITY, f);
		m_pShaderFrag->setFogDensity(f[0]);
	    }

	    m_pScene->getFogParam(GL_FOG_COLOR, f);
	    m_pShaderFrag->setFogColor(f);

	}

	if (!m_pScene->getFogEnabled())
	    m_pShaderFrag = m_pShaderFragment;

    }
    else // no scene, so assume no fog
	m_pShaderFrag = m_pShaderFragment;



    m_pShaderVertex->setViewpos(  (svt_vector3<Real32>) m_oCam.viewPos() );
    m_pShaderVertex->setUp( (svt_vector3<Real32>) m_oCam.up() );
    m_pShaderFrag->setViewport( oViewport );
    //m_pShaderFrag->setViewpos(  (svt_vector3<Real32>) m_oCam.viewPos() );
    m_pShaderFrag->setLightpos( (svt_vector3<Real32>) m_oCam.viewPos() );
    m_pShaderFrag->setLighting(m_oLighting, m_oSpecularColor);
    m_pShaderFrag->setZParams(fZ1, fZ2);



    m_pShaderVertex->bind();
    m_pShaderFrag->bind();

    // draw the quads
    if (m_bVBO_Supported)
	glDrawElementsEXT(GL_QUADS,  m_iNumIndices,  GL_UNSIGNED_INT,  (char*)(NULL));
    else
	glDrawArrays(GL_QUADS, 0, m_aQuads.size());

    m_pShaderFrag->unbind();
    m_pShaderVertex->unbind();



    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );


    // Cleanup VBO's
    //
    if (m_bVBO_Supported)
    {
	m_oVertexBuffer.releaseBuffer( GL_ARRAY_BUFFER_ARB );
	m_oIndexBuffer.releaseBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
    }
}

/**
 * delete all elements
 */
void svt_container_toon::delAll()
{
    m_aQuads.clear();
    m_aIndices.clear();
    m_iNumIndices = 0;

    m_bVBO_Generated = false;
};

/**
 * draw the content of the container (Alias/Wavefront)
 */
void svt_container_toon::drawOBJ()
{
    svtout << "OBJ export not supported high-quality van-der-Waals spheres (switch to standard quality VDW)\n";
}

/**
 * check if toons are supported
 * \return true if toons are supported, false otherwise
 */
bool svt_container_toon::getToonSupported()
{
    return m_bToonSupported;
}

/**
 * this method initializes the shader programs
 */
void svt_container_toon::initShader()
{
    // get the central shader manager and ask it for the shaders
    svt_shader_manager* pShaderManager = svt_getScene()->getShaderManager();

    m_pShaderVertex =             (svt_shader_cg_toon*)pShaderManager->getVDWShader(svt_shader_manager::SVT_SHADER_TOON_VDW_VERTEX);
    m_pShaderFragment =           (svt_shader_cg_toon*)pShaderManager->getVDWShader(svt_shader_manager::SVT_SHADER_TOON_VDW_FRAG_PLAIN);
    m_pShaderFragment_FogLinear = (svt_shader_cg_toon*)pShaderManager->getVDWShader(svt_shader_manager::SVT_SHADER_TOON_VDW_FRAG_FOGLINEAR);
    m_pShaderFragment_FogExp =    (svt_shader_cg_toon*)pShaderManager->getVDWShader(svt_shader_manager::SVT_SHADER_TOON_VDW_FRAG_FOGEXP);
    m_pShaderFragment_FogExp2 =   (svt_shader_cg_toon*)pShaderManager->getVDWShader(svt_shader_manager::SVT_SHADER_TOON_VDW_FRAG_FOGEXP2);

    m_bShaderInitialized = true;
}
