 /***************************************************************************
                          svt_shader_cg_ao.cpp
                          ---------------
    begin                : 24/02/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


#include <svt_shader_cg_ao.h>
#include <svt_vector3.h>
#include <svt_random.h>


svt_shader_cg_ao_pass1::svt_shader_cg_ao_pass1(bool bFile, const char* pFragment, int iAOMethod)
    : svt_shader_cg(bFile, NULL, pFragment),
      m_aPointSamples( NULL ),
      m_aLineSamples( NULL ),
      m_iNumPointSamples( 0 ),
      m_iNumLineSamples( 0 ),
      m_iDepthtexture( 0 ),
      m_iDithertexture( 0 ),
      m_fFBOwidth( 0 ),
      m_fFBOheight( 0 ),
      m_fP1( 0.0f ),
      m_fP2( 0.0f ),
      m_fA( 0.0f ),
      m_fB( 0.0f ),
      m_fC( 0.0f ),
      m_fD( 0.0f ),
      m_fR( 1.0f ),
      m_fIntensity( 1.0f ),
      m_iAOMethod( iAOMethod )
{
    m_cgDepthtexture    = cgGetNamedParameter(m_pFragProg, "depthtexture");
    m_cgFBOsize         = cgGetNamedParameter(m_pFragProg, "FBOsize");
    m_cgP1P2            = cgGetNamedParameter(m_pFragProg, "P1P2");
    m_cgABCD            = cgGetNamedParameter(m_pFragProg, "ABCD");
    m_cgAOIntensity     = cgGetNamedParameter(m_pFragProg, "intensity");
    m_cgRadius          = cgGetNamedParameter(m_pFragProg, "radius");

    if (m_iAOMethod == SVT_AO_POINT_BASED)
    {
	m_cgDithertexture = cgGetNamedParameter(m_pFragProg, "dithertexture");
	m_cgPointSamples  = cgGetNamedParameter(m_pFragProg, "pointsamples");
	generatePointSamples();
	generateDitheringMap();
    }
    else // if (m_iAOMethod == SVT_AO_LINE_BASED)
    {
	m_cgLineSamples = cgGetNamedParameter(m_pFragProg, "linesamples");
	generateLineSamples();
    }
}

svt_shader_cg_ao_pass1::~svt_shader_cg_ao_pass1()
{
    if (m_aLineSamples != NULL)
	delete[] m_aLineSamples;
    if (m_aPointSamples != NULL)
	delete[] m_aPointSamples;
    if (m_iDithertexture != 0)
	glDeleteTextures(1, &m_iDithertexture);
}

void svt_shader_cg_ao_pass1::generateDitheringMap()
{
//     if (m_fFBOwidth == 0 || m_fFBOheight == 0)
// 	return;

//     m_fHeight = m_fFBOwidth;
//     m_fWidth = m_fFBOheight;
    
    GLsizei m_iHeight = 1024;
    GLsizei m_iWidth = 1024;
   
    int iSize, iN;
    GLfloat * pF;
    svt_vector3< GLfloat > oV;

    iSize = (int)m_iWidth * (int)m_iHeight * 3;
    pF = new GLfloat[iSize];
    iN = sizeof(GLfloat) * 3;

    for (int i=0; i<iSize; i+=3)
    {
	oV.x(svt_ranUni());
	oV.y(svt_ranUni());
	oV.z(svt_ranUni());
	oV.normalize();
// 	Real32 f = svt_ranUni() * 2.0f*(Real32)PI;
// 	oV.x(sin(f));
// 	oV.y(cos(f));
// 	oV.z(1.0f - oV.x()*oV.x() - oV.y()*oV.y());
	memcpy(pF + i, oV.c_data(), iN);
    }

    if (m_iDithertexture == 0)
	glGenTextures(1, &m_iDithertexture);
    glBindTexture(GL_TEXTURE_2D, m_iDithertexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iWidth, m_iHeight, 0, GL_RGB, GL_FLOAT, pF);

    delete[] pF;
}

void svt_shader_cg_ao_pass1::setAOIntensity(Real32 fIntensity)
{
    m_fIntensity = (GLfloat)fIntensity;
}

Real32 svt_shader_cg_ao_pass1::getAOIntensity()
{
    return (Real32)m_fIntensity;
}

void svt_shader_cg_ao_pass1::setAOSampleRadiusScaling(Real32 fR)
{
    m_fR = (GLfloat)fR;

    if (m_iAOMethod == SVT_AO_POINT_BASED)
	generatePointSamples();
    else
	generateLineSamples();
}

Real32 svt_shader_cg_ao_pass1::getAOSampleRadiusScaling()
{
    return (Real32)m_fR;
}

void svt_shader_cg_ao_pass1::generatePointSamples()
{
    svt_vector3< GLfloat > oVec;

    // FIXME number of point samples fixed for now
    m_iNumPointSamples = 8;

    if (m_aPointSamples == NULL)
	m_aPointSamples = new GLfloat[m_iNumPointSamples*3];


    for (int i=0; i<8; ++i)
    {
	switch (i)
	{
	    case 0: oVec.set( 1.0f,  1.0f,  1.0f); break;
	    case 1: oVec.set(-1.0f,  1.0f,  1.0f); break;
	    case 2: oVec.set(-1.0f, -1.0f,  1.0f); break;
	    case 3: oVec.set( 1.0f, -1.0f,  1.0f); break;
	    case 4: oVec.set( 1.0f,  1.0f, -1.0f); break;
	    case 5: oVec.set(-1.0f,  1.0f, -1.0f); break;
	    case 6: oVec.set(-1.0f, -1.0f, -1.0f); break;
	    case 7: oVec.set( 1.0f, -1.0f, -1.0f); break;
	}
	oVec.normalize();
	oVec = oVec * m_fR;

	memcpy(&m_aPointSamples[ i*3 ], oVec.c_data(), sizeof(GLfloat)*3);
    }

//     for (int i=0; i<m_iNumPointSamples; i++)
//     {
// 	oVec.set(m_aPointSamples[i*3], m_aPointSamples[i*3+1], m_aPointSamples[i*3+2]);
// 	printf("%2i: % 2.2f % 2.2f % 2.2f length: %2.3f\n", i, oVec.x(), oVec.y(), oVec.z(), oVec.length());
//     }
//     printf("\n");
}

void svt_shader_cg_ao_pass1::generateLineSamples()
{   
    // FIXME number of line samples fixed for now
    m_iNumLineSamples = 12;

    unsigned int i;
    vector< svt_vector4< Real32 > > aV;
    svt_vector4< Real32 > oV;
    Real32 f, r;

/*    r = 0.6f;
    for (i=0, f=0.0f; i<4; ++i, f += 2.0f*PI/4.0f)
    {
        oV.set( sin(f)*r, cos(f)*r,  0.0f,  0.0f);
        aV.push_back(oV);
    }

    r = 0.85f;
    for (i=4, f=0.0f; i<m_iNumLineSamples; ++i, f += 2.0f*PI/8)
    {
        oV.set( sin(f)*r, cos(f)*r,  0.0f,  0.0f);
        aV.push_back(oV);
    }
*/

    r = 0.85f;
    for (i=0, f=0.0f; i<m_iNumLineSamples; ++i, f += 2.0f*PI/m_iNumLineSamples)
    {
        oV.set( sin(f)*r, cos(f)*r,  0.0f,  0.0f);
        aV.push_back(oV);
    }


    f=0.0f;

    // set z coord at each point and accumulate lengths of all upright rays inside the hemisphere
    for (i=0; i<m_iNumLineSamples; ++i)
    {
        aV[i].z(cos(asin(aV[i].length())));
        f += aV[i].z();
    }
        
    
    // put each upright ray's proportion of accumulated length into fourth coordinate
    for (i=0; i<m_iNumLineSamples; ++i)
    {
        aV[i].w( aV[i].z() / (f*m_fR) );
        aV[i] *= m_fR;
    }
        
    
    if (m_aLineSamples == NULL)
	m_aLineSamples = new GLfloat[m_iNumLineSamples*4];

    for (i=0; i<m_iNumLineSamples; ++i)
	memcpy(&(m_aLineSamples[i*4]), aV[i].c_data(), sizeof(GLfloat)*4);
}

void svt_shader_cg_ao_pass1::setStateFragParams()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    cgGLSetTextureParameter(m_cgDepthtexture, m_iDepthtexture);
    cgGLEnableTextureParameter(m_cgDepthtexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (m_iAOMethod == SVT_AO_POINT_BASED)
    {
	cgGLSetTextureParameter(m_cgDithertexture, m_iDithertexture);
	cgGLEnableTextureParameter(m_cgDithertexture);

	cgGLSetParameterArray3f(m_cgPointSamples, 0, m_iNumPointSamples, m_aPointSamples);
    }
    else // if (m_iAOMethod == SVT_AO_LINE_BASED)
    {
	cgGLSetParameterArray4f(m_cgLineSamples, 0, m_iNumLineSamples,  m_aLineSamples);
    }

    cgGLSetParameter2f(m_cgFBOsize, m_fFBOwidth, m_fFBOheight);
    cgGLSetParameter2f(m_cgP1P2, m_fP1, m_fP2);
    cgGLSetParameter4f(m_cgABCD, m_fA, m_fB, m_fC, m_fD);
    cgGLSetParameter1f(m_cgAOIntensity, m_fIntensity);
    cgGLSetParameter1f(m_cgRadius, m_fR);
}


void svt_shader_cg_ao_pass1::unbind()
{
    cgGLDisableProfile(m_pFragProfile);

    cgGLDisableTextureParameter(m_cgDepthtexture);
    if (m_iAOMethod == SVT_AO_POINT_BASED)
	cgGLDisableTextureParameter(m_cgDithertexture);
}

void svt_shader_cg_ao_pass1::setDepthTexture(unsigned int iDepthtexture)
{
    m_iDepthtexture   = (GLuint)iDepthtexture;
}

void svt_shader_cg_ao_pass1::setFBOsize(unsigned int iFBOwidth, unsigned int iFBOheight)
{
    if ((GLfloat)iFBOwidth != m_fFBOwidth || (GLfloat)iFBOheight != m_fFBOheight)
    {
	m_fFBOwidth =  (GLfloat)iFBOwidth;
	m_fFBOheight = (GLfloat)iFBOheight;

	//generateDitheringMap();
    }
}

void svt_shader_cg_ao_pass1::setP1P2(Real32 fP1, Real32 fP2)
{
    m_fP1 = (GLfloat)fP1;
    m_fP2 = (GLfloat)fP2;
}

void svt_shader_cg_ao_pass1::setABCD(Real32 fA, Real32 fB, Real32 fC, Real32 fD)
{
    m_fA = (GLfloat)fA;
    m_fB = (GLfloat)fB;
    m_fC = (GLfloat)fC;
    m_fD = (GLfloat)fD;
}

int svt_shader_cg_ao_pass1::getAOMethod()
{
    return m_iAOMethod;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////



svt_shader_cg_ao_pass2::svt_shader_cg_ao_pass2(bool bFile, const char* pFragment)
    : svt_shader_cg(bFile, NULL, pFragment),
      m_iAOtexture( 0 ),
      m_iDepthtexture( 0 ),
      m_fPixelSizeX( 0.0f ),
      m_fPixelSizeY( 0.0f ),
      m_fFBOwidth( 0.0f ),
      m_fFBOheight( 0.0f )
{
    m_cgAOtexture    = cgGetNamedParameter(m_pFragProg, "aotexture");
    m_cgDepthtexture = cgGetNamedParameter(m_pFragProg, "depthtexture");
    m_cgPixelSize    = cgGetNamedParameter(m_pFragProg, "pixelsize");
    m_cgFBOsize      = cgGetNamedParameter(m_pFragProg, "FBOsize");
}

svt_shader_cg_ao_pass2::~svt_shader_cg_ao_pass2()
{
}

void svt_shader_cg_ao_pass2::setFBOsize(unsigned int fFBOwidth, unsigned int fFBOheight)
{
    m_fFBOwidth  = fFBOwidth;
    m_fFBOheight = fFBOheight;

    m_fPixelSizeX =  (GLfloat)1.0f / (GLfloat)fFBOwidth;
    m_fPixelSizeY =  (GLfloat)1.0f / (GLfloat)fFBOheight;
}

void svt_shader_cg_ao_pass2::setStateFragParams()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    cgGLSetTextureParameter(m_cgDepthtexture, m_iDepthtexture);
    cgGLEnableTextureParameter(m_cgDepthtexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    cgGLSetTextureParameter(m_cgAOtexture, m_iAOtexture);
    cgGLEnableTextureParameter(m_cgAOtexture);

    cgGLSetParameter2f(m_cgPixelSize, m_fPixelSizeX, m_fPixelSizeY);

    cgGLSetParameter2f(m_cgFBOsize, m_fFBOwidth, m_fFBOheight);
}

void svt_shader_cg_ao_pass2::unbind()
{
    cgGLDisableTextureParameter(m_cgDepthtexture);
    cgGLDisableTextureParameter(m_cgAOtexture);

    cgGLDisableProfile(m_pFragProfile);
}

void svt_shader_cg_ao_pass2::setDepthTexture(unsigned int iDepthtexture)
{
    m_iDepthtexture = (GLuint)iDepthtexture;
}

void svt_shader_cg_ao_pass2::setAOTexture(unsigned int iAOtexture)
{
    m_iAOtexture = (GLuint)iAOtexture;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////



svt_shader_cg_ao_pass3::svt_shader_cg_ao_pass3(bool bFile, const char* pFragment)
    : svt_shader_cg(bFile, NULL, pFragment),
      m_iAOtexture( 0 ),
      m_iDepthtexture( 0 ),
      m_iColortexture( 0 ),
      m_fPixelSizeX( 0.0f ),
      m_fPixelSizeY( 0.0f ),
      m_fFBOwidth( 0.0f ),
      m_fFBOheight( 0.0f )
{
    m_cgAOtexture    = cgGetNamedParameter(m_pFragProg, "aotexture");
    m_cgDepthtexture = cgGetNamedParameter(m_pFragProg, "depthtexture");
    m_cgColortexture = cgGetNamedParameter(m_pFragProg, "colortexture");
    m_cgPixelSize    = cgGetNamedParameter(m_pFragProg, "pixelsize");
    m_cgFBOsize      = cgGetNamedParameter(m_pFragProg, "FBOsize");
}

svt_shader_cg_ao_pass3::~svt_shader_cg_ao_pass3()
{
}

void svt_shader_cg_ao_pass3::setFBOsize(unsigned int fFBOwidth, unsigned int fFBOheight)
{
    m_fFBOwidth  = fFBOwidth;
    m_fFBOheight = fFBOheight;

    m_fPixelSizeX =  (GLfloat)1.0 / (GLfloat)fFBOwidth;
    m_fPixelSizeY =  (GLfloat)1.0 / (GLfloat)fFBOheight;
}

void svt_shader_cg_ao_pass3::setStateFragParams()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    cgGLSetTextureParameter(m_cgDepthtexture, m_iDepthtexture);
    cgGLEnableTextureParameter(m_cgDepthtexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    cgGLSetTextureParameter(m_cgAOtexture, m_iAOtexture);
    cgGLEnableTextureParameter(m_cgAOtexture);

    cgGLSetTextureParameter(m_cgColortexture, m_iColortexture);
    cgGLEnableTextureParameter(m_cgColortexture);

    cgGLSetParameter2f(m_cgPixelSize, m_fPixelSizeX, m_fPixelSizeY);

    cgGLSetParameter2f(m_cgFBOsize, m_fFBOwidth, m_fFBOheight);
}

void svt_shader_cg_ao_pass3::unbind()
{
    cgGLDisableTextureParameter(m_cgAOtexture);
    cgGLDisableTextureParameter(m_cgDepthtexture);
    cgGLDisableTextureParameter(m_cgColortexture);

    cgGLDisableProfile(m_pFragProfile);
}

void svt_shader_cg_ao_pass3::setDepthTexture(unsigned int iDepthtexture)
{
    m_iDepthtexture = (GLuint)iDepthtexture;
}

void svt_shader_cg_ao_pass3::setColorTexture(unsigned int iColortexture)
{
    m_iColortexture = (GLuint)iColortexture;
}

void svt_shader_cg_ao_pass3::setAOTexture(unsigned int iAOtexture)
{
    m_iAOtexture = (GLuint)iAOtexture;
}
