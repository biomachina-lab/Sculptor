/***************************************************************************
                          svt_shader_cg_tube
                          ---------------
    begin                : 05/01/2007
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_shader_cg_tube.h>
#include <svt_opengl.h>



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////svt_shader_cg_tube/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
 * Constructor
 */
svt_shader_cg_tube::svt_shader_cg_tube(bool bFile, const char * pTubeVert, const char * pTubeFrag, bool bFog, CGprofile pVertProfile, CGprofile pFragProfile)
    : svt_shader_cg(bFile, pTubeVert, pTubeFrag, pVertProfile, pFragProfile),
      m_bFog( bFog )
{
    if (pTubeVert != NULL)
    {
	m_cgMVP           = cgGetNamedParameter(m_pVertProg, "MVP");
	m_cgViewPos       = cgGetNamedParameter(m_pVertProg, "viewPos");
	m_cgLighting      = cgGetNamedParameter(m_pVertProg, "lighting");
	m_cgSpecularColor = cgGetNamedParameter(m_pVertProg, "specular_color");
	m_cgAngle         = cgGetNamedParameter(m_pVertProg, "angle");
	m_cgSpan          = cgGetNamedParameter(m_pVertProg, "span");
    }

    // to compute the fog coordinate, modelview matrix is needed
    // FIXME actually not the whole matrix (see shader source)
    if (bFog)
	m_cgMV = cgGetNamedParameter(m_pVertProg, "MV");
}


/**
 * Destructor
 */
svt_shader_cg_tube::~svt_shader_cg_tube()
{
}


/**
 * set the current state of parameters
 */
void svt_shader_cg_tube::setStateVertParams()
{
    cgGLSetStateMatrixParameter(m_cgMVP,
				CG_GL_MODELVIEW_PROJECTION_MATRIX,
				CG_GL_MATRIX_IDENTITY);

    if (m_bFog)
	cgGLSetStateMatrixParameter(m_cgMV,
				    CG_GL_MODELVIEW_MATRIX,
				    CG_GL_MATRIX_IDENTITY);

    cgGLSetParameter1f(m_cgSpan, m_iSpan);

    cgGLSetParameter3f(m_cgViewPos, m_oViewPos.x(), m_oViewPos.y(), m_oViewPos.z());

    cgGLSetParameter3f(m_cgLighting, m_oLighting.x(), m_oLighting.y(), m_oLighting.z());

    cgGLSetParameter3f(m_cgSpecularColor, m_oSpecularColor.x(), m_oSpecularColor.y(), m_oSpecularColor.z());
}


/**
 * Set which quad strip should be drawn
 * \param oAngle the angular position of a quad strip of the tube (in radian)
 */
void svt_shader_cg_tube::setAngle(Real32 iAngle)
{
    m_iAngle = iAngle;
    cgGLSetParameter1f(m_cgAngle, m_iAngle);
}


/**
 * Set the width in radian of a quad strip. This is 2*PI divided by the number of corners of the
 * tube's cross section \param oSpan the width of a tube's quad strip (in radian)
 */
void svt_shader_cg_tube::setSpan(Real32 iSpan)
{
    m_iSpan = iSpan;
}


/**
 * set viewing position 
 * \param oViewPos svt_vector3<Real32> with current viewing position
 */
void svt_shader_cg_tube::setViewPos(svt_vector3<Real32> oViewPos)
{
    m_oViewPos = oViewPos;
}


/**
 * set lighting parameters
 * \param oLighting svt_vector3<Real32> with
 * - x ambient coefficient
 * - y diffuse coefficient
 * - z specular coefficient
 * \param oSpecularColor svt_vector3<Real32> with specular color
 */
void svt_shader_cg_tube::setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor)
{
    m_oLighting      = oLighting;
    m_oSpecularColor = oSpecularColor;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////svt_shader_cg_tube_helixsheet/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
 * Constructor
 */
svt_shader_cg_tube_helixsheet::svt_shader_cg_tube_helixsheet(bool bFile, const char * pTubeVert, const char * pTubeFrag, bool bFog, CGprofile pVertProfile, CGprofile pFragProfile)
    : svt_shader_cg_tube(bFile, pTubeVert, pTubeFrag, bFog, pVertProfile, pFragProfile)
{
    m_cgR1 = cgGetNamedParameter(m_pVertProg, "r1");
    m_cgR2 = cgGetNamedParameter(m_pVertProg, "r2");
}


/**
 * set radius in one direction
 */
void svt_shader_cg_tube_helixsheet::setR1(Real32 fR1)
{
    m_fR1 = fR1;
    cgGLSetParameter1f(m_cgR1, m_fR1);
}


/**
 * set radius in the other direction
 */
void svt_shader_cg_tube_helixsheet::setR2(Real32 fR2)
{
    m_fR2 = fR2;
    cgGLSetParameter1f(m_cgR2, m_fR2);
}
