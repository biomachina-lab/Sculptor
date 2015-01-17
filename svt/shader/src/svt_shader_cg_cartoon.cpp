/***************************************************************************
                          svt_shader_cg_cartoon.cpp
                          ---------------
    begin                : 05/01/2007
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_shader_cg_cartoon.h>


/**
 * Constructor
 */
svt_shader_cg_cartoon::svt_shader_cg_cartoon(bool bFile, const char * pCartoonVert, const char * pCartoonFrag, bool bFog)
  : svt_shader_cg(bFile, pCartoonVert, pCartoonFrag),
    m_bFog( bFog )
{
    m_cgMVP           = cgGetNamedParameter(m_pVertProg, "MVP");
    m_cgHeight        = cgGetNamedParameter(m_pVertProg, "height");
    m_cgViewPos       = cgGetNamedParameter(m_pVertProg, "viewPos");
    m_cgLighting      = cgGetNamedParameter(m_pVertProg, "lighting");
    m_cgSpecularColor = cgGetNamedParameter(m_pVertProg, "specular_color");

    if (bFog)
	m_cgMV = cgGetNamedParameter(m_pVertProg, "MV");
};
/**
 * Destructor
 */
svt_shader_cg_cartoon::~svt_shader_cg_cartoon()
{
}

/**
 *
 */
void svt_shader_cg_cartoon::setStateVertParams()
{
    printInfoLog();
    cgGLSetStateMatrixParameter(m_cgMVP,
				CG_GL_MODELVIEW_PROJECTION_MATRIX,
				CG_GL_MATRIX_IDENTITY);

    printInfoLog();
    if (m_bFog)
	cgGLSetStateMatrixParameter(m_cgMV,
				    CG_GL_MODELVIEW_MATRIX,
				    CG_GL_MATRIX_IDENTITY);

    printInfoLog();
    cgGLSetParameter1f(m_cgHeight, m_fHeight);
    printInfoLog();

    cgGLSetParameter3f(m_cgViewPos, m_oViewPos.x(), m_oViewPos.y(), m_oViewPos.z());
    printInfoLog();

    cgGLSetParameter3f(m_cgLighting, m_oLighting.x(), m_oLighting.y(), m_oLighting.z());
    printInfoLog();

    cgGLSetParameter3f(m_cgSpecularColor, m_oSpecularColor.x(), m_oSpecularColor.y(), m_oSpecularColor.z());
    printInfoLog();
}

/**
 *
 */
void svt_shader_cg_cartoon::setHeight(Real32 fHeight)
{
    m_fHeight = fHeight;
}

/**
 *
 */
void svt_shader_cg_cartoon::setViewPos(svt_vector3<Real32> oViewPos)
{
    m_oViewPos = oViewPos;
}

/**
 *
 */
void svt_shader_cg_cartoon::setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor)
{
    m_oLighting = oLighting;
    m_oSpecularColor = oSpecularColor;
}
